/*
 * rudRemoteStickFinder
 * 
 * Just spits out the analog values of the control stick positions.
 * Used to find the min/max values to put in the rudRemote settings.
 * See https://hackaday.io/project/158068-rudremote for more information
 * about rudRemote and what it's for.
 * 
 * Copyright © 2018 Del Rudolph, https://www.darthrudolph.com/ <del@darthrudolph.com>
 * Released under the MIT License, see included file LICENSE
 * 
 */

// the analog pins your gimbals are plugged into
#define lStickX  A9     // left stick, X (horizontal) direction (yaw) (channel 4 in Mode 2)
#define lStickY  A8     // left stick, Y (vertical) direction (throttle) (channel 3 in Mode 2)
#define rStickX  A7     // right stick, X (roll) (channel 1 in Mode 2)
#define rStickY  A6     // left stick, Y (pitch) (channel 2 in Mode 2)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("rudRemoteStickFinder");  
}

void loop() {
  // put your main code here, to run repeatedly:  
  Serial.print("LX: ");
  Serial.print(analogRead(lStickX));
  Serial.print(" LY: ");
  Serial.print(analogRead(lStickY));
  Serial.print(" RX: ");
  Serial.print(analogRead(rStickX));
  Serial.print(" RY: ");
  Serial.println(analogRead(rStickY));
  delay(100);
}
