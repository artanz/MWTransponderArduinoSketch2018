#ifndef Transponder_EEPROM_h
#define Transponder_EEPROM_h

#define EEPROM_ADDR_HP 0              // Address of EEPROM to store HP
#define EEPROM_ADDR_RULES 1           // Address of EEPROM to store Ruleset
#define EEPROM_ADDR_TP1 2             // Address of EEPROM to rule one variables
#define EEPROM_ADDR_TP2 3             // Address of EEPROM to rule one variables
#define EEPROM_ADDR_TP3 4             // Address of EEPROM to rule one variables
#define EEPROM_ADDR_TP4 5             // Address of EEPROM to rule one variables

// Function Protoypes
void WriteHPEEPROM();
void WriteEEPROM();
void ReadEEPROM();

void WriteHPEEPROM() {
  EEPROM.write(EEPROM_ADDR_HP, hitpoint);
  if ((rules == 1) || (rules == 3)) {
    EEPROM.write(EEPROM_ADDR_TP1,tphit[0]);
    EEPROM.write(EEPROM_ADDR_TP2,tphit[1]); 
    EEPROM.write(EEPROM_ADDR_TP3,tphit[2]); 
    EEPROM.write(EEPROM_ADDR_TP4,tphit[3]); 
    }
}

void WriteEEPROM() {
  EEPROM.write(EEPROM_ADDR_HP, hitpoint);
  EEPROM.write(EEPROM_ADDR_RULES, rules);
  if ((rules == 1) || (rules == 3)) {
    EEPROM.write(EEPROM_ADDR_TP1,tphit[0]);
    EEPROM.write(EEPROM_ADDR_TP2,tphit[1]); 
    EEPROM.write(EEPROM_ADDR_TP3,tphit[2]); 
    EEPROM.write(EEPROM_ADDR_TP4,tphit[3]); 
    }
}

void ReadEEPROM() {
  // read hit points and rules from EEPROM
  hitpoint = EEPROM.read(EEPROM_ADDR_HP);
  rules = EEPROM.read(EEPROM_ADDR_RULES);
  if ((hitpoint < 0) || (hitpoint > 50)) {
    hitpoint = 0;
  }
  if ((rules < 0) || (rules > 3)) {
    rules = 0;
  }

  if (rules == 1) {
    tphit[0] = EEPROM.read(EEPROM_ADDR_TP1);
    tphit[1] = EEPROM.read(EEPROM_ADDR_TP2);
    tphit[2] = EEPROM.read(EEPROM_ADDR_TP3);
    tphit[3] = EEPROM.read(EEPROM_ADDR_TP4);
  }  
}


#endif // Transponder_EEPROM_h
