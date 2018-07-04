#ifndef Transponder_Comm_h
#define Transponder_Comm_h

#define XBEE_BAUDRATE 38400

// Function Protoypes
bool ReceiveComm();
void ISRTimerTXHP();

bool ReceiveComm(){
  // Check for HP change
  // Scoring Receiver sends broadcast message to set HP
  // If no message default is 20 HP
  // Receive message 0x55 ID 255-ID HP RULES

  byte receive[4];
  
  if (Serial.available() > 0) {
    Serial.readBytes(receive,5);
    // Receive Data from Computer
    if(receive[0] == 0x55) {
      if((receive[1] == id) && ((receive[1] + receive[2]) == 255)) {
        hitpoint = (int)receive[3];
        rules = (int)receive[4];
        tphit[0] = 0;
        tphit[1] = 0;
        tphit[2] = 0;
        tphit[3] = 0;
        return true;
      }
      else {
        return false;
      }
    }

    // Receive Data from ID setter
    if(receive[0] == 0xA5) {
      if((receive[1] == id) && ((receive[1] + receive[2]) == 255)) {
        if((receive[3] + receive[4]) == 255) {
          id = receive[3];
          return true;
        }
        else {
          return false;
        }
      }
      else {
        return false;
      }
    }
  }
  else {
    return false;
  }
  return false;
}

void ISRTimerTXHP(){
  // tx hit packet
  Serial.write((uint8_t) 0x55);
  Serial.write((uint8_t) id);
  Serial.write((uint8_t) (0xff - id));
  Serial.write((uint8_t) panel);
  Serial.write((uint8_t) hitpoint);
}

#endif // Transponder_Comm_h
