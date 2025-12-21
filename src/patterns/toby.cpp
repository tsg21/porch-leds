#include <crgb.h>

#include "patterns.h"

// Size of the blob (number of LEDs)
#define BLOB_SIZE 20
// Speed of the bounce (lower = faster)
#define BOUNCE_SPEED 1

void Toby::render(CRGB* leds) {
  // 6 blobs total: 3 red, 3 green, alternating colors
  // Positions start evenly spaced around the strip
  static int positions[6] = {0, NUM_LEDS/6, NUM_LEDS/3, NUM_LEDS/2, 2*NUM_LEDS/3, 5*NUM_LEDS/6};
  static int directions[6] = {1, -1, 1, -1, 1, -1};  // alternating directions
  static CRGB colors[6] = {
    CRGB(255, 0, 0),    // red (left to right)
    CRGB(0, 255, 0),    // green (right to left)
    CRGB(255, 0, 0),    // red (left to right)
    CRGB(0, 255, 0),    // green (right to left)
    CRGB(255, 0, 0),    // red (left to right)
    CRGB(0, 255, 0)     // green (right to left)
  };
  static int counter = 0;

  // Clear all LEDs
  fill_black(leds, NUM_LEDS);

  // Update positions every BOUNCE_SPEED frames
  if (++counter >= BOUNCE_SPEED) {
    counter = 0;

    for (int blob = 0; blob < 6; blob++) {
      positions[blob] += directions[blob];

      // Bounce at the ends
      if (positions[blob] >= NUM_LEDS - BLOB_SIZE) {
        positions[blob] = NUM_LEDS - BLOB_SIZE;
        directions[blob] = -1;
      } else if (positions[blob] <= 0) {
        positions[blob] = 0;
        directions[blob] = 1;
      }
    }
  }

  // Draw blobs going right to left first (will be underneath)
  for (int blob = 0; blob < 6; blob++) {
    if (directions[blob] == -1) {  // right to left
      for (int i = 0; i < BLOB_SIZE; i++) {
        int led_index = positions[blob] + i;
        if (led_index >= 0 && led_index < NUM_LEDS) {
          leds[led_index] = colors[blob];
        }
      }
    }
  }

  // Draw blobs going left to right second (will be on top)
  for (int blob = 0; blob < 6; blob++) {
    if (directions[blob] == 1) {  // left to right
      for (int i = 0; i < BLOB_SIZE; i++) {
        int led_index = positions[blob] + i;
        if (led_index >= 0 && led_index < NUM_LEDS) {
          leds[led_index] = colors[blob];
        }
      }
    }
  }
}

#undef BLOB_SIZE
#undef BOUNCE_SPEED
