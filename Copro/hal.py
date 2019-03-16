import machine
import utime as time
import network

nic = network.WIZNET5K(machine.SPI(2), machine.Pin('PD14', machine.Pin.OUT), machine.Pin('PD12', machine.Pin.OUT))
nic.ifconfig(('192.168.1.42', '255.255.255.0', '192.168.1.1', '8.8.8.8')) 

backplaneI2C = machine.I2C(1, machine.I2C.MASTER)
robotI2C = machine.I2C(2, machine.I2C.MASTER)

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



class BatteryBalancer:
	deviceAddress = 0x1F

	def __init__(self):
		while robotI2C.mem_read(1, deviceAddress, 0x0C)[0] & 0b00000010 != 0:
			pass
		# Operational mode 0 (includes temperature) and external vref
		robotI2C.mem_write([0b001], deviceAddress, 0x0B)
		# Set continuous conversion
		robotI2C.mem_write([1], deviceAddress, 0x07)
		# Disable unused channels
		robotI2C.mem_write([0b01100000], deviceAddress, 0x08)
		# Mask all interrupts
		robotI2C.mem_write([0xFF], deviceAddress, 0x03)
		# Start ADC and disable interrupts
		robotI2C.mem_write([1], deviceAddress, 0x00)

	def collectStbdCurrent():
		data = robotI2C.mem_read(2, deviceAddress, 0x20)
		return (((data[0] << 8) + data[1]) >> 4) * 40 / 4096
	def collectPortCurrent():
		data = robotI2C.mem_read(2, deviceAddress, 0x21)
		return (((data[0] << 8) + data[1]) >> 4) * 40 / 4096
	def collectBalancedVoltage():
		data = robotI2C.mem_read(2, deviceAddress, 0x22)
		return (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096 * (118 / 18)
	def collectStbdVoltage():
		data = robotI2C.mem_read(2, deviceAddress, 0x23)
		return (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096 * (118 / 18)
	def collectPortVoltage():
		data = robotI2C.mem_read(2, deviceAddress, 0x24)
		return (((data[0] << 8) + data[1]) >> 4) * 3.3 / 4096 * (118 / 18)
	def collectTemp():
		data = robotI2C.mem_read(2, deviceAddress, 0x27)
		return ((data[0] << 8) + data[1]) / 256

	stbdCurrent = Sensor(collectStbdCurrent)
	portCurrent = Sensor(collectPortCurrent)
	balancedVoltage = Sensor(collectBalancedVoltage)
	stbdVoltage = Sensor(collectStbdVoltage)
	portVoltage = Sensor(collectPortVoltage)
	temp = Sensor(collectTemp)


class Converter:
	deviceAddress = 0x37
	moboControl = machine.Pin('C2', machine.Pin.OUT)
	jetsonControl = machine.Pin('C3', machine.Pin.OUT)
	petierControl = machine.Pin('C1', machine.Pin.OUT)
	threeControl = machine.Pin('C0', machine.Pin.OUT)
	fiveControl = machine.Pin('C13', machine.Pin.OUT)
	twelveControl = machine.Pin('B0', machine.Pin.OUT)

	def __init__(self):
		while backplaneI2C.mem_read(1, deviceAddress, 0x0C)[0] & 0b00000010 != 0:
			pass
		# Operational mode 0 (includes temperature) and external vref
		backplaneI2C.mem_write([0b000], deviceAddress, 0x0B)
		# Set continuous conversion
		backplaneI2C.mem_write([1], deviceAddress, 0x07)
		# Disable unused channels
		backplaneI2C.mem_write([0b01000000], deviceAddress, 0x08)
		# Mask all interrupts
		backplaneI2C.mem_write([0xFF], deviceAddress, 0x03)
		# Start ADC and disable interrupts
		backplaneI2C.mem_write([1], deviceAddress, 0x00)

	def setMoboPower(power):
		moboControl.value(power)
	def setJetsonPower(power):
		jetsonControl.value(power)
	def setPeltierPower(power):
		peltierControl.value(power)
	def setThreePower(power):
		threeControl.value(power)
	def setFivePower(power):
		fiveControl.value(power)
	def setTwelvePower(power):
		twelveControl.value(power)

	def collectFiveCurrent():
		data = backplaneI2C.mem_read(2, deviceAddress, 0x20)
		return (((data[0] << 8) + data[1]) >> 4) * 10 / 4096 * 2.56 / 3.3
	def collectThreeCurrent():
		data = backplaneI2C.mem_read(2, deviceAddress, 0x21)
		return (((data[0] << 8) + data[1]) >> 4) * 10 / 4096 * 2.56 / 3.3
	def collectTwelveCurrent():
		data = backplaneI2C.mem_read(2, deviceAddress, 0x22)
		return (((data[0] << 8) + data[1]) >> 4) * 10 / 4096 * 2.56 / 3.3
	def collectTwelveVoltage():
		data = backplaneI2C.mem_read(2, deviceAddress, 0x23)
		return (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096 * (12.4 / 2.4)
	def collectFiveVoltage():
		data = backplaneI2C.mem_read(2, deviceAddress, 0x24)
		return (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096 * (18 / 8)
	def collectThreeVoltage():
		data = backplaneI2C.mem_read(2, deviceAddress, 0x25)
		return (((data[0] << 8) + data[1]) >> 4) * 2.56 / 4096 * (18 / 8)
	def collectTemp():
		data = backplaneI2C.mem_read(2, deviceAddress, 0x27)
		return ((data[0] << 8) + data[1]) / 256

	fiveVoltage = Sensor(collectFiveVoltage)
	threeVoltage = Sensor(collectThreeVoltage)
	twelveVoltage = Sensor(collectTwelveVoltage)
	fiveCurrent = Sensor(collectFiveCurrent)
	threeCurrent = Sensor(collectThreeCurrent)
	twelveCurrent = Sensor(collectTwelveCurrent)
	temp = Sensor(collectTemp)


class ESC():
	deviceAddress = 0x2F

	def __init__(self):
		while backplaneI2C.mem_read(1, deviceAddress, 0x0C)[0] & 0b00000010 != 0:
			pass
		# Operational mode 0 (includes temperature) and external vref
		backplaneI2C.mem_write([0b011], deviceAddress, 0x0B)
		# Set continuous conversion
		backplaneI2C.mem_write([1], deviceAddress, 0x07)
		# Disable unused channels
		backplaneI2C.mem_write([0b00000000], deviceAddress, 0x08)
		# Mask all interrupts
		backplaneI2C.mem_write([0xFF], deviceAddress, 0x03)
		# Start ADC and disable interrupts
		backplaneI2C.mem_write([1], deviceAddress, 0x00)

	def collectCurrents():
		currents = []
		for i in range(7):
			data = backplaneI2C.mem_read(2, deviceAddress, 0x20 + i)
			currents[i] = (((data[0] << 8) + data[1]) >> 4) * 10 / 4096

	currents = Sensor(collectCurrents)


BatteryBalancer = BatteryBalancer()
Converter = Converter()
ESC = ESC()