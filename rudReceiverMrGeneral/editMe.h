/*
 * rudReceiver - editMe.h
 * 
 * pin definitions and settings to be modified
 * 
 * Copyright © 2018 Del Rudolph, https://www.darthrudolph.com/ <del@darthrudolph.com>
 * Released under the MIT License, see included file "LICENSE"
 * 
 */

// uncomment to spew debugging data to Serial
//#define DEBUG

// which of the 10 known pipes belongs to this robot
#define botID 0

// the actual stop points, i.e. center, of the drive servos
#define L_STOP 93    // bigger is reverse
#define R_STOP 82    // bigger is forward

// the pins to which the servos connect
#define L_SERVO 7
#define R_SERVO 8

// the CE and CSN pins of the NRF24L01+
#define CEPIN 9
#define CSNPIN 10

// the channel and the addresses/pipes to use when speaking to other robots/cars/models
// max of 10 known pipes, for now anyway
// this must exactly match what is in the rudRemote that will be speaking to this rudReceiver
#define RUD_CHANNEL 66
const uint8_t rudPipes[][6] = {"rudP0", "rudP1", "rudP2", "rudP3", "rudP4",
                               "rudP5", "rudP6", "rudP7", "rudP8", "rudP9"
                              };

// define the command packet we will be receiving from rudRemote
// if you have a remote with more channels add them here
// this must exactly match what is in the rudRemote that will be speaking to this rudReceiver
typedef struct
{
  byte header;
  int16_t ch1;      // right stick X, aka roll, aka channel 1, -511 to 511
  int16_t ch2;      // right stick Y, aka pitch, aka channel 2, -511 to 511
  uint16_t ch3;     // left stick Y, aka throttle, aka channel 3, 0-1023
  int16_t ch4;      // left stick X, aka yaw, aka channel 4, -511 to 511
  uint8_t ch5;      // the toggle on left shoulder, aka gear, aka channel 5, really just 0 or 1
  uint16_t ch6;     // the pot/lever on right shoulder, aka flaps, aka channel 6, 0-1023
} __attribute__((__packed__)) rudPacket;








