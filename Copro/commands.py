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

def logicCurrents(args):
	threeCurrent = int(hal.Converter.threeCurrent.value() * 1000)
	fiveCurrent = int(hal.Converter.fiveCurrent.value() * 1000)
	twelveCurrent = int(hal.Converter.twelveCurrent.value() * 1000)
	return [threeCurrent // 256, threeCurrent % 256, fiveCurrent // 256, fiveCurrent % 256, twelveCurrent // 256, twelveCurrent % 256, ]

def logicVoltages(args):
	threeVoltage = int(hal.Converter.threeVoltage.value() * 1000)
	fiveVoltage = int(hal.Converter.fiveVoltage.value() * 1000)
	twelveVoltage = int(hal.Converter.twelveVoltage.value() * 500)
	return [threeVoltage // 256, threeVoltage % 256, fiveVoltage // 256, fiveVoltage % 256, twelveVoltage // 256, twelveVoltage % 256, ]

def switches(args):
	data = hal.killSwitch.value()
	data = (data << 1) + hal.switch1.value()
	data = (data << 1) + hal.switch2.value()
	data = (data << 1) + hal.switch3.value()
	data = (data << 1) + hal.switch4.value()
	data = (data << 1) + hal.switch5.value()
	return [data]

commandList = [
	moboPower,			#0
	jetsonPower,		#1
	thrusterEnable,		#2
	peltierPower,		#3
	getBatVoltages,		#4
	getBatCurrents,		#5
	getTemperature,		#6
	thrusterForce,		#7
	logicCurrents,		#8
	logicVoltages,		#9
	switches			#10
]
