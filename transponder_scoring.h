#ifndef Transponder_Scoring_h
#define Transponder_Scoring_h

#define PANELMASK  240
#define PANEL1     112
#define PANEL2     176
#define PANEL3     208
#define PANEL4     224

#define MS_COOLDOWN 1000
#define MS_SIGNAL 50

#define LED_RATE 50
#define LED_PERIOD 300                // LED_RATE * 6

// Optional Rule Variables
#define MAX_PANEL_HIT 15              // Max hit points scored on panel
#define HEALING_HP_THRESHOLD 10       // Hit points when healing starts
#define HEALING_RATE 5000             // Healing Rate (ms)
#define RESET_COOLDOWN 5000           // Time after cooldown resets (ms)

//stores transponder ID
uint8_t id;

// Variables to store
volatile uint8_t hit = 0;
volatile uint8_t hitpoint = 0;
volatile uint8_t panel = 0;

// Optional rules variables
// 0 = Default Rule
// 1 = Max Hit Point per Panel
// 2 = Healing Enabled
volatile uint8_t rules = 0;

// Optional rules variables
volatile uint8_t tphit[4];
volatile uint32_t lasthittime = 0;
volatile uint32_t lasthealtime = 0;

// Arduino Pin Assignments
int hiti = 3;
int hitu = 2;
int hitu2 = 8;
int hitu3 = 9;
int hitu4 = 10;

// Arduino Pin Assignments for Target Panels
int tp1 = 4;
int tp2 = 5;
int tp3 = 6;
int tp4 = 7;

// ID Pin Assignments (Analog Pins)
int sw0 = A0;
int sw1 = A1;
int sw2 = A2;
int sw3 = A3;
int sw4 = A4;
int sw5 = A5;
int sw6 = A6;
int sw7 = A7;

// Function Protoypes
bool ValidPanel();
bool ValidHit();
bool IsDead();
void ScoreHit();
bool ValidHeal();
void UpdateLed();
void hittp1();
void hittp2();
void hittp3();
void hittp4();


bool ValidPanel() {
  if ((hit & PANELMASK) == PANEL1) {
      panel = 1;
      return true;
      }
    else if ((hit & PANELMASK) == PANEL2) {
      panel = 2;
      return true;
      }
    else if ((hit & PANELMASK) == PANEL3) {
      panel = 3;
      return true;
      }
    else if ((hit & PANELMASK) == PANEL4) {
      panel = 4;
      return true;
      }
    else {
      panel = 5;
      return false;
      }
    return false;
}


bool ValidHit() {
  uint32_t currenttime = 0;
  uint32_t cooldown = 0;
  
  if (rules == 3) {
    cooldown = (tphit[panel-1] + 1) * MS_COOLDOWN;
    currenttime = millis();
    if ((currenttime - lasthittime) > (cooldown + RESET_COOLDOWN)) {
      tphit[panel-1] = 0;
      cooldown = MS_COOLDOWN;
    }
  }
  else {
    cooldown = MS_COOLDOWN;
  }

  if (cooldown < MS_COOLDOWN) {
    cooldown = MS_COOLDOWN;
  }

  currenttime = millis();
  if ((currenttime - lasthittime) >= cooldown) {
    if (rules == 1) {
      // max hits per panel rule
      tphit[panel-1]++;
      if (tphit[panel-1] <= MAX_PANEL_HIT) {
        // set scoring hit flag to true
        lasthittime = millis();
        return true;
      }
      else {
        return false;
      }
    }
    else if (rules == 2) {
      // healing rule
      lasthittime = millis();
      lasthealtime = lasthittime;
      return true;
    }
    else if (rules == 3) {
      // cool down rule
      tphit[panel-1]++;
      lasthittime = millis();
      return true;
    }
    else {
      // default rules
      lasthittime = millis();
      return true;
    }
  }
  
  return false;
}


bool IsDead() { 
  // leave LED on board when dead
  if (hitpoint == 0) {
    #ifdef TRANSPONDER_OLD   
    digitalWrite(hitu, HIGH);
    #else   
    digitalWrite(hitu, LOW);
    digitalWrite(hitu2, LOW);
    digitalWrite(hitu3, LOW);
    digitalWrite(hitu4, LOW);
    #endif
    return true;
  }
  else {
    #ifdef TRANSPONDER_OLD 
    digitalWrite(hitu, LOW);
    #else
    digitalWrite(hitu, HIGH);
    digitalWrite(hitu2, HIGH);
    digitalWrite(hitu3, HIGH);
    digitalWrite(hitu4, HIGH);
    #endif
    return false;
  } 
}


void UpdateLed() {
  uint32_t currenttime = 0;
  uint32_t cooldown1 = 0;
  uint32_t cooldown2 = 0;
  uint32_t cooldown3 = 0;
  uint32_t cooldown4 = 0;
  
  // rule 1
  // turn on LED on target panel if panel is dead
  if(rules == 1) {
    if (tphit[0] >= MAX_PANEL_HIT) {
      digitalWrite(hitu, LOW);
      }
    else if (tphit[1] >= MAX_PANEL_HIT) {
      digitalWrite(hitu2, LOW);
      }
    else if (tphit[2] >= MAX_PANEL_HIT) {
      digitalWrite(hitu3, LOW);
      }
    else if (tphit[3] >= MAX_PANEL_HIT) {
      digitalWrite(hitu4, LOW);
      }
    else {
      digitalWrite(hitu, HIGH);
      digitalWrite(hitu2, HIGH);
      digitalWrite(hitu3, HIGH);
      digitalWrite(hitu4, HIGH);
    }
  }

  // rule 3
  // turn on LED on target panel if panel can be hit
  if (rules == 3) {
    cooldown1 = (tphit[0] + 1) * MS_COOLDOWN;
    cooldown2 = (tphit[1] + 1) * MS_COOLDOWN;
    cooldown3 = (tphit[2] + 1) * MS_COOLDOWN;
    cooldown4 = (tphit[3] + 1) * MS_COOLDOWN;
    currenttime = millis();
    if ((currenttime - lasthittime) <= cooldown1) {
      digitalWrite(hitu, LOW);
    }
    else {
      digitalWrite(hitu, HIGH);
    }
    if ((currenttime - lasthittime) <= cooldown2) {
      digitalWrite(hitu2, LOW);
    }
    else {
      digitalWrite(hitu2, HIGH);
    }
    if ((currenttime - lasthittime) <= cooldown3) {
      digitalWrite(hitu3, LOW);
    }
    else {
      digitalWrite(hitu3, HIGH);
    }
    if ((currenttime - lasthittime) <= cooldown4) {
      digitalWrite(hitu4, LOW);
    }
    else {
      digitalWrite(hitu4, HIGH);
    }
  }
}


void ScoreHit() {
  // decrease hitpoints
  hitpoint--;
        
  // hit output high
  // blink LED board 1 time at same time of hit indicate
  digitalWrite(hiti, HIGH);
  #ifdef TRANSPONDER_OLD 
  digitalWrite(hitu, HIGH);
  #else
  digitalWrite(hitu, LOW);
  digitalWrite(hitu2, LOW);
  digitalWrite(hitu3, LOW);
  digitalWrite(hitu4, LOW);
  #endif

  // delay and reset hit output
  delay(MS_SIGNAL * panel);
  digitalWrite(hiti, LOW);

  if (hitpoint > 0) {
    #ifdef TRANSPONDER_OLD 
    digitalWrite(hitu, LOW);
    #else
    digitalWrite(hitu, HIGH);
    digitalWrite(hitu2, HIGH);
    digitalWrite(hitu3, HIGH);
    digitalWrite(hitu4, HIGH);
    #endif
    delay(LED_RATE);

    // blink LED board more 2 times
    for (int x = 0; x < 2; x++) {
      #ifdef TRANSPONDER_OLD 
      digitalWrite(hitu, HIGH);
      #else
      digitalWrite(hitu, LOW);
      digitalWrite(hitu2, LOW);
      digitalWrite(hitu3, LOW);
      digitalWrite(hitu4, LOW);
      #endif
      delay(LED_RATE);
      #ifdef TRANSPONDER_OLD 
      digitalWrite(hitu, LOW);
      #else
      digitalWrite(hitu, HIGH);
      digitalWrite(hitu2, HIGH);
      digitalWrite(hitu3, HIGH);
      digitalWrite(hitu4, HIGH);
      #endif
      delay(LED_RATE);
      }
    }
}

bool ValidHeal() {
  uint32_t currenttime = 0;
  
  if (rules == 2) {
    currenttime = millis();
    
    if (((currenttime - lasthealtime) > HEALING_RATE) && (hitpoint < HEALING_HP_THRESHOLD))  {
      // increment HP
      hitpoint++;
      lasthealtime = millis();
      return true;
    }
  }
  return false;
}

void hittp1() {
  hit = PANEL1;
}

void hittp2() {
  hit = PANEL2;
}

void hittp3() {
  hit = PANEL3;
}

void hittp4() {
  hit = PANEL4;
}

#endif // Transponder_Scoring_h
