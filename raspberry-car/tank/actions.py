from car import *
#from human_infrared import *
from distance import *
from self_move import *
import clean


def action(com):

    #when control the car
    if com=="forward":
        a = car()
        a.forward()
    elif com=="back":
        a = car()
        a.back()
    elif com=="left":
        a = car()
        a.left()
    elif com=="right":
        a = car()
        a.right()
    elif com=="stop":
        a = car()
        a.stop()
    #when something pass
   # elif com=="human_infrared":
   #     human_infrared()
    #when measure the distance
    elif com=="distence":
        dis = distence()
        return dis
    #when the car moves itself
    elif com=="self_move":
        self_move()
    #clean all
    elif com=="clean":
        clean.clean()
        
