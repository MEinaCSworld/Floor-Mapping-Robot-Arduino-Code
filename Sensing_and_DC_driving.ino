#include <AFMotor.h>
// Project: The project this program is designed for is the floorplan mapping robot built by Zack Wood.
// This is the robot driving Arduino program for the 4 DC motor chassis with digital hall meter tachometers at each motor to track speed.
// The program runs as a slave device from a master microcomputer. The microcomputer controls the Arduino using serial communication.
// The Arduino sends the hall data back to the master microcomputer to be combined with mapping data from an ultrasonic sensor.

// Set pin numbers for hall pins. With Rev 1 Adafruit motor shield these should be set to analog input pins
int hallPin1 = 14;// A0
int hallPin2 = 15;// A1
int hallPin3 = 16;// A2
int hallPin4 = 17;// A3
int hpOld1;
int hpOld2;
int hpOld3;
int hpOld4;
int count1 = 0;
int count2 = 0;
int count3 = 0;
int count4 = 0;
// Motor variables
int spd1 = 0;
int spd2 = 0;
int spd3 = 0;
int spd4 = 0;
int direction1 = 1;//1 is forward, 0 is backward
int direction2 = 1;
int direction3 = 1;
int direction4 = 1;
AF_DCMotor motor1(1);// Input position here
AF_DCMotor motor2(2);// Input position here
AF_DCMotor motor3(3);// Input position here
AF_DCMotor motor4(4);// Input position here

// Serial receiving variables
const byte numChars = 32; //
char receivedChars[numChars]; // Stores received characters until end line character is received.
boolean newData = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Ready\n"); // Tells Pi the Arduino is prepared to send communications
  // Serial communication between Arduino and Raspberry Pi
  // Raspberry Pi will send speed commands
  // Arduino will send hall sensor tachometer readout in RPM or m/s depending on application
  pinMode(hallPin1, INPUT);
  pinMode(hallPin2, INPUT);
  pinMode(hallPin3, INPUT);
  pinMode(hallPin4, INPUT);
  hpOld1 = digitalRead(hallPin1);
  hpOld2 = digitalRead(hallPin2);
  hpOld3 = digitalRead(hallPin3);
  hpOld4 = digitalRead(hallPin4);
  // 
  motor1.setSpeed(spd1);
  motor2.setSpeed(spd2);
  motor3.setSpeed(spd3);
  motor4.setSpeed(spd4);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void recvWithStartEndMarker(){
  static boolean recvInProgress = false;
  static byte index = 0;
  char startMarker = '<';
  char endMarker = '>';
  char RecChar;
  // Receives commands from Pi
  while (Serial.available() > 0 && newData == false){
    RecChar = Serial.read();
    if (recvInProgress == true){
      
      if (RecChar != endMarker) {
        receivedChars[index] = RecChar;
        index++;
        if (index >= numChars){
          index = numChars - 1;
        }
      }
      else {
        receivedChars[index] = 'z';// terminates the string
        recvInProgress = false;
        index = 0;
        newData = true;
      }
    }
    else if(RecChar == startMarker){
      recvInProgress = true;
    }
  }
}

void showNewData(){
  if (newData == true){
    Serial.println(receivedChars);
    newData = false; 
  }
}

void turn(){
  if (receivedChars == "left" && newData == true){
    // Values determined experimentally to execute 90 degree left turn.
    spd1 = 2;
    spd2 = 2;
    spd3 = 1; 
    spd4 = 1;
    delay(1000);
  }
  else if(receivedChars == "right" && newData == true){
    spd1 = 1;
    spd2 = 1;
    spd3 = 2; 
    spd4 = 2;
    delay(1000);
  }
}

void speedChange(){
  // c marks the string to change target speed
  int tempSpeed = 0;
  int tempStor = 0;
  if (receivedChars[0] = 'c'){
    
    for (int i = 1; i<4; i = i + 1) {
      tempStor = (int)receivedChars[i];
      tempSpeed = tempSpeed + tempStor * pow(10,i-1);
    }
    spd1 = tempSpeed;
    spd2 = tempSpeed;
    spd3 = tempSpeed;
    spd4 = tempSpeed;
  }
}

int hallPinRead(int hp,int valOld,int count, int dir){
  // hp is the hall pin, valOld is the old value. Initial value is taken in setup function
  int val = digitalRead(hp);

  if (val != valOld && dir ==1){
    count++;
    valOld = val;
    if (count % 2 == 0){
      return millis();
    }
    
  }
  else if (val != valOld && dir == 0){
    count--;
    valOld = val;
    if (count % 2 == 0){
      return millis();
    }
      
  }
  
}

void loop() {
  
  recvWithStartEndMarker();
  showNewData();
  hallPinRead(hallPin1, hpOld1, count1, direction1);
}
