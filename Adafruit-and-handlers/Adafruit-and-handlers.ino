/**
 * This is a hack of the butt handling w/ sound.
 * It requires the Adafruit_Soundboard library. 
 */

/**
 * SOUND
 */
#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h"

// Choose any two pins that can be used with SoftwareSerial to RX & TX
#define SFX_TX 5
#define SFX_RX 6

// Connect to the RST pin on the Sound Board
#define SFX_RST 4

// You can also monitor the ACT pin for when audio is playing!

// we'll be using software serial
SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);

// pass the software serial to Adafruit_soundboard, the second
// argument is the debug port (not used really) and the third 
// arg is the reset pin
Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);
// can also try hardware serial with
// Adafruit_Soundboard sfx = Adafruit_Soundboard(&Serial1, NULL, SFX_RST);



/**
 * (Now with sounds)
 * This program provides two levels of debounced events for a button.
 * 'Butt' events/state, derived from digital pin input. 
 * 'Sit' events/state, derived from butt state.
 * 
 * Butt events have a 50ms debounce to mitigate mechanical/electronic irregularities.
 * Sit events require a user to sit for 1000ms before they kick in.  
 * 
 * The two tiers exist to avoid false positives from people smacking the lid and the like.
 * Also to allow things like, 'stood up after sitting for a certain amount of time', e.g. onSitComplete()
 * 
 * -Ian Bellomy
 */

//
// CUSTOM EVENTS
//

/**
 * Called about 1 second after a user has sat, long enough to count as haing began sitting. see minimumTimeToBeginSit
 * There's a brief dealy to avoid fast repeat lid smacks from triggering this event.
 * For a more immediate response see onButtDown()
 */
void onSitStart(){
  Serial.println("onSitStart"); 
//  sfx.playTrack((uint8_t)1);//tick-tock
}

/**
 * Called continuously (every loop), while a person is sitting, if they have sat long enough to trigger a sit, but not long enough to trigger a satisfied sit. see minimumTimeToSatisfaction 
 * Called immediaetly after onButtContinue().
 */
void onSitContinue(){
   Serial.println("onSitContinue");   
}

/**
 * Called once when the user has sat long enough.
 * Called immediately after onSitContinue().
 */
void onSitSatisfied(){
  Serial.println("onSitSatisfied");
//  sfx.playTrack((uint8_t)3);
}

/**
 * Called once whenon user stands after NOT having sat long enough for satisfaction.
 * Called immediately after onButtUp(). 
 */
void onSitCancel(){
  Serial.println("onSitCancel");
//  sfx.playTrack((uint8_t)2);// Flush  
}

/**
 * Called once when a user has stood after having sat long enough for satisfaction.
 * Called immediately after onButtUp().
 */
void onSitComplete(){
  Serial.println("onSitComplete");
}


/**
 * This should be called after whatever onSitComplete() does is finished.
 * It should be called from WITHIN the onSitComplete handler()
 * TODO: Consider an onResponseInterrupted() handler, or set a flag to prevent response from being interrupted.
 */
void onResponseComplete(){
  Serial.println("onResponseComplete");
}


/**
 * Called once, immediately when a press/butt is detected.
 * Probably want to use onSitStart instead of this event.
 * TODO: Consider throttling this so it can't be called multiple times very quickly by someone tapping on the seat.
 */
void onButtDown(){
  Serial.println("onButtDown");
}

/**
 * Called once, when user stops sitting, no matter what.
 * Called immediately before onSitCancel or onSitComplete
 * TODO: pass in success param?
 */
void onButtUp(){  
  Serial.println("onButtUp");
}

/**
 * Called continuously (every loop) while a person is sitting.
 * Called before onSitSatisfied.
 */
void onButtContinue(){
   //Serial.println("onButtContinue..."); // Too noisy. It works though.
}


//
// SETUP
//

/**
 * EVENT HANDLING
 */
unsigned long currentTimeMS; // used to cache current time in loop code.

const int buttPin = 2;   // input for sensor
const int ledPin = 13;   // test output

int buttPinState = LOW;          // current pin state
int buttPinState_previous= LOW;  // current pin state
int ledPinState = HIGH;          // current pin state

