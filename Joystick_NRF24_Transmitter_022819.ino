#include  <SPI.h>
//#include  <Wire.h>
//#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>

//joystick-----------------------------------
Servo xservo;
Servo yservo;
int joyX = 33;
int joyY = 2;
int joySW = 22; 
int SWstatus = 2;
#define samples 4
int xservoVal = 0; int xservoValp = 85; int xservoValA = 0; double xservoReal[samples];
int yservoVal = 0; int yservoValp = 40; int yservoValA = 0; double yservoReal[samples];
int s = 0; int writeflag=1;
int xservoSum = 0; int yservoSum = 0; 
int servostate = 2;
int prevstate = 0;
int servoSpeed = 0;
int minValx=5; int maxValx=175;
int minValy=10; int maxValy=85;
int xyservodelay = 50;
int xyservostep = 1;
int servo1delay = 50;     //ms
int servo1step = 2;      //degrees
int servoInterval = 1; 

char msg[7];
unsigned long msg_n[28];
unsigned long linktimer;

//RF24(uint16_t _cepin, uint16_t _cspin, uint16_t sck, uint16_t miso, uint16_t mosi)
RF24 radio(21, 5);

//const uint64_t transmit1 = 0xE8E8F0F0E1LL;
const uint64_t transmit1 = 1000;
const uint64_t receiver1 = 1001;

void setup(void) {
  
  Serial.begin(115200);
  SPI.begin();
  radio.begin();
  radio.setChannel(4);
  radio.setPayloadSize(8);
  radio.setDataRate(RF24_1MBPS);
  radio.printDetails(); 
  radio.openWritingPipe(receiver1);
  
 // radio.setPALevel(RF24_PA_LOW);                      //set the power consumption
 // radio.setRetries(15, 15);                           //no clue what this does

  //pinMode(resetPin1, OUTPUT);
  //digitalWrite(resetPin1, HIGH);
  pinMode(joySW, INPUT_PULLUP);
  digitalWrite(joySW, HIGH);

  
}
void loop(void) {
  
SWstatus = digitalRead(joySW); //delay(200);
if (SWstatus == LOW and servostate == 0){ delay(80); servostate = 2;  Serial.println(String(SWstatus) +" Switch Status, " + "\t\tServostate:\t" + servostate);}
else if (SWstatus == LOW and servostate == 2 ){ delay(80); servostate = 3; Serial.println(String(SWstatus) +" Switch Status, " + "\t\tServostate:\t" + servostate);} 
else if (SWstatus == LOW and servostate == 3 ){ delay(80); servostate = 0; Serial.println(String(SWstatus) +" Switch Status, " + "\t\tServostate:\t" + servostate);} 
else if (SWstatus == LOW and servostate == 1 ){ delay(80); servostate = 0; Serial.println(String(SWstatus) +" Switch Status, " + "\t\tServostate:\t" + servostate); }
msg_n[0] = SWstatus;

 //read joystick
 //delay(xyservodelay);
 xservoVal= analogRead(joyX);
 yservoVal= analogRead(joyY); 

 if(s<samples){xservoSum += xservoReal[s]; yservoSum += yservoReal[s];s++;}
 if(s>=samples){ xservoValA = int(xservoSum/samples); xservoSum = 0; yservoValA = int(yservoSum/samples); yservoSum = 0; s= 0;}
 xservoReal[s] = xservoValp;
 yservoReal[s] = yservoValp;
 
 // if((timer%4)==0){
  Serial.print("\n");
  Serial.print("X-axis: ");
  Serial.print(xservoValp); Serial.print("\t\t"); Serial.print(xservoVal); Serial.print("\t\t"); Serial.print(xservoValA);Serial.print("\t\t");
  Serial.print("\t\t");
  Serial.print("Y-axis: ");
  Serial.print(yservoValp);Serial.print("\t\t"); Serial.print(yservoVal); Serial.print("\t\t"); Serial.print(yservoValA);Serial.println("\t\t");
  Serial.print("\n\n  xservodelay:");
  Serial.println(xyservodelay);
 // }
  //------------
  //if(xservoVal == 4095 and yservoVal == 4095){ writeflag = 0;}
  
  if (servostate == 2){
    int i;
               
      if ((xservoVal >= 0 and xservoVal <= 2400) or (xservoVal >=3100)) {
        xservoValp = int(map(xservoVal, 0,4095, minValx, maxValx));
       delay(xyservodelay); writeflag = 1; 
      //xservoReal[s] = map(xservoValp+5,minValx,maxValx, 0, 4095);
      //xservoValp = i+1;
      msg_n[1] = xservoVal; msg_n[2] = xservoValA;
      }
      if (xservoVal == 4095){
       delay(xyservodelay); writeflag = 1;
      //xservoReal[s] = map(xservoValp-5,minValx,maxValx, 0, 4095);
      //xservoValp = i-1; 
      msg_n[1] = xservoVal; msg_n[2] = xservoValA;
      }
    
    if ((yservoVal>= 0 and yservoVal <= 2700) or (yservoVal>= 2900 and yservoVal <= 4095)) { //and (xservoValp != 0)){
      yservoValp = int(map(yservoVal,4095, 0,minValy, maxValy));
      delay (150); msg_n[3] = yservoVal; msg_n[4] = yservoValp;
      //yservo.write(int((yservoValp+yservoValA)/2)); delay (120);
    }
  
  msg_n[5] = xservoSum; msg_n[6] = yservoSum;  delay(70);
  }
  //--------------------------------------------------------------------
  Serial.println("sending ...");

    linktimer = millis();
    bool link = radio.write(&linktimer,4); delay (20);
    bool ok = radio.write(&msg_n[0], 2); //delay(100);
    bool ok1 = radio.write(&msg_n[1], 2);// delay(100);
    bool ok2 = radio.write(&msg_n[2], 2);// delay(100);
    bool ok3 = radio.write(&msg_n[3], 2);//delay(100);
    bool ok4 = radio.write(&msg_n[4], 2);// delay(100);
    bool ok5 = radio.write(&msg_n[5], 2);// delay(100);
    bool ok6 = radio.write(&msg_n[6], 2);// delay(100);
    delay(70);

}
