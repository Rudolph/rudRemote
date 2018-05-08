/*
 * crazyTestTransmitter
 * 
 * based on jpihet's Crazyflie Arduino Controller:
 * https://github.com/jpihet/crazyflie-arduino-controller
 * which was based on tbitson's Crazyflie Arduino Controller:
 * https://bitbucket.org/tbitson/crazyflie-arduino-controller
 * 
 * I recommend using the Tmrh20 RF24 library because that's what I used
 * You can find it in the Arduino library manager
 * 
 * Released under the MIT License, see included file "LICENSE"
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <printf.h> // seems it's needed on Uno for printDetails() to work

// send debug info to Serial?
bool $DEBUG = 0;

// the analog pins your gimbals are plugged into
#define lStickX  A0     // left stick, X (horizontal) direction
#define lStickY  A1     // left stick, Y (vertical) direction
#define rStickX  A2     // right stick, X
#define rStickY  A3     // left stick, Y

// Radio channel
// My Crazyflie 2.0 is on channel 80. I have no idea if yours is too,
// though I believe that is the default channel.
#define RADIO_CHANNEL  80

// Init RF24 lib. Pass CE and CSN pins
//RF24 radio(RF24_CE, RF24_CSN);
//RF24 radio(53, 49);   // Mega
RF24 radio(9, 10);      // 168/328 and Teensy 3.2

// shouldn't need to change anything else beyond here

// Port to send control commands
#define PORT_COMMANDER 3

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

// TODO: create a union with cmpPacket and byte array
// send & receive array. Lame way to send struct
char payload[15];

uint32_t lastLoop;

void setup(){
  Serial.begin(9600);
  printf_begin();           // not needed on Teensy, though apparently doesn't hurt
  //while(!Serial){};       // not needed on Uno (hrm, or Teensy? Sometime?)
  Serial.println("crazyTestTransmitter");
  
  radio.begin();
  
  // define initial values for packet
  crtp.header = (PORT_COMMANDER & 0xF) << 4 | 3 << 2 | (RADIO_CHANNEL & 0x03);
  crtp.roll   = 0.0;
  crtp.pitch  = 0.0;
  crtp.yaw    = 0.0;
  crtp.thrust = 0;

  // enable dynamic payloads, channel, data rate 250K
  radio.enableDynamicPayloads();
  radio.setChannel(RADIO_CHANNEL);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(5,3);
  radio.setCRCLength(RF24_CRC_16);
  radio.openWritingPipe(0xE7E7E7E7E7LL);
  radio.openReadingPipe(1, 0xE7E7E7E7E7LL);

  delay(3000);

  if($DEBUG){
    radio.printDetails();
  }
}

void loop(){
  if(millis() - lastLoop > 100){
    lastLoop = millis();

    // test gimbals
//    crtp.pitch = map(analogRead(rStickY), 0, 1023, 30.0, -30.0);
//    if(abs(crtp.pitch) < 3)
//      crtp.pitch = 0;
//    
//    crtp.roll = map(analogRead(rStickX), 930, 60, 30.0, -30.0);
//    if(abs(crtp.roll) < 3)
//      crtp.roll = 0;
//    
//    float yaw = map(analogRead(lStickX), 0, 1023, -200.0, 200.0);
//    if(abs(yaw) < 10)
//      yaw = 0;
//    crtp.yaw = constrain(yaw, -200.0, 200.0);
//
//    uint16_t t = map(analogRead(lStickY), 0, 996, 0, 50000);
//    if(t < 10000)
//      t = 0;
//    crtp.thrust = constrain(t, 0, 50000);


    // Tower Hobbies radio gimbals
    crtp.pitch = map(analogRead(rStickY), 365, 181, 30.0, -30.0);
    if(abs(crtp.pitch) < 3)
      crtp.pitch = 0;
    
    crtp.roll = map(analogRead(rStickX), 346, 158, 30.0, -30.0);
    if(abs(crtp.roll) < 3)
      crtp.roll = 0;
    
    float yaw = map(analogRead(lStickX), 173, 354, -200.0, 200.0);
    if(abs(yaw) < 15)
      yaw = 0;
    crtp.yaw = constrain(yaw, -200.0, 200.0);
    
    uint16_t t = map(analogRead(lStickY), 174, 363, 0, 50000);
    if(t < 10000)
      t = 0;
    crtp.thrust = constrain(t, 0, 50000);
    
    radio.stopListening();
    memcpy(&payload, &crtp, sizeof(crtp));
//    if(!radio.write(payload, sizeof(crtp))){
//      Serial.print("Error sending");
//    }
    radio.write(&crtp, sizeof(crtp));
    
    // start listening for an ACK
    radio.startListening();

    uint32_t start = millis();
    bool timeout = false;

    while(!radio.available() && !timeout){
      if(millis() - start > 20){
        timeout = true;
      }
    }

    if(timeout){
      if($DEBUG){
        Serial.println("response timed out");
      }
    }else{
      uint8_t len = radio.getDynamicPayloadSize();
      radio.read(payload, len);
      if($DEBUG){
        Serial.print("Got response, size ");
        Serial.println(len);
      }
    }

    if($DEBUG){
      printCRTPValues();
      delay(1000);
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

