/*
 * rudRemote
 * 
 * Implements an R/C style two-stick transmitter for flying a Crazyflie 2.0 and
 * driving/controlling other robot projects
 * See https://hackaday.io/project/158068-rudremote for more information
 * 
 * Copyright © 2018 Del Rudolph, https://www.darthrudolph.com/ <del@darthrudolph.com>
 * Released under the MIT License, see included file "LICENSE"
 * 
 * Inspired by jpihet's Crazyflie Arduino Controller:
 * https://github.com/jpihet/crazyflie-arduino-controller
 * Which was based on tbitson's Crazyflie Arduino Controller:
 * https://bitbucket.org/tbitson/crazyflie-arduino-controller
 * 
 * I recommend using the TMRh20 RF24 library because that's what I used
 * You can find it in the Arduino library manager by searching for "RF24"
 * and installing the one from TMRh20.
 * 
 */
#include <Encoder.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <U8g2lib.h>
#include "editMe.h"

// get the OLED moving
U8G2_SSD1306_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0);

// shouldn't need to edit anything below here, edit the file "editMe.h" instead!

// init RF24 lib. Pass CE and CSN pins
RF24 radio(CEPIN, CSNPIN);

// add the encoder
// this seemed to make the encoder too punchy
//#define ENCODER_OPTIMIZE_INTERRUPTS
Encoder theEncoder(ENCA,ENCB);

// some vars to hold things
boolean needsDrawn = false;
uint32_t lastDrawn = 0;
int16_t oldCount = 0;
int16_t newCount = 0;
boolean buttonClicked = false;
uint32_t lastLoop = 0;
uint8_t theMode = 0;

// set up CRTP for the Crazyflie 2.0
// port and channel for sending control commands
#define PORT_COMMANDER 3
#define PORT_CHANNEL 0

// define the command packet we want to send to the Crazyflie 2.0
typedef struct
{
  byte header;
  float roll;
  float pitch;
  float yaw;
  uint16_t  thrust;
} __attribute__((__packed__)) cmdPacket;

// create an instance of the CRTP command packet
cmdPacket crtp;

// a place to put ack packets
char payload[32];

/* 
 * and now for the menus
 * 
 * each menu gets four things
 * first, define menu count, how many items are in the menu
 * second, a char array of the menu items, first one is always the "back to
 *  previous menu" entry. e.g. Exit, Main Menu...
 * third and fourth, a couple of notes-to-self about which menu is selected and
 *  which menu page is currently showing
 */
boolean showMenu = false;

// Main Menu
#define mainMenuCount 3
const char *mainMenuItems[mainMenuCount] = { "Exit", "Choose Mode", "Settings" };
uint8_t mainMenuSelected = 0;
uint8_t mainMenuShowing = 0;

// Choose Mode menu
// at startup default to Robot Mode
#define modeMenuCount 3
const char *modeMenuItems[modeMenuCount] = { "Main Menu", "Robot", "Crazy" };
uint8_t modeMenuSelected = 0;
uint8_t modeMenuShowing = 0;

// TODO: make real menu pages that actually do something
#define settingsMenuCount 4
const char *settingsMenuItems[settingsMenuCount] = { "Main Menu", "One", "Two", "Three" };
uint8_t settingsMenuSelected = 0;
uint8_t settingsMenuShowing = 0;

void setup() {
  // set up the digital inputs
  pinMode(encButton, INPUT_PULLUP);
  attachInterrupt(encButton, encButtonPress, FALLING);
  pinMode(channelFive, INPUT_PULLUP);

  // fire up Serial
  Serial.begin(9600);
  #ifdef DEBUG
    while(!Serial){};
  #endif
  Serial.println("rudRemote");
  
  // start the OLED
  u8g2.begin();

  // at boot default to Choose Mode menu
  // TODO: save last used mode to EEPROM and default to that instead
  theEncoder.write(1);
  showMenu = 1;
  mainMenuSelected = 1;
  modeMenuSelected = 1;

  // start up the radio
  radio.begin();
  
  // enable dynamic payloads, ack payloads, data rate 250K, etc...
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(5,3);
  radio.setCRCLength(RF24_CRC_16);
  
  // we're a transmitter, no need to listen at all (though this probably doesn't
  // need to be explicitly called)
  radio.stopListening();

  // define initial values for CRTP command packet
  crtp.header = (PORT_COMMANDER & 0xF) << 4 | 3 << 2 | (PORT_CHANNEL & 0x03);
  crtp.roll   = 0.0;
  crtp.pitch  = 0.0;
  crtp.yaw    = 0.0;
  crtp.thrust = 0;

  #ifdef DEBUG
    delay(3000);
    radio.printDetails();
    delay(3000);
  #endif

}

void loop() {
  // catch encoder input
  static uint32_t oldCount = 0;
  newCount = theEncoder.read();
  if(newCount < 0) newCount = 0;

  // if a menu is being shown
  if(showMenu){
    // and the encoder has been changed, determine which menu gets the encoder input
    if(newCount != oldCount) oldCount = newCount;
    if(mainMenuSelected == 2){
      // update Settings submenu
      if(newCount >= settingsMenuCount) newCount = settingsMenuCount-1;
      settingsMenuShowing = newCount;
      
    }else if(mainMenuSelected == 1){
      // update Mode submenu
      if(newCount >= modeMenuCount) newCount = modeMenuCount-1;
      modeMenuShowing = newCount;
      
    }else{
      // update Main submenu
      if(newCount >= mainMenuCount) newCount = mainMenuCount-1;
      mainMenuShowing = newCount;
      
    }
    theEncoder.write(newCount);
    needsDrawn = true;
  }else{
    // otherwise ignore encoder changes
    newCount = 0;
    oldCount = 0;
    theEncoder.write(0);
  }

  // update the display if something has changed or if timeout has been reached
  if((needsDrawn) or (millis() - lastDrawn > 100)){
    drawScreen();
    lastDrawn = millis();
    needsDrawn = false;
  }

  // if theMode is set and timeout has been reached get sticks' values and 
  // send the appropriate packet
  if((theMode > 0) && (millis() - lastLoop > 100)){
    lastLoop = millis();
    if(theMode == 2){
      // crazy mode
      getCrazySticks();
      sendCrazyPacket();
    }else if(theMode == 1){
      // robot mode
    }

    #ifdef DEBUG
      printCRTPValues();
    #endif
  }
}

