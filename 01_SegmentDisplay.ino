const short STCPPIN = 4;
const short SHCPPIN = 3;
const short DSPIN = 2;

//array & when segment ends
const short AMOUNTOFDISPLAYNUMBERS = 6;
const short SEGMENTEND = -1;
const short SEGMENTSTART = 5;

//loop
short iSegmentCount = 5;

//timer
const short COUNTDOWNTIME = 1000;
unsigned long long previousMillisCountDown = 0;

bool countdownStatus = true;

const short DISPLAYNUMBERS[AMOUNTOFDISPLAYNUMBERS] = {B00111111, B00000110, B01011011, B01001111, B01100110, B01101101};
//  B00111111,  // 0
//  B00000110,  // 1
//  B01011011,  // 2
//  B01001111,  // 3
//  B01100110,  // 4
//  B01101101,  // 5

short getiSegmentCount(){
  return iSegmentCount;
}

void setiSegmentCount(short count){
  iSegmentCount = count;
}

//should return true when countdown is done
bool getCountdownStatus(){
  return countdownStatus;
}

void setCountdownStatus(bool status){
  countdownStatus = status;
}

void displayNumber(const short number){
  digitalWrite(STCPPIN, LOW);
  shiftOut(DSPIN, SHCPPIN, MSBFIRST, DISPLAYNUMBERS[number]);
  digitalWrite(STCPPIN, HIGH);
}

void countSegmentDown(){
    if(hasTimePassed(previousMillisCountDown, COUNTDOWNTIME) && iSegmentCount == SEGMENTEND){
    setiSegmentCount(5);
    displayNumber(getiSegmentCount());
    setCountdownStatus(true);
    setTrainPassing(false);

    //the reason i do this here is not because they are still red but because the array holding 
    //whatever colour is lit on which side, is only 2 elements large. they are yellow here before i call this.
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


void segmentDisplaySetup(){
  pinMode(STCPPIN, OUTPUT);
  pinMode(SHCPPIN, OUTPUT);
  pinMode(DSPIN, OUTPUT);
  displayNumber(5);
}

void segmentDisplayLoop() {
//count down every second
if(iSegmentCount >= 0 && hasTimePassed(previousMillisCountDown, COUNTDOWNTIME)){
  displayNumber(iSegmentCount);
  previousMillisCountDown = millis();
  iSegmentCount--;
 }
 //the only time when this should run is when the segmentdisplay is also running
 //(or the other way around, this is just the way i did it)
  buzzerTickThreeTimes();
}