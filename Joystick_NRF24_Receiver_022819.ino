#include <SPI.h>
//#include "nRF24L01.h"
#include <RF24.h>
//#include <Wire.h>
#include <Servo.h>

int servoPin;
int xservoPin = 27; //blue
int yservoPin = 22; //white
int servostate = 2;
int prevstate = 0;
int servoSpeed = 0;
int minValx=5; int maxValx=175;
int minValy=10; int maxValy=85;
int xyservodelay = 59;
int xyservostep = 1;
int servo1delay = 50;     //ms
int servo1step = 2;      //degrees
int servoInterval = 1;  //whole seconds
int oldAngleX = 0;
int oldAngleY = 0;
int Angle = 0; 
int PWM_WIDTH = 20000;
int motorpin = 27; //orange
int flag =0;

#define samples 8
int xservoVal = 0; int xservoValp = 85; int xservoValA = 0; double xservoReal[samples];
int yservoVal = 0; int yservoValp = 40; int yservoValA = 0; double yservoReal[samples];

char msg[7];
unsigned long msg_n[7];
unsigned long linktimer; unsigned long linktimerOld;

//RF24 radio(12, 14, 26, 25, 27);
RF24 radio(21, 5);
Servo xservo; Servo yservo;

const uint64_t receiver1 = 1001;
const uint64_t transmit1 = 1000;
int linktimerDiff = 0;

void setup(void){
 Serial.begin(115200);
 SPI.begin();
 radio.begin();
 radio.setChannel(4);
 radio.setPayloadSize(8);
 radio.setDataRate(RF24_1MBPS);
 radio.openReadingPipe(1,receiver1);
 radio.printDetails(); 
 radio.startListening();

 xservo.attach(xservoPin); yservo.attach(yservoPin);
}

void loop(void){
 int i;
if (radio.available()){  
  radio.read(&linktimer, 4);
  Serial.print("linktimer:\t"); Serial.println(linktimer);
  if (linktimer > linktimerOld){
  delay(10);
  Serial.println("new data...");
  for(i =0; i<= 6; i++){
     radio.read(&msg_n[i], 2); delay(20);    
     
     if(linktimerDiff==0){
     xservoVal = msg_n[2]; xservoValp = map(xservoVal,0,4095,5,175); xservo.write(xservoValp);
     yservoVal = msg_n[4]; yservoValp = map(yservoVal,0,4095,5,175); yservo.write(yservoValp);}
      
     Serial.print(msg_n[i]); Serial.print("\t");
     }     
     delay(80);
     //Serial.println(msg_n[0]); Serial.println(msg_n[1]);Serial.println(msg_n[2]);
     //if((millis()%2)==0){Serial.println("receiving...");}
      
      if((millis()%3)==0){Serial.print("linktimerDiff:\t"); Serial.println(linktimerDiff);
      Serial.print(xservoVal); Serial.print(":X\tY:"); Serial.println(yservoVal);
      Serial.print(xservoValp); Serial.print(":X\tY:"); Serial.println(yservoValp);
      }  
      }
 }
 else{
  if((millis()%750)==0){Serial.print("No radio available");}
 }
 linktimerDiff = linktimer-linktimerOld;
 linktimerOld = linktimer;
}
