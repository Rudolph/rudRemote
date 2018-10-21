/*
 * rudRemote - display.ino
 * 
 * holds various functions for drawing on the OLED
 * 
 * Font Groups: https://github.com/olikraus/u8g2/wiki/fntgrp
 * header: https://github.com/olikraus/u8g2/wiki/fntgrpcodeman38
 *    u8g2_font_pressstart2p_8f
 * clock: https://github.com/olikraus/u8g2/wiki/fntgrpx11#5x7
 *    u8g2_font_5x7_tf
 * icons: https://github.com/olikraus/u8g2/wiki/fntgrpiconic
 * 
 * Copyright © 2018 Del Rudolph, https://www.darthrudolph.com/ <del@darthrudolph.com>
 * Released under the MIT License, see included file "LICENSE"
 * 
 */

 // the thing that draws the screen
void drawScreen(){
  u8g2.firstPage();
  do {
    if(showMenu){
      // which menu to show?
      if(mainMenuSelected == 2){
        // showing Settings menu
        // TODO: actually make this do something
        drawHead(mainMenuItems[2]);
        drawSubHead(settingsMenuItems[settingsMenuShowing], settingsMenuShowing, settingsMenuCount-1);
        
        if(buttonClicked){
          if(settingsMenuShowing > 0){
            settingsMenuSelected = settingsMenuShowing;
            theEncoder.write(settingsMenuSelected);
          }else{
            settingsMenuSelected = 0;
            mainMenuSelected = 0;
            mainMenuShowing = 2;
            theEncoder.write(2);
          }
          buttonClicked = false;
        }
        
      }else if(mainMenuSelected == 1){
        // showing Mode menu
        drawHead(mainMenuItems[1]);
        drawSubHead(modeMenuItems[modeMenuShowing], modeMenuShowing, modeMenuCount-1);
        u8g2.setFont(u8g2_font_5x8_tr);
        if(modeMenuShowing == 2){
          // crazy mode page
          // TODO: make a quadcopter xbm
          u8g2.drawStr(0,24,"Crazyflie 2.0");
        }else if(modeMenuShowing == 1){
          // robot mode page
          // TODO: make a robot xbm
          u8g2.drawStr(0,24,"Robot control");
        }else{
          u8g2.drawStr(0,24,"Back to Main Menu");
        }
        if(buttonClicked){
          if(modeMenuShowing > 0){
            theMode = modeMenuShowing;
          }

          if(theMode == 2){
            // if crazy mode, get radio set up
            setCrazyRadio();
          }else if(theMode == 1){
            // if robot mode, set up radio on that channel
            // TODO: make this so you can choose a pipe (i.e. robot) to talk to
            setRudRx(0);
          }
          
          mainMenuSelected = 0;
          mainMenuShowing = 1;
          modeMenuSelected = 0;
          modeMenuShowing = 0;
          buttonClicked = false;
          theEncoder.write(1);
        }
      }else{
        // showing Main Menu
        drawHead("rudRemote");
        drawSubHead(mainMenuItems[mainMenuShowing], mainMenuShowing, mainMenuCount-1);
        
        u8g2.setFont(u8g2_font_5x8_tr);
        if(mainMenuShowing == 2){
          // change settings
          u8g2.drawStr(0,24,"Modify Settings");
        }else if(mainMenuShowing == 1){
          // choose a mode (robot or crazy)
          u8g2.drawStr(0,24,"Choose operating mode");
        }else{
          // exit menu, back to default screen
          u8g2.drawStr(0,24,"Exit Menu");
        }
        
        if(buttonClicked){
          if(mainMenuShowing == 0){
            // exit Main Menu
            showMenu = false;
          }
          mainMenuSelected = mainMenuShowing;
          buttonClicked = false;
          theEncoder.write(1);
        }
      }

    }else{
      // not showing menu, show the main radio page
      drawHead("rudRemote");
      
      if(theMode == 0){
        drawSubHead("Mode Unset",0,0);
      }else{
        char defaultSubHead[12];
        char firstPart[6] = "Mode:";
        sprintf(defaultSubHead, "%s %s",firstPart,modeMenuItems[theMode]);
        drawSubHead(defaultSubHead,0,0);
      }

      // TODO: More, better looking, information on the main page
      u8g2.setFont(u8g2_font_5x8_tr);
      if(theMode == 2){
        u8g2.drawStr(0,24,"Flyin' a Crazyflie");
      }else if(theMode == 1){
        u8g2.drawStr(0,24,"Drivin' a robot");
      }else{
        u8g2.drawStr(0,24,"Mode not set");
        u8g2.drawStr(0,32,"Click for Menu");
      }
    }
  } while(u8g2.nextPage());
}

/* 
 * draws the first row of the display
 * argument is the text to display in the top-left corner of the OLED
 */
void drawHead(const char* theText){
  u8g2.setFont(u8g2_font_pressstart2p_8r);
  u8g2.drawStr(0,8, theText);

  // charging icon 8x8
  static unsigned char charging_bits[] = {0x10, 0x08, 0x04, 0x7e, 0x3f, 0x10, 0x08, 0x04};

  // battery level icons 8x8
  static unsigned char batt_empty_bits[] = { 0x00, 0xfc, 0x82, 0x81, 0x81, 0x82, 0xfc, 0x00 };
  static unsigned char batt_half_bits[] = { 0x00, 0xfc, 0xe2, 0xf1, 0xf1, 0xe2, 0xfc, 0x00 };
  static unsigned char batt_full_bits[] = { 0x00, 0xfc, 0xfe, 0xff, 0xff, 0xfe, 0xfc, 0x00 };

  // TODO: check power status and draw batt_empty_bits, batt half, batt full, or charging
  u8g2.drawXBM(94,0,8,8, charging_bits);

  // TODO: install RTC battery, get clock data a draw it
  // https://www.pjrc.com/teensy/td_libs_Time.html#teensy3
  u8g2.setFont(u8g2_font_5x7_tf);
  u8g2.drawStr(103,7,"23:59");
}

/*
 * draws the second row of the display
 * first argument is the text to display
 * second and third args are which selection, out of how many options
 * zero-indexed like arrays
 * e.g. 0,1 means showing the first option of two
 */
void drawSubHead(const char *theText, uint8_t showing, uint8_t howmany){
  u8g2.setFont(u8g2_font_pressstart2p_8r);
  uint8_t textPixels = u8g2.getStrWidth(theText);
  uint8_t padPixels = (u8g2.getDisplayWidth() - textPixels)/2;
  u8g2.drawStr(padPixels,16, theText);

  // if showing the Main Menu/Exit option, display up arrow
  //static unsigned char up_arrow[]= { 0x00, 0x0c, 0x1e, 0x3f, 0x0c, 0x0c, 0x3c, 0x38 };
  static unsigned char up_arrow[]= { 0x08, 0x1c, 0x3e, 0x49, 0x08, 0x08, 0x18, 0x70 };
  if((showing == 0) && (showMenu)){
    u8g2.drawXBM(padPixels-9,7,8,8, up_arrow);
  }
// because using a triangle was ugly looking
//  if((showing == 0) && (showMenu)){
//    u8g2.drawTriangle(8,8,12,16,4,16);
//  }

  // if page showing is not the first page, draw left arrow
  if(showing > 0){
    u8g2.drawTriangle(6,8,6,16,0,12);
  }

  // if page showing is not the last page, draw right arrow
  if(showing < howmany){
    u8g2.drawTriangle(122,8,122,16,128,12);
  }
}

