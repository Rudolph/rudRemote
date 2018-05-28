/*
 * rudRemote - helpers.ino
 * 
 * holds some extra functions that don't fit anywhere else
 * 
 * Copyright © 2018 Del Rudolph, https://www.darthrudolph.com/ <del@darthrudolph.com>
 * Released under the MIT License, see included file "LICENSE"
 * 
 */

// interrupt service routine to catch encoder button clicks
void encButtonPress(){
  // TODO: Verify throttle is at zero
  static uint32_t lastInterrupt = 0;
  uint32_t thisInterrupt = millis();

  // FIXME: not sure if it's in this debouncing part or what, but
  // holding the button too long returns unintended results
  // e.g. click and hold on Mode Menu sets Robot Mode without giving
  // the option to choose Crazy Mode
  if(thisInterrupt - lastInterrupt > 200){
    // do something with the button press
    if(showMenu){
      buttonClicked = true;
    }else{
      showMenu = true;
    }
    lastInterrupt = thisInterrupt;
    needsDrawn = true;
  }
}
