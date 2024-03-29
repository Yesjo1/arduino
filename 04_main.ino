void setup() {
  Serial.begin(115200);
  setupButtons();
  setupLeds();
  setupServo();
  segmentDisplaySetup();
}

void loop() {
  buttonsLoop();
  statesLoop();
  // segmentDisplayLoop();

}