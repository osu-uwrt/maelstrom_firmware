try:
	import hal 
except:
	import halSimulated as hal

def runCommand(data):
	commandNum = data.pop(0)
	response = commandList[commandNum](data)
	return response



def setMobo(state):
	hal.Converter.setMoboPower(state[0])
	return [1]

def setJetson(state):
	hal.Converter.setJetsonPower(state[0])
	return [1]

def setThrusters(state):
	hal.ESC.setThrusterEnable(state[0])
	return [1]

def getPortVoltage(data):
	voltage = int(hal.BatteryBalancer.portVoltage.value() * 100)
	return [voltage // 256, voltage % 256]

def getStbdVoltage(data):
	voltage = int(hal.BatteryBalancer.stbdVoltage.value() * 100)
	return [voltage // 256, voltage % 256]

def getPortCurrent(data):
	current = int(hal.BatteryBalancer.portCurrent.value() * 100)
	return [current // 256, current % 256]

def getStbdCurrent(data):
	current = int(hal.BatteryBalancer.stbdCurrent.value() * 100)
	return [current // 256, current % 256]

def getTemperature(data):
	temp = int(hal.BatteryBalancer.temp.value() * 10)
	return [temp // 256, temp % 256]

def setPeltier(data):
	hal.Converter.petierControl.value(data[0])
	return [1]

commandList = [
	setMobo,			#0
	setJetson,			#1
	setThrusters,		#2
	getPortVoltage,		#3
	getStbdVoltage,		#4
	getPortCurrent,		#5
	getStbdCurrent,		#6
	getTemperature,		#7
	setPeltier			#8
]
