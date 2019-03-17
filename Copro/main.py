onCopro = False
try:
	import socket
	from time import sleep
	import halSimulated
	import sys
	import traceback
except:
	onCopro = True
	import hal
	import usocket as socket
	

import commands
import select

print('Setting up socket...')
incomingConnection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
incomingConnection.bind(('0.0.0.0', 50000))
incomingConnection.listen(1)
connections = [incomingConnection]
print('Listening for connections...')


inputBuffer = []
try:
	while 1:
		readable, writable, exceptional = select.select(connections, [], connections, 0)

		for s in readable:
			if s == incomingConnection:
				conn, addr = incomingConnection.accept()
				print("Connected to "+str(addr))
				connections.append(conn)
			else:
				try:
					data = s.recv(1024)
				except:
					connections.remove(s)
					print("Lost connection")
					continue
				if not data:
					connections.remove(s)
					print("Lost connection")
					continue

				# Command structure: Length, Command, Args...
				# Response structure: Length, values
				# Below code allows for multiple or partial commands to be received
				if not onCopro and sys.version_info < (3, 0):
					data = list(map(ord, data))
				inputBuffer += data

				# While there is a whole command in the buffer
				while len(inputBuffer) > 0 and inputBuffer[0] <= len(inputBuffer):
					command = inputBuffer[1 : inputBuffer[0]]

					# Act on the command
					response = commands.runCommand(command)
					response = [len(response) + 1] + response
					s.sendall(bytearray(response))

					# Remove the command from the buffer
					inputBuffer = inputBuffer[inputBuffer[0]:]
		
		if not onCopro:
			sleep(0.01)
except Exception as exc:
	if not onCopro:
		traceback.print_exc()
	print(exc)
	for s in connections:
		s.close()


	
	
