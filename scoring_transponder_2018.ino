/*
	MWScore Transponder 2018
	R-Team Robotics Version
  
	XBEE setup:
	ATBD = 5 (38400bps)
	ATID = 6200
	MY   = 6200 + TRANSPONDER_ID
	DL   = 6201
	CH   = c
	
	Scoring Receiver XBEE setup (Send Broadcast message)
	ATBD = 5 (38400bps)
	ATID = 6200
	MY   = 6201
	DL   = FFFF
	DH   = 0
	CH   = c
*/ 

// Uncomment for 2017 Transponder or Rteam 2013 transponder
//#define TRANSPONDER_OLD

//define transponder ID for 2018 transponder, no switches
#define TRANSPONDER_ID 1

#include <TimerOne.h>
#include <EEPROM.h>
#include <PinChangeInt.h>
#include "transponder_scoring.h"
#include "transponder_comm.h"
#include "transponder_eeprom.h"

//#define TimerMS 7813                // 128 Hz
//#define TimerMS 15625               //  64 Hz 
//#define TimerMS 31250               //  32 Hz
//#define TimerMS 62500               //  16 Hz
//#define TimerMS 125000              //   8 Hz
#define TimerMS 250000                //   4 Hz
//#define TimerMS 500000              //   2 Hz
//#define TimerMS 1000000             //   1 Hz
//#define TimerMS 2000000             // 0.5 Hz

void setup() {
  pinMode(hiti, OUTPUT);
  pinMode(hitu, OUTPUT);
  pinMode(hitu2, OUTPUT);
  pinMode(hitu3, OUTPUT);
  pinMode(hitu4, OUTPUT);
  
  pinMode(tp1, INPUT_PULLUP);
  pinMode(tp2, INPUT_PULLUP);
  pinMode(tp3, INPUT_PULLUP);
  pinMode(tp4, INPUT_PULLUP);
  
  PCintPort::attachInterrupt(tp1, hittp1, CHANGE);
  PCintPort::attachInterrupt(tp2, hittp2, CHANGE);
  PCintPort::attachInterrupt(tp3, hittp3, CHANGE);
  PCintPort::attachInterrupt(tp4, hittp4, CHANGE);

  pinMode(sw0, INPUT_PULLUP);
  pinMode(sw1, INPUT_PULLUP);
  pinMode(sw2, INPUT_PULLUP);
  pinMode(sw3, INPUT_PULLUP);
  pinMode(sw4, INPUT_PULLUP);
  pinMode(sw5, INPUT_PULLUP);
  pinMode(sw6, INPUT_PULLUP);
  pinMode(sw7, INPUT_PULLUP);
  
  Serial.begin(XBEE_BAUDRATE);

  // set ID initially
  #ifdef TRANSPONDER_2017
  id = 63 - (0x3f & PINC);
  #else
  id = TRANSPONDER_ID;
  #endif

  // read hit points and rules from EEPROM
  ReadEEPROM();
    
  // set up interrupt to send HP status message
  Timer1.initialize(TimerMS);
  Timer1.attachInterrupt(ISRTimerTXHP);
  delay(50);
}

void loop() {
  bool scoringhit = false;
  bool checkcomm = false;
  bool checkpanel = false;
  bool checkhit = false;
  bool checkheal = false;
  
  // check for ID update
  #ifdef TRANSPONDER_OLD
  id = 63 - (0x3f & PINC);
  #endif

  // Check for comm from scoring computer
  checkcomm = ReceiveComm();
  if(checkcomm == true) {
    WriteEEPROM();
  }
  
  if ((hit != 0) && (hitpoint > 0)) {		
    // determine panel that was hit
    checkpanel = ValidPanel();
    if (checkpanel == true) {
      // determine if hit is valid to score
      checkhit = ValidHit();
      if (checkhit == true) {
        ScoreHit();
        WriteHPEEPROM();
      }
    }
    
    // reset variables for next time hit
    hit = 0;
    panel = 0;
  }

  // Optional Healing Rule
  checkheal = ValidHeal(); 
  if (checkheal == true) {
    WriteHPEEPROM();
  }

  // leave LED on board when dead
  #ifdef TRANSPONDER_OLD
  IsDead();
  #else
  IsDead();
  UpdateLed();
  #endif
}
