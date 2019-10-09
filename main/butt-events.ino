/**
   This code provides two levels of debounced events for a button.
   'Butt' events/state, derived from digital pin input.
   'Sit' events/state, derived from butt state.

   Butt events have a 50ms debounce to mitigate mechanical/electronic irregularities.
   Sit events require a user to sit for 1000ms before they kick in.

   The two tiers exist to avoid false positives from people smacking the lid and the like.
   Also to allow things like, 'stood up after sitting for a certain amount of time', e.g. onSitComplete()


   Table of contents:

   (1) custom events. <- modify me!
   (2) variables
   (3) setup()
   (4) eventProcessing


   -Ian Bellomy
*/

#define SITTING_TRACK 1
#define SIT_SATISFIED_TRACK 2
#define SIT_COMPLETE_TRACK 3
#define IDLE_TRACK 4
#define BIRDS_TRACK 5


//
// BUTT EVENTS VARIABLES
//


int buttPinState = LOW;          // current pin state
int buttPinState_previous = LOW; // current pin state

boolean analogInputTriggered = false;

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
unsigned long sitStartTime = 0;              // the time when the sit began
unsigned long sitTime = 0;                   // how long has someone been sitting in milliseconds
unsigned long minimumTimeToSatisfaction = 5000;  // how long must someone sit to trigger onSitSatisfied()



/**
 * FSR sensor threshold for a 'press'
 */
int voltageTriggerThreshold = 0; // set on setup
int restingFSRVoltage = 0;  // set on setup

//
// CUSTOM EVENTS
//

/**
   Whether or not to ignore any butt input. This should be set to true on sit complete, and then to false when the sound ends.
   @type {Boolean}
*/
bool lockInput = false;   //

/**
 * How long of no activity until triggering onIdle()
 */
// FIXME: idle time is hacked to do 'light breathing'
unsigned long baseIdleTime = 1000;   // how long to wait until idle event, idle time will double each onIdle()
unsigned long currentIdleThreshold = baseIdleTime;   // currentIdleThreshold should be doubled each time onIdle() happens.
unsigned long lastEventTime = 0;
int idleCount = 0;

/**
 *  Called on regular intervals when no interaction in N time....
 */
void onIdle() { 
//  Serial.println("onIdle");  
  breath();
  idleCount = (idleCount+1)%20;
  if(idleCount==0){
    Serial.println("onIdle Bonus Event");
    sendCommand(CMD_PLAY_W_INDEX, 0x00, IDLE_TRACK); // knock 
    fogOn();
  }
}

//void onIdleContinue(){
//  
//}

/**
 * Breath fade handling
 */
boolean breathOut = false;
void breath(){
  if(breathOut){
    Serial.println("to white");
    fadeToColor(0x0f0f0f,1000);
  }else{
    Serial.println("to dark");
    fadeToColor(0x000000,1000);
  }
  breathOut = !breathOut;
//  fogOff(); moved to onResponseComplete
}

/**
   Called about 1 second after a user has sat, long enough to count as haing began sitting. see minimumTimeToBeginSit
   There's a brief dealy to avoid fast repeat lid smacks from triggering this event.
   For a more immediate response see onButtDown()
*/

void onSitStart() {
  Serial.println("onSitStart");  
  fogOff();
}

/**
   Called continuously (every loop), while a person is sitting, if they have sat long enough to trigger a sit, but not long enough to trigger a satisfied sit. see minimumTimeToSatisfaction
   Called immediaetly after onButtContinue().
*/
void onSitContinue() {
  if(cycle%500 == 0){
    Serial.println("onSitContinue");
  }
}

/**
   Called once when the user has sat long enough.
   Called immediately after onSitContinue().
*/
void onSitSatisfied() {
  Serial.println("onSitSatisfied");
  sendCommand(CMD_PLAY_W_INDEX, 0x00, SIT_SATISFIED_TRACK); // ding
  setColor(0xFFFFFF);
  fogOn();
}

/**
   Called once whenon user stands after NOT having sat long enough for satisfaction.
   Called immediately after onButtUp().
*/
void onSitCancel() {
  Serial.println("onSitCancel");
  sendCommand(CMD_STOP_PLAY);
}

/**
   Called once when a user has stood after having sat long enough for satisfaction.
   Called immediately after onButtUp().
*/
void onSitComplete() {
  Serial.println("onSitComplete");
  lockInput = true;  
  sendCommand(CMD_PLAY_W_INDEX, 0x00, SIT_COMPLETE_TRACK); // flush  !! NOTE: onResponseComplete() called once this sound is done!
  fadeToColor(nextColor(),8000);
  setVal('f',100);       // f for flicker
  fadeVal('f',0,6000);  // f for flicker
//  fogOn();
}

/**
   This is called whenever a sound is complete. 
   Based on the track number, a sit event may be triggered.   
*/
void onResponseComplete(uint8_t trackNumber) {
  if (trackNumber == SIT_COMPLETE_TRACK) { // flush track
    lockInput = false;
    lastEventTime = currentTimeMS;
    fadeToColor(0x333333,5000);    
    fogOff();
  }
  if(trackNumber == IDLE_TRACK){  
    fogOff();
  }  
  if(trackNumber == SITTING_TRACK){    
    sitSatisfied = true; 
    onSitSatisfied();
  }
  if(trackNumber == SIT_SATISFIED_TRACK){
    fogOff();
  }
}

// todo: implement; should be called only after the specific onSitComplete response is done.
// void onSitCompleteResponseComplete() {
//   Serial.println("onSitCompletelResponseComplete");
// }


