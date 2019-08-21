
unsigned long currentTimeMS = 0; // used to cache current time in loop code. 
unsigned long cycle = 0;         // e.g. frames() — the iteration of the main loop we're on.

const int testLedPin = 3;   // test output (has pwm)
int LEDbrightness = 0; // can't get this to work

// lights
#define RED_PIN 9    // PWM required
#define GREEN_PIN 10 // PWM required
#define BLUE_PIN 11  // PWM required

// butt-events
#define buttPin 2   // input for digital test button
#define FSR_PIN A0  // input for analog FSR / pressure sensor

// aideepen-control
#define ARDUINO_RX 5  // PWM required; should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 6  // PWM required; connect to RX of the module

void setup() {  
  Serial.begin(9600);
  randomSeed(analogRead(0));
  buttEventsSetup();  
  aideepenSetup();
  lightSetup();
}

void loop() { 
  cycle++;
  currentTimeMS = millis(); 
  
  buttEventProcessing();
  aideepenLoop();  // used for parsing serial commands.
  lightLoop();
}
