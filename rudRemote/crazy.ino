/*
 * rudRemote - crazy.ino
 * 
 * the functions for speaking CRTP to the Crazyflie 2.0
 * 
 * Copyright © 2018 Del Rudolph, https://www.darthrudolph.com/ <del@darthrudolph.com>
 * Released under the MIT License, see included file "LICENSE"
 * 
 * Parts from jpihet's code at https://github.com/jpihet/crazyflie-arduino-controller
 * 
 */

// connect to the channel and pipe of a Crazyflie 2.0
void setCrazyRadio(){
  radio.setChannel(CRAZY_CHANNEL);
  radio.openWritingPipe(CRAZY_PIPE);

  #ifdef DEBUG
    Serial.print("Connecting to CRAZY_CHANNEL ");
    Serial.println(CRAZY_CHANNEL);
    radio.printDetails();
  #endif
}

// get the readings of the control sticks and map() them into the correct
// values to send to the crazyflie
void getCrazySticks(){
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

  // was at 50000, can be up to 65k it seems
  uint16_t t = map(analogRead(lStickY), lStickYMin, lStickYMax, 0, 60000);
  if(t < 10000)
    t = 0;
  crtp.thrust = constrain(t, 0, 60000);
}

// sends the CRTP packet to the Crazyflie 2.0 and gets the ACK packet
void sendCrazyPacket(){
  uint8_t len = 0;
  if(radio.write(&crtp, sizeof(crtp))){
    #ifdef DEBUG
      Serial.println("CRTP packet sent");
    #endif
    len = radio.getDynamicPayloadSize();
    radio.read(&crtpAck, len);
  }else{
    #ifdef DEBUG
      Serial.println("crazy write failed");
    #endif
  }
  
  #ifdef DEBUG
    if(len > 0){
      Serial.print("Ack received, size ");
      Serial.println(len);
      for(int i=0;i<=len;i++){
        Serial.print(crtpAck[i]);
      }
      Serial.println("");
    }
  #endif
}

/*
 * from jpihet's code at https://github.com/jpihet/crazyflie-arduino-controller
 */
 #ifdef DEBUG
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
#endif

