from socket import *
from hehe import *
s=socket()
host = "127.0.0.1"
port = 1234

commonds={"hehe":hehe()}
s.bind((host,port))

s.listen(5)
while 1:
    c,addr = s.accept()

    he = c.recv(1024).replace('\n','')
    if he == "hehe":
        print "yes"
        hehe()
        commonds[he]
    c.close()
