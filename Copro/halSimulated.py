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



class BatteryBalancer:
	deviceAddress = 0x1F

	def collectStbdCurrent():
		return random.uniform(0,40)
	def collectPortCurrent():
		return random.uniform(0,40)
	def collectBalancedVoltage():
		return random.uniform(19,21)
	def collectStbdVoltage():
		return random.uniform(19,21)
	def collectPortVoltage():
		return random.uniform(19,21)
	def collectTemp():
		return random.uniform(0,110)

	stbdCurrent = Sensor(collectStbdCurrent)
	portCurrent = Sensor(collectPortCurrent)
	balancedVoltage = Sensor(collectBalancedVoltage)
	stbdVoltage = Sensor(collectStbdVoltage)
	portVoltage = Sensor(collectPortVoltage)
	temp = Sensor(collectTemp)


class Converter:
	deviceAddress = 0x37

	def setMoboPower(self, power):
		pass
	def setJetsonPower(self, power):
		pass
	def setPeltierPower(self, power):
		pass
	def setThreePower(self, power):
		pass
	def setFivePower(self, power):
		pass
	def setTwelvePower(self, power):
		pass
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
		return random.uniform(0,110)

	fiveVoltage = Sensor(collectFiveVoltage)
	threeVoltage = Sensor(collectThreeVoltage)
	twelveVoltage = Sensor(collectTwelveVoltage)
	fiveCurrent = Sensor(collectFiveCurrent)
	threeCurrent = Sensor(collectThreeCurrent)
	twelveCurrent = Sensor(collectTwelveCurrent)
	temp = Sensor(collectTemp)


class ESC():
	deviceAddress = 0x2F

	def collectCurrents():
		currents = []
		for i in range(7):
			currents[i] = random.uniform(0,10)

	currents = Sensor(collectCurrents)


BatteryBalancer = BatteryBalancer()
Converter = Converter()
ESC = ESC()