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



def moboPower(args):
	if len(args) == 1:
		hal.Converter.moboPower.value(args[0])
		return [1]
	return [hal.Converter.moboPower.value()]

def jetsonPower(args):
	if len(args) == 1:
		hal.Converter.jetsonPower.value(args[0])
		return [1]
	return [hal.Converter.jetsonPower.value()]

def thrusterEnable(args):
	if len(args) == 1:
		hal.ESC.setThrusterEnable(args[0])
		return [1]
	return [hal.ESC.thrustersEnabled]

def peltierPower(args):
	if len(args) == 1:
		hal.Converter.peltierPower.value(args[0])
		return [1]
	return [hal.Converter.peltierPower.value()]

def getBatVoltages(args):
	portVoltage = int(hal.BatteryBalancer.portVoltage.value() * 100)
	stbdVoltage = int(hal.BatteryBalancer.stbdVoltage.value() * 100)
	return [portVoltage // 256, portVoltage % 256, stbdVoltage // 256, stbdVoltage % 256]

def getBatCurrents(args):
	portCurrent = int(hal.BatteryBalancer.portCurrent.value() * 100)
	stbdCurrent = int(hal.BatteryBalancer.stbdCurrent.value() * 100)
	return [portCurrent // 256, portCurrent % 256, stbdCurrent // 256, stbdCurrent % 256]

def getTemperature(args):
	temp = int(hal.BatteryBalancer.temp.value() * 10)
	return [temp // 256, temp % 256]

def thrusterForce(args):
	if len(args) == 16:
		values = []
		for i in range(8):
			values.append((args[2 * i] << 8) + args[2 * i + 1])
		hal.ESC.setThrusters(values)
		return [1]
	values = []
	thrusts = hal.ESC.thrusts
	for i in range(8):
		values.append(thrusts[i] // 256)
		values.append(thrusts[i] % 256)
	return values

commandList = [
	moboPower,			#0
	jetsonPower,		#1
	thrusterEnable,		#2
	peltierPower,		#3
	getBatVoltages,		#4
	getBatCurrents,		#5
	getTemperature,		#6
	thrusterForce		#7
]