unsigned long lastButtDebounceTime = 0;   // used for debouncing butt input
unsigned long buttDebounceDelay = 50;     // min duration of continuous pin reading needed to trigger state change 
unsigned long buttDownStartTime = 0;      // what time was the button pressed
unsigned long buttDownTime = 0;           // how longs has the button been pressed

bool isButtDown = false;  // whether or not the seat is in a pressed state
bool wasButtDown = false; // whether or not the seat was in a pressed state last update
bool isSitting = false;   // whether or not we're in a sitting state
bool wasSitting = false;  // whether or not we're in a sitting state last update
bool sitSatisfied = false;// whether or not someone has sat long enough
bool sitComplete = false; // whether or not someone has stood after having sat long enough

unsigned long minimumTimeToBeginSit = 1000; // how long must a user's butt be down in order to qualify as the start of a sit
unsigned long sitStartTime = 0;             // the time when the sit began
unsigned long sitTime = 0;                   // how long has someone been sitting in milliseconds
unsigned long minimumTimeToSatisfaction = 5000;  // how long must someone sit to trigger onSitSatisfied()

void setup() {  

  // SOUND
  Serial.begin(115200);
  Serial.println("Adafruit Sound Board!");
  
  // softwareserial at 9600 baud
  ss.begin(9600);
  // can also do Serial1.begin(9600)

  if (!sfx.reset()) {
    Serial.println("Not found");
    while (1);
  }
  Serial.println("SFX board found");   
  
  // EVENTS
  pinMode(buttPin, INPUT_PULLUP);  
  pinMode(ledPin, OUTPUT);
}


//
// MAIN LOOP / EVENT LOOP
//
 
void loop() {

  currentTimeMS = millis();

  //
  // read pin(s), debounce readings, set butt flag(s)
  //
  
  buttPinState  = !digitalRead(buttPin);  
  

  // has the pin value changed?
  if(buttPinState != buttPinState_previous){
    // reset debounce timer
    lastButtDebounceTime = currentTimeMS; 
  }

  // is the pinValue different than the derivedState AND
  // has enough time passed for debouncing? 
  if( (currentTimeMS - lastButtDebounceTime ) > buttDebounceDelay){
    // We have a change.
    isButtDown = buttPinState == HIGH;
  }

  // update pin tracking flags for next loop
  buttPinState_previous = buttPinState;
  

  //  
  // Derive sit flag(s) from buttFlags and call events
  // i.e. You're not sitting until your butt has been down for a moment...
  //

  // JUST put butt down...
  if(isButtDown && !wasButtDown){   
    buttDownStartTime = currentTimeMS; 
    buttDownTime = 0;
    onButtDown();
  }  
    
  // Continues to have butt down...
  if(isButtDown && wasButtDown){    
    
    onButtContinue();     
    buttDownTime = currentTimeMS - buttDownStartTime;
    
    // ...buttDown long enough to count as a sit start?
    if(!isSitting && buttDownTime > minimumTimeToBeginSit){
      // (re)set flags and timer(s)
      isSitting = true;
      sitComplete = false;
      sitSatisfied = false;
      sitTime = 0;
      sitStartTime = currentTimeMS;
      onSitStart();
    }        
    
    // ... continues to sit (before satisfaction)?
    if(isSitting && wasSitting && !sitSatisfied){
      sitTime = currentTimeMS - sitStartTime;
      onSitContinue();
    }

    // TODO: continues to sit for some time AFTER sitSatisfied...
    // ...
    
    // ... sat long enough for satisfaction
    if(sitTime >= minimumTimeToSatisfaction && !sitSatisfied){
      sitSatisfied = true;
      onSitSatisfied();
    }
    
  }

  // JUST stood...
  if(!isButtDown && wasButtDown){
    onButtUp();    
        
    // ... early
    if(isSitting && !sitSatisfied){      
      isSitting = false;
      onSitCancel();        
        
    // ... after sitting long enough
    }else if(isSitting && sitSatisfied){
      
      isSitting = false;
      sitComplete = true;
      
      onSitComplete();
      
      sitComplete = false; // reset flags
      sitSatisfied = false;
    }

  }  

  // 
  if(isSitting){
    digitalWrite(13,HIGH); // <- for testing
  }else{
    digitalWrite(13,LOW); // <- for testing
  }

  // update state tracking flags for next loop  
  wasButtDown = isButtDown; // track for future
  wasSitting = isSitting;
}
