const short POTENTIOMETERPIN = A0;

void potentiometerSetup() {

}

void potentiometerLoop() {
 int analogValue = analogRead(A0);
}

//potentiometer is meant to represent train speeds, values under 384 count as the train being "slow".
bool isTrainSpeedSlow(){
  return analogRead(POTENTIOMETERPIN)<384;
}

//if isTrainSpeedSlow(){ don't instantly set traffic light to yellow}