// global (for testing)
const int testLedPin = 11;   // test output (has pwm)
int LEDbrightness = 0; // can't get this to work

// butt-events
const int buttPin = 2;   // input for digital test button
const int FSR_PIN = A0;  // input for analog FSR / pressure sensor

// aideepen-control
#define ARDUINO_RX 5  //should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 6  //connect to RX of the module

void setup() {  
  Serial.begin(9600);
  buttEventsSetup();  
  aideepenSetup();
  lightSetup();
}

void loop() {  
  buttEventProcessing();
  aideepenLoop();  
  lightLoop();
}
