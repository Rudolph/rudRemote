/*
 * rudRemoteCrazyOnly
 * 
 * Implements an R/C style two-stick transmitter for flying a Crazyflie 2.0
 * See https://hackaday.io/project/158068-rudremote for more information
 * 
 * Copyright © 2018 Del Rudolph, https://www.darthrudolph.com/ <del@darthrudolph.com>
 * Released under the MIT License, see included file "LICENSE"
 * 
 * based on jpihet's Crazyflie Arduino Controller:
 * https://github.com/jpihet/crazyflie-arduino-controller
 * which was based on tbitson's Crazyflie Arduino Controller:
 * https://bitbucket.org/tbitson/crazyflie-arduino-controller
 * 
 * I recommend using the TMRh20 RF24 library because that's what I used
 * You can find it in the Arduino library manager by searching for "RF24"
 * and installing the one from TMRh20.
 * 
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <printf.h> // seems it's needed on Uno for printDetails() to work

// send debug info to Serial?
bool DEBUG = 0;

// The analog pins your gimbals are plugged into, and the minimum and
// maximum values for each, as determined by running rudRemoteStickFinder.
// These stick values are for my transmitter and probably won't work for yours!!!
#define lStickX  A9     // left stick, X (horizontal) direction (yaw) (channel 4 in Mode 2)
#define lStickXMin 192
#define lStickXMax 367

#define lStickY  A8     // left stick, Y (vertical) direction (throttle) (channel 3 in Mode 2)
#define lStickYMin 174
#define lStickYMax 363

#define rStickX  A7     // right stick, X (roll) (channel 1 in Mode 2)
#define rStickXMin 158
#define rStickXMax 346

#define rStickY  A6     // right stick, Y (pitch) (channel 2 in Mode 2)
#define rStickYMin 181
#define rStickYMax 365

// Radio channel
// The default channel for Crazyflie 2.0 is 80. Change it here if you've
// modified yours.
#define RADIO_CHANNEL  80

// Init RF24 lib. Pass CE and CSN pins
//RF24 radio(RF24_CE, RF24_CSN);
//RF24 radio(53, 49);   // Mega
RF24 radio(9, 10);      // 168/328 and Teensy 3.2

// shouldn't need to change anything else beyond here

// Port to send control commands
#define PORT_COMMANDER 3
#define PORT_CHANNEL 0

// define the packet we want to send
typedef struct
{
  byte header;
  float roll;
  float pitch;
  float yaw;
  uint16_t  thrust;
} __attribute__((__packed__)) cmdPacket;

// create an instance of the packet
cmdPacket crtp;

// a place to put ack packets
char payload[32];

uint32_t lastLoop;

void setup(){
  Serial.begin(9600);
  printf_begin();           // not needed on Teensy, though apparently doesn't hurt
  //while(!Serial){};       // not needed on Uno (hrm, or Teensy? Sometime?)
  Serial.println("rudRemoteCrazyOnly");
  
  radio.begin();
  
  // define initial values for packet
  crtp.header = (PORT_COMMANDER & 0xF) << 4 | 3 << 2 | (PORT_CHANNEL & 0x03);
  crtp.roll   = 0.0;
  crtp.pitch  = 0.0;
  crtp.yaw    = 0.0;
  crtp.thrust = 0;

  // enable dynamic payloads, ack payloads, set channel, data rate 250K, etc...
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.setChannel(RADIO_CHANNEL);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(5,3);
  radio.setCRCLength(RF24_CRC_16);
  radio.openWritingPipe(0xE7E7E7E7E7LL);

  // we don't need a reading pipe since we just get ackPayloads for each write
  //radio.openReadingPipe(1, 0xE7E7E7E7E7LL);

  // we're a transmitter, no need to listen at all (though this probably doesn't
  // need to be explicitly set)
  radio.stopListening();

  if(DEBUG){
    delay(3000);
    radio.printDetails();
    delay(3000);
  }
}

void loop(){
  if(millis() - lastLoop > 100){
    lastLoop = millis();

    // Get the sticks' readings
    crtp.pitch = map(analogRead(rStickY), rStickYMax, rStickYMin, 30.0, -30.0);
    if(abs(crtp.pitch) < 3)
      crtp.pitch = 0;
    
    crtp.roll = map(analogRead(rStickX), rStickXMax, rStickXMin, 30.0, -30.0);
    if(abs(crtp.roll) < 3)
      crtp.roll = 0;
    
    float yaw = map(analogRead(lStickX), lStickXMin, lStickXMax, -200.0, 200.0);
    if(abs(yaw) < 15)
      yaw = 0;
    crtp.yaw = constrain(yaw, -200.0, 200.0);

    // this should really be 65k instead of 50000, but until I fly better...
    uint16_t t = map(analogRead(lStickY), lStickYMin, lStickYMax, 0, 50000);
    if(t < 10000)
      t = 0;
    crtp.thrust = constrain(t, 0, 50000);
    

    // this way receives ack packets but they're always of zero length
    if(radio.write(&crtp, sizeof(crtp))){
      uint8_t len = radio.getDynamicPayloadSize();
      radio.read(&payload, len);

      if(DEBUG){
        Serial.print("Ack received, size ");
        Serial.println(len);
        if(len > 0){
          for(int i=0;i<=len;i++){
            Serial.print(payload[i]);
          }
          Serial.println("");
        }
      }
    }else{
      // should probably count lost packets or something
      if(DEBUG){
        Serial.println("Write failed");
      }
    }

// the old way that didn't get ack packets
//	  radio.stopListening();
//    radio.write(&crtp, sizeof(crtp));
//    
//    // start listening for an ACK
//    radio.startListening();
//
//    uint32_t start = millis();
//    bool timeout = false;
//
//    while(!radio.available() && !timeout){
//      if(millis() - start > 20){
//        timeout = true;
//      }
//    }
//
//    if(timeout){
//      if(DEBUG){
//        Serial.println("response timed out");
//      }
//    }else{
//      uint8_t len = radio.getDynamicPayloadSize();
//      radio.read(payload, len);
//      if(DEBUG){
//        Serial.print("Got response, size ");
//        Serial.println(len);
//        }
//      }
//    }

    if(DEBUG){
      printCRTPValues();
    }
  }
}

void printCRTPValues()
{
  uint8_t i;
  char *ptr = (char *) &crtp;

  Serial.print("Header = ");
  Serial.print(crtp.header, HEX);
  Serial.print(" Roll = ");
  Serial.print(crtp.roll);
  Serial.print(" Pitch = ");
  Serial.print(crtp.pitch);
  Serial.print(" Yaw = ");
  Serial.print(crtp.yaw);
  Serial.print(" Thrust = ");
  Serial.print(crtp.thrust);

  Serial.print(" - ");
  for (i = 0; i < sizeof(crtp); i++)
    Serial.print(ptr[i] & 0xFF, HEX);
  Serial.println("");
}

