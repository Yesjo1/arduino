const short BUZZERPIN = 13;

unsigned long long prevMillisBuzzer = 0;
unsigned long long prevMillisBuzzerThree = 0;
const short INTERVALBUZZER = 150;

void buzzerTick(){
  if(hasTimePassed(prevMillisBuzzer, INTERVALBUZZER)){
    tone(BUZZERPIN, 1000, 5);
    prevMillisBuzzer = millis();
  }
  // noTone(BUZZERPIN);
}

void buzzerTickThreeTimes(){
  int i = 0;
  if(hasTimePassed(prevMillisBuzzerThree, INTERVALBUZZER*4)){
    while(i < 3){
      if(hasTimePassed(prevMillisBuzzer, ((INTERVALBUZZER/5)*2))){
        tone(BUZZERPIN, 1000, 5);
        i++;
        prevMillisBuzzer = millis();
      }
    }
    prevMillisBuzzerThree = millis();
  }
}