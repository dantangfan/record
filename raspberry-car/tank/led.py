import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BOARD)
GPIO.cleanup()
GPIO.setup(3,GPIO.OUT)
for i in range(3):
        GPIO.output(3,GPIO.HIGH)
        time.sleep(0.5)
        GPIO.output(3,GPIO.LOW)
        time.sleep(0.5)
        GPIO.setup(3,GPIO.IN)
        print i
        GPIO.setup(3,GPIO.OUT)
GPIO.cleanup()

                
