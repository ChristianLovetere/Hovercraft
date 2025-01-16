#include <Servo.h>
#include "Hovercraft.h"
;

//pwm: 3,5,6,9,10,11
const byte liftChannelPin = 2;
const byte leftChannelPin = 4;
const byte rightChannelPin = 7;

const byte liftPWMPin = 3;
const byte leftPWMPin = 5;
const byte rightPWMPin = 6;

Servo liftEsc, leftEsc, rightEsc;

unsigned long previousMillis;
const unsigned long serialInterval = 2000;

circBuffer liftBuf, leftBuf, rightBuf;

void setup() {
  pinMode(liftChannelPin, INPUT);
  pinMode(leftChannelPin, INPUT);
  pinMode(rightChannelPin, INPUT);
  
  Serial.begin(9600);
  
  liftEsc.attach(liftPWMPin); //attach lift ESC to PWM pin 3
  leftEsc.attach(leftPWMPin); //attach left ESC to PWM pin 5
  rightEsc.attach(rightPWMPin); //attach right ESC to PWM pin 6
}

void loop() {
   
  liftBuf.append(pulseIn(liftChannelPin, HIGH));
  leftBuf.append(pulseIn(leftChannelPin, HIGH));
  rightBuf.append(pulseIn(rightChannelPin, HIGH));
  
  int liftChannelPulse = avgArr(liftBuf.buf, liftBuf.getBufSize());
  int rightChannelPulse = avgArr(rightBuf.buf, rightBuf.getBufSize());
  int leftChannelPulse = avgArr(leftBuf.buf, leftBuf.getBufSize());

  //int liftPWM = tieredMap(liftChannelPulse, 1090, 1860, 1000, 2000); //digital conversion
  int leftPWM = tieredMap(leftChannelPulse, 1500, 1900, 1000, 1600, 4);
  int rightPWM = tieredMap(rightChannelPulse, 1500, 1900, 1000, 1600, 4);
  
  int liftPWM = map(liftChannelPulse, 1090, 1860, 1000, 2000); // analog conversion 
  //int leftPWM = map(leftChannelPulse, 1500, 1900, 1000, 1400);
  //int rightPWM = map(rightChannelPulse, 1500, 1900, 1000, 1400); 

  normalizePWM(liftChannelPulse, &liftPWM);
  normalizePWM(leftChannelPulse, &leftPWM);
  normalizePWM(rightChannelPulse, &rightPWM);
  
  liftEsc.writeMicroseconds(liftPWM);
  leftEsc.writeMicroseconds(leftPWM);
  rightEsc.writeMicroseconds(rightPWM);

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= serialInterval) {
    previousMillis = currentMillis;
    
    Serial.print("lift raw: ");
    Serial.println(liftChannelPulse);
    Serial.print("left raw: ");
    Serial.println(leftChannelPulse);
    Serial.print("right raw: ");
    Serial.println(rightChannelPulse);

    Serial.println(" ");
    
    Serial.print("liftPWM: ");
    Serial.println(liftPWM);
    Serial.print("leftPWM: ");
    Serial.println(leftPWM);
    Serial.print("rightPWM: ");
    Serial.println(rightPWM);

    leftBuf.printBuf();
    rightBuf.printBuf(); 
    Serial.println(leftPWM - rightPWM);
    Serial.println("-----------------------------------------------------------------");
  }
  
  delay(10);
}

void normalizePWM(int channelPulse, int * PWM){
  if (channelPulse == 0)
    *PWM = 0;
  else if (*PWM > 2000)
    *PWM = 2000;
  else if (*PWM < 1000)
    *PWM = 1000;
}

int avgArr(int* arr, int arrLength){
  int avg = 0;
  for(int i = 0; i < arrLength; i++) //add all values of the array to avg
    avg += arr[i];
  avg /= arrLength; //divide by number of entries to get average
  return avg;
}

int tieredMap(int inValue, int inLow, int inHigh, int outLow, int outHigh, int numTiers){

  int outValue;
  int tiers[numTiers];
  for(int i = 0; i < numTiers; i++){
    tiers[i] = (inLow*(numTiers-i-1) + inHigh*i)  / (numTiers-1);
    if (inValue <= tiers[i]) {
      outValue = map(tiers[i], inLow, inHigh, outLow, outHigh);
      break;
    }
  }

  return outValue;
}
