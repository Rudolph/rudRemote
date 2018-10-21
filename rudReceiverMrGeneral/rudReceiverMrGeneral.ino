/*
 * rudRemoteMrGeneral
 * 
 * Example robot code that implements a receiver that takes control commands from rudRemote
 * 
 * Copyright © 2018 Del Rudolph, https://www.darthrudolph.com/ <del@darthrudolph.com>
 * Released under the MIT License, see included file "LICENSE"
 * 
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>
#include "editMe.h"
#include <printf.h>

// init RF24 lib. Pass CE and CSN pins
RF24 radio(CEPIN, CSNPIN);

// set up the drive servos
Servo driveLeft;
Servo driveRight;

// some vars to hold things
uint32_t lastRxLoop = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  printf_begin();
  
  // start up the radio
  radio.begin();
  
  // enable dynamic payloads, ack payloads, data rate 250K, etc...
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.setChannel(RUD_CHANNEL);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(5,3);
  radio.setCRCLength(RF24_CRC_16);
  radio.openReadingPipe(1, rudPipes[botID]);

  // Get into listening mode
  radio.startListening();
  radio.writeAckPayload(1,0,0);
  
  // set up the drive servos
  driveLeft.attach(L_SERVO);
  driveLeft.write(L_STOP);
  driveRight.attach(R_SERVO);
  driveRight.write(R_STOP);

  Serial.println("rudReceiver");
  #ifdef DEBUG
    radio.printDetails();
    delay(5000);
  #endif
}

void loop() {
  // create an instance of the received control packet
  rudPacket rudReceived;

  // fill said packet with love
  doRudReceive(&rudReceived);

  // spew debuggy stuff to Serial
  #ifdef DEBUG
    Serial.print("roll: ");
    Serial.print(rudReceived.ch1);
    Serial.print(" pitch: ");
    Serial.println(rudReceived.ch2);
    delay(1000);
  #endif

  // map the incoming data to numbers suitable to write to continuous-rotation servos
  int steering = map(rudReceived.ch1, -511, 511, -90, 90);
  int throttle = map(rudReceived.ch2, -511, 511, 0, 180);

  // and make it work like a tank drive
  int leftMotorSpeed = throttle-steering;
  int rightMotorSpeed = throttle+steering;

  // have to reverse the right-hand servo
  rightMotorSpeed = map(rightMotorSpeed, 0, 180, 180, 0);

  // create a dead spot in the middle
  if(abs(leftMotorSpeed - L_STOP) < 15)
    leftMotorSpeed = L_STOP;
  if(abs(rightMotorSpeed - R_STOP) < 15)
    rightMotorSpeed = R_STOP;

  // and write the positions/speeds to the servos
  driveLeft.write(leftMotorSpeed);
  driveRight.write(rightMotorSpeed);

  // more blathering to Serial
  #ifdef DEBUG
    Serial.print("L: ");
    Serial.print(leftMotorSpeed);
    Serial.print(" R: ");
    Serial.println(rightMotorSpeed);
  #endif
}

// just a function to stop the motors a turnin
void allStop(){
  Serial.println("allStop");
  driveLeft.write(L_STOP);
  driveRight.write(R_STOP);
}

// actually receive the control packet from rudRemote
// try to do this 20 times a second
void doRudReceive(rudPacket *rudRX){
  if(millis() - lastRxLoop >= 50){
    uint8_t buf[32];
    if(radio.available()){
      radio.read(&buf, sizeof(buf));
      uint8_t thePort = ((buf[0] >> 4) & 0xF);
      uint8_t theChannel = buf[0] & 0x3;

      // for now we're just accepting input to the Commander port
      if(thePort == 0x03){
        rudRX = (rudPacket*)(buf+1);
      }else{
        #ifdef DEBUG
          Serial.println("Unknown Port");
        #endif
      }
    }else{
      // if there's no radio available set motor speeds to nothing
      allStop();
      #ifdef DEBUG
        Serial.println("no radio");
      #endif
    }
    lastRxLoop = millis();
  }
}















