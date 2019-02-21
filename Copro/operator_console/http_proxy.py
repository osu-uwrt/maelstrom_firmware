try:
	from http.server import BaseHTTPRequestHandler,HTTPServer
except:
	from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
import json
import socket
import threading
import time
import select

PORT_NUMBER = 2000
coproConection = None

class commandWaiter:
    def __init__(self, command):
        self.command = command
        self.event = threading.Event()
        self.response = None

    def setResponse(self, response):
        self.response = response
        self.event.set()

    def getEvent(self):
        return self.event

    def getResponse(self):
        return self.response
    


#This class will handles any incoming request from
#the browser 
class myHandler(BaseHTTPRequestHandler):
	
    #Handler for the POST requests
    def do_POST(self):
        requestData = json.loads(self.rfile.read(int(self.headers['Content-Length'])))
        result = processCommand(requestData)

        resultStr = ""
        if result == None:
            self.send_response(503)
        else:
            self.send_response(200)
            resultStr = json.dumps(result)
        self.send_header('Content-type','text/html')
        self.send_header('Access-Control-Allow-Origin','*')
        self.send_header('Access-Control-Allow-Headers', '*')
        self.end_headers()
        self.wfile.write(resultStr.encode())
        return

    #Handler for the GET requests
    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.send_header('Access-Control-Allow-Origin','*')
        self.send_header('Access-Control-Allow-Headers', '*')
        self.end_headers()
        return

toBeSentQueue = []
toBeReceivedQueue = []

def processCommand(byteArray):
    global coproConection
    global toBeSentQueue

    waiter = commandWaiter(byteArray)
    toBeSentQueue += [waiter]   # Enqueue data to be sent later
    waiter.getEvent().wait()    # Wait for a response
    return waiter.getResponse()


def background():
    global coproConection
    global toBeReceivedQueue
    global toBeSentQueue

    inputBuffer = ""

    while True:
        if coproConection != None:                  # If we are connected
            try:
                readable, writable, exceptional = select.select([coproConection], [coproConection], [coproConection], 0)
                if len(writable) > 0:                       # If we can send data...
                    if len(toBeSentQueue) > 0:              # And we have data to send...
                        toBeSent = toBeSentQueue.pop(0)     # Send command with length prefix
                        coproConection.sendall(bytearray([len(toBeSent.command)+1]+toBeSent.command))
                        toBeReceivedQueue += [toBeSent]     # Wait for response
                
                if len(readable) > 0:
                    data = coproConection.recv(1024)        # Get data
                    if data == None or len(data) == 0:      
                        raise TypeError
                    inputBuffer += data                     # While we have a complete command in the buffer
                    while len(inputBuffer) > 0 and ord(inputBuffer[0]) <= len(inputBuffer):
                        w = toBeReceivedQueue.pop(0)                # Get the request this belongs to
                        response = inputBuffer[1 : ord(inputBuffer[0])]
                        w.setResponse(list(bytearray(response)))    # Send a response to the http protocol
                        inputBuffer = inputBuffer[ord(inputBuffer[0]):]
                
                if len(exceptional) > 0:
                    raise TypeError
            except Exception as exc:
                print("Lost copro connection")
                inputBuffer = ""
                coproConection.close()
                coproConection = None
        else:
            inputBuffer = ""                        # If we are not connected, clear all buffers and queues
            while len(toBeReceivedQueue) != 0:
                toBeReceivedQueue.pop().event.set()

            if len(toBeSentQueue) > 0:              # If we want to send something, try to connect
                try:
                    coproConection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                    coproConection.settimeout(1)
                    coproConection.connect(('localhost', 50005))
                except:
                    coproConection = None           # If it failed, clear queue
                    while len(toBeSentQueue) != 0:  
                        toBeSentQueue.pop().event.set()
        time.sleep(0.01)
                

                

try:
    #Create a web server and define the handler to manage the
    #incoming request
    server = HTTPServer(('', PORT_NUMBER), myHandler)
    print ('Started httpserver on port ' , PORT_NUMBER)

    b = threading.Thread(name='background', target=background)
    b.daemon = True
    b.start()

    #Wait forever for incoming htto requests
    server.serve_forever()

except KeyboardInterrupt:
    print ('^C received, shutting down the web server')
    server.socket.close()
