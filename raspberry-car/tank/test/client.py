from socket import *

s = socket()
host = "127.0.0.1"
port = 1234

s.connect((host,port))
s.send("hehe")

