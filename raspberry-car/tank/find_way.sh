#!/bin/sh
#filename:find_way.sh

while [ '1'='1' ]
do
	#test the distance toward
	sudo python /home/pi/tank/ultrasonic_1.py
	read line < distance.txt
	dis=${line}
	
	#if if there is something get in the way,turnleft,then turn right
	if [ ${dis} -lt 15 ]; 
	then
		#led to warning
		sudo python /home/pi/tank/led.py
		#turn
		#sudo python /home/pi/tank/ultrasonic_2.py
	fi
done
