from BaseHTTPServer import BaseHTTPRequestHandler,HTTPServer
import json
import socket

PORT_NUMBER = 8080
coproConection = None

#This class will handles any incoming request from
#the browser 
class myHandler(BaseHTTPRequestHandler):
	
    #Handler for the GET requests
    def do_POST(self):
        requestData = bytearray(json.loads(self.rfile.read(int(self.headers['Content-Length']))))

        # Send the html message
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
        self.wfile.write(resultStr)
        return

    #Handler for the GET requests
    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Content-type','text/html')
        self.send_header('Access-Control-Allow-Origin','*')
        self.send_header('Access-Control-Allow-Headers', '*')
        self.end_headers()
        return

def processCommand(byteArray):
    global coproConection

    try:
        if coproConection == None:
            coproConection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            coproConection.connect(('localhost', 50005))
            coproConection.settimeout(1)
        coproConection.send(bytes(byteArray))
        result = list(bytearray(coproConection.recv(1024)))
    except:
        print("Lost copro connection")
        result = None
        coproConection = None
    return result

try:
	#Create a web server and define the handler to manage the
	#incoming request
	server = HTTPServer(('', PORT_NUMBER), myHandler)
	print 'Started httpserver on port ' , PORT_NUMBER
	
	#Wait forever for incoming htto requests
	server.serve_forever()

except KeyboardInterrupt:
	print '^C received, shutting down the web server'
	server.socket.close()