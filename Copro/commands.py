try:
    import hal
except:
	import halSimulated as hal

import time

def runCommand(data):
	commandNum = data.pop(0)
	hal.greenLed.on()
	try:
		response = commandList[commandNum](data)
	except Exception as e:
		print("Error on command "+str(commandNum)+": " + str(e))
		response = []
		hal.raiseFault(hal.COMMAND_EXEC_CRASH_FLAG + commandNum)
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

def getBatVolts(args):
	if not hal.BB.initialized:
		return [0]
	try:
		portVolt = int(hal.BB.portVolt.value() * 100)
		stbdVolt = int(hal.BB.stbdVolt.value() * 100)
		balancedVolt = int(hal.BB.balancedVolt.value() * 100)
		return [1, portVolt // 256, portVolt % 256, stbdVolt // 256, stbdVolt % 256, balancedVolt // 256, balancedVolt % 256]
	except OSError:
		return [0]

def getBatCurrents(args):
	if not hal.BB.initialized:
		return [0]
	try:
		portCurrent = int(hal.BB.portCurrent.value() * 100)
		stbdCurrent = int(hal.BB.stbdCurrent.value() * 100)
		return [1, portCurrent // 256, portCurrent % 256, stbdCurrent // 256, stbdCurrent % 256]
	except OSError:
		return [0]

def getTemperature(args):
	if not hal.BB.initialized:
		return [0]
	try:
		temp = int(hal.BB.temp.value() * 10)
		return [1, temp // 256, temp % 256]
	except OSError:
		return [0]

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
	if not hal.Converter.initialized:
		return [0]
	try:
		threeCurrent = int(hal.Converter.threeCurrent.value() * 1000)
		fiveCurrent = int(hal.Converter.fiveCurrent.value() * 1000)
		twelveCurrent = int(hal.Converter.twelveCurrent.value() * 1000)
		return [1, threeCurrent // 256, threeCurrent % 256, fiveCurrent // 256, fiveCurrent % 256, twelveCurrent // 256, twelveCurrent % 256, ]
	except OSError:
		return [0]

def logicVolts(args):
	if not hal.Converter.initialized:
		return [0]
	try:
		threeVolt = int(hal.Converter.threeVolt.value() * 1000)
		fiveVolt = int(hal.Converter.fiveVolt.value() * 1000)
		twelveVolt = int(hal.Converter.twelveVolt.value() * 500)
		return [1, threeVolt // 256, threeVolt % 256, fiveVolt // 256, fiveVolt % 256, twelveVolt // 256, twelveVolt % 256, ]
	except OSError:
		return [0]

def switches(args):
	data = hal.resetSwitch.value()
	data = (data << 1) + hal.switch4.value()
	data = (data << 1) + hal.switch3.value()
	data = (data << 1) + hal.switch2.value()
	data = (data << 1) + hal.switch1.value()
	data = (data << 1) + hal.killSwitch.value()
	return [0x3F - data]

def depth(args):
	if hal.Depth.initialized:
		data = int(hal.Depth.depth()*100000)
		return [1, (data >> 16), (data >> 8) & 0xFF, data & 0xFF]
	else:
		return [0, 0, 0, 0]

def twelvePower(args):
	if len(args) == 1:
		hal.Converter.twelvePower.value(args[0])
		return [1]
	return [hal.Converter.twelvePower.value()]

def fiveReset(args):
	hal.Converter.fivePower.value(0)
	time.sleep(1)
	hal.Converter.fivePower.value(1)
	return [1]

def getThrusterCurrents(args):
	if not hal.ESC.initialized:
		return [0]
	try:
		values = hal.ESC.currents.value()
		data = [1]
		for i in values:
			data.append(min(int(i*25), 255))
		return data
	except OSError:
		return [0]

def reset(args):
	hal.Copro.restart()

def actuator(args):
	#return hal.Converter.actuators(args)
	return [1]

def latency_check(args):
	return [1]


def memory_check(args):
	usage = int(hal.Copro.memory_usage()*(256*256-1))
	return [usage// 256,  usage % 256]

temp = 40
def temp_threshold(args):
    global temp
    if len(args) == 1:
        temp = args[0]
    return [temp]
    
def get_fault_state(args):
	if len(hal.faultList) != 0:
		# Make sure that the fault list doesn't have an invalid message causing the connection to drop
		if len(hal.faultList) > 254:
			return [1, hal.FAULT_STATE_INVALID]
		
		for entry in hal.faultList:
			if type(entry) != int or entry < 0 or entry > 255:
				return [1, hal.FAULT_STATE_INVALID]
		return [1] + hal.faultList
	else:
		return [0]


commandList = [
	moboPower,			#0
	jetsonPower,		#1
	thrusterEnable,		#2
	peltierPower,		#3
	getBatVolts,		#4
	getBatCurrents,		#5
	getTemperature,		#6
	thrusterForce,		#7
	logicCurrents,		#8
	logicVolts,			#9
	switches,			#10
	depth,				#11
	getThrusterCurrents,#12
	twelvePower,		#13
	fiveReset,			#14
	reset,				#15
	actuator,	     	#16
	latency_check,      #17 
	memory_check,       #18 
    temp_threshold,     #19 
	get_fault_state,	#20
]
