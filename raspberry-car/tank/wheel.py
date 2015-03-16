import RPi.GPIO as GPIO
class wheel:
    pins = {'left':[18,16],'right':[26,24]}
	
    def __init__(self,name):
		self.name = name
		self.pin = wheel.pins[self.name]
		GPIO.setmode(GPIO.BOARD)
		GPIO.setup(self.pin[0],GPIO.OUT)
		GPIO.setup(self.pin[1],GPIO.OUT)
		self.stop()

    def forward(self):
		GPIO.output(self.pin[0],GPIO.HIGH)
		GPIO.output(self.pin[1],GPIO.LOW)
		
    def back(self):
		GPIO.output(self.pin[0],GPIO.LOW)
		GPIO.output(self.pin[1],GPIO.HIGH)
		
    def stop(self):
		GPIO.output(self.pin[0],GPIO.LOW)
		GPIO.output(self.pin[1],GPIO.LOW)
    
    def __del__(self):
        pass
        #GPIO.cleanup()
