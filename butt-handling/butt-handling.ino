void setup() {  
  Serial.begin(9600);
  buttEventsSetup();  
}

void loop() {
  buttEventProcessing();
}
