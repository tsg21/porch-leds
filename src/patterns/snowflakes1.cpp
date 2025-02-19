#include <crgb.h>

#include <algorithm>
#include <vector>

#include "led_common.h"
#include "patterns.h"

void Snowflakes1::render(CRGB* leds) {
  static std::vector<int> snowflakesLeft;
  static std::vector<int> snowflakesRight;
  static const int sideLength = 214;

  // Clear LEDs
  fill_black(leds, NUM_LEDS);

  int valP = 2;
  // Spawn new snowflakes
  if (rand() % 100 < valP) {       // P% chance to spawn a new snowflake
    snowflakesRight.push_back(0);  // Top of the right side
  }
  if (rand() % 100 < valP) {      // P% chance to spawn a new snowflake
    snowflakesLeft.push_back(0);  // Top of the left side
  }

  // Update snowflake positions
  for (int& pos : snowflakesLeft) pos++;
  for (int& pos : snowflakesRight) pos++;

  // Remove snowflakes that have reached the bottom
  snowflakesLeft.erase(
      std::remove_if(snowflakesLeft.begin(), snowflakesLeft.end(),
                     [](int pos) { return pos >= sideLength; }),
      snowflakesLeft.end());

  snowflakesRight.erase(
      std::remove_if(snowflakesRight.begin(), snowflakesRight.end(),
                     [](int pos) { return pos >= sideLength; }),
      snowflakesRight.end());

  // Draw snowflakes
  for (const int pos : snowflakesLeft) {
    leds[sideLength - 1 - pos] = CRGB::White;  // Map to the left half
  }
  for (const int pos : snowflakesRight) {
    leds[sideLength + pos] = CRGB::White;  // Map to the right half
  }
}
