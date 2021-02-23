import machine
import utime as time
import network
import pyb
from pyb import Timer, Pin, I2C, LED
import uasyncio as asyncio
import gc

nic = network.WIZNET5K(machine.SPI(1), machine.Pin('A4', machine.Pin.OUT), machine.Pin('C5', machine.Pin.OUT))
nic.ifconfig(('192.168.1.42', '255.255.255.0', '192.168.1.1', '8.8.8.8'))

backplaneI2C = I2C(1, I2C.MASTER, baudrate=200000)
robotI2C = I2C(2, I2C.MASTER, baudrate=200000)

faultLed = LED(1)
faultLed.off()

PROGRAM_TERMINATED = 1
MAIN_LOOP_CRASH = 2
DEPTH_LOOP_CRASH = 3
BATTERY_CHECKER_CRASH = 4
AUTO_COOLING_CRASH = 5
BB_INIT_FAIL = 6
ESC_INIT_FAIL = 7
DEPTH_INIT_FAIL = 8
BACKPLANE_INIT_FAIL = 9
FAULT_STATE_INVALID = 10
BATT_LOW = 11
WATCHDOG_RESET = 12
CONV_BOARD_INIT_FAIL = 13

# When this bit it set, the following 7 bits are the command number for fault
COMMAND_EXEC_CRASH_FLAG = (1<<7)

faultList = []
def raiseFault(faultId: int):
	faultLed.on()
	if faultId not in faultList:
		faultList.append(faultId)

def lowerFault(faultId: int):
	if faultId in faultList:
		faultList.remove(faultId)
	if len(faultList) == 0:
		faultLed.off()

if machine.reset_cause() == machine.WDT_RESET:
	raiseFault(WATCHDOG_RESET)

blueLed = machine.Pin('B4', machine.Pin.OUT)
greenLed = machine.Pin('A15', machine.Pin.OUT)

def getTime():
    return time.ticks_ms()

class Sensor:
	def __init__(self, collectFunction):
		self.data = 0
		self.lastCollectionTime = 0
		self.cacheDuration = 100
		self.collectFunction = collectFunction

	def value(self):
		if time.ticks_diff(getTime(), self.lastCollectionTime) > self.cacheDuration:
			self.collect()
		return self.data

	def collect(self):
		self.data = self.collectFunction()
		self.lastCollectionTime = getTime()



