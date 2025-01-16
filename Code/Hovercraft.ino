//Communicates with a hovercraft. Writes PWM values between 1000 and 2000 to ESCs to control drone motors. the Lift motor fills the bag on the bottom of the hovercraft. The left and right
//motors are for forward movement. Of course, powering the left more than right causes a right turn and vice versa. This code expects values from the analog sticks of an NX6 drone controller.
//As such, the range of microsecond values from the receiver is typically about 1100 to 2000 us. This implementation assumes the arduino is not being used directly for RX and instead is wired
//to the AR637T receiver used with the controller.

#include <Servo.h>
#include "Hovercraft.h"

#define LIFT_US_MIN 1090
#define LIFT_US_MAX 1860
#define LR_US_MIN 1500
#define LR_US_MAX 1900
#define ESC_PWM_MIN 1000
#define ESC_PWM_MAX 2000
#define NUM_TIERS 4

//#define DEBUG //comment me out to disable debug output to Serial monitor.

;

//UNO pwm pins: 3,5,6,9,10,11
const byte liftChannelPin = 2;
const byte leftChannelPin = 4;
const byte rightChannelPin = 7;

const byte liftPWMPin = 3; //lift ESC connected to pin 3 as signal
const byte leftPWMPin = 5; //left ESC connected to pin 5 as signal
const byte rightPWMPin = 6; //right ESC connected to pin 6 as signal

Servo liftEsc, leftEsc, rightEsc;
circBuf liftBuf, leftBuf, rightBuf;

//vars used for printing info to terminal
#ifdef DEBUG
unsigned long previousMillis;
const unsigned long serialInterval = 2000;
#endif //DEBUG

//initialize pins, Serial, ESCs
void setup() {
  pinMode(liftChannelPin, INPUT);
  pinMode(leftChannelPin, INPUT);
  pinMode(rightChannelPin, INPUT);

  #ifdef DEBUG
  Serial.begin(9600);
  #endif //DEBUG
  
  liftEsc.attach(liftPWMPin); //attach lift ESC to PWM pin 3
  leftEsc.attach(leftPWMPin); //attach left ESC to PWM pin 5
  rightEsc.attach(rightPWMPin); //attach right ESC to PWM pin 6
}

//loop to determine current values requested fom controller and send them to ESCs
void loop() {
   
  liftBuf.append(pulseIn(liftChannelPin, HIGH));
  leftBuf.append(pulseIn(leftChannelPin, HIGH));
  rightBuf.append(pulseIn(rightChannelPin, HIGH));
  
  int liftChannelPulse = avgArr(liftBuf.buf, liftBuf.getBufSize()); //pulses are the average of the entire circular buffer's entries
  int rightChannelPulse = avgArr(rightBuf.buf, rightBuf.getBufSize());
  int leftChannelPulse = avgArr(leftBuf.buf, leftBuf.getBufSize());

  //conversions here: convert from controller uS to PWM. uncomment one of each to choose tiered values or fully gradual
  //TIERED:
  //int liftPWM = tieredMap(liftChannelPulse, LIFT_US_MIN, LIFT_US_MAX, ESC_PWM_MIN, ESC_PWM_MAX, NUM_TIERS);
  int leftPWM = tieredMap(leftChannelPulse, LR_US_MIN, LR_US_MAX, ESC_PWM_MIN, ESC_PWM_MAX, NUM_TIERS);
  int rightPWM = tieredMap(rightChannelPulse, LR_US_MIN, LR_US_MAX, ESC_PWM_MIN, ESC_PWM_MAX, NUM_TIERS);

  //GRADUAL (ANALOG):
  int liftPWM = map(liftChannelPulse, LIFT_US_MIN, LIFT_US_MAX, ESC_PWM_MIN, ESC_PWM_MAX);
  //int leftPWM = map(leftChannelPulse, LR_US_MIN, LR_US_MAX, ESC_PWM_MIN, ESC_PWM_MAX);
  //int rightPWM = map(rightChannelPulse, LR_US_MIN, LR_US_MAX, ESC_PWM_MIN, ESC_PWM_MAX); 

  normalizePWM(liftChannelPulse, &liftPWM);
  normalizePWM(leftChannelPulse, &leftPWM);
  normalizePWM(rightChannelPulse, &rightPWM);
  
  liftEsc.writeMicroseconds(liftPWM);
  leftEsc.writeMicroseconds(leftPWM);
  rightEsc.writeMicroseconds(rightPWM);

  //begin Serial output for debugging values and conversions:
  #ifdef DEBUG
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
  #endif //DEBUG
  //end Serial output for debugging
  
  delay(10);
}

//if mapping from uS to PWM leads to a number below min or above max, set to min or max respectively
void normalizePWM(int channelPulse, int * PWM){
  if (channelPulse == 0)
    *PWM = 0;
  else if (*PWM > 2000)
    *PWM = 2000;
  else if (*PWM < 1000)
    *PWM = 1000;
}

//given an integer array and its size, return the average of all elements in the array
int avgArr(int* arr, int arrLength){
  int avg = 0;
  for(int i = 0; i < arrLength; i++) //add all values of the array to avg
    avg += arr[i];
  avg /= arrLength; //divide by number of entries to get average
  return avg;
}

//given the value in, its low and high bounds, the low and high bounds of the conversion, and the number of desired tiers, create a tiered conversion.
//for example: 4 tiers means the first 25% of the range of values is all the same outcome, then from 25 to 50%, and so on
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
