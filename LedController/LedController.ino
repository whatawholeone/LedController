#include "FastLED.h"
#include <IRremote.h>
#include <EEPROM.h>

#define NUM_LEDS 50
#define DATA_PIN 13

#define SPEED_SAVE 1
#define PATTERN_SAVE 10

int RECV_PIN = 2;
CRGB leds[NUM_LEDS];
IRrecv irrecv(RECV_PIN);
decode_results results;
int patternNum = 0;
int patternDelay = 50;
int speedStep = 5;
int pattern = 1;
bool ledsOn = false;
uint8_t baseHue = 0;

void setup() {
  Serial.begin(9600);
  //while (!Serial) {   ;  }
  irrecv.enableIRIn(); // Start the receiver
  LEDS.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  patternDelay = epRead(SPEED_SAVE);
  Serial.print("Speed from memory: ");
  Serial.println(patternDelay);
  patternNum = epRead(PATTERN_SAVE);
  Serial.print("Pattern from memory: ");
  Serial.println(patternNum);
}

void loop() {

  if (irrecv.decode(&results)) {
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
      case 0xFFF00F:
        Serial.println("Button: Rainbow");
        pattern = 2;
        epWrite(PATTERN_SAVE, pattern );
        break;
      case 0xFFC837:
        Serial.println("Button: Cylon");
        pattern = 1;
        epWrite(PATTERN_SAVE, pattern );
        break;
    }
    irrecv.resume(); // Receive the next value
  }

  if (ledsOn) {
    playPattern();
    EVERY_N_MILLISECONDS( 20 ) {
      baseHue++;
    }
  }
}

void playPattern() {

  if (pattern = 1) {
    for (int i = 0; i < NUM_LEDS; i++) {
      fadeall();
      leds[i] = CRGB::Red;
      FastLED.show();
      if (irrecv.decode(&results)) break;
      delay(patternDelay);
    }
    for (int i = (NUM_LEDS) - 1; i >= 0; i--) {
      fadeall();
      leds[i] = CRGB::Red;
      FastLED.show();
      if (irrecv.decode(&results)) break;
      delay(patternDelay);
    }
  }

  if (pattern = 2) {
    fill_rainbow( leds, NUM_LEDS, baseHue, 7);
    FastLED.show();
  }
}

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(128);
    if (irrecv.decode(&results)) break;
  }
}

void speedUp() {
  // decease delay
  if ( patternDelay > speedStep) {
    patternDelay = patternDelay - speedStep;
    Serial.println(patternDelay);
    epWrite(SPEED_SAVE, patternDelay );
  }
}

void slowDown() {
  // incease delay
  patternDelay = patternDelay + speedStep;
  Serial.println(patternDelay);
  epWrite(SPEED_SAVE, patternDelay);
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

void epWrite(int location, int data) {
  EEPROM.put(location, data);
}

int epRead(int location) {
  int i = 0;
  EEPROM.get(location, i);
  return (i);
}