/**
   Called once, immediately when a press/butt is detected.
   Probably want to use onSitStart instead of this event.
   TODO: Consider throttling this so it can't be called multiple times very quickly by someone tapping on the seat.
*/
void onButtDown() {
  Serial.println("onButtDown");
//  if(random(0,100) > 50){
    sendCommand(CMD_PLAY_W_INDEX, 0x00, SITTING_TRACK); // tick-tock  
//  }else{
//    sendCommand(CMD_PLAY_W_INDEX, 0x00, BIRDS_TRACK); // birds
//  }
  
  fadeToColor(0x111111,250); // light up
}

/**
   Called once, when user stops sitting, no matter what.
   Called immediately before onSitCancel or onSitComplete
   TODO: pass in success param?
*/
void onButtUp() {
  Serial.println("onButtUp");
  fadeToColor(0x000000,250); // 
  fogOff();
}

/**
   Called once, when a user stops sitting, if the sit was not long enough to qualify as a sit
   Called immediately after onButtUp.
*/
void onButtUpBeforeSit() {
  Serial.println("onButtUpBeforeSit");
  sendCommand(CMD_STOP_PLAY);  
}

/**
   Called continuously (every loop) while a person is sitting.
   Called before onSitSatisfied.
*/
void onButtContinue() {
  //Serial.println("onButtContinue..."); // Too noisy. It works though.
}



//
// Setup for butt events, should be called in setup()
//

void buttEventsSetup() {
  pinMode(buttPin, INPUT_PULLUP);
  pinMode(FSR_PIN, INPUT);
  pinMode(DIGITAL_LED_TEST_PIN,OUTPUT);
  lastEventTime = millis();
  restingFSRVoltage = analogRead(FSR_PIN) + 15;
  voltageTriggerThreshold = restingFSRVoltage  + 100;
  
  Serial.print("FSR resting voltage measured as: "); Serial.println(restingFSRVoltage);
  Serial.print("Trigger voltage set to: "); Serial.println(voltageTriggerThreshold);
  onIdle();
}

//
// Processing input and emitting buttEvents. Should be called every loop
//

void buttEventProcessing() {  
  
  if (lockInput) { // do nothing if the input is locked.
    return;
  }
  LEDbrightness = 0;

  //
  // read pin(s), debounce readings, set butt flag(s)
  //

  // anlog input
  int fsrADC = analogRead(FSR_PIN);
  analogInputTriggered = (fsrADC > voltageTriggerThreshold);

  if(cycle%500 == 0){ 
    Serial.print("fsr : "); Serial.println(fsrADC);
  }
//return;


  // digital test input
  int discreetButtInput = digitalRead(buttPin);
  if(!discreetButtInput){
      LEDbrightness = 45;
  }else{
      LEDbrightness = map(fsrADC,0,voltageTriggerThreshold,0,65);
  }
  
  buttPinState  = !discreetButtInput || analogInputTriggered;


  // has the pin value changed?
  if (buttPinState != buttPinState_previous) {
    // reset debounce timer
    lastButtDebounceTime = currentTimeMS;
  }

  // is the pinValue different than the derivedState AND
  // has enough time passed for debouncing?
  if ( (currentTimeMS - lastButtDebounceTime ) > buttDebounceDelay) {
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
  if (isButtDown && !wasButtDown) {
    buttDownStartTime = currentTimeMS;
    buttDownTime = 0;
    onButtDown();
  }

  // Continues to have butt down...
  if (isButtDown && wasButtDown) {

    onButtContinue();
    buttDownTime = currentTimeMS - buttDownStartTime;

    // ...buttDown long enough to count as a sit start?
    if (!isSitting && buttDownTime > minimumTimeToBeginSit) {
      // (re)set flags and timer(s)
      isSitting = true;
      sitStartTime = currentTimeMS;
      onSitStart();
    }

    // ... continues to sit (before satisfaction)?
    if (isSitting && wasSitting && !sitSatisfied) {
      sitTime = currentTimeMS - sitStartTime;
      onSitContinue();
    }

    // TODO: continues to sit for some time AFTER sitSatisfied...
    // ...

  
    // ... sat long enough for satisfaction
//    if (sitTime >= minimumTimeToSatisfaction && !sitSatisfied) {
//      sitSatisfied = true;
//      onSitSatisfied();
//    }

  }

  // JUST stood...
  if (!isButtDown && wasButtDown) {
    lastEventTime = currentTimeMS;
    onButtUp();

    // ... real early
    if (!isSitting) {
      onButtUpBeforeSit();
    }

    // ... early
    if (isSitting && !sitSatisfied) {
      isSitting = false;
      onSitCancel();

      // ... after sitting long enough
    } else if (isSitting && sitSatisfied) {

      isSitting = false;
      sitComplete = true;

      onSitComplete();

      sitComplete = false; // reset flags
      sitSatisfied = false;
      sitTime = 0;
    }
  }
  
  if (isSitting) {
    digitalWrite(DIGITAL_LED_TEST_PIN,HIGH);
    LEDbrightness = 255;     // for testing; see light.ino    
  } else if( ((lastEventTime + currentIdleThreshold) <= currentTimeMS) && !lockInput && !isButtDown){
    digitalWrite(DIGITAL_LED_TEST_PIN,LOW);
    lastEventTime = currentTimeMS;
    onIdle();
    //    currentIdleThreshold *= 2; // for exponentially increasing idle calls...
  }

  

  // update state tracking flags for next loop
  wasButtDown = isButtDown; // track for future
  wasSitting = isSitting;
}
