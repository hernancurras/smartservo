# smartservo
A project to command autonomous sailboats rudder & sailwinch actuators by USB. 

# i. Components:
- Arduino (328 processor, or better)
- Sailwinch Driver Hbridge LM298 (it depends on the current that you want to handle, 2A@12v is enought for this mockup)
- Multi-turn potentiometer 10K ohms, Lineal (any value will work). [POT10K]
- Rudder Servo (Regular Futaba S3003 compatible will work.

# ii. Scope:
Receive a String by Serial USB, and set a value for the sailwinch or the servo rudder.

# iii.USB Protocol: 

#### SetRudder(99)\n 
99 is an angle between 0-180 for a servo, 90 is the initial angle, centered.

#### SetWinch(99)\n 
99 is a percentage between 0%-99% of the sailwinch travel.
The percentage feedback is a [POT10K] placed on the return pulley.

# iv. In this release:
+ Read Receiver channel #1 (Sailwinch), channel #3 (Rudder), channel #5 AUX (Mode) with interruptions.
+ Set a Speed strategy for the sailwinch.
+ Set CONST settings to setup the boat. (ENDPOINTS, PORTS)
+ Set MIN/MAX validation values received from serial
+ Set the BUILT-IN LED while actuators are working
+ 50Hz refresh rate interval rate for the Rudder. (intervalrudder = 20ms)
+ 5Hz refresh rate interval rate for the Winch. (intervalwinch = 200ms)

# v. TO DO (Pending Milestones): 
+ Endpoints switchs were set.
+ Code Organization
+ Some functions must be created to reduce code


