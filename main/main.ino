
unsigned long currentTimeMS = 0; // used to cache current time in loop code. 
unsigned long cycle = 0;         // e.g. frames() — the iteration of the main loop we're on.

const int testLedPin = 3;   // test output (has pwm)
const int DIGITAL_LED_TEST_PIN = 13;   // 
int LEDbrightness = 0; // can't get this to work

// fog
#define FOG_PIN 7    
boolean isFogOn = false;
void fogOn(){
  Serial.println("FOG ON");
  digitalWrite(FOG_PIN,HIGH);
  isFogOn = true;
}
void fogOff(){
  Serial.println("FOG OFF");
  digitalWrite(FOG_PIN,LOW);
  isFogOn = false;
}

// lights
#define RED_PIN 9    // PWM required
#define GREEN_PIN 10 // PWM required
#define BLUE_PIN 11  // PWM required

// butt-events
#define buttPin 2   // input for digital test button
#define FSR_PIN A0  // input for analog FSR / pressure sensor

// aideepen-control
// CHANGED - see below
//#define ARDUINO_RX 5  // PWM required; should connect to TX of the Serial MP3 Player module
//#define ARDUINO_TX 6  // PWM required; connect to RX of the module

// ALT AIDEEPEN
#define ARDUINO_RX 6  // PWM required; should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 5  // PWM required; connect to RX of the module

void setup() {  
  //---------------------------------------------- Set PWM frequency for D9 & D10 ------------------------------
  TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz

  //---------------------------------------------- Set PWM frequency for D3 & D11 ------------------------------
  TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 divisor to     1 for PWM frequency of 31372.55 Hz

  Serial.begin(9600);
  randomSeed(analogRead(0));
  pinMode(FOG_PIN, OUTPUT);
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
