unsigned long setupTimer(unsigned long interval){
  return 0 - interval;  
}

bool hasTimePassed(unsigned long long previousMillis, unsigned long interval) {
  return millis() - previousMillis >= interval;  
}