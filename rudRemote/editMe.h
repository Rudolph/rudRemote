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
//#define DEBUG

// the analog pins your gimbals are plugged into
// TODO: make settings page to set min/max values to EEPROM
//       then use the EEPROM values instead
#define lStickX  A9     // left stick, X (horizontal) direction (yaw) (channel 4 in Mode 2)
#define lStickXMin 191
#define lStickXMax 367

#define lStickY  A8     // left stick, Y (vertical) direction (throttle) (channel 3 in Mode 2)
#define lStickYMin 163
#define lStickYMax 372

#define rStickX  A7     // right stick, X (roll) (channel 1 in Mode 2)
#define rStickXMin 157
#define rStickXMax 350

#define rStickY  A6     // right stick, Y (pitch) (channel 2 in Mode 2)
#define rStickYMin 176
#define rStickYMax 373

// the non-gimbal inputs
#define channelFive	24	// toggle switch on left shoulder ("gear")
#define channelSix A3	// potentiometer on right shoulder ("flaps")
#define channelSixMin 204
#define channelSixMax 436

// the encoder pins, swap A and B pins if encoder works backwards
#define ENCA 33			// encoder A
#define ENCB 32			// encoder B
#define encButton 31	// encoder button

// the CE and CSN pins of the NRF24L01+
#define CEPIN 9
#define CSNPIN 10

// the channel and pip of your Crazyflie 2.0
// these are the defaults of the Crazyflie, only change them here if you've modified 
// your Crazyflie's settings
#define CRAZY_CHANNEL 80
#define CRAZY_PIPE 0xE7E7E7E7E7LL

