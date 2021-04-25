onCopro = False
try:
	import socket
	from time import sleep
	import halSimulated as hal
	import traceback
	import asyncio
except:
	onCopro = True
	import hal
	import usocket as socket
	import uasyncio as asyncio

import sys
import commands
import select

CONNECTION_TIMEOUT_MS = 1500

print('Setting up socket...')
incomingConnection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
incomingConnection.bind(('', 50000))
incomingConnection.listen(5)
connections = [incomingConnection]
connectionsBuffers = [[]]
connectionsLastResponse = [0]
print('Listening for connections...')


def dropConnection(s):
	s.close()
	connectionIndex = connections.index(s)
	del connectionsBuffers[connectionIndex]
	del connectionsLastResponse[connectionIndex]
	connections.remove(s)
	hal.ESC.stopThrusters()
	print("Lost connection")

def processIncomingData(s):
	if s == incomingConnection:
		conn, addr = incomingConnection.accept()

		try:
			hello = conn.recv(8)
		except:
			dropConnection(s)
			return

		if hello != b"\010UWRT_Hi":
			print("Invalid Hello Message")
			dropConnection(s)
			return
		
		# Send hello message
		conn.send(b"\010UWRT_Hi")

		print("Connected to "+str(addr))
		connections.append(conn)
		connectionsBuffers.append([])
		connectionsLastResponse.append(hal.getTime())
	else:
		try:
			data = s.recv(50)
		except:
			dropConnection(s)
			return
		if not data:
			dropConnection(s)
			return

		connectionIndex = connections.index(s)

		# Update last connection time
		connectionsLastResponse[connectionIndex] = hal.getTime()

		# Command structure: Length, Command, Args...
		# Response structure: Length, values
		# Below code allows for multiple or partial commands to be received
		if not onCopro and sys.version_info < (3, 0):
			data = list(map(ord, data))
		inputBuffer = connectionsBuffers[connectionIndex]
		inputBuffer += data

		# While there is a whole command in the buffer
		while len(inputBuffer) > 0 and inputBuffer[0] <= len(inputBuffer):
			command = inputBuffer[1 : inputBuffer[0]]

			# Act on the command. Terminate connection on command length of 0
			if inputBuffer[0] == 0:
				print('Terminating a connection')
				connections.pop(connectionIndex)
				connectionsBuffers.pop(connectionIndex) 
				connectionsLastResponse.pop(connectionIndex)
				hal.ESC.stopThrusters()
				
				s.close()
				return
			else:
				response = commands.runCommand(command)
				response = [len(response) + 1] + response
				try:
					s.send(bytearray(response))
				except:
					dropConnection(s)
					return

				# Remove the command from the buffer
				inputBuffer = inputBuffer[inputBuffer[0]:]

				connectionsBuffers[connectionIndex] = inputBuffer


async def mainLoop():
	try:
		try:
			f = open("watchdog_enable", "r")
			f.close()
			print("Enabling Watchdog Timer")
			hal.Copro.start_watchdog()
		except OSError:
			print("Disabling Watchdog Timer")
		while True:
			# Handle any incoming data
			readable, _, _ = select.select(connections, [], connections, 0)

			for s in readable:
				processIncomingData(s)

			# Handle timeouts
			dropList = []
			for connectionIndex in range(len(connections)):
				if connections[connectionIndex] == incomingConnection:
					continue
				if hal.getTimeDifference(hal.getTime(), connectionsLastResponse[connectionIndex]) >= CONNECTION_TIMEOUT_MS:
					# The connection can't be dropped immediately since it would break the loop index
					dropList.append(connections[connectionIndex])
			
			for connection in dropList:
				dropConnection(connection)

			# Feed Watchdog
			hal.Copro.feed_watchdog()

			# Yield
			if not onCopro:
				sleep(0.01)
			else:
				await asyncio.sleep(0)
	except Exception as exc:
		if not onCopro:
			traceback.print_exc()
			print(exc)
		else:
			sys.print_exception(exc)
			hal.raiseFault(hal.MAIN_LOOP_CRASH)
		for s in connections:
			s.close()


async def depthLoop():
	try:
		await asyncio.sleep(1.0)
		if (hal.Depth.initialized):
			print("Zeroing depth")
			for _ in range(1, 20):
				await hal.Depth.read()
			for _ in range(1, 20):
				await hal.Depth.zeroDepth()
			print("Collecting depth")
			while True:
				try:
					await hal.Depth.read()
				except Exception as e:
					print("Depth error: " + str(e))
					await asyncio.sleep_ms(50)
	except Exception as exc:
		print("Depth loop error:")
		sys.print_exception(exc)
		hal.raiseFault(hal.DEPTH_LOOP_CRASH)


async def lowVolt():
	try:
		while hal.BB.portVolt.value() < 18.5 or hal.BB.stbdVolt.value() < 18.5:
			await asyncio.sleep(1.0)
		while True:
			await asyncio.sleep(1.0)
			if hal.BB.portVolt.value() < 18.5 or hal.BB.stbdVolt.value() < 18.5:
				hal.ESC.setThrusterEnable(False)
				hal.raiseFault(hal.BATT_LOW)
				hal.blueLed.on()
				print("Low Battery")
			else:
				hal.ESC.setThrusterEnable(True)
				hal.lowerFault(hal.BATT_LOW)
	except Exception as exc:
		print("Battery Checker error:")
		sys.print_exception(exc)
		hal.raiseFault(hal.BATTERY_CHECKER_CRASH)
		
async def auto_cooling():
	try:
		while True:
			current_temp = hal.BB.temp.value()
			temp_thresh = commands.temp_threshold([])[0]
			if current_temp > temp_thresh:
				hal.Converter.peltierPower.value(1)
			else:
				hal.Converter.peltierPower.value(0)
			await asyncio.sleep(0)
	except Exception as exc:
		print ("Auto Cooling Error ")
		sys.print_exception(exc)
		hal.raiseFault(hal.AUTO_COOLING_CRASH)


loop = asyncio.get_event_loop()
loop.create_task(depthLoop())
loop.create_task(mainLoop())
loop.create_task(lowVolt())
loop.create_task(auto_cooling())
loop.run_forever()
loop.close()

hal.raiseFault(hal.PROGRAM_TERMINATED)