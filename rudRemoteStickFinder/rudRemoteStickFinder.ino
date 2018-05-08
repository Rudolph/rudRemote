/*
 * rudRemoteStickFinder
 * 
 * Just spits out the analog values of the control stick positions.
 * Used to find the min/max values to put in the rudRemote settings.
 * 
 * Released under the MIT License, see included file LICENSE
 */

// the analog pins your gimbals are plugged into
#define lStickX  A9     // left stick, X (horizontal) direction
#define lStickY  A8     // left stick, Y (vertical) direction
#define rStickX  A7     // right stick, X
#define rStickY  A6     // left stick, Y

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("rudRemote analog test");  
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
