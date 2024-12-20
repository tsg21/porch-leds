#define FASTLED_OVERCLOCK 1.2

#include <FastLED.h>
#include "common_led.h"
#include <limits.h>
#include <arduino.h>
#include "mqtt.h"

#define INTERVAL_MICROS 20000

#define STATS_EVERY 1000
//#define PRINT_STATS 1

typedef unsigned long ulong;

CRGB leds[NUM_LEDS];

ulong lastUpdateMicros;
ulong lastSerialUpdateMicros;

LedRace ledRace;
TestPattern testPattern;
TestPattern2 testPattern2;
Rainbow rainbow;
Snowflakes2 snowflake2;
Twinkles twinkles;
Wolfram135 wolfram135;

PatternRenderer* activePatternRenderer;

void onOptionChange(std::string option) {
  if (option.find("Snowflakes2") != std::string::npos) {
    activePatternRenderer = &snowflake2;
  } else if (option.find("Rainbow") != std::string::npos) {
    activePatternRenderer = &rainbow;
  } else if (option.find("Twinkles") != std::string::npos) {
    activePatternRenderer = &twinkles;
  } else if (option.find("Wolfram135") != std::string::npos) {
    activePatternRenderer = &wolfram135;
  } else {
    Serial.println("Unknown pattern");
    activePatternRenderer = &ledRace;
  }
}

void setup() {
  delay(500);
  
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting...");

  FastLED.addLeds<NEOPIXEL, D4>(leds, NUM_LEDS);

  start(onOptionChange);

  lastUpdateMicros = 0;

  activePatternRenderer = &snowflake2;
  Serial.println("LED control running...");
}

ulong elapsedMicros(ulong since) {
    ulong microsNow = micros();
    ulong elapsed = microsNow - lastUpdateMicros; 
    if (elapsed < 0) {
      // clock has overflowed
      elapsed += ULONG_MAX;
    }
    return elapsed;
}

void loop() {

  static ulong totalRenderElapsed = 0;
  static ulong totalShowElapsed = 0;
  static int loopCount = 0;

  activePatternRenderer->render(leds);
  ulong renderElapsed = elapsedMicros(lastUpdateMicros); 
  totalRenderElapsed += renderElapsed;

  FastLED.show();
  ulong showElapsed = elapsedMicros(lastUpdateMicros) - renderElapsed; 
  totalShowElapsed += showElapsed;

  // Wait for INTERVAL_MICROS
  ulong totaElapsed;
  do {
    totaElapsed = elapsedMicros(lastUpdateMicros); 
  } while (totaElapsed < INTERVAL_MICROS);
  lastUpdateMicros = micros();

  if (loopCount++ > STATS_EVERY) {

    #ifdef PRINT_STATS
    ulong averageRenderElapsed = totalRenderElapsed / loopCount;
    ulong averageShowElapsed = totalShowElapsed / loopCount;

    Serial.print("Avg render ");
    Serial.print(averageRenderElapsed);
    Serial.print("us. Avg show ");
    Serial.print(averageShowElapsed);
    Serial.println("us");
    #endif

    totalShowElapsed = 0;
    totalRenderElapsed = 0;
    loopCount = 0;
  }
}

