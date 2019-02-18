import socket
import select
import commands
from time import sleep

print('Setting up socket...')
incomingConnection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
incomingConnection.bind(('127.0.0.1', 50005))
incomingConnection.listen(1)
conections = [incomingConnection]
print('Listening for connections...')


while 1:
	readable, writable, exceptional = select.select(conections, [], conections, 0)

	for s in readable:
		if s == incomingConnection:
			conn, addr = incomingConnection.accept()
			print("Connected to "+str(addr))
			conections.append(conn)
		else:
			try:
				data = s.recv(1024)
			except:
				conections.remove(s)
				print("Lost connection")
				continue
			if not data:
				conections.remove(s)
				print("Lost connection")
				continue
			
			s.sendall(commands.runCommand(data))
	sleep(0.01)


	
	
