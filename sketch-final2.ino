#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Servo setup ------------------------------------------
Servo servo;
int angle = 1;

// IR setup ------------------------------------------
// IR dectectors IRLeft_Value, IRFront_Value, IRRight_Value
int analogPin1 = A1;
int IRLeft_Value =0;

int analogPin2 = A2;
int IRFront_Value =0;

int analogPin3 = A3;
int IRRight_Value =0;
// Boolean array to check if IR sensors are blocked
bool[] IRBlocked = {false, false, false};

// Motor setup ------------------------------------------
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(4);
Adafruit_DCMotor *myMotor3 = AFMS.getMotor(3);



void setup() {
  // Fire up the serial monitor
  Serial.begin(9600);
  //Initialize the servo with IR sensor attached.
  initServo();
  // Initialize the motors
  initMotors();
  
}

void loop() {

  // Check if there is a crosswalk with the servo
  //bool sd = VerifyCrosswalkWithServo();
  checkIRSensors();
  delay(3000); 
  resetIRSensors();
}

void MoveForward(){
  myMotor->run(FORWARD);
  myMotor3->run(BACKWARD);
}

// Initialize the motors
void initMotors(){
  AFMS.begin();
  // Initialize the DC motors, start with the Adafruit_MotorShield
  AFMS.begin();

  myMotor->setSpeed(150);
  myMotor->run(FORWARD);
  myMotor->run(RELEASE);

  myMotor3->setSpeed(150);
  myMotor3->run(BACKWARD);
  myMotor3->run(RELEASE);  
}


void MotorTurnLeft(uint8_t speed){
  myMotor->run(BACKWARD); // left backwards
  myMotor3->run(FORWARD);// right forward
  myMotor->setSpeed(speed);
  myMotor3->setSpeed(speed);
}

// This code is to move the motors forward
void MotorForward(uint8_t i){
 myMotor->run(FORWARD);
  myMotor3->run(BACKWARD);
   myMotor->setSpeed(i);
    myMotor3->setSpeed(i);
}



// This code is to control the servo 
void initServo(){
  servo.attach(10);
  servo.write(angle);
}

// This code is to control the servo by turning it out until it reaches 46 degrees, 
// then checks the IR sensor to see if it is blocked. If it is blocked, it will turn back return true
// showing there is a crosswalk. If it is not blocked, it will 
// return false showing there is no crosswalk.
bool VerifyCrosswalkWithServo(){  
  for(angle=0; angle < 46; angle += 2){
    servo.write(angle);
    if(angle > 16){

      IRFront_Value = analogRead(analogPin2);
      if(IRFront_Value < 100){
        Serial.println("IR Sensor is blocked");
        return true;   
    }
    delay(15);
  }

  for(angle=46; angle > 0; angle -=2){
    servo.write(angle);
    delay(15);
  }

  // IR is not triggered, return false
  return false;
}



// Check the IR sensors to see if they are blocked, 
//then set the IRBlocked array to true if one or many are blocked
void checkIRSensors(){
  IRLeft_Value = analogRead(analogPin1);
  IRFront_Value = analogRead(analogPin2);
  IRRight_Value = analogRead(analogPin3);
  Serial.println(IRLeft_Value);
  Serial.println(IRFront_Value);
  Serial.println(IRRight_Value);

  if(IRLeft_Value < 100){
    Serial.println("IR Left Sensor is blocked");
    IRBlocked[0]=true;
  }
  if(IRFront_Value < 100){
    Serial.println("IR Front Sensor is blocked");
    IRBlocked[1]=true;
  }
  if(IRRight_Value < 100){
    Serial.println("IR Right Sensor is blocked");
    IRBlocked[2]=true;
  }
}

// Reset the IR sensors to false
void resetIRSensors(){
  for(int i=0; i<3; i++){
    IRBlocked[i]=false;
  }
}
 
