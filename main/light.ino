//extern int LEDbrightness;


// FIXME: can't seem use #define for these...
const long WHITE = 0xFFFFFF; // don't put in colors[]
const long NONE = 0x000000; // don't put in colors[]

const long RED = 0xFF0000;
const long GREEN = 0x00FF00;
const long BLUE = 0x0000FF;
const long TEAL = 0xff00FF;
const long PURPLE = 0x0050FF;
const long PINK = 0x00FFFF;
const long ORANGE = 0x64FF00;
const long YELLOW = 0xFFFF00;

#define COLOR_COUNT 8

/**
   A list of colors EXCLUDING WHITE and NONE
*/
const long colors[COLOR_COUNT] = {
  RED,
  GREEN,
  BLUE,
  TEAL,
  PURPLE,
  PINK,
  ORANGE,
  YELLOW,
};
int selectedColor = 0;

long randomColor() {
  return colors[random(0, COLOR_COUNT)];
}

long nextColor() {
  return colors[++selectedColor % COLOR_COUNT];
}

/**
   Used to by setVal and fadeVal to find right index
*/
String labels = "rgbf";

#define ANIMATED_VALUE_COUNT 4

int currentValues[] = {0, 0, 0, 0};
int startValues[] = {0, 0, 0, 0};
int endValues[] = {0, 0, 0, 0};
unsigned long startTimes[] = {0, 0, 0, 0};
unsigned long endTimes[] = {0, 0, 0, 0};

/**
   set the value with flag 'item' to 'val'. See 'String labels'
*/
void setVal(char item, int val) {
  int index = labels.indexOf(item);
  currentValues[index] = val;
  startValues[index] = val;
  endValues[index] = val;
  startTimes[index] = currentTimeMS;
  endTimes[index] = currentTimeMS;
}

/**
   Animte the value with flag 'item' to 'target' over time 'durationMS'. See 'String labels'
*/
void fadeVal(char item, int target, int durationMS) {
  int index = labels.indexOf(item);
  startValues[index] = currentValues[index];
  endValues[index] = target;
  startTimes[index] = currentTimeMS;
  endTimes[index] = currentTimeMS + durationMS;
  Serial.print(item); Serial.print("; "); Serial.print(startValues[index]); Serial.print(" -> "); Serial.println(endValues[index]);
}


/**
   Change the hex color over time from wharever it is at to a target value using the supplied easing.
   Where 0 < easeAmount <= 1.0 , where 1 is instant, and values closer to 0 are slower.
*/
void fadeToColor(long rgb_hex, int durationMS) {
  fadeVal('r', rgb_hex >> 16, durationMS);
  fadeVal('g', (rgb_hex & 0x00ff00) >> 8, durationMS);
  fadeVal('b', rgb_hex & 0x0000ff, durationMS);
}

/**
   Set color values instantly, overriding any fades.
   Call this before fadeToColor to jump to a color and fade to a new one.
*/
void setColor(long rgb_hex) {
  setVal('r', rgb_hex >> 16);
  setVal('g', (rgb_hex & 0x00ff00) >> 8);
  setVal('b', rgb_hex & 0x0000ff);
}


/**
   Configure pins
*/
void lightSetup() {
  pinMode(testLedPin, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}


void lightLoop() {
  for (int i = 0; i < ANIMATED_VALUE_COUNT; i++) {
    if (currentTimeMS < endTimes[i]) {
      // linear interpolation
      float interpolatedValue = map(
                                  currentTimeMS,
                                  startTimes[i], endTimes[i],
                                  startValues[i], endValues[i]);
      currentValues[i] = round(interpolatedValue);

    } else {
      currentValues[i] = endValues[i];
    }
  }
  if(cycle%1000 == 0){
//    Serial.print("r: ");Serial.println(currentValues[0]);
  }


  int8_t finalFlickerAdjust = int8_t(random(-currentValues[3], currentValues[3]));

  byte redOut = constrain(currentValues[0] + finalFlickerAdjust, 0, 255);
  byte greenOut = constrain(currentValues[1] + finalFlickerAdjust, 0, 255);
  byte blueOut = constrain(currentValues[2] + finalFlickerAdjust, 0, 255);

  // write pin values
  analogWrite(RED_PIN, redOut);
  analogWrite(GREEN_PIN, greenOut);
  analogWrite(BLUE_PIN, blueOut);

}
