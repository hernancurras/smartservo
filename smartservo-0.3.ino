// Get a String by Serial USB, & run a single LM298 to command the Sailwinch, and a Servo for rudder direction.
// A3 is a potentiometer multiturn of 10k lineal
// SetRudder(99)\n (99 is an angle between 0-180 for a servo, 90 is the initial angle, centered.)
// SetWinch(99)\n (99 is a percentage between 0%-99% of the sailwinch travel)
// https://github.com/hernancurras/smartservo
//

#include <Servo.h>

Servo rudder;  // create servo object to control a servo

String inputString = "";         // a String to hold incoming data
String value = "";

bool stringComplete = false;     // whether the string is complete
int xf, xo, distance = 0;        // Sailwinch
int angle = 90, newangle = 90;   // Rudder

// MECANICAL ENDPOINTS
int RUDDER_MIN_ENDPOINT = 45;     // Anly value from (0-180). Default 0
int RUDDER_MAX_ENDPOINT = 135;   // Anly value from (0-180). Default 180
int SAILWINCH_MIN_ENDPOINT = 0;   // Anly value from (0-100). Default 0
int SAILWINCH_MAX_ENDPOINT = 100;  // Anly value from (0-100). Default 100

int const MAXSPEED = 255;        // PWM Max Speed with 12v power
int const WINCH_TOLERANCE = 5;   // 5% of tolerance between distance(xo-xf) stops the sailwinch
int const WINCH_SLOW = 15;       // 15% of distance(xo-xf) sailwinch slows the speed

// Sailwinch Driver ports ENA (PWM), IN1, IN2 (Direction)
int const ENA = 6;
int const IN1 = 4;
int const IN2 = 5;

int const RUDDERPORT = 3; 

unsigned long intervalwinch = 200;    // Miliseconds to wait the Winch
unsigned long intervalrudder = 40;   // Miliseconds to wait the rudder servo turn (response latency) 200ms -> 5Hz

unsigned long previousMillisrudder, previousMilliswinch = 0; // millis() returns an unsigned long.

unsigned long counter_1, counter_2, counter_3, counter_4, current_count; //We create variables for the time width values of each PWM input signal
byte last_CH1_state, last_CH2_state, last_CH3_state, last_CH4_state; //We create 4 variables to stopre the previous value of the input signal (if LOW or HIGH)

int input_RUDDER;     //In my case channel 1 of the receiver and pin D8 of arduino
int input_MODE;       //In my case channel 2 of the receiver and pin D9 of arduino
int input_SAILWINCH;  //In my case channel 3 of the receiver and pin D10 of arduino

int NAVMODE = 0;      //NAVMODE = 0 (Manual, Default), 1 (Auto)

void setup() {
   
  PCICR |= (1 << PCIE0);    //enable PCMSK0 scan                                                 
  PCMSK0 |= (1 << PCINT0);  //Set pin D8 trigger an interrupt on state change. 
  PCMSK0 |= (1 << PCINT1);  //Set pin D9 trigger an interrupt on state change.                                             
  PCMSK0 |= (1 << PCINT2);  //Set pin D10 trigger an interrupt on state change. 

  rudder.attach(RUDDERPORT);  // attaches the servo object to a Digital port
  
  pinMode (ENA, OUTPUT);
  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Starting Smart Servo Unit...[OK]");
 
  inputString.reserve(50);    // reserve 50 bytes for the inputString:
}

void ForwardWinch (int speed)
{
 digitalWrite (IN1, HIGH);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, speed); 
}

void BackwardWinch (int speed)
{
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, HIGH);
 analogWrite (ENA, speed);
}

void StopWinch ()
{
 digitalWrite (IN1, LOW);
 digitalWrite (IN2, LOW);
 analogWrite (ENA, 0);
}

