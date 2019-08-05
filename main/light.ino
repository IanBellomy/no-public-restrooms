extern int LEDbrightness;

void lightSetup(){
  pinMode(testLedPin, OUTPUT);
}

void lightLoop(){
  analogWrite(testLedPin, LEDbrightness);
}
