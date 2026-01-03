// Control one or more strands of LED Christmas lights arranged parallel/antiparallel
// Uses DRV8833

const unsigned drv1 = PB7; // DRV8833 IN3
const unsigned drv2 = PB8; // DRV8833 IN4
const unsigned buttonPin = PA1; 
const int32_t dutyCyclePercent = 30; // must be between 1 and 50 inclusive
const uint32_t offTimeHours = 7;

const uint32_t frequency = 1000; // Hz
const uint32_t cycleTime = 1000000l / frequency;
const int32_t timeOn = (dutyCyclePercent * cycleTime) / 100;
static_assert(timeOn * 2 <= cycleTime);
const int32_t timeOff = (cycleTime - 2 * timeOn) / 2;
bool active = true;
int state = 0;
uint32_t lastButtonPress = 0;
const uint32_t debounceTime = 40;
int buttonState = false;
const uint32_t offTimeMillis = offTimeHours * (60ul * 60 * 1000);
uint32_t offTimeActivated = 0;
const uint32_t dayMillis = 86400ul * 1000;

void setup() {
  pinMode(drv1,OUTPUT); 
  pinMode(drv2,OUTPUT); 
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  uint32_t t = millis();
  if (buttonState || t-lastButtonPress >= debounceTime) {
    bool newButtonState = !digitalRead(buttonPin);
    if (newButtonState != buttonState) {
      buttonState = newButtonState;
      if (buttonState) {
        lastButtonPress = t;
        if (active) {
          active = false;
          offTimeActivated = millis();
        }
        else {
          active = true;
          offTimeActivated = 0;
        }
      }
    }
  }

  if (!active) {
    if ((millis()-offTimeActivated) >= offTimeMillis) {
      offTimeActivated += dayMillis;
      active = true;
    }
  }
  else {
    if (offTimeActivated != 0 && millis() >= offTimeActivated) {
      active = false;
    }  
  }

  if (active) {
    digitalWrite(drv1,state);
    digitalWrite(drv2,!state);
    delayMicroseconds(timeOn);
    if (timeOff > 0) {
      digitalWrite(drv1,0);
      digitalWrite(drv2,0);
      delayMicroseconds(timeOff);
    }
    state = !state;
  }
  else {
      digitalWrite(PB7,0);
      digitalWrite(PB8,0);
      delay(5);
  }
}
