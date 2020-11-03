// #include "Joystick.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "math.h"

// Joystick_ Joystick;

// set pin numbers for switch, joystick axes, and LED:
const int button1pin = 4;
const int button2pin = 5;
const int button3pin = 6;
const int button4pin = 7;

int button1State = LOW; 
int button2State = LOW; 
int button3State = LOW; 
int button4State = LOW; 

#define keyboardButton2 0x81
#define keyboardButton1 0x20
const char keyboardButton3 = 'r';
const char keyboardButton4 = 'e';

int joystickState = 1;
const int mouseButton = 3;    // input pin for the mouse pushButton
const int xAxis = A0;
const int yAxis = A1;

const char KEY_W = 'w';
const char KEY_A = 'a';
const char KEY_S = 's';
const char KEY_D = 'd';
const char KEY_NULL = ' ';

char currentX = KEY_NULL;
char currentY = KEY_NULL;

// Joystick Parameters
int range = 12; // should be odd //255
int responseDelay = 5;
int threshold = range/4; // must be an int // min is 3 before drift
int center = range / 2; // range /2

// Other stuff for analog to digital mod
int maxVertical = range/2;
//double maxHypotenuse = sqrt(sq(maxVertical) + sq(maxVertical));
double maxHypotenuse = maxVertical;
int maxKeyPress = 2;


void setup() {
  Serial.begin(9600);
//  while (!Serial);
  delay(200);
  Serial.println( "Starting...");

  pinMode(button1pin, INPUT_PULLUP);
  pinMode(button2pin, INPUT_PULLUP);
  pinMode(button3pin, INPUT);
  pinMode(button4pin, INPUT_PULLUP);

  pinMode(mouseButton, INPUT);
  digitalWrite(mouseButton, HIGH);
  Mouse.begin();
  Keyboard.begin();
  // Joystick.begin();
  Serial.println("Mouse, joystick and keyboard modules are initialized");
}

int xaxis = 0;
int yaxis = 0;

void loop() {
  
  // Joystick Press Button
  // 1 is mouse
  // 0 is WASD emulation
  int mouseState = digitalRead(mouseButton);
  if ( mouseState == LOW) {
//    if (!Mouse.isPressed(MOUSE_LEFT)) {
//      Mouse.press(MOUSE_LEFT);
//    }
    Keyboard.releaseAll();
    
    if (!joystickState) {
      joystickState = 1;
    } else {
      range = 12;
      responseDelay = 5;
      threshold = range/4;
      center = range / 2;
      joystickState = 0;
    }
    delay(200);
  }
  else {
//    if (Mouse.isPressed(MOUSE_LEFT)) {
//      Mouse.release(MOUSE_LEFT);
//    }
  }

  // Mechanical Switch Control
  int button1Read = digitalRead(button1pin);
  if (button1State != button1Read){
    if (button1State == LOW) {
      Keyboard.press(keyboardButton1);
      delay(100);
    } else {
      Keyboard.release(keyboardButton1);
    }
    button1State = button1Read;
  }
  
  int button2Read = digitalRead(button2pin);
  if (button2State != button2Read){
    if (button2State == LOW) {
      Keyboard.press(keyboardButton2);
      delay(100);
    } else {
      Keyboard.release(keyboardButton2);
    }
    button2State = button2Read;
  }
 

  int button3Read = digitalRead(button3pin);
  if (button3State != button3Read){
    if (button3State == LOW) {
      Keyboard.press(keyboardButton3);
      delay(100);
    } else {
      Keyboard.release(keyboardButton3);
    }
    button3State = button3Read;
  }
  
  int button4Read = digitalRead(button4pin);
  if (button4State != button4Read){
    if (button4State == LOW) {
      Keyboard.press(keyboardButton4);
      delay(100);
    } else {
      Keyboard.release(keyboardButton4);
    }
    button4State = button4Read;
  }
  
  // Joystick Control
  int xReading = readAxis(A0);
  int yReading = readAxis(A1);
  yReading = 0 - yReading;
  xReading = 0 - xReading;

  xaxis = analogRead(A0);
  xaxis = map(xaxis, 0, 1023, 0, range);
  Joystick.setXAxis(xaxis);

  yaxis = analogRead(A1);
  yaxis = map(yaxis, 0, 1023, 0, range);
  Joystick.setYAxis(yaxis);

  if (joystickState){
    analogToDigital(xReading, yReading);
  } else {
    Mouse.move(xReading, yReading, 0);
  }

  delay(responseDelay);
}

/*
  reads an axis (0 or 1 for x or y) and scales the analog input range to a range
  from 0 to <range>
*/
int readAxis(int thisAxis) {
  int reading = analogRead(thisAxis);
  // map the reading from the analog input range to the output range:
  reading = map(reading, 0, 1023, 0, range);
  int distance = reading - center;
  if ( abs(distance) < threshold) {
    distance = 0;
  }

  return distance;
}

