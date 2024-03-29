const short NORTH = 0;
const short SOUTH = 1;

const short RED = 0;
const short YELLOW = 1;
const short GREEN = 2;

const short OFF = 0;
const short ON = 1;

int iLEDS = 0;
bool ledState = ON;
const short YELLOWBLINKAMOUNT = 8;

const short AMOUNTOFLEDS = 6;
const short LEDPINSTARTOFFSET = 5;

short trafficLight[2]= {0, 0};

unsigned long long previousMillisLEDS = 0;
unsigned long long previousMillisClearance = 0;

//5 seconds of green before turning yellow
const unsigned short GREENTIME = 2000;
const unsigned short YELLOWTIME = 1000;
const unsigned short FLASHTIME = 500;

const unsigned short CLEARANCETIME = 2000;

//cycleRunning holds the boolean that knows whether or not a traffic cycle is finished per direction (north, south)
bool cycleRunning = false;

bool clearanceStatus = false;

const short LIGHTS_PINS[2][3] = {
  { 5, 6, 7 },
  { 8, 9, 10 }
};

bool getClearanceStatus(){
  return clearanceStatus;
}

void setClearanceStatus(bool status){
  clearanceStatus = status;
}

bool getCycleRunning(){
  return cycleRunning;
}

void setCycleRunning(bool status){
  cycleRunning = status;
}

void controlTrafficLight(short direction, short lightColour, bool onOff) {
  digitalWrite(LIGHTS_PINS[direction][lightColour], onOff);
  if (onOff) {
    trafficLight[direction] = lightColour;
  }
}

void controlBothTrafficLights(short lightColour, bool onOff){
  digitalWrite(LIGHTS_PINS[NORTH][lightColour], onOff);
  digitalWrite(LIGHTS_PINS[SOUTH][lightColour], onOff);
  if (onOff) {
    trafficLight[NORTH] = lightColour;
    trafficLight[SOUTH] = lightColour;
  }
}

void controlTrafficLights(short direction, bool onOff) {
  controlTrafficLight(direction, RED, onOff);
  controlTrafficLight(direction, YELLOW, onOff);
  controlTrafficLight(direction, GREEN, onOff);
}

//a traffic cycle is just the LEDs turning green, yellow and then red. has some code to check \
for double presses.
void trafficCycle(short direction) {
  Serial.println("traffic cycle");
  setCycleRunning(true);
  //clearance(time) = ontruimingstijd
  if(hasTimePassed(previousMillisClearance, CLEARANCETIME) && getCountdownStatus()){
    Serial.println("getcountdownstatus");
    if (trafficLight[direction] == RED) {
      Serial.println("trafficlight is red");
      previousMillisLEDS = millis();
      controlTrafficLight(direction, RED, OFF);
      controlTrafficLight(direction, GREEN, ON);
    }
    else if (trafficLight[direction] == GREEN) {
      if (getDoublePress(direction)) {  //double greentime if there is a double press
      Serial.println("double greentime");
        if (hasTimePassed(previousMillisLEDS, GREENTIME*2)) {
          previousMillisLEDS = millis();
          controlTrafficLight(direction, GREEN, OFF);
          controlTrafficLight(direction, YELLOW, ON);
          setDoublePress(direction, false);
          setButtonPressedCounter(direction, false);
        }
      } 
      else if (!getDoublePress(direction)) {
        if (hasTimePassed(previousMillisLEDS, GREENTIME)) {
          previousMillisLEDS = millis();
          controlTrafficLight(direction, GREEN, OFF);
          controlTrafficLight(direction, YELLOW, ON);
        }
      }
    }
    if (trafficLight[direction] == YELLOW) {
      if (hasTimePassed(previousMillisLEDS, YELLOWTIME)) {
        previousMillisLEDS = millis();
        controlTrafficLight(direction, YELLOW, OFF);
        controlTrafficLight(direction, RED, ON);
        setCycleRunning(false);
        previousMillisClearance = millis();
      }
    }
  }
}

//makes the yellow LEDs blink on interval
void trainPassingCycle(){
  setCycleRunning(false); //if cycle is interrupted by a train coming, the cycleRunning needs to be false
  if(hasTimePassed(previousMillisLEDS, FLASHTIME)){
    controlBothTrafficLights(YELLOW, !ledState);
    ledState = !ledState;
    previousMillisLEDS = millis();
    iLEDS++;
  }
}

void setupLeds() {
  for (int i = LEDPINSTARTOFFSET; i < AMOUNTOFLEDS + LEDPINSTARTOFFSET; i++) {
    pinMode(i, OUTPUT);
  }
  controlTrafficLight(NORTH, RED, ON);
  controlTrafficLight(SOUTH, RED, ON);
  trafficLight[NORTH] = RED;
  trafficLight[SOUTH] = RED;
}
