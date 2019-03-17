import time as time #computer
#import time as time #micro

from random import randint

def getTime():
    return int(time.time()*1000) #computer
    #return time.ticks_ms()       #micro

class Sensor:
    def __init__(self):
        self.data = 0
        self.lastCollectionTime = 0
        self.cacheDuration = 100

    def value(self):
        if getTime() - self.lastCollectionTime > self.cacheDuration:
            self.collect()
        return self.data

    def collect(self):
        self.lastCollectionTime = getTime()


class DepthSensor(Sensor):
    def collect(self):
        Sensor.collect(self)
        # Do something to read sensor
        self.data = 10

class StbdBatVoltage(Sensor):
    def collect(self):
        Sensor.collect(self)
        # Do something to read sensor
        self.data = 20.5 + randint(0, 9) / 100.0

class PortBatVoltage(Sensor):
    def collect(self):
        Sensor.collect(self)
        # Do something to read sensor
        self.data = 20 + randint(0, 9) / 100.0

class PortBatCurrent(Sensor):
	def collect(self):
		Sensor.collect(self)
		# Do something to read sensor
		self.data = 20 + randint(0, 9) / 100.0

class StbdBatCurrent(Sensor):
	def collect(self):
		Sensor.collect(self)
		# Do something to read sensor
		self.data = 20 + randint(0, 9) / 100.0


stbdBatVoltage = StbdBatVoltage()
portBatVoltage = PortBatVoltage()
stbdBatCurrent = StbdBatCurrent()
portBatCurrent = PortBatCurrent()

def runCommand(data):
	commandNum = data.pop(0)
	response = commandList[commandNum](data)
	return response




def setMobo(state):
	if state[0]:
		print("Mobo on")
	else:
		print("Mobo off")
	return [1]

def setJetson(state):
	if state[0]:
		print("Jetson on")
	else:
		print("Jetson off")
	return [1]

def setThrusters(state):
	if state[0]:
		print("Thrusters on")
	else:
		print("Thrusters off")
	return [1]

def getPortVoltage(data):
	voltage = int(portBatVoltage.value() * 100)
	return [int(voltage / 256), voltage % 256]

def getStbdVoltage(data):
	voltage = int(stbdBatVoltage.value() * 100)
	return [int(voltage / 256), voltage % 256]

def getPortCurrent(data):
	current = int (portBatCurrent.value() * 100)
	return [int(current / 256), current % 256]

def getStbdCurrent(data):
	current = int (stbdBatCurrent.value() * 100)
	return [int(current / 256), current % 256]

commandList = [
	setMobo,			#0
	setJetson,			#1
	setThrusters,		#2
	getPortVoltage,		#3
	getStbdVoltage,		#4
	getPortCurrent,		#5
	getStbdCurrent		#6
]
