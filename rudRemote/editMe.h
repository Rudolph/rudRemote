/*
 * rudRemote - editMe.h
 * 
 * pin definitions and settings to be modified
 * 
 * Copyright © 2018 Del Rudolph, https://www.darthrudolph.com/ <del@darthrudolph.com>
 * Released under the MIT License, see included file "LICENSE"
 * 
 */

// uncomment this to spew debugging data to Serial
#define DEBUG

// the analog pins your gimbals are plugged into
// TODO: make settings page to set min/max values to EEPROM
//       then use the EEPROM values instead
#define lStickX  A9     // left stick, X (horizontal) direction ("yaw") (channel 4 in Mode 2)
#define lStickXMin 191
#define lStickXMax 365

#define lStickY  A8     // left stick, Y (vertical) direction ("throttle") (channel 3 in Mode 2)
#define lStickYMin 160
#define lStickYMax 360

#define rStickX  A7     // right stick, X ("roll") (channel 1 in Mode 2)
#define rStickXMin 159
#define rStickXMax 347

#define rStickY  A6     // right stick, Y ("pitch") (channel 2 in Mode 2)
#define rStickYMin 183
#define rStickYMax 368

// the non-gimbal inputs
#define channelFive	24	// toggle switch on left shoulder ("gear")
#define channelSix A3	// potentiometer on right shoulder ("flaps")
#define channelSixMin 205
#define channelSixMax 434

// the encoder pins, swap A and B pins if encoder works backwards
#define ENCA 33			// encoder A
#define ENCB 32			// encoder B
#define encButton 31	// encoder button

// the CE and CSN pins of the NRF24L01+
#define CEPIN 9
#define CSNPIN 10

// the channel and pipe of your Crazyflie 2.0
// these are the defaults of the Crazyflie, only change them here if you've modified 
// your Crazyflie's settings
#define CRAZY_CHANNEL 80
#define CRAZY_PIPE 0xE7E7E7E7E7LL

// the channel and the addresses/pipes to use when speaking to other robots/cars/models
// max of 10 known pipes, for now anyway
// this must exactly match what is in every rudRx to which this rudRemote will speak
#define RUD_CHANNEL 66
uint8_t rudPipes[][6] = {"rudP0", "rudP1", "rudP2", "rudP3", "rudP4",
                         "rudP5", "rudP6", "rudP7", "rudP8", "rudP9"
                        };

// define the command packet we want to send to rudRx-es
// if you have a remote with more channels add them here
// this must exactly match what is in every rudRx to which this rudRemote will speak
typedef struct
{
  byte header;
  int16_t ch1;      // right stick X, aka roll, aka channel 1, -511 to 511
  int16_t ch2;      // right stick Y, aka pitch, aka channel 2, -511 to 511
  uint16_t ch3;     // left stick Y, aka throttle, aka channel 3, 0-1023
  int16_t ch4;      // left stick X, aka yaw, aka channel 4, -511 to 511
  uint8_t ch5;      // the toggle on left shoulder, aka gear, aka channel 5, really just 0 or 1
  uint16_t ch6;     // the pot/lever on right shoulder, aka flaps, aka channel 6, 0-1023
} __attribute__((__packed__)) rudTx;





