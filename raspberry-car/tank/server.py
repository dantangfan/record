from actions import *
from socket import *
import sys
import time
import RPi.GPIO as GPIO

host = "192.168.1.10"
port = 8888
s = socket()

s.bind((host,port))
s.listen(5)
print "listen on port 8888"

while 1:
    conn, addr = s.accept()
    print "connected by:",addr

    while 1:
        command = conn.recv(1024).replace('\n','')
        #print command
        if not command: break
        #this command to test the distance 
        elif command=="distance":
            dis=action(command)
            conn.send(dis)
        else:
            action(command)
    #action("clean")
    #GPIO.cleanup()
    conn.close()

    
