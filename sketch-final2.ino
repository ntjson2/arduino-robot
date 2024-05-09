#include <Servo.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <LiquidCrystal_I2C.h>

// Ultrasonic setup ------------------------------------------
const int trigPin = 9;
const int echoPin = 10;
long objDistance[] = {0,0}; // For comparing the current distance to the previous distance
// The maximum distance the object can be from the sensor to be considered a verifiable object
long maxObjDistance = 100; 
long minObjDistance = 10;
bool stoppedFromObject = false; // Boolean to check if the object is too close to the sensor


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
int analogPin4 = A4;
int IRBackRight_Value =0;

// Int array to check IR sensor data, yellow vs white vs black, etc
int IRBlocked[] = {0, 0, 0, 0};

// Motor setup ------------------------------------------
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motorLeft = AFMS.getMotor(4);
Adafruit_DCMotor *motorRight = AFMS.getMotor(3);
// Direction of the motors, Forward or Backward, 
//Forward is the default, Forward = true, Backward = false
bool isForward = true;
// Motor speed, default is 150
int motorSpeed = 150;


// Function to initialize the ultrasound sensor
void initUltrasound() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}


// Initialize the motors
void initMotors(){
  // Initialize the DC motors, start with the Adafruit_MotorShield
  AFMS.begin();

  motorLeft->setSpeed(motorSpeed);
  motorLeft->run(FORWARD);
  motorLeft->run(RELEASE);

  motorRight->setSpeed(motorSpeed);
  motorRight->run(BACKWARD);
  motorRight->run(RELEASE);  
}


// This code is to control the servo 
void initServo(){
  servo.attach(10);
  servo.write(angle);
}


void setup() {
  // Fire up the serial monitor
  Serial.begin(9600);
  // Initialize the ultrasound sensor
  initUltrasound();
  //Initialize the servo with IR sensor attached.
  initServo();
  // Initialize the motors
  initMotors();  
}

void loop() {

  // Check if there is a crosswalk with the servo
  //bool sd = VerifyCrosswalkWithServo();
  checkIRSensors();
  //delay(3000); 
  resetIRSensors();
  CheckUltraSound();

}




void MotorTurnLeft(uint8_t speed){
  motorLeft->run(FORWARD); // left backwards
  motorRight->run(BACKWARD);// right forward
  motorLeft->setSpeed(0);
  motorRight->setSpeed(speed);
}


void MotorTurnRight(uint8_t speed){
  motorLeft->run(FORWARD); // left backwards
  motorRight->run(BACKWARD);// right forward
  motorLeft->setSpeed(speed);
  motorRight->setSpeed(0);
}

// This code is to move the motors forward
void MotorForward(uint8_t i){
  motorLeft->run(FORWARD);
  motorRight->run(BACKWARD);
  motorLeft->setSpeed(i);
  motorRight->setSpeed(i);
}

// This code is to move the motors forward
void MotorBackward(uint8_t i){
  motorLeft->run(BACKWARD);
  motorRight->run(FORWARD);
  motorLeft->setSpeed(i);
  motorRight->setSpeed(i);
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
  
  int yellowRange[] = {100, 200};

  IRLeft_Value = analogRead(analogPin1);
  IRFront_Value = analogRead(analogPin2);
  IRRight_Value = analogRead(analogPin3);
  IRBackRight_Value = analogRead(analogPin4);

  Serial.println(IRLeft_Value);
  Serial.println(IRFront_Value);
  Serial.println(IRRight_Value);
  Serial.println(IRBackRight_Value);

  if(IRLeft_Value < 100){
    Serial.println("IR Left Sensor is blocked");
    IRBlocked[0]=IRLeft_Value;
  }
  if(IRFront_Value < 100){
    Serial.println("IR Front Sensor is blocked");
    IRBlocked[1]=IRFront_Value;
  }
  if(IRRight_Value < 100){
    Serial.println("IR Right Sensor is blocked");
    IRBlocked[2]=IRRight_Value;
  }
  if(IRBackRight_Value < 100){
    Serial.println("IR Back Right Sensor is blocked");
    IRBlocked[2]=IRBackRight_Value;
  }
}

// Reset the IR sensors to false
void resetIRSensors(){
  for(int i=0; i<3; i++){
    IRBlocked[i]=false;
  }
}



void CheckUltraSound(){

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);


  long duration = pulseIn(echoPin, HIGH);
  //distance_inches = microsSectionstoInches(duration);
  long distance_cm = microsSectionstoCentemers(duration);
 /*  Serial.print(distance_inches);
  Serial.print(" in, "); */

  Serial.print(distance_cm);
  Serial.print(" cm, ");
  Serial.println();

  //Set global variable objDistance to the distance in cm
  objDistance[0] = objDistance[1]; // Update the previous distance
  objDistance[1] = distance_cm; // Update the current distance
  delay(100);
}

void CheckObjectDistance(){
  // If the current object measurement is 0, return
  if(objDistance[1] == 0){
    return;
  }

  // If the object is too far away, reset the object distance
  if(objDistance[1] > maxObjDistance){
    objDistance[0] = 0;
    objDistance[1] = 0;
    return;
  }

  //If the current object is closer or equal to the minimum object distance, stop the motors
  if(objDistance[1] <= minObjDistance){
    MotorForward(0);
    stoppedFromObject = true;
    return;
  }
    
  // If the current object is future than the previous object, speed up the motors  
  if(objDistance[1] > objDistance[0] && objDistance[1] < maxObjDistance && objDistance[1] > 1){
    MotorForward(120);
  }
  // If current object distance is less than the previous object distance, slow down the motors
  else if(objDistance[1] < objDistance[0] && objDistance[1] < maxObjDistance && objDistance[1] > 1){
    MotorForward(100);
  }
}



// Function to convert microseconds to centimeters
long microsSectionstoCentemers(long duration) {
  // Speed of sound in air is 343 meters per second or 29 microseconds per centimeter
  return duration / 29 / 2;
}

