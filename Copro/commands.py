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
	hal.Converter.setTwelvePower(state[0])
	return [1]

def getPortVoltage(data):
	voltage = int(hal.BatteryBalancer.portVoltage.value() * 100)
	return [voltage // 256, voltage % 256]

def getStbdVoltage(data):
	voltage = int(hal.BatteryBalancer.stbdVoltage.value() * 100)
	return [voltage // 256, voltage % 256]

commandList = [
	setMobo, 
	setJetson,
	setThrusters,
	getPortVoltage,
	getStbdVoltage
]
