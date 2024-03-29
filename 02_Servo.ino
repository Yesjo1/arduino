#include <Servo.h>
Servo barrier;  // create servo object to control the "barrier" (slagboom in NL)

const short SERVO_ANGLE_OPENED = 90;
const short SERVO_ANGLE_CLOSED = 0;

const short SERVO_PIN = 11;

short servoAngle = 0;
const short SERVO_INTERVAL = 15;

unsigned long previousMillis;

void setupServo() {
  barrier.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo
}

short getServoAngle(){
  return servoAngle;
}

void closeGate(){
  if(servoAngle > SERVO_ANGLE_CLOSED && hasTimePassed(previousMillis, SERVO_INTERVAL)){
    servoAngle -=1;
    barrier.write(servoAngle);
    previousMillis = millis();
  }
}

void openGate(){
  if(servoAngle < SERVO_ANGLE_OPENED && hasTimePassed(previousMillis, SERVO_INTERVAL)){
    servoAngle +=1;
    barrier.write(servoAngle);
    previousMillis = millis();
  }
}

void gateOpenEntry(){
  previousMillisCountDown = millis();
}

void gateOpenDo(){
  //should only display number 5 after the countdown is 0.
  if(hasTimePassed(previousMillisCountDown, COUNTDOWNTIME) && iSegmentCount == SEGMENTEND){
    setiSegmentCount(5);
    displayNumber(getiSegmentCount());
    setCountdownStatus(true);
    setTrainPassing(false);

    //the reason i do this here is not because they are still red but because the array holding whatever colour is lit on which side, is only 2 elements large. they are yellow here before i call this.
    controlBothTrafficLights(RED, ON);
  }
  if(!getCountdownStatus()){
    segmentDisplayLoop();
    trainPassingCycle();
  }
  //the yellow lights need to be turned off when the countdown has finished
  else if(getCountdownStatus()){
    controlBothTrafficLights(YELLOW, OFF);
  }
}

void gateOpenExit(){
}

void gateClosingEntry(){//in case of a double press this needs to be different as the traffic lights need to wait
  // Serial.println("gate is closing servo.ino");
  controlTrafficLights(NORTH, OFF);
  controlTrafficLights(SOUTH, OFF);
  controlBothTrafficLights(YELLOW, ON);
  
  servoAngle = SERVO_ANGLE_OPENED;

  previousMillis = millis();

  previousMillisLEDS = millis();
}

void gateClosingDo(){
  trainPassingCycle();
  closeGate();
  if(getServoAngle()>SERVO_ANGLE_CLOSED){
    buzzerTick();
  }
  // Serial.println("gateclosingdo");
}

void gateClosingExit(){
  iLEDS = 0;
}

void gateOpeningEntry(){
  // Serial.println("gate is opening servo.ino");
  previousMillis = millis();
  previousMillisLEDS = millis();
  servoAngle = SERVO_ANGLE_CLOSED;

  setCountdownStatus(false);
}

void gateOpeningDo(){
  openGate();
  trainPassingCycle();
  if(getServoAngle()<SERVO_ANGLE_OPENED){
    buzzerTick();
  }
}

void gateOpeningExit(){
}

void gateClosedEntry(){

}

void gateClosedDo(){
  controlBothTrafficLights(RED, ON);
  trainPassingCycle();
}

void gateClosedExit(){
  //do stuff
}
