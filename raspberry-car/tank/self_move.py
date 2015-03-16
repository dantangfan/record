from car import *
import RPi.GPIO as GPIO

def self_move():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(3,GPIO.IN)
    while 1:
        a=car()
        a.forward()
        while GPIO.input(3)==1:
            a.left()

    GPIO.cleanup()
