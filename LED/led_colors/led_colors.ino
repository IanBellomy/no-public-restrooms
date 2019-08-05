/*
  Fade

  This example shows how to fade an LED on pin 9 using the analogWrite()
  function.

  The analogWrite() function uses PWM, so if you want to change the pin you're
  using, be sure to use another PWM capable pin. On most Arduino, the PWM pins
  are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fade
*/

int led = 9;           // the PWM pin the LED is attached to
int redPin = 3;
int greenPin = 5;
int bluePin = 6;
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// the setup routine runs once when you press reset:
void setup() {
  // declare pin 9 to be an output:
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  blue();
  delay(800);
  green();
  delay(800);
  purple(); 
  delay(800);
  pink();
  delay(800);
  orange();
  delay(800);
  yellow();
  delay(800);
  teal();
  delay(800);
}

//red pin full tilt, green and blue 0.
void red () {
  analogWrite(greenPin, 0);
  analogWrite(redPin, 255);
  analogWrite(bluePin, 0);
}

void green () {
  analogWrite(greenPin, 255);
  analogWrite(redPin, 0);
  analogWrite(bluePin, 0);
}

void blue () {
  analogWrite(greenPin, 0);
  analogWrite(redPin, 0);
  analogWrite(bluePin, 255);
}

void teal(){
  analogWrite(greenPin, 255);
  analogWrite(redPin, 0);
  analogWrite(bluePin, 255);
}

void purple () {
  analogWrite(greenPin, 0);
  analogWrite(redPin, 80);
  analogWrite(bluePin, 255);
}

void pink() {
  analogWrite(greenPin, 0);
  analogWrite(redPin, 255);
  analogWrite(bluePin, 255);
}

void orange() {
  analogWrite(greenPin, 100);
  analogWrite(redPin, 255);
  analogWrite(bluePin, 0);
}

void yellow() {
  analogWrite(greenPin, 255);
  analogWrite(redPin, 255);
  analogWrite(bluePin, 0);
}
