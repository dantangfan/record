from wheel import *
import RPi.GPIO as GPIO
class car:
    wheels=[wheel('left'),wheel('right')]

    def __init__(self):
        GPIO.setmode(GPIO.BOARD)
    
    def forward(self):
        for wheel in car.wheels:
            wheel.forward()

    def back(self):
        for wheel in car.wheels:
            wheel.back()

    def left(self):
        car.wheels[0].forward()
        car.wheels[1].back()

    def right(self):
        car.wheels[0].back()
        car.wheels[1].forward()

    def stop(self):
        for wheel in car.wheels:
            wheel.stop()


