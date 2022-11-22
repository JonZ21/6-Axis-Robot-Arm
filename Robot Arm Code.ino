/*
  6-Axis Arduino Robot Arm V1.0
  Jonathan Zhou

  Controlled by a single 2-axis joystick and three modes
  First mode - base and shoulder
  Second mode - elbow and wristRoll
  Third mode - wristPitch and claw

  Future Changes:
  - Make better controls (add another joystick)
  - Make Bluetooth controller
  - Make macros for common tasks (reset to default position)
  - webApp integration 


  A button changes inbetween the modes
*/


#include <SoftwareSerial.h>
#include <Servo.h>

//defining servos and constants
Servo base;
Servo shoulder;
Servo elbow;
Servo wristRoll;
Servo wristPitch;
Servo claw;
// joystick
const int JOYSTICK_X_PORT = A0; // x axis analog input port
const int JOYSTICK_Y_PORT = A1; // y axis analog input port
const int JOYSTICK_BUTTON_PORT = 2;
const int BUTTON_PORT = 3;
const int BUTTON_S_UP_PORT = 12;
const int BUTTON_S_DOWN_PORT = 13;


int xPosition = 0;
int yPosition = 0;

// servo control
 //mapped x and y values (0,180)
int mapX = 0;
int mapY = 0;

int index = 0; // index for switching between modes
int buttonState = 0; 
int bUState = 0;
int bDState = 0;
int joystickState = 0;
double speedRatio = 0.0;
bool bUPressing = false;
bool bDPressing = false;
bool pressing = false; 
bool joystickPressing = false;

//arrays containing the x and y values for each mode
int xa[3] = {90,90,90}; 
int ya[3] = {90,90,90};

const int JOYSTICK_DEADZONE = 20; 
const int JOYSTICK_MID_VALUE = 512;
const int JOYSTICK_MAX_VALUE = 1023;
const int JOYSTICK_RATE = 5; // a higher joystick value means the joystick is more sensitive
void setup() {
 
  Serial.begin(9600); 

  //Joystick 
  pinMode(JOYSTICK_X_PORT, INPUT);
  pinMode(JOYSTICK_Y_PORT, INPUT);
  pinMode(JOYSTICK_BUTTON_PORT, INPUT_PULLUP); 

  //servos
  base.attach(5);
  shoulder.attach(6);
  elbow.attach(7);
  wristRoll.attach(8);
  wristPitch.attach(9);
  claw.attach(10);

  // Robot arm initial positions
  base.write(90);
  shoulder.write(90);
  elbow.write(90);
  wristRoll.write(90);
  wristPitch.write(90);
  claw.write(90); 
}

void loop() {
  xPosition = analogRead(JOYSTICK_X_PORT);
  yPosition = analogRead(JOYSTICK_Y_PORT);
  buttonState = digitalRead(BUTTON_PORT);
  joystickState = digitalRead(JOYSTICK_BUTTON_PORT);
  

  if(abs(JOYSTICK_MID_VALUE - xPosition) > JOYSTICK_DEADZONE) {
    mapX = map(xPosition, 0, JOYSTICK_MAX_VALUE,-JOYSTICK_RATE,JOYSTICK_RATE); // map the value of x from 0 - 1023(JOYSTICK_MAX_VALUE) to +- Joystick rate
    xa[index] += mapX;

    // keep the servo value between 0 and 180
    if(xa[index] > 180)
      xa[index] = 180;
    if (xa[index] < 0)
      xa[index] = 0;
  }

  if(abs(JOYSTICK_MID_VALUE - yPosition) > JOYSTICK_DEADZONE){
  mapY = map(yPosition, 0, JOYSTICK_MAX_VALUE,-JOYSTICK_RATE,JOYSTICK_RATE); // map the value of y from 0 - 1023(JOYSTICK_MAX_VALUE) to +- Joystick rate
  ya[index] += mapY;

  // keep the servo value between 0 and 180
  if(ya[index] > 180)
    ya[index]= 180;
  if(ya[index] < 0)
    ya[index] = 0;

  }

  if(joystickState == HIGH) // reset
    joystickPressing = true;
  if(joystickPressing && joystickState != HIGH)
  {
    xa[0] = xa[1] = xa[2] = 90;
    ya[0] = ya[1] = ya[2] = 90;
    base.write(xa[1]);  
    shoulder.write(ya[1]);
    elbow.write(xa[1]);  
    wristRoll.write(ya[1]);
    wristPitch.write(xa[1]);  
    claw.write(ya[1]);
    joystickPressing = false;
  }

  if(buttonState == HIGH)
    pressing = true; 

  if(pressing && buttonState != HIGH) // wait until the button is released to change modes
  {
    index++; // update modes
    pressing = false;
    if(index == 3) // reset to 0 once cycled through 3 modes
      index = 0;
  }


  //move servos
  if(index == 0) {
    base.write(xa[index]);  
    shoulder.write(ya[index]);
  }
  else if(index == 1) {
    elbow.write(xa[index]);  
    wristRoll.write(ya[index]);
  }
  else {
    wristPitch.write(xa[index]);  
    claw.write(ya[index]);
  }

  delay(50);
}
