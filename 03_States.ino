unsigned long long currentMillis = 0;
unsigned long long previousMillisSlowTrain = 0;
const unsigned long ONTRUIMINGSTIJD = 2000;
const unsigned long SLOWTRAININTERVAL = 2000;

int state = 0;

const short GATE_STATE_OPEN = 0;
const short GATE_STATE_CLOSING = 1;
const short GATE_STATE_CLOSED = 2;
const short GATE_STATE_OPENING = 3;
const short TRAFFIC_CYCLE_SOUTH = 4;
const short TRAFFIC_CYCLE_SOUTH_LONG = 5;
const short TRAFFIC_CYCLE_NORTH = 6;
const short TRAFFIC_CYCLE_NORTH_LONG = 7;


void trafficCycleSouthEntry() {
  setButtonPressedDuringCyclePrecedence(2);
  setButtonPressedDuringCycle(TRAFFIC_SOUTH_BUTTON, false);
  Serial.println("traffic cycle south entry");
}

void trafficCycleNorthEntry() {
  setButtonPressedDuringCyclePrecedence(2);
  setButtonPressedDuringCycle(TRAFFIC_NORTH_BUTTON, false);
  Serial.println("traffic cycle north entry");
}

void trafficCycleSouthDo() {
    Serial.println("traffic cycle south do");

  trafficCycle(SOUTH);
}

void trafficCycleNorthDo() {
  trafficCycle(NORTH);
}

void trafficCycleSouthExit() {
  Serial.println("traffic cycle south exit");
  setButtonPressedDuringCyclePrecedence(2);
  setButtonPressedDuringCycle(TRAFFIC_SOUTH_BUTTON, false);
}

void trafficCycleNorthExit() {
  Serial.println("traffic cycle north exit");
  setButtonPressedDuringCyclePrecedence(2);
  setButtonPressedDuringCycle(TRAFFIC_NORTH_BUTTON, false);
}

void statesLoop() {
  switch (state) {
    case GATE_STATE_OPEN:
    potentiometerLoop();
      gateOpenDo();
      if(getCountdownStatus()){
        if (getTrainPassing() && ((getTrainComing() == TRAIN_COMING_EAST_BUTTON) || (getTrainComing() == TRAIN_COMING_WEST_BUTTON))) {
          Serial.println("train is coming east");  // east = 3
          gateOpenExit();
          state = GATE_STATE_CLOSING;
          gateClosingEntry();
        }
          //didn't really feel like making an elegant solution for this
          else if(getButtonPressedDuringCyclePrecedence() == 1){
            Serial.println("traffic crossing south");
            setButtonPressedDuringCyclePrecedence(2);
            gateOpenExit();
            state = TRAFFIC_CYCLE_SOUTH;
            trafficCycleSouthEntry();
          }
          else if(getButtonPressedDuringCyclePrecedence() == 0){
            Serial.println("traffic crossing north");
            setButtonPressedDuringCyclePrecedence(2);
            gateOpenExit();
            state = TRAFFIC_CYCLE_NORTH;
            trafficCycleNorthEntry();
          }
          // getButtonPressedDuringCycle(TRAFFIC_NORTH_BUTTON)
          else if (getButtonState(TRAFFIC_SOUTH_BUTTON) || getButtonPressedDuringCycle(TRAFFIC_SOUTH_BUTTON)) {
            Serial.println("traffic crossing south");
            gateOpenExit();
            state = TRAFFIC_CYCLE_SOUTH;
            trafficCycleSouthEntry();
          }
          else if (getButtonState(TRAFFIC_NORTH_BUTTON) || getButtonPressedDuringCycle(TRAFFIC_NORTH_BUTTON)) {
            Serial.println("traffic crossing north");
            gateOpenExit();
            state = TRAFFIC_CYCLE_NORTH;
            trafficCycleNorthEntry();
          }
        }
      break;
    case GATE_STATE_CLOSING:
      // Serial.println("-- Gate is closing");
      //Serial.println();
      gateClosingDo();
      // Serial.println(getServoAngle() == SERVO_ANGLE_CLOSED);
      if (getServoAngle() == SERVO_ANGLE_CLOSED && iLEDS >= YELLOWBLINKAMOUNT) {
        gateClosingExit();
        state = GATE_STATE_CLOSED;
        gateClosedEntry();
      }
      break;
    case GATE_STATE_CLOSED:
      // Serial.println("-- Gate is closed");
      gateClosedDo();
      if (!getTrainPassing()) {
        //Serial.println("Opening");
        gateClosedExit();
        state = GATE_STATE_OPENING;
        gateOpeningEntry();
      }
      break;
    case GATE_STATE_OPENING:
      // Serial.println("-- Gate is opening");
      gateOpeningDo();
      if (getServoAngle() == SERVO_ANGLE_OPENED) {
        gateOpeningExit();
        state = GATE_STATE_OPEN;
        gateOpenEntry();
      }
      break;
    case TRAFFIC_CYCLE_SOUTH:
      trafficCycleSouthDo();
      if (!getCycleRunning()) { //if cycle running is true, that means cycle is still running
        trafficCycleSouthExit();
        state = GATE_STATE_OPEN;
        gateOpenEntry();
      } else if (getTrainPassing() && (getTrainComing() == TRAIN_COMING_EAST_BUTTON || getTrainComing() == TRAIN_COMING_WEST_BUTTON) && !isTrainSpeedSlow()) {
        trafficCycleSouthExit();
        state = GATE_STATE_CLOSING;
        gateClosingEntry();
      } else if(getTrainPassing() && (getTrainComing() == TRAIN_COMING_EAST_BUTTON || getTrainComing() == TRAIN_COMING_WEST_BUTTON) && isTrainSpeedSlow()){
        if(hasTimePassed(previousMillisLEDS, GREENTIME+YELLOWTIME)){
          trafficCycleSouthExit();
          state = GATE_STATE_CLOSING;
          gateClosingEntry();
        }
      }
      break;
    case TRAFFIC_CYCLE_NORTH:
      trafficCycleNorthDo();
      if (!getCycleRunning()) {
        trafficCycleNorthExit();
        state = GATE_STATE_OPEN;
        gateOpenEntry();
      } else if (getTrainPassing() && (getTrainComing() == TRAIN_COMING_EAST_BUTTON || getTrainComing() == TRAIN_COMING_WEST_BUTTON) &&  !isTrainSpeedSlow()) {
        trafficCycleNorthExit();
        state = GATE_STATE_CLOSING;
        gateClosingEntry();
      } else if(getTrainPassing() && (getTrainComing() == TRAIN_COMING_EAST_BUTTON || getTrainComing() == TRAIN_COMING_WEST_BUTTON) && isTrainSpeedSlow()){
        if(hasTimePassed(previousMillisLEDS, GREENTIME+YELLOWTIME)){
          trafficCycleNorthExit();
          state = GATE_STATE_CLOSING;
          gateClosingEntry();      
        }
      }  
     break;
    }
 
}