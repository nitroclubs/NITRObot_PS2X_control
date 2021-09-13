#include "PS2X_lib.h"  //for v1.6

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
 
#define PS2_DAT     4
#define PS2_CMD     7
#define PS2_SEL     8
#define PS2_CLK     11

#define LEFT_FOR 9 // PWMB
#define LEFT_BACK 5  // DIRB  ---  left
#define RIGHT_FOR 6  // PWMA
#define RIGHT_BACK 10  // DIRA  ---  right
int xAxis,yAxis;

int speedLeft = 255; 
int speedRight = 255; 


/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
//#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you connect the controller, 
//or call config_gamepad(pins) again after connecting the controller.

int error = 0;
byte type = 0;
byte vibrate = 0;

// Reset func 
void (* resetFunc) (void) = 0;

void setup(){

  pinMode(LEFT_FOR, OUTPUT);
  pinMode(LEFT_BACK, OUTPUT);
  pinMode(RIGHT_FOR, OUTPUT);
  pinMode(RIGHT_BACK, OUTPUT);
  Serial.begin(9600);
  
  delay(500);  //added delay to give wireless ps2 module some time to startup, before configuring it
   
  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if(error == 0){
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
	if (pressures)
	  Serial.println("true ");
	else
	  Serial.println("false");
	Serial.print("rumble = ");
	if (rumble)
	  Serial.println("true)");
	else
	  Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }  
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  
  type = ps2x.readType(); 
  switch(type) {
    case 0:
      Serial.println("Unknown Controller type found ");
      break;
    case 1:
      Serial.println("DualShock Controller found ");
      break;
    case 2:
      Serial.println("GuitarHero Controller found ");
      break;
	case 3:
      Serial.println("Wireless Sony DualShock Controller found ");
      break;
   }
   stopMoving();
}

void loop() {
   
  if(error == 1){ //skip loop if no controller found
    resetFunc();
  }
  
  if(type == 2){ //Guitar Hero Controller
    ps2x.read_gamepad();          //read controller 
   
 
    if(ps2x.Button(PSB_START))         // press the "START" button
      Serial.println("Start is being held");
   
  }
  else { //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
      

//----------------------------Stick Values----------------------------------
  yAxis= map(ps2x.Analog(PSS_RY),255,0,-255,255); 
  xAxis= map(ps2x.Analog(PSS_RX),255,0,255,-255);
  
 
  Serial.print(yAxis);
  Serial.print("---");
  Serial.println(xAxis);

 if ((xAxis>-100)&(xAxis<100)&(yAxis>100)) moveForward();

 else if ((xAxis>-100)&(xAxis<100)&(yAxis<-100)) moveBackward();

 else if ((yAxis>-100)&(yAxis<100)&(xAxis<-100)) turnLeft(); 

 else if  ((yAxis>-100)&(yAxis<100)&(xAxis>100))turnRight();

 else if  ((yAxis>100)&(xAxis>100)) moveForwardRight();

  else if  ((yAxis>100)&(xAxis<-100)) moveForwardLeft();

  else if  ((yAxis<-100)&(xAxis>100)) moveBackwardRight();

  else if  ((yAxis<-100)&(xAxis<-100)) moveBackwardLeft();  

 else  stopMoving();
  }
  delay(50);  
}


//==================================== FUNCTIONS =====================================================

void moveForward() // Move forward
{
  analogWrite(LEFT_FOR, abs(speedLeft));
  analogWrite(LEFT_BACK, LOW);
  analogWrite(RIGHT_FOR, abs(speedRight));
  analogWrite(RIGHT_BACK, LOW);
}

void moveBackward() // Move backward
{
  analogWrite(LEFT_FOR, LOW);
  analogWrite(LEFT_BACK, abs(speedLeft));
  analogWrite(RIGHT_FOR, LOW);
  analogWrite(RIGHT_BACK, abs(speedRight));
}

void turnLeft() // Turn Left
{
  analogWrite(LEFT_FOR, LOW);
  analogWrite(LEFT_BACK, speedLeft);
  analogWrite(RIGHT_FOR, speedRight);
  analogWrite(RIGHT_BACK, LOW);
}

void turnRight() // Turn Right
{
  analogWrite(LEFT_FOR, speedRight);
  analogWrite(LEFT_BACK, LOW);
  analogWrite(RIGHT_FOR, LOW);
  analogWrite(RIGHT_BACK, speedRight);
}

void stopMoving() // Stop movement
{
  analogWrite(LEFT_FOR, HIGH);
  analogWrite(LEFT_BACK, HIGH);
  analogWrite(RIGHT_FOR, HIGH);
  analogWrite(RIGHT_BACK, HIGH);
}
void moveForwardLeft() // Move forward
{
  analogWrite(LEFT_FOR, 0);
  analogWrite(LEFT_BACK, LOW);
  analogWrite(RIGHT_FOR, abs(speedRight));
  analogWrite(RIGHT_BACK, LOW);
}
void moveForwardRight() // Move forward
{
  analogWrite(LEFT_FOR, abs(speedLeft));
  analogWrite(LEFT_BACK, LOW);
  analogWrite(RIGHT_FOR, 0);
  analogWrite(RIGHT_BACK, LOW);
}
void moveBackwardLeft() // Move backward
{
  analogWrite(LEFT_FOR, LOW);
  analogWrite(LEFT_BACK, 0);
  analogWrite(RIGHT_FOR, LOW);
  analogWrite(RIGHT_BACK, abs(speedRight));
}

void moveBackwardRight() // Move backward
{
  analogWrite(LEFT_FOR, LOW);
  analogWrite(LEFT_BACK, abs(speedLeft));
  analogWrite(RIGHT_FOR, LOW);
  analogWrite(RIGHT_BACK, 0);
}
