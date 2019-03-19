import machine
import utime as time
import network
import pyb
from pyb import Timer, Pin, I2C

nic = network.WIZNET5K(machine.SPI(1), machine.Pin('A4', machine.Pin.OUT), machine.Pin('C5', machine.Pin.OUT))
nic.ifconfig(('192.168.1.42', '255.255.255.0', '192.168.1.1', '8.8.8.8')) 

backplaneI2C = I2C(1, I2C.MASTER)
robotI2C = I2C(2, I2C.MASTER)

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
		if getTime() - self.lastCollectionTime > self.cacheDuration:
			self.collect()
		return self.data

	def collect(self):
		self.data = self.collectFunction()
		self.lastCollectionTime = getTime()



class BatteryBalancerBoard:
	deviceAddress = 0x1F

	def __init__(self):
		try:
			while robotI2C.mem_read(1, BatteryBalancerBoard.deviceAddress, 0x0C)[0] & 0b00000010 != 0:
				pass
			# Operational mode 0 (includes temperature) and external vref
			robotI2C.mem_write(chr(0b001), BatteryBalancerBoard.deviceAddress, 0x0B)
			# Set continuous conversion
			robotI2C.mem_write(chr(1), BatteryBalancerBoard.deviceAddress, 0x07)
			# Disable unused channels
			robotI2C.mem_write(chr(0b01100000), BatteryBalancerBoard.deviceAddress, 0x08)
			# Mask all interrupts
			robotI2C.mem_write(chr(0xFF), BatteryBalancerBoard.deviceAddress, 0x03)
			# Start ADC and disable interrupts
			robotI2C.mem_write(chr(1), BatteryBalancerBoard.deviceAddress, 0x00)
		except Exception as e: print("Error on BB init: " + str(e))

	def collectStbdCurrent():
		data = robotI2C.mem_read(2, BatteryBalancerBoard.deviceAddress, 0x20)
		voltage = (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096
		return max((voltage - .33) / .066, 0)
	def collectPortCurrent():
		data = robotI2C.mem_read(2, BatteryBalancerBoard.deviceAddress, 0x21)
		voltage = (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096
		return max((voltage - .33) / .066, 0)
	def collectBalancedVoltage():
		data = robotI2C.mem_read(2, BatteryBalancerBoard.deviceAddress, 0x22)
		return (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096 * (118 / 18)
	def collectStbdVoltage():
		data = robotI2C.mem_read(2, BatteryBalancerBoard.deviceAddress, 0x23)
		return (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096 * (118 / 18) / .988
	def collectPortVoltage():
		data = robotI2C.mem_read(2, BatteryBalancerBoard.deviceAddress, 0x24)
		return (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096 * (118 / 18) / .986
	def collectTemp():
		data = robotI2C.mem_read(2, BatteryBalancerBoard.deviceAddress, 0x27)
		return ((data[0] << 8) + data[1]) / 256

	stbdCurrent = Sensor(collectStbdCurrent)
	portCurrent = Sensor(collectPortCurrent)
	balancedVoltage = Sensor(collectBalancedVoltage)
	stbdVoltage = Sensor(collectStbdVoltage)
	portVoltage = Sensor(collectPortVoltage)
	temp = Sensor(collectTemp)

BatteryBalancer = BatteryBalancerBoard()


class ConverterBoard:
	deviceAddress = 0x37

	def __init__(self):
		try:
			self.moboControl = machine.Pin('C2', machine.Pin.OUT, value=1)
			self.jetsonControl = machine.Pin('C3', machine.Pin.OUT, value=1)
			self.peltierControl = machine.Pin('C1', machine.Pin.OUT, value=1)
			self.threeControl = machine.Pin('C0', machine.Pin.OUT, value=1)
			self.fiveControl = machine.Pin('C13', machine.Pin.OUT, value=1)
			self.twelveControl = machine.Pin('B0', machine.Pin.OUT, value=1)

			while backplaneI2C.mem_read(1, ConverterBoard.deviceAddress, 0x0C)[0] & 0b00000010 != 0:
				pass
			# Operational mode 0 (includes temperature) and external vref
			backplaneI2C.mem_write(chr(0b000), ConverterBoard.deviceAddress, 0x0B)
			# Set continuous conversion
			backplaneI2C.mem_write(chr(1), ConverterBoard.deviceAddress, 0x07)
			# Disable unused channels
			backplaneI2C.mem_write(chr(0b01000000), ConverterBoard.deviceAddress, 0x08)
			# Mask all interrupts
			backplaneI2C.mem_write(chr(0xFF), ConverterBoard.deviceAddress, 0x03)
			# Start ADC and disable interrupts
			backplaneI2C.mem_write(chr(1), ConverterBoard.deviceAddress, 0x00)
		except Exception as e: print("Error on Conv init: " + str(e))

	def setMoboPower(self, power):
		self.moboControl.value(power)
	def setJetsonPower(self, power): 
		self.jetsonControl.value(power)
	def setPeltierPower(self, power):
		self.peltierControl.value(power)
	def setThreePower(self, power):
		self.threeControl.value(power)
	def setFivePower(self, power):
		self.fiveControl.value(power)
	def setTwelvePower(self, power):
		self.twelveControl.value(power)

	def collectFiveCurrent():
		data = backplaneI2C.mem_read(2, ConverterBoard.deviceAddress, 0x20)
		voltage = (((data[0] << 8) + data[1]) >> 4) * 2.54 / 4096
		return max((voltage - .33) / .264, 0)
	def collectThreeCurrent():
		data = backplaneI2C.mem_read(2, ConverterBoard.deviceAddress, 0x21)
		voltage = (((data[0] << 8) + data[1]) >> 4) * 2.54 / 4096
		return max((voltage - .33) / .264, 0)
	def collectTwelveCurrent():
		data = backplaneI2C.mem_read(2, ConverterBoard.deviceAddress, 0x22)
		voltage = (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096
		return max((voltage - .33) / .264, 0)
	def collectTwelveVoltage():
		data = backplaneI2C.mem_read(2, ConverterBoard.deviceAddress, 0x23)
		return (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096 * (12.4 / 2.4)
	def collectFiveVoltage():
		data = backplaneI2C.mem_read(2, ConverterBoard.deviceAddress, 0x24)
		return (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096 * (18 / 8)
	def collectThreeVoltage():
		data = backplaneI2C.mem_read(2, ConverterBoard.deviceAddress, 0x25)
		return (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096 * (18 / 8)
	def collectTemp():
		data = backplaneI2C.mem_read(2, ConverterBoard.deviceAddress, 0x27)
		return ((data[0] << 8) + data[1]) / 256

	fiveVoltage = Sensor(collectFiveVoltage)
	threeVoltage = Sensor(collectThreeVoltage)
	twelveVoltage = Sensor(collectTwelveVoltage)
	fiveCurrent = Sensor(collectFiveCurrent)
	threeCurrent = Sensor(collectThreeCurrent)
	twelveCurrent = Sensor(collectTwelveCurrent)
	temp = Sensor(collectTemp)

Converter = ConverterBoard()


class ESCBoard():
	deviceAddress = 0x2F
	thrustersEnabled = 0

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

			while backplaneI2C.mem_read(1, ESCBoard.deviceAddress, 0x0C)[0] & 0b00000010 != 0:
				pass
			# Operational mode 0 (includes temperature) and external vref
			backplaneI2C.mem_write(chr(0b011), ESCBoard.deviceAddress, 0x0B)
			# Set continuous conversion
			backplaneI2C.mem_write(chr(1), ESCBoard.deviceAddress, 0x07)
			# Disable unused channels
			backplaneI2C.mem_write(chr(0b00000000), ESCBoard.deviceAddress, 0x08)	
			# Mask all interrupts
			backplaneI2C.mem_write(chr(0xFF), ESCBoard.deviceAddress, 0x03)
			# Start ADC and disable interrupts
			backplaneI2C.mem_write(chr(1), ESCBoard.deviceAddress, 0x00)
		except Exception as e: print("Error on ESC init: " + str(e))

	def collectCurrents():
		currents = []
		for i in range(8):
			data = backplaneI2C.mem_read(2, ESCBoard.deviceAddress, 0x20 + i)
			voltage = (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096
			currents[i] = max((voltage - .33) / .066, 0)

	def stopThrusters(self):
		for t in self.thrusters:
				t.pulse_width_percent(60)

	def setThrusters(self, thrusts):
		if self.thrustersEnabled:
			for i in range(8):
				value = thrusts[i] / 25
				self.thrusters[i].pulse_width_percent(value)

	def setThrusterEnable(self, enable):
		self.thrustersEnabled = enable
		self.stopThrusters()

	currents = Sensor(collectCurrents)

ESC = ESCBoard()

class StatusBoard():
	screenAddress = 0x78

	def __init__(self):
		try:
			data = ''.join(map(chr, [0, 0x38]))
			robotI2C.send(data , self.screenAddress)
			time.sleep_ms(10)
			data = ''.join(map(chr, [0, 0x39]))
			robotI2C.send(data , self.screenAddress)
			time.sleep_ms(10)
			data = ''.join(map(chr, [0, 0x14, 0x78, 0x5E, 0x6D, 0x0C, 0x01, 0x06]))
			robotI2C.send(data , self.screenAddress)
			time.sleep_ms(10)
		except Exception as e: print("Error on Status init: " + str(e))

	def write(self, text):
		robotI2C.send(chr(0x40)+text, self.screenAddress)

Status = StatusBoard()


killSwitch = machine.Pin('B12', machine.Pin.IN, machine.Pin.PULL_UP)
switch1 = machine.Pin('B13', machine.Pin.IN, machine.Pin.PULL_UP)
switch2 = machine.Pin('C6', machine.Pin.IN, machine.Pin.PULL_UP)
switch3 = machine.Pin('C7', machine.Pin.IN, machine.Pin.PULL_UP)
switch4 = machine.Pin('C4', machine.Pin.IN, machine.Pin.PULL_UP)
switch5 = machine.Pin('B1', machine.Pin.IN, machine.Pin.PULL_UP)

def killSwitchChanged(pin):
	ESC.setThrusterEnable(not pin.value())

killSwitch.irq(killSwitchChanged)
killSwitchChanged(killSwitch)


