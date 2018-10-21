/*
 * rudRemote - robot.ino
 * 
 * the functions for speaking to rudRx-es
 * 
 * Copyright © 2018 Del Rudolph, https://www.darthrudolph.com/ <del@darthrudolph.com>
 * Released under the MIT License, see included file "LICENSE"
 * 
 */

// connect to the pipe of a specific rudRx
void setRudRx(uint8_t botID){
  radio.setChannel(RUD_CHANNEL);
  radio.openWritingPipe(rudPipes[botID]);

  #ifdef DEBUG
    Serial.print("Connecting to rudRx ");
    Serial.print(botID);
    Serial.print(" on channel ");
    Serial.print(RUD_CHANNEL);
    Serial.print(" using pipe ");
    Serial.println((char*) rudPipes[botID]);
    radio.printDetails();
  #endif
}

// get the readings of the control sticks and map() them into the correct
// values to send to the rudRx
void getRudSticks(){
  // Get the sticks' readings into rudPacket
  int16_t ch1 = map(analogRead(rStickX), rStickXMin, rStickXMax, -511, 511);    // roll
  if(abs(ch1) <= 25)
    ch1 = 0;
  rudPacket.ch1 = constrain(ch1, -511, 511);
  
  int16_t ch2 = map(analogRead(rStickY), rStickYMin, rStickYMax, -511, 511);    // pitch
  if(abs(ch2) <= 25)
    ch2 = 0;
  rudPacket.ch2 = constrain(ch2, -511, 511);
  
  uint16_t ch3 = map(analogRead(lStickY), lStickYMin, lStickYMax, 0, 1023);      // throttle
  if(ch3 <= 25)
    ch3 = 0;
  rudPacket.ch3 = constrain(ch3, 0, 1023);
  
  int16_t ch4 = map(analogRead(lStickX), lStickXMin, lStickXMax, -511, 511);    // yaw
  if(abs(ch4) <= 25)
    ch4 = 0;
  rudPacket.ch4 = constrain(ch4, -511, 511);
  
  rudPacket.ch5 = digitalRead(channelFive);
  
  uint16_t ch6 = map(analogRead(channelSix), channelSixMin, channelSixMax, 0, 1023);
  rudPacket.ch6 = constrain(ch6, 0, 1023);
}

// sends the rudPacket to the rudRx and gets the ACK packet
void sendRudPacket(){
  if(radio.write(&rudPacket, sizeof(rudPacket))){
    if(radio.isAckPayloadAvailable()){
      radio.read(&rudAck, sizeof(rudAck));
    }else{
      // maybe should monitor unreceived acks
    }
  }else{
    // probably should monitor failed writes
    #ifdef DEBUG
      Serial.println("write failed");
    #endif
  }
}

#ifdef DEBUG
void printRudValues()
{
  uint8_t i;
  char *ptr = (char *) &rudPacket;

  Serial.print("Header = ");
  Serial.print(rudPacket.header, HEX);
  Serial.print(" Roll: ");
  Serial.print(rudPacket.ch1);
  Serial.print(" Pitch: ");
  Serial.print(rudPacket.ch2);
  Serial.print(" Throttle: ");
  Serial.print(rudPacket.ch3);
  Serial.print(" Yaw: ");
  Serial.print(rudPacket.ch4);
  Serial.print(" Gear: ");
  Serial.print(rudPacket.ch5);
  Serial.print(" Flaps: ");
  Serial.print(rudPacket.ch6);

  Serial.print(" - ");
  for (i = 0; i < sizeof(rudPacket); i++)
    Serial.print(ptr[i] & 0xFF, HEX);
  Serial.println("");
}
#endif
