#include "FastLED.h"
#include <IRremote.h>
#include <EEPROM.h>

#define NUM_LEDS 50
#define DATA_PIN 13
int RECV_PIN = 2;
CRGB leds[NUM_LEDS];
IRrecv irrecv(RECV_PIN);
decode_results results;
int patternNum = 0;
int patternDelay = 50;
int speedStep = 5;
bool ledsOn = true;


void setup() {
  Serial.begin(9600);
  //while (!Serial) {  ;  }
  attachInterrupt(2, CHECK_IR, CHANGE);
  irrecv.enableIRIn(); // Start the receiver
  LEDS.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  patternDelay = epRead();
}

void loop() {
  if (ledsOn) {
    playPattern();
  }
}

void playPattern() {
  for (int i = 0; i < NUM_LEDS; i++) {
    fadeall();
    leds[i] = CRGB::Red;
    FastLED.show();
    delay(patternDelay);
  }
  for (int i = (NUM_LEDS) - 1; i >= 0; i--) {
    fadeall();
    leds[i] = CRGB::Red;
    FastLED.show();
    delay(patternDelay);
  }
}

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(128);
  }
}

void speedUp() {
  // decease delay
  if ( patternDelay > speedStep) {
    patternDelay = patternDelay - speedStep;
    Serial.println(patternDelay);
    epWrite(patternDelay );
  }
}

void slowDown() {
  // incease delay
  patternDelay = patternDelay + speedStep;
  Serial.println(patternDelay);
  epWrite(patternDelay);
}

void powerSwitch() {
  if (ledsOn) {
    for (int j = 0; j < 10; j++)
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i].fadeToBlackBy(27);
      }
    ledsOn = false;
    Serial.println("Power OFF");
    return;
  }
  if (!ledsOn) {
    ledsOn = true;
    Serial.println("Power ON");
    return;
  }
}

void CHECK_IR() {
  while (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irrecv.resume();
  }
}

void readIR() {
  while (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);

    switch (results.value) {
      case 0xFFE01F:
        Serial.println("Button: Power");
        powerSwitch();
        break;
      case 0xFFA05F:
        Serial.println("Button: UP");
        speedUp();
        break;
      case 0xFF20DF:
        Serial.println("Button: DOWN");
        slowDown();
        break;
    }
    irrecv.resume(); // Receive the next value
  }
  //  delay(100);
}

void epWrite(int i) {
  EEPROM.put(0, i);
  Serial.print("save speed to memory: ");
  Serial.println(i);
}

int epRead() {
  int i = 0;
  EEPROM.get(0, i);
  Serial.print("read speed from memory: ");
  Serial.println(i);
  return (i);
}