void MoveSailwinch(int origin, int destination)
{
  // Strategy -> if it is far away distance = abs(x0-xf), set 100% speed, 
  //                    if it is WINCH_SLOW% closer, set maxspeed-150 (255-150=105 -> 40% speed)
  //                          if it is WINCH_TOLERANCE% or less, stop the winch.

  int distance = 0;
  int sailwinchspeed = 0;

  distance = abs(origin-destination);

  if (distance >= WINCH_TOLERANCE && distance < WINCH_SLOW) sailwinchspeed = (MAXSPEED - 150); 
  else sailwinchspeed = MAXSPEED;
  
  if ( distance > WINCH_TOLERANCE) {
    if (origin < destination) {
      ForwardWinch(sailwinchspeed);
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else {
      BackwardWinch(sailwinchspeed);
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
  else {
    StopWinch();
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void loop() {

  int firstClosingBracket, secondClosingBracket = 0;
  int manual_RUDDER = 90, manual_SAILWINCH = 0;
  unsigned long currentMillis = millis(); // save the current time

  if (input_MODE < 1500) {
    NAVMODE = 0;
  }
  else {
    NAVMODE = 1;
  }

  manual_SAILWINCH = map(input_SAILWINCH, 1000, 2000, SAILWINCH_MIN_ENDPOINT, SAILWINCH_MAX_ENDPOINT);
  manual_RUDDER = map(input_RUDDER, 1000, 2000, RUDDER_MIN_ENDPOINT, RUDDER_MAX_ENDPOINT); 

// print the string when a newline arrives:
  if (stringComplete) {
//  Serial.println(inputString);
  
  if (inputString.startsWith("SetRudder(") || inputString.startsWith("SetWinch(")) {
  firstClosingBracket = inputString.indexOf('(') + 1;
  secondClosingBracket = inputString.indexOf(')');
  }
 
  if (inputString.startsWith("SetWinch(")) {    
    
    value = (inputString.substring(firstClosingBracket,secondClosingBracket));    
    xf = value.toInt();

    if ((xf > SAILWINCH_MAX_ENDPOINT) || (xf < SAILWINCH_MIN_ENDPOINT)) {
    xf = xo;
    Serial.println("ERROR: The newwinch (xf) position received is out of range");
    }

    Serial.print("New Instruction set Sailwinch from (xo): ");Serial.print(xo);
    Serial.print(" to (xf): ");Serial.println(xf);
  }

  if (inputString.startsWith("SetRudder(")) {

    value = (inputString.substring(firstClosingBracket,secondClosingBracket));   
    newangle = value.toInt();

    if ((newangle > RUDDER_MAX_ENDPOINT) || (newangle < RUDDER_MIN_ENDPOINT)) {
    newangle = angle;
    Serial.println("ERROR: The newangle received is out of range");
    }
  
    Serial.print("New Instruction set Rudder Angle (angle): ");Serial.print(angle);
    Serial.print(" to (newangle): ");Serial.println(newangle); 
  }
   
    // clear the string to receive a new message    
    inputString = "";
    value = "";
    stringComplete = false;
  }

  //Set Winch  
  if ((unsigned long)(currentMillis - previousMilliswinch) >= intervalwinch) {

  xo = map(analogRead(A3), 0, 1024, 0, 100);

    if (NAVMODE == 0) {
      MoveSailwinch(xo,manual_SAILWINCH);
    }

    if (NAVMODE == 1) {
      MoveSailwinch(xo,xf); 
      //you can calculate the time here to set a new intervalrudder, considering dist(angle,newangle) to wait a new command to move the servo
      //probably not needed to prevent commands latency on the rudder.    
    }
     
      previousMilliswinch = millis();
      //you can calculate the time here to set a new intervalwinch, considering dist(xo,xf) to wait a new command to move the sailwinch
      //is a must since the sailwinch travel are large.   
    }


  //Set Rudder
  if ((unsigned long)(currentMillis - previousMillisrudder) >= intervalrudder) {
    
    if (NAVMODE == 0) {
      digitalWrite(LED_BUILTIN, HIGH);
      rudder.write(manual_RUDDER);
    }

    if (NAVMODE == 1) {
      digitalWrite(LED_BUILTIN, HIGH);
      rudder.write(newangle);
      //you can calculate the time here to set a new intervalrudder, considering dist(angle,newangle) to wait a new command to move the servo
      //probably not needed to prevent commands latency on the rudder.    
    }

    digitalWrite(LED_BUILTIN, LOW);
    previousMillisrudder = millis();
    //you can calculate the time here to set a new intervalrudder, considering dist(angle,newangle) to wait a new command to move the servo
    //probably not needed to prevent commands latency on the rudder.

    angle = newangle;
           
  }

  Serial.print("MODE: ");
  Serial.print(NAVMODE);

  Serial.print(" Manual-RUDDER: ");
  Serial.print(manual_RUDDER);

  Serial.print(" Auto-RUDDER: ");
  Serial.print(angle);

  Serial.print(" Sailwinch-Position: ");
  Serial.print(xo);

  Serial.print(" Manual-Sailwinch: ");
  Serial.print(manual_SAILWINCH);

  Serial.print(" Auto-Sailwinch: ");
  Serial.println(xf);

}

//------------------------------------------------------------------------------------------------------------------------------------------------------------

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can, do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

//This is the interruption routine

ISR(PCINT0_vect){
  
  current_count = micros();

  ///////////////////////////////////////Channel 1 - RUDDER
  if(PINB & B00000001){                              //We make an AND with the pin state register, We verify if pin 8 is HIGH???
    if(last_CH1_state == 0){                         //If the last state was 0, then we have a state change...
      last_CH1_state = 1;                            //Store the current state into the last state for the next loop
      counter_1 = current_count;                     //Set counter_1 to current value.
    }
  }
  else if(last_CH1_state == 1){                      //If pin 8 is LOW and the last state was HIGH then we have a state change      
    last_CH1_state = 0;                              //Store the current state into the last state for the next loop
    input_RUDDER = current_count - counter_1;   //We make the time difference. Channel 1 is current_time - timer_1.
  }

  ///////////////////////////////////////Channel 2 - MODE (AUTO-MANUAL)
  if(PINB & B00000010 ){                             //pin D9 -- B00000010                                              
    if(last_CH2_state == 0){                                               
      last_CH2_state = 1;                                                   
      counter_2 = current_count;                                             
    }
  }
  else if(last_CH2_state == 1){                                           
    last_CH2_state = 0;                                                     
    input_MODE = current_count - counter_2;                             
  }

  ///////////////////////////////////////Channel 3 - SAILWINCH (Throttle, mode B receiver)
  if(PINB & B00000100 ){                             //pin D10 - B00000100                                         
    if(last_CH3_state == 0){                                             
      last_CH3_state = 1;                                                  
      counter_3 = current_count;                                               
    }
  }
  else if(last_CH3_state == 1){                                             
    last_CH3_state = 0;                                                    
    input_SAILWINCH = current_count - counter_3;                            

  }
 
}
 
