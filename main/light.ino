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
 * A list of colors EXCLUDING WHITE and NONE
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

long randomColor(){
  return colors[random(0,COLOR_COUNT)];
}

long nextColor(){
  return colors[++selectedColor % COLOR_COUNT];
}


/**
 * Each loop, we move thecolor values closer to these values.
 */
byte targetRed   = 0;
byte targetGreen = 0;
byte targetBlue  = 0;

/**
 * The current color values;
 */

byte redValue   = 0;
byte greenValue = 0;
byte blueValue  = 0;

/**
 * Used in tweening the color values. 
 */
float currentEaseValue = 0.2;

/**
 * Change the r,g,b color over time from whatever it's at to the target value using the supplied easing. 
 * Where 0 < easeAmount <= 1.0 , where 1 is instant, and values closer to 0 are slower. 
 */
void fadeToColor(byte r, byte g, byte b,float ease){  
  currentEaseValue = ease; //((ease <= 0.0) ? 0.0001 : ease); // don't accept 0 or less.    
  Serial.print("fade to color: ");
  Serial.print(r);
  Serial.print(' ');
  Serial.print(g);
  Serial.print(' ');
  Serial.print(b);
  Serial.print(" , ease: ");
  Serial.println(ease);
  targetRed = r;
  targetGreen = g;
  targetBlue = b;
}

/**
 * Change the hex color over time from wharever it is at to a target value using the supplied easing. 
 * Where 0 < easeAmount <= 1.0 , where 1 is instant, and values closer to 0 are slower. 
 */
void fadeToColor(long rgb_hex, float ease){  
  fadeToColor(
    rgb_hex >> 16,
    (rgb_hex & 0x00ff00) >> 8,
    rgb_hex & 0x0000ff,
    ease
    );
}
/**
 * Set color values instantly, overriding any fades.
 * Call this before fadeToColor to jump to a color and fade to a new one.
 */
void setColor(byte r,byte g,byte b){
  redValue = r;
  greenValue = g;
  blueValue = b;
  targetRed = r;
  targetGreen = g;
  targetBlue = b;
}

void setColor(long rgb_hex){
  redValue = rgb_hex >> 16;
  greenValue = (rgb_hex & 0x00ff00) >> 8;
  blueValue = rgb_hex & 0x0000ff;
  targetRed = redValue;
  targetGreen = greenValue;
  targetBlue = blueValue;
}


 
/**
 * Configure pins
 */
void lightSetup(){
  
  pinMode(testLedPin, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT); 


//   Serial.println(roundAwayFromZero(-0.3));
//   Serial.println(roundAwayFromZero(0.3));
//   Serial.println(roundAwayFromZero(-0.7));
//   Serial.println(roundAwayFromZero(0.7));   
}

float roundAwayFromZero(float val){
  return ceil(fabs(val)) * fabs(val)/val;
}

/**
 * Move the color values closer to the target values, then set the actual pin outs.
 */
float deltaRed = 0;
float deltaGreen = 0;
float deltaBlue= 0;

void lightLoop(){  
 
  deltaRed = float(targetRed - redValue) * currentEaseValue;
  deltaGreen = float(targetGreen - greenValue) * currentEaseValue;
  deltaBlue = float(targetBlue - blueValue) * currentEaseValue;

  if(targetRed!=redValue){
//    Serial.print("tr: "); Serial.print(targetRed);
//    Serial.print(" r: "); Serial.print(redValue);
//    Serial.print(" red dif: ");      
//    Serial.println(deltaRed);
//    Serial.print(" rounded dif: ");  
//    Serial.println(byte(roundAwayFromZero(redValue)));    
  }

  redValue += byte(roundAwayFromZero(deltaRed));
  greenValue += byte(roundAwayFromZero(deltaGreen));
  blueValue += byte(roundAwayFromZero(deltaBlue));
  
  // write pin values
  analogWrite(RED_PIN, redValue);
  analogWrite(GREEN_PIN, greenValue);  
  analogWrite(BLUE_PIN, blueValue);

//  Serial.print("target: ");
//  Serial.print(targetRed);
//  Serial.print(",");
//  Serial.print(targetGreen);
//  Serial.print(",");
//  Serial.println(targetBlue);
//
//  
//
//  Serial.print("actual: ");
//  Serial.print(redValue);
//  Serial.print(",");
//  Serial.print(greenValue);
//  Serial.print(",");
//  Serial.println(blueValue);

  


  //
  analogWrite(testLedPin, LEDbrightness);
}
