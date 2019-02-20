import time as time #computer
#import time as time #micro

def getTime():
    return int(time.time()*1000) #computer
    #return time.ticks_ms()       #micro

class Sensor:
    def __init__(self):
        self.data = 0
        self.lastCollectionTime = 0
        self.cacheDuration = 10

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

class BBTempSensor(Sensor):
    def collect(self):
        Sensor.collect(self)
        # Do something to read sensor
        self.data = 44

class KillSwitchSensor(Sensor):
    def collect(self):
        Sensor.collect(self)
        # Do something to read sensor
        self.data = 12

depth = DepthSensor()

def runCommand(data):
	try:
		data = list(map(ord,data))
	except:
		pass
	commandNum = data[0]
	return bytearray(commandList[commandNum](data[1:]))

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

def getDepthData(data):
	return [depth.value()]

commandList = [
	setMobo, 
	setJetson,
	setThrusters,
	getDepthData
]
