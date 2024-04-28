unsigned long long currentMillis = 0;
unsigned long long previousMillisSlowTrain = 0;
const unsigned short ONTRUIMINGSTIJD = 2000;
const unsigned short SLOWTRAININTERVAL = 2000;

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
}

void trafficCycleNorthEntry() {
  setButtonPressedDuringCyclePrecedence(2);
  setButtonPressedDuringCycle(TRAFFIC_NORTH_BUTTON, false);
}

void trafficCycleSouthDo() {
  trafficCycle(SOUTH);
}

void trafficCycleNorthDo() {
  trafficCycle(NORTH);
}

void trafficCycleSouthExit() {
  setButtonPressedDuringCyclePrecedence(2);
  setButtonPressedDuringCycle(TRAFFIC_SOUTH_BUTTON, false);
}

void trafficCycleNorthExit() {
  setButtonPressedDuringCyclePrecedence(2);
  setButtonPressedDuringCycle(TRAFFIC_NORTH_BUTTON, false);
}

void statesLoop() {
  switch (state) {
    case GATE_STATE_OPEN:
      gateOpenDo();
      if(getCountdownStatus()){
        if (getTrainPassing() && ((getTrainComing() == TRAIN_COMING_EAST_BUTTON) || (getTrainComing() == TRAIN_COMING_WEST_BUTTON))) {
          gateOpenExit();
          state = GATE_STATE_CLOSING;
          gateClosingEntry();
        }
          //didn't really feel like making an elegant solution for this
          else if(getButtonPressedDuringCyclePrecedence() == 1){
            setButtonPressedDuringCyclePrecedence(2);
            gateOpenExit();
            state = TRAFFIC_CYCLE_SOUTH;
            trafficCycleSouthEntry();
          }
          else if(getButtonPressedDuringCyclePrecedence() == 0){
            setButtonPressedDuringCyclePrecedence(2);
            gateOpenExit();
            state = TRAFFIC_CYCLE_NORTH;
            trafficCycleNorthEntry();
          }
          // getButtonPressedDuringCycle(TRAFFIC_NORTH_BUTTON)
          else if (getButtonState(TRAFFIC_SOUTH_BUTTON) || getButtonPressedDuringCycle(TRAFFIC_SOUTH_BUTTON)) {
            gateOpenExit();
            state = TRAFFIC_CYCLE_SOUTH;
            trafficCycleSouthEntry();
          }
          else if (getButtonState(TRAFFIC_NORTH_BUTTON) || getButtonPressedDuringCycle(TRAFFIC_NORTH_BUTTON)) {
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