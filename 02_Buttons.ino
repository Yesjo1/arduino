const short BUTTON_PIN_TRAIN_NORTH = A4;
const short BUTTON_PIN_TRAIN_SOUTH = A5;
const short BUTTON_PIN_LEDS_NORTH = A2;
const short BUTTON_PIN_LEDS_SOUTH = A3;

//this array holds the button pins so it's easier to loop through
const short BUTTON_PINS[4] = {BUTTON_PIN_LEDS_NORTH, BUTTON_PIN_LEDS_SOUTH, BUTTON_PIN_TRAIN_NORTH, BUTTON_PIN_TRAIN_SOUTH};

const short BUTTON_STATE_RELEASED = 0;
const short BUTTON_STATE_CHECK_PRESSED = 1;
const short BUTTON_STATE_PRESSED = 2;
const short BUTTON_STATE_CHECK_RELEASED = 3;

const short TRAFFIC_NORTH_BUTTON = 0;
const short TRAFFIC_SOUTH_BUTTON = 1;
const short TRAIN_COMING_EAST_BUTTON = 2;
const short TRAIN_COMING_WEST_BUTTON = 3;

//2 is just a number that isn't 0 or 1, as there are 2 buttons (not 3)
short buttonPressedDuringCyclePrecedence = 2;
bool buttonPressedDuringCycle[2] = {0, 0};

short buttonPressedCounter[2] = {0, 0};
bool doublePressDirections[2] = {0, 0};//naamgeving is een ding i guess geen idee hoe ik deze array moet noemen

//this array holds the state of each button
//BUTTON_PIN_LEDS_NORTH, BUTTON_PIN_LEDS_SOUTH, BUTTON_PIN_TRAIN_NORTH, BUTTON_PIN_TRAIN_SOUTH; 
//traffic buttons north south on element nr 0 and 1, traincoming north/south element nr 2 and 3
short buttonStates[4] = {0, 0, 0, 0};

unsigned long long buttonTimer;
unsigned long long buttonPreviousMillis;
const short DEBOUNCE_INTERVAL = 10; //debounce timer

unsigned long long doublePressTimer = 0;
const int DOUBLEPRESSWINDOW=500;

const short AMOUNT_OF_BUTTONS = 4;

short signed trainComing = 0;
bool trainPassing;

short getButtonPressedDuringCyclePrecedence(){
  return buttonPressedDuringCyclePrecedence;
}

void setButtonPressedDuringCyclePrecedence(short buttonId){
  buttonPressedDuringCyclePrecedence = buttonId;
}

//used when a traffic cycle was going on and the button on the other side had been pressed.
bool getButtonPressedDuringCycle(const short direction){
  return buttonPressedDuringCycle[direction];
}

//used when a traffic cycle is going on and the button on the other side has been pressed.
void setButtonPressedDuringCycle(const short direction, bool value){
  buttonPressedDuringCycle[direction] = value;
}

bool getDoublePress(const short direction){
  return doublePressDirections[direction];
}

void setDoublePress(const short direction, bool status){
  doublePressDirections[direction] = status;
}

short getButtonPressedCounter(const short direction){
  return buttonPressedCounter[direction];
};

void setButtonPressedCounter(const short direction, short count){
  buttonPressedCounter[direction] = count;
}

void setButtonState(short button, short BUTTON_STATE){
  buttonStates[button] = BUTTON_STATE;
}

//takes a button and returns its state
short getButtonState(short button){
  // for(int i = 0; i < 4; i++){
  //   Serial.print("Button ");
  //   Serial.print(i);
  //   Serial.print(": ");
  //   Serial.println(buttonStates[i]);
  // }
  return buttonStates[button];
}

//more than just a check, also sets the button that was pressed during the cycle to true if it gets through the check
void checkTrafficButtonPressesDuringCycle(short direction){ //direction can only be 1 or 2
  if(getButtonState(direction) && getButtonPressedDuringCyclePrecedence() == 2){ //the moment this getter returns a value higher than 1 it will return as true. value 0 is state released.
    setButtonPressedDuringCycle(direction, true);
    setButtonPressedDuringCyclePrecedence(direction);
  }
  else if(((getButtonState(direction) && getButtonPressedDuringCyclePrecedence() == 0) || (getButtonState(direction) && getButtonPressedDuringCyclePrecedence() == 1))){
    setButtonPressedDuringCycle(direction, true);
  }
  Serial.println("checkTrafficButtonPresses");
}

void setupButtons() {
  pinMode(BUTTON_PIN_TRAIN_NORTH, INPUT_PULLUP);
  pinMode(BUTTON_PIN_TRAIN_SOUTH, INPUT_PULLUP);
  pinMode(BUTTON_PIN_LEDS_NORTH, INPUT_PULLUP);
  pinMode(BUTTON_PIN_LEDS_SOUTH, INPUT_PULLUP);

  trainPassing = false;
//  buttonTimer = setupTimer(DEBOUNCE_INTERVAL);
//  buttonReleasedEntry();/
}

//events
bool buttonDown(short BUTTON_PIN) {
  return(analogRead(BUTTON_PINS[BUTTON_PIN])<500);
}
bool buttonUp(short BUTTON_PIN) {
  return(analogRead(BUTTON_PINS[BUTTON_PIN])>500);
}


// --- BUTTON_STATE_RELEASED -----------
void buttonReleasedEntry(short buttonId) {
  // Serial.println();
  // Serial.print("Setting buttonId ");
  // Serial.print(buttonId);
  // Serial.print(" to: state_released");
  buttonStates[buttonId] = BUTTON_STATE_RELEASED;
}

void buttonReleasedDo() {
// <nothing>
}

void buttonReleasedExit() {
// <nothing>
}

// --- BUTTON_STATE_CHECK_PRESSED -----------
void buttonCheckPressedEntry(short buttonId) {
  // Serial.println();
  // Serial.print("Setting buttonId ");
  // Serial.print(buttonId);
  // Serial.print(" to: state_check_pressed");
  buttonStates[buttonId] = BUTTON_STATE_CHECK_PRESSED;
  buttonPreviousMillis = millis();
}

void buttonCheckPressedDo() {
// <nothing>
}

void buttonCheckPressedExit() {
// <nothing>
}
// --- BUTTON_STATE_PRESSED -----------
void buttonPressedEntry(short buttonId) {
  buttonStates[buttonId] = BUTTON_STATE_PRESSED;
  
  //here, trainComing acts as a "boolean", but is actually an integer that holds the value of which button was pressed last.
  //a train wouldn't be coming if the last button pressed was under 2, (values 0 or 1), only if the buttons pressed were 2 or 3,
  //as values 0 and 1 are reserved for crossing the railway
  if((buttonId == TRAIN_COMING_EAST_BUTTON || buttonId == TRAIN_COMING_WEST_BUTTON) && !trainPassing){
    setTrainComing(buttonId);
    setTrainPassing(true);
  }
  //buttonId > 1 meaning the possible buttons pressed were either 2 or 3 which *are* "train sensors" and buttonId != last button pressed
  else if(buttonId != trainComing && buttonId > 1 && trainPassing) {
    setTrainComing(-1); //no need to remember the previous button pressed as in this case the train "passed", so just reset it to 0
    setTrainPassing(false);
  }
  else if(buttonId == TRAFFIC_NORTH_BUTTON){
    //if the window of double press hasn't yet been reached, set the double press variable to be true for respective direction
    if(!hasTimePassed(doublePressTimer, DOUBLEPRESSWINDOW) && getButtonPressedCounter(TRAFFIC_NORTH_BUTTON) == 1){
      setDoublePress(TRAFFIC_NORTH_BUTTON, true);
      setButtonPressedCounter(TRAFFIC_NORTH_BUTTON, 2);
    }
    else if(getButtonPressedCounter(TRAFFIC_NORTH_BUTTON)<2){ //only under 2, if it is already 2, it shouldn't be possible to make it a double press
      setButtonPressedCounter(TRAFFIC_NORTH_BUTTON, 1);
    }
    doublePressTimer = millis();
  }
  else if(buttonId == TRAFFIC_SOUTH_BUTTON){
    if(!hasTimePassed(doublePressTimer, DOUBLEPRESSWINDOW) && getButtonPressedCounter(TRAFFIC_SOUTH_BUTTON) == 1){
      setDoublePress(TRAFFIC_SOUTH_BUTTON, true);
      setButtonPressedCounter(TRAFFIC_SOUTH_BUTTON, 2);
    }
    else if(getButtonPressedCounter(TRAFFIC_SOUTH_BUTTON)<2){
      setButtonPressedCounter(TRAFFIC_SOUTH_BUTTON, 1);
    }
    doublePressTimer = millis();
  }
}

void buttonPressedDo() {
// <nothing>
}

void buttonPressedExit() {
// <nothing>
}

// --- BUTTON_STATE_CHECK_RELEASED -----------
void buttonCheckReleasedEntry(short buttonId) {

  if(buttonId == 0 || buttonId == 1){
    checkTrafficButtonPressesDuringCycle(buttonId);
  }

  buttonStates[buttonId] = BUTTON_STATE_CHECK_RELEASED;
  buttonPreviousMillis = millis();
}

void buttonCheckReleasedDo() {
// <nothing>
}
void buttonCheckReleasedExit() {
// <nothing>
}

void setTrainPassing(bool value){
  trainPassing = value;
}

bool getTrainPassing(){
  return trainPassing;
}

short getTrainComing(){
  return trainComing;
}

void setTrainComing(short value){
  trainComing = value;
}

bool isButtonPressedSouth(){
  return false;
}

bool isButtonPressedNorth(){
  return false;
}

bool isButtonDoublePressedSouth(){
  return false;
}

bool isButtonDoublePressedNorth(){
  return false;
}

void buttonsLoop() {
  for(int buttonId = 0; buttonId < AMOUNT_OF_BUTTONS; buttonId++){
    // Serial.println(i);
    switch (buttonStates[buttonId]) {
      case BUTTON_STATE_RELEASED:
        buttonReleasedDo();
        if (buttonDown(buttonId)) {
          buttonReleasedExit();
          buttonCheckPressedEntry(buttonId);
        }
      break;
      case BUTTON_STATE_CHECK_PRESSED:
        buttonCheckPressedDo();
        if (buttonUp(buttonId)) {
          // Serial.println("ButtonUp check pressed");
          buttonCheckPressedExit();
          buttonReleasedEntry(buttonId);
        }
        else if (buttonDown(buttonId) && hasTimePassed(buttonTimer, DEBOUNCE_INTERVAL)) {
          // Serial.println("ButtonDown check pressed");
          buttonCheckPressedExit();
          buttonPressedEntry(buttonId);
        }
        break;
      case BUTTON_STATE_PRESSED:
        buttonPressedDo();
        if (buttonUp(buttonId)) {
          buttonPressedExit();
          buttonCheckReleasedEntry(buttonId);
        }
        break;
      case BUTTON_STATE_CHECK_RELEASED:
        buttonCheckReleasedDo();
        if (buttonDown(buttonId)) {
          buttonCheckReleasedExit();
          buttonPressedEntry(buttonId);
        }
        else if (buttonUp(buttonId) && hasTimePassed(buttonTimer, DEBOUNCE_INTERVAL)) {
          buttonCheckReleasedExit();
          buttonReleasedEntry(buttonId);
        }
        break;
    }
  }
}