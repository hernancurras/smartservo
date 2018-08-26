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

# iv. TO DO (Pending Milestones): 
+ No endpoints calculation or endpoints switchs were set.
+ Set a Speed strategy for the sailwinch
      Strategy -> if it is far away distance = abs(x0-xf), set 100% speed,
            if it is 20% closer, set 3/4 speed.
                  if it is 5% or less, stop the Winch.
+ Set led green/yellow/red, as a semaphore, to determine the Winch operation.
+ Review the 250ms (4Hz refresh rate) interval rate for the Rudder. (intervalrudder = 250)
+ Review the 500ms (2Hz refresh rate) interval rate for the Winch. (intervalwinch = 500)
