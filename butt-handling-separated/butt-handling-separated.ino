void setup() {  
  Serial.begin(9600);
  buttEventsSetup();
  aideepenSetup();
}

void loop() {
  buttEventProcessing();
  aideepenLoop();
}
