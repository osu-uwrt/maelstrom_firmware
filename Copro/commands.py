try:
	import hal 
except:
	import halSimulated as hal

def runCommand(data):
	commandNum = data.pop(0)
	hal.greenLed.on()
	try:
		response = commandList[commandNum](data)
	except Exception as e: 
		print("Error on command "+str(commandNum)+": " + str(e))
		response = []
	hal.greenLed.off()
	return response



def setMobo(state):
	hal.Converter.setMoboPower(state[0])
	return [1]

def setJetson(state):
	hal.Converter.setJetsonPower(state[0])
	return [1]

def setThrusterPower(state):
	hal.ESC.setThrusterEnable(state[0])
	return [1]

def setPeltier(data):
	hal.Converter.petierControl.value(data[0])
	return [1]

def getBatVoltages(data):
	portVoltage = int(hal.BatteryBalancer.portVoltage.value() * 100)
	stbdVoltage = int(hal.BatteryBalancer.stbdVoltage.value() * 100)
	return [portVoltage // 256, portVoltage % 256, stbdVoltage // 256, stbdVoltage % 256]

def getBatCurrents(data):
	portCurrent = int(hal.BatteryBalancer.portCurrent.value() * 100)
	stbdCurrent = int(hal.BatteryBalancer.stbdCurrent.value() * 100)
	return [portCurrent // 256, portCurrent % 256, stbdCurrent // 256, stbdCurrent % 256]

def getTemperature(data):
	temp = int(hal.BatteryBalancer.temp.value() * 10)
	return [temp // 256, temp % 256]

def setThrusterForce(data):
	values = []
	for i in range(8):
		values.append((data[2 * i] << 8) + data[2 * i + 1])
	hal.ESC.setThrusters(values)
	return [1]

commandList = [
	setMobo,			#0
	setJetson,			#1
	setThrusterPower,	#2
	setPeltier,			#3
	getBatVoltages,		#4
	getBatCurrents,		#5
	getTemperature,		#6
	setThrusterForce	#7
]
