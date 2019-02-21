import socket
import select
import commands
import traceback
from time import sleep

inputBuffer = ""
print('Setting up socket...')
incomingConnection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
incomingConnection.bind(('127.0.0.1', 50005))
incomingConnection.listen(1)
connections = [incomingConnection]
print('Listening for connections...')

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
				inputBuffer += data
				while len(inputBuffer) > 0 and ord(inputBuffer[0]) <= len(inputBuffer):
					command = inputBuffer[1 : ord(inputBuffer[0])]
					s.sendall(commands.runCommand(command))
					inputBuffer = inputBuffer[ord(inputBuffer[0]):]
				
		sleep(0.01)
except Exception as exc:
	print(exc)
	for s in connections:
		s.close()


	
	
