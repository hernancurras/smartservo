# smartservo
A project to command autonomous sailboats rudder & sailwinch actuators by USB. 

# i. Components:
- Arduino (328 processor, or better)
- Sailwinch Driver Hbridge LM298 (it depends on the current that you want to handle, 2A@12v is enought for this mockup)
- Multi-turn potentiometer 10K ohms, Lineal (any value will work). [POT10K]
- OLED IC2 Screen SH1106, IC2 attached to A4/A5 SDA-SCL.
- Rudder Servo (Regular Futaba S3003 compatible will work.)

# ii. Scope:
Receive a String by Serial, at 9600bps and set a value for the sailwinch or the servo rudder.
Receive 3 Channels by RC-Receiver, and control the boat Manually.
CH#1 - Rudder
CH#3 - Sailwinch (Throttle on almost RC Controllers) 
CH#5 - Switch NAVMODEs (0 = Manual, Default), (1 = Autonomous, controlled by SerialPort)

# iii.USB Protocol: 

#### SetRudder(99)\n 
99 is an angle between 0-180 for a servo, 90 is the initial angle, centered.
RUDDER_MIN_ENDPOINT / RUDDER_MAX_ENDPOINT are mechanicall endopoints, by default, are set (45,135) it means 90 degrees freedom.

#### SetWinch(99)\n 
99 is a percentage between 0%-99% of the sailwinch travel.
The percentage feedback is a [POT10K] placed on the return pulley.
SAILWINCH_MIN_ENDPOINT / SAILWINCH_MAX_ENDPOINT are mechanicall endopoints, by default, are set (0,100) it means 100% of freedom.

# iv. In this release:
+ Read Receiver channel #1 (Sailwinch), channel #3 (Rudder), channel #5 AUX (Mode) with interruptions.
+ Set a Speed strategy for the sailwinch. 2 different speeds at WINCH_SLOW (15% of the target) -> WINCH_TOLERANCE (5% of the target stops the servo)
+ OLED IC2 U8GLIB_SSD1306_128X64 Screen to see outputs attached to A4-A5 (SDA-SCL)
+ Set CONST settings to setup the boat. (ENDPOINTS, PORTS)
+ Set MIN/MAX validation values received from serial
+ Set the BUILT-IN LED while actuators are working
+ 25Hz refresh rate interval rate for the Rudder. (intervalrudder = 40ms)
+ 5Hz refresh rate interval rate for the Winch. (intervalwinch = 200ms)
+ Code arrangement

# v. TO DO (Pending Milestones):
+ Rudder Potentiometer Feedback attached to A6 
+ Endpoints switchs were set.
+ Some functions must be created to optimize code