void pressKeys(char xKey, char yKey){
  if (xKey != currentX) {
    if (xKey == KEY_NULL) {
      Keyboard.release(currentX);
      delay(100);
    } else {
      Keyboard.release(currentX);
      delay(100);
      Keyboard.press(xKey);
      delay(100);
    }
    currentX = xKey;
  }

  if (yKey != currentY) {
    if (yKey == KEY_NULL) {
      Keyboard.release(currentY);
      delay(100);
    } else {
      Keyboard.release(currentY);
      delay(100);
      Keyboard.press(yKey);
      delay(100);
    }
    currentY = yKey;
  }
}

void emulateAnalog( double angle, double hypotenuse){
  char xdirection, ydirection;
  int xPressCount, yPressCount;
  angle = (int)angle;
  
  // Case that the joystick is in resting state
  if (hypotenuse == 0){
    xdirection = KEY_NULL;
    ydirection = KEY_NULL;
    pressKeys(xdirection, ydirection);
    return;
  }

  // Calculate number of key presses based off how far from resting state
//  int keyPressCount = (hypotenuse/maxHypotenuse)*maxKeyPress;

  // Determine keys to press and the count per direction
  if (angle >=0 && angle <= 15 || (angle >= 345 && angle <= 360)){
    xdirection = KEY_D;
    ydirection = KEY_NULL;
//    xPressCount = keyPressCount;
//    yPressCount = 0;
  } else if (angle >= 75 && angle <= 105){
    xdirection = KEY_NULL;
    ydirection = KEY_S;
//    xPressCount = 0;
//    yPressCount = keyPressCount;
  } else if (angle >= 165 && angle <= 195){
    xdirection = KEY_A;
    ydirection = KEY_NULL;
//    xPressCount = keyPressCount;
//    yPressCount = 0;
  } else if (angle >= 255 && angle <= 285){
    xdirection = KEY_NULL;
    ydirection = KEY_W;
//    xPressCount = 0;
//    yPressCount = keyPressCount;

  // Diagonal bottom right
  } else if (angle > 15 && angle < 75) {
    xdirection = KEY_D;
    ydirection = KEY_S;
//    if (angle > 0 && angle < 30){
//      xPressCount = keyPressCount*2/3;
//      yPressCount = keyPressCount*1/3;
//    } else if (angle > 60 && angle < 90) {
//      xPressCount = keyPressCount*1/3;
//      yPressCount = keyPressCount*2/3;
//    } else if (angle >= 30 && angle <= 60) {
//      xPressCount = keyPressCount/2;
//      yPressCount = keyPressCount/2;
//    }

  // Diagonal bottom left
  } else if (angle > 105 && angle < 165) {
    xdirection = KEY_A;
    ydirection = KEY_S;
//    if (angle > 90 && angle < 120){
//      xPressCount = keyPressCount*1/3;
//      yPressCount = keyPressCount*2/3;
//    } else if (angle > 150 && angle < 180) {
//      xPressCount = keyPressCount*2/3;
//      yPressCount = keyPressCount*1/3;
//    } else if (angle >= 120 && angle <= 150) {
//      xPressCount = keyPressCount/2;
//      yPressCount = keyPressCount/2;
//    }

  // Diagonal top left
  } else if (angle > 195 && angle < 255) {
    xdirection = KEY_A;
    ydirection = KEY_W;
//    if (angle > 180 && angle < 210){
//      xPressCount = keyPressCount*2/3;
//      yPressCount = keyPressCount*1/3;
//    } else if (angle > 240 && angle < 270) {
//      xPressCount = keyPressCount*1/3;
//      yPressCount = keyPressCount*2/3;
//    } else if (angle >= 210 && angle <= 240) {
//      xPressCount = keyPressCount/2;
//      yPressCount = keyPressCount/2;
//    }

  // Diagonal top right
  } else if (angle > 255 && angle < 345) {
    xdirection = KEY_D;
    ydirection = KEY_W;
//    if (angle > 270 && angle < 300){
//      xPressCount = keyPressCount*1/3;
//      yPressCount = keyPressCount*2/3;
//    } else if (angle > 330 && angle < 360) {
//      xPressCount = keyPressCount*2/3;
//      yPressCount = keyPressCount*1/3;
//    } else if (angle >= 300 && angle <= 330) {
//      xPressCount = keyPressCount/2;
//      yPressCount = keyPressCount/2;
//    }
  }

  pressKeys(xdirection, ydirection);
}

double radToDegrees(double rad) {
  return (rad * 4068) / 71;
}

double convertAngleTo360(double angle){
  if (angle >= 0){
    return angle;
  } else {
    // angle should be negative
    return 360 + angle;
  }
}

void analogToDigital(int x, int y) {

  double angle = radToDegrees(atan2(y,x));
  int intAngle = convertAngleTo360(angle);
//  Serial.print("Angle: ");
//  Serial.println(intAngle);
  
  double hypotenuse = sqrt( sq(x) + sq(y) );
  // prevent diagonals from having more speed than vertical or horizontal
  if (hypotenuse > maxVertical){
    hypotenuse = maxVertical;
  }
//  Serial.print("Hypotenuse: ");
//  Serial.println(hypotenuse);
  
  emulateAnalog(intAngle, hypotenuse);
//  Serial.println("-------------------");
//  Serial.println();
  return;
}