class BBBoard:
	deviceAddress = 0x1F
	initialized = False

	def __init__(self):
		try:
			while robotI2C.mem_read(1, BBBoard.deviceAddress, 0x0C)[0] & 0b00000010 != 0:
				pass
			# Operational mode 0 (includes temperature) and external vref
			robotI2C.mem_write(chr(0b001), BBBoard.deviceAddress, 0x0B)
			# Set continuous conversion
			robotI2C.mem_write(chr(1), BBBoard.deviceAddress, 0x07)
			# Disable unused channels
			robotI2C.mem_write(chr(0b01100000), BBBoard.deviceAddress, 0x08)
			# Mask all interrupts
			robotI2C.mem_write(chr(0xFF), BBBoard.deviceAddress, 0x03)
			# Start ADC and disable interrupts
			robotI2C.mem_write(chr(1), BBBoard.deviceAddress, 0x00)
			self.initialized = True
		except Exception as e: 
			print("Error on BB init: " + str(e))
			raiseFault(BB_INIT_FAIL)

	def getStbdCurrent():
		data = robotI2C.mem_read(2, BBBoard.deviceAddress, 0x20)
		voltage = (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096
		return max((voltage - .33) / .066, 0)
	def getPortCurrent():
		data = robotI2C.mem_read(2, BBBoard.deviceAddress, 0x21)
		voltage = (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096
		return max((voltage - .33) / .066, 0)
	def getBalancedVolt():
		data = robotI2C.mem_read(2, BBBoard.deviceAddress, 0x22)
		return (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096 * (118 / 18)
	def getStbdVolt():
		data = robotI2C.mem_read(2, BBBoard.deviceAddress, 0x23)
		return (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096 * (118 / 18)* .984
	def getPortVolt():
		data = robotI2C.mem_read(2, BBBoard.deviceAddress, 0x24)
		return (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096 * (118 / 18)* .984
	def getTemp():
		data = robotI2C.mem_read(2, BBBoard.deviceAddress, 0x27)
		return ((data[0] << 8) + data[1]) / 256

	stbdCurrent = Sensor(getStbdCurrent)
	portCurrent = Sensor(getPortCurrent)
	balancedVolt = Sensor(getBalancedVolt)
	stbdVolt = Sensor(getStbdVolt)
	portVolt = Sensor(getPortVolt)
	temp = Sensor(getTemp)

BB = BBBoard()


class ConvBoard:
	deviceAddress = 0x37
	actuatorAddress = 0x1C
	initialized = False

	def __init__(self):
		try:
			self.moboPower = machine.Pin('C2', machine.Pin.OUT, value=1)
			self.jetsonPower = machine.Pin('C3', machine.Pin.OUT, value=1)
			self.peltierPower = machine.Pin('C1', machine.Pin.OUT, value=1)
			self.threePower = machine.Pin('C0', machine.Pin.OUT, value=1)
			self.fivePower = machine.Pin('C13', machine.Pin.OUT, value=1)
			self.twelvePower = machine.Pin('B0', machine.Pin.OUT, value=1)

			while backplaneI2C.mem_read(1, ConvBoard.deviceAddress, 0x0C)[0] & 0b00000010 != 0:
				pass
			# Operational mode 0 (includes temperature) and internal vref
			backplaneI2C.mem_write(chr(0b000), ConvBoard.deviceAddress, 0x0B)
			# Set continuous conversion
			backplaneI2C.mem_write(chr(1), ConvBoard.deviceAddress, 0x07)
			# Disable unused channels
			backplaneI2C.mem_write(chr(0b01000000), ConvBoard.deviceAddress, 0x08)
			# Mask all interrupts
			backplaneI2C.mem_write(chr(0xFF), ConvBoard.deviceAddress, 0x03)
			# Start ADC and disable interrupts
			backplaneI2C.mem_write(chr(1), ConvBoard.deviceAddress, 0x00)
			self.initialized = True
		except Exception as e:
			print("Error on Conv init: " + str(e))
			raiseFault(CONV_BOARD_INIT_FAIL)

	def getFiveCurrent():
		data = backplaneI2C.mem_read(2, ConvBoard.deviceAddress, 0x20)
		voltage = (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096
		return max((voltage - .33) / .264, 0)
	def getThreeCurrent():
		data = backplaneI2C.mem_read(2, ConvBoard.deviceAddress, 0x21)
		voltage = (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096
		return max((voltage - .33) / .264, 0)
	def getTwelveCurrent():
		data = backplaneI2C.mem_read(2, ConvBoard.deviceAddress, 0x22)
		voltage = (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096
		return max((voltage - .33) / .264, 0)
	def getTwelveVolt():
		data = backplaneI2C.mem_read(2, ConvBoard.deviceAddress, 0x23)
		return (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096 * (12.4 / 2.4)
	def getFiveVolt():
		data = backplaneI2C.mem_read(2, ConvBoard.deviceAddress, 0x24)
		return (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096 * (18 / 8)
	def getThreeVolt():
		data = backplaneI2C.mem_read(2, ConvBoard.deviceAddress, 0x25)
		return (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096 * (30 / 20)
	def getTemp():
		data = backplaneI2C.mem_read(2, ConvBoard.deviceAddress, 0x27)
		return ((data[0] << 8) + data[1]) / 256

	def actuators(self, args):
		print(args)
		backplaneI2C.send(bytearray(args), ConvBoard.actuatorAddress)
		#data = backplaneI2C.recv(1, ConvBoard.actuatorAddress)
		return [1]#list(backplaneI2C.recv(1, ConvBoard.actuatorAddress))



	fiveVolt = Sensor(getFiveVolt)
	threeVolt = Sensor(getThreeVolt)
	twelveVolt = Sensor(getTwelveVolt)
	fiveCurrent = Sensor(getFiveCurrent)
	threeCurrent = Sensor(getThreeCurrent)
	twelveCurrent = Sensor(getTwelveCurrent)
	temp = Sensor(getTemp)

Converter = ConvBoard()


class ESCBoard():
	deviceAddress = 0x2F
	thrustersEnabled = 1
	thrusts = []
	initialized = False

	def __init__(self):
		try:
			self.tim4 = Timer(4, freq=400)
			self.tim12 = Timer(12, freq=400)
			self.tim2 = Timer(2, freq=400)

			self.thrusters = [
				self.tim4.channel(3, Timer.PWM, pin=Pin('B8')),
				self.tim4.channel(4, Timer.PWM, pin=Pin('B9')),
				self.tim12.channel(1, Timer.PWM, pin=Pin('B14')),
				self.tim12.channel(2, Timer.PWM, pin=Pin('B15')),
				self.tim2.channel(1, Timer.PWM, pin=Pin('A0')),
				self.tim2.channel(2, Timer.PWM, pin=Pin('A1')),
				self.tim2.channel(3, Timer.PWM, pin=Pin('A2')),
				self.tim2.channel(4, Timer.PWM, pin=Pin('A3'))
			]
			self.stopThrusters()
			# Set the time when the kill switch position is changed
			self.timeChange = 0

			while backplaneI2C.mem_read(1, ESCBoard.deviceAddress, 0x0C)[0] & 0b00000010 != 0:
				pass
			# Operational mode 1 (excludes temperature) and external vref
			backplaneI2C.mem_write(chr(0b011), ESCBoard.deviceAddress, 0x0B)
			# Set continuous conversion
			backplaneI2C.mem_write(chr(1), ESCBoard.deviceAddress, 0x07)
			# Disable unused channels
			backplaneI2C.mem_write(chr(0b00000000), ESCBoard.deviceAddress, 0x08)
			# Mask all interrupts
			backplaneI2C.mem_write(chr(0xFF), ESCBoard.deviceAddress, 0x03)
			# Start ADC and disable interrupts
			backplaneI2C.mem_write(chr(1), ESCBoard.deviceAddress, 0x00)
			self.initialized = True
		except Exception as e:
			print("Error on ESC init: " + str(e))
			raiseFault(ESC_INIT_FAIL)

	def getCurrents():
		current_vals = []
		for i in range(8):
			data = backplaneI2C.mem_read(2, ESCBoard.deviceAddress, 0x20 + i)
			voltage = (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096
			current_vals.append(max((voltage - .33) / .264, 0))
		return current_vals

	def stopThrusters(self):
		self.thrusts = [1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500]
		for t in self.thrusters:
				t.pulse_width_percent(60)

	def setThrusters(self, thrusts):
		if self.thrustersEnabled and killSwitch.value() == 0 and time.ticks_diff(getTime(), self.timeChange) > 5000:
			for i in range(8):
				value = thrusts[i] / 25
				self.thrusters[i].pulse_width_percent(value)
			self.thrusts = thrusts

	def setThrusterEnable(self, enable):
		self.thrustersEnabled = enable
		self.stopThrusters()

	currents = Sensor(getCurrents)

ESC = ESCBoard()

class StatusBoard():
	screenAddress = 0x78
	lightAddress = 0x20
	green = 0
	blue = 0
	red = 0
	blink = 0

	def __init__(self):
		try:
			# Global intensity, no blink or interrupt
			robotI2C.mem_write(chr(0b00000100), self.lightAddress, 0x0F)
			# All outputs
			robotI2C.mem_write(chr(0), self.lightAddress, 0x03)
			# Always on / full intensity
			robotI2C.mem_write(chr(0xFF), self.lightAddress, 0x0E)

			#data = ''.join(map(chr, [0, 0x38]))
			#robotI2C.send(data , self.screenAddress)
			#time.sleep_ms(10)
			#data = ''.join(map(chr, [0, 0x39]))
			#robotI2C.send(data , self.screenAddress)
			#time.sleep_ms(10)
			#data = ''.join(map(chr, [0, 0x14, 0x78, 0x5E, 0x6D, 0x0C, 0x01, 0x06]))
			#robotI2C.send(data , self.screenAddress)
			#time.sleep_ms(10)

			self.red = 1
			self.updateLights()
			time.sleep(.33)
			self.red = 0
			self.green = 1
			self.updateLights()
			time.sleep(.33)
			self.green = 0
			self.blue = 1
			self.updateLights()
			time.sleep(.33)
			self.blue = 0
			self.updateLights()
		except Exception as e: print("Error on Status init: " + str(e))

	def setRed(self, state):
		self.red = state
		self.updateLights()

	def setGreen(self, state):
		self.green = state
		self.updateLights()

	def setBlue(self, state):
		self.blue = state
		self.updateLights()

	def setBlink(self, state):
		self.blink = state
		self.updateLights()

	def updateLights(self):
		# blink, red, blue, green
		code = 8 * self.blink + 4 * self.red + 2 * self.blue + self.green
		robotI2C.mem_write(chr(0xF - code), self.lightAddress, 0x01)

	def write(self, text):
		robotI2C.send(chr(0x40)+text, self.screenAddress)

Status = None #StatusBoard()

class DepthSensor():
	deviceAddress = 0x76
	_fluidDensity = 997
	_pressure = 0
	surfacePressure = -1
	initialized = False

	def __init__(self):
		try:
			robotI2C.send(chr(0x1E), self.deviceAddress)
			time.sleep(0.01)
			self._C = []

			# Read calibration and crc
			for i in range(7):
				c = robotI2C.mem_read(2, self.deviceAddress, 0xA0 + 2*i)
				time.sleep(0.01)
				c = (c[0] << 8) + c[1]
				#c =  ((c & 0xFF) << 8) | (c >> 8) # SMBus is little-endian for word transfers, we need to swap MSB and LSB
				self._C.append(c)

			assert (self._C[0] & 0xF000) >> 12 == self.crc4(self._C), "PROM read error, CRC failed!"

			self.initialized = True
		except Exception as e:
			raiseFault(DEPTH_INIT_FAIL)
			print("Error on Depth init: " + str(e))

	# Cribbed from datasheet
	def crc4(self, n_prom):
		n_rem = 0

		n_prom[0] = ((n_prom[0]) & 0x0FFF)
		n_prom.append(0)

		for i in range(16):
			if i%2 == 1:
				n_rem ^= ((n_prom[i>>1]) & 0x00FF)
			else:
				n_rem ^= (n_prom[i>>1] >> 8)

			for n_bit in range(8,0,-1):
				if n_rem & 0x8000:
					n_rem = (n_rem << 1) ^ 0x3000
				else:
					n_rem = (n_rem << 1)

		n_rem = ((n_rem >> 12) & 0x000F)

		self.n_prom = n_prom
		self.n_rem = n_rem

		return n_rem ^ 0x00

	# Cribbed from datasheet
	def calculate(self):
		OFFi = 0
		SENSi = 0
		Ti = 0

		dT = self._D2-self._C[5]*256
		SENS = self._C[1]*32768+(self._C[3]*dT)/256
		OFF = self._C[2]*65536+(self._C[4]*dT)/128
		self._pressure = (self._D1*SENS/(2097152)-OFF)/(8192)

		self._temperature = 2000+dT*self._C[6]/8388608

		# Second order compensation

		if (self._temperature/100) < 20: # Low temp
			Ti = (3*dT*dT)/(8589934592)
			OFFi = (3*(self._temperature-2000)*(self._temperature-2000))/2
			SENSi = (5*(self._temperature-2000)*(self._temperature-2000))/8
			if (self._temperature/100) < -15: # Very low temp
				OFFi = OFFi+7*(self._temperature+1500)*(self._temperature+1500)
				SENSi = SENSi+4*(self._temperature+1500)*(self._temperature+1500)
		elif (self._temperature/100) >= 20: # High temp
			Ti = 2*(dT*dT)/(137438953472)
			OFFi = (1*(self._temperature-2000)*(self._temperature-2000))/16
			SENSi = 0

		OFF2 = OFF-OFFi
		SENS2 = SENS-SENSi

		self._temperature = (self._temperature-Ti)
		self._pressure = (((self._D1*SENS2)/2097152-OFF2)/8192)/10.0

	async def read(self):
		oversampling = 5

		# Request D1 conversion (temperature)
		robotI2C.send(chr(0x40 + 2*oversampling), self.deviceAddress)

		# Maximum conversion time increases linearly with oversampling
		# max time (seconds) ~= 2.2e-6(x) where x = OSR = (2^8, 2^9, ..., 2^13)
		# We use 2.5e-6 for some overhead
		await asyncio.sleep_ms(int(2.5e-3 * 2**(8+oversampling)) + 2)

		d = robotI2C.mem_read(3, self.deviceAddress, 0x00)
		self._D1 = d[0] << 16 | d[1] << 8 | d[2]

		# Request D2 conversion (pressure)
		robotI2C.send(chr(0x50 + 2*oversampling), self.deviceAddress)

		# As above
		await asyncio.sleep_ms(int(2.5e-3 * 2**(8+oversampling)) + 2)

		d = robotI2C.mem_read(3, self.deviceAddress, 0x00)
		self._D2 = d[0] << 16 | d[1] << 8 | d[2]

		self.calculate()

	def pressure(self):
		return self._pressure

	def temperature(self):
		degC = self._temperature / 100.0
		return degC

	# Depth relative to MSL pressure in given fluid density
	def depth(self):
		return ((self.pressure() - self.surfacePressure)*100)/(self._fluidDensity*9.80665)

	async def zeroDepth(self):
		await self.read()
		await self.read()

		if self.surfacePressure == -1:
			self.surfacePressure = self._pressure

		self.surfacePressure = self.surfacePressure * .7 + self._pressure * .3

Depth = DepthSensor()

class CoproBoard():
    wdt = None

    def restart(self):
        machine.reset()

    def start_watchdog(self):
        self.wdt = machine.WDT(timeout=5000)

    def feed_watchdog(self):
        if self.wdt is not None:
            self.wdt.feed()

    def memory_usage(self):
        gc.collect()
        free_memory = gc.mem_free()
        occupy_memory = gc.mem_alloc()
        total_memory = free_memory+occupy_memory
        percent_usage = free_memory/total_memory

        return percent_usage

Copro = CoproBoard()


killSwitch = machine.Pin('B12', machine.Pin.IN, machine.Pin.PULL_UP)
switch1 = machine.Pin('B13', machine.Pin.IN, machine.Pin.PULL_UP)
switch2 = machine.Pin('C6', machine.Pin.IN, machine.Pin.PULL_UP)
switch3 = machine.Pin('C7', machine.Pin.IN, machine.Pin.PULL_UP)
switch4 = machine.Pin('C4', machine.Pin.IN, machine.Pin.PULL_UP)
resetSwitch = machine.Pin('B1', machine.Pin.IN, machine.Pin.PULL_UP)

while (not resetSwitch.value()):
	time.sleep(0.1)

def killSwitchChanged(pin):
	ESC.stopThrusters()
	ESC.timeChange = getTime()

killSwitch.irq(killSwitchChanged)
killSwitchChanged(killSwitch)

def resetSwitchChanged(pin):
	if (not pin.value()):
		Copro.restart()

resetSwitch.irq(resetSwitchChanged)
