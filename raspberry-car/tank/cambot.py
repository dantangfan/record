# Imports
import webiopi

# Retrieve GPIO lib
GPIO = webiopi.GPIO

# -------------------------------------------------- #
# Constants definition                               #
# -------------------------------------------------- #

# Left motor GPIOs
L1=17  # GPIO0 

# Right motor GPIOs
R1=18 # GPIO1


# -------------------------------------------------- #
# Macro definition part                              #
# -------------------------------------------------- #

def go_forward():
    GPIO.output(L1, GPIO.LOW)
    GPIO.output(R1, GPIO.LOW)


#def go_backward():

def turn_left():
    GPIO.output(L1, GPIO.HIGH)
    GPIO.output(R1, GPIO.LOW)

def turn_right():
    GPIO.output(L1, GPIO.LOW)
    GPIO.output(R1, GPIO.HIGH)

def stop():
    GPIO.output(L1, GPIO.HIGH)
    GPIO.output(R1, GPIO.HIGH)
    
# -------------------------------------------------- #
# Initialization part                                #
# -------------------------------------------------- #

# Setup GPIOs
GPIO.setFunction(L1, GPIO.OUT)

GPIO.setFunction(R1, GPIO.OUT)


stop()

# -------------------------------------------------- #
# Main server part                                   #
# -------------------------------------------------- #


# Instantiate the server on the port 8000, it starts immediately in its own thread
server = webiopi.Server(port=8000, login="cambot", password="cambot")

# Register the macros so you can call it with Javascript and/or REST API

server.addMacro(go_forward)
#server.addMacro(go_backward)
server.addMacro(turn_left)
server.addMacro(turn_right)
server.addMacro(stop)

# -------------------------------------------------- #
# Loop execution part                                #
# -------------------------------------------------- #

# Run our loop until CTRL-C is pressed or SIGTERM received
webiopi.runLoop()

# -------------------------------------------------- #
# Termination part                                   #
# -------------------------------------------------- #

# Stop the server
server.stop()

# Reset GPIO functions
GPIO.setFunction(L1, GPIO.IN)
GPIO.setFunction(R1, GPIO.IN)


