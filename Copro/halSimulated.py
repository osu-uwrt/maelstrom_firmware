import random
import time

def getTime():
    return int(time.time()*1000)

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

class Pin:
	def on(self):
		pass
	def off(self):
		pass
	def value(self, a=None):
		if a is None:
			return 1



blueLed = Pin()
greenLed = Pin()

class BatteryBalancer:
	deviceAddress = 0x1F

	def collectStbdCurrent():
		return random.uniform(0,35)
	def collectPortCurrent():
		return random.uniform(0,35)
	def collectBalancedVoltage():
		return random.uniform(19,21)
	def collectStbdVoltage():
		return random.uniform(19,21)
	def collectPortVoltage():
		return random.uniform(19,21)
	def collectTemp():
		return random.uniform(0,70)

	stbdCurrent = Sensor(collectStbdCurrent)
	portCurrent = Sensor(collectPortCurrent)
	balancedVoltage = Sensor(collectBalancedVoltage)
	stbdVoltage = Sensor(collectStbdVoltage)
	portVoltage = Sensor(collectPortVoltage)
	temp = Sensor(collectTemp)

BatteryBalancer = BatteryBalancer()

class Converter:
	deviceAddress = 0x37

	moboPower = Pin()
	jetsonPower = Pin()
	peltierPower = Pin()
	threePower = Pin()
	fivePower = Pin()
	twelvePower = Pin()

	def collectFiveCurrent():
		return random.uniform(0,10)
	def collectThreeCurrent():
		return random.uniform(0,10)
	def collectTwelveCurrent():
		return random.uniform(0,10)
	def collectTwelveVoltage():
		return random.uniform(0,21)
	def collectFiveVoltage():
		return random.uniform(0,21)
	def collectThreeVoltage():
		return random.uniform(0,21)
	def collectTemp():
		return random.uniform(0,70)

	fiveVoltage = Sensor(collectFiveVoltage)
	threeVoltage = Sensor(collectThreeVoltage)
	twelveVoltage = Sensor(collectTwelveVoltage)
	fiveCurrent = Sensor(collectFiveCurrent)
	threeCurrent = Sensor(collectThreeCurrent)
	twelveCurrent = Sensor(collectTwelveCurrent)
	temp = Sensor(collectTemp)

Converter = Converter()

class ESC():
	deviceAddress = 0x2F
	thrusts = [1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500]

	def collectCurrents():
		current_vals = []
		for i in range(8):
			current_vals.append(int(random.uniform(0,10)*25))
		return current_vals

	currents = Sensor(collectCurrents)

ESC = ESC()

class StatusBoard():
	screenAddress = 0x78

	def write(self, text):
		print(text)

Status = StatusBoard()

class DepthSensor():
	deviceAddress = 0x76
	_fluidDensity = 997
	_pressure = 0

	def pressure(self):
		return self._pressure
		
	def temperature(self):
		degC = self._temperature / 100.0
		return degC
		
	# Depth relative to MSL pressure in given fluid density
	def depth(self):
		return (random.uniform(0, 2))

Depth = DepthSensor()

killSwitch = Pin()
switch1 = Pin()
switch2 = Pin()
switch3 = Pin()
switch4 = Pin()
resetSwitch = Pin()