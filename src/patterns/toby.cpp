#include <crgb.h>

#include "patterns.h"

// Size of the blob (number of LEDs)
// With 8 blobs and no gaps, each blob = NUM_LEDS / 8 = 53.5
// Round up to 54 for even blobs to fill the strip
#define BLOB_SIZE 54
// Speed of the bounce (lower = faster)
#define BOUNCE_SPEED 1

void Toby::render(CRGB* leds) {
  // 8 blobs total: red, green, red, green, red, green, red, green
  // Positions start evenly spaced with no gaps
  static int positions[8] = {0, 54, 108, 162, 216, 270, 324, 378};
  static CRGB colors[8] = {
    CRGB(255, 0, 0),    // red
    CRGB(0, 255, 0),    // green
    CRGB(255, 0, 0),    // red
    CRGB(0, 255, 0),    // green
    CRGB(255, 0, 0),    // red
    CRGB(0, 255, 0),    // green
    CRGB(255, 0, 0),    // red
    CRGB(0, 255, 0)     // green
  };
  static int counter = 0;

  // Clear all LEDs
  fill_black(leds, NUM_LEDS);

  // Update positions every BOUNCE_SPEED frames
  if (++counter >= BOUNCE_SPEED) {
    counter = 0;

    for (int blob = 0; blob < 8; blob++) {
      positions[blob]++;

      // Wrap around at the end
      if (positions[blob] >= NUM_LEDS) {
        positions[blob] = 0;
      }
    }
  }

  // Draw all blobs
  for (int blob = 0; blob < 8; blob++) {
    for (int i = 0; i < BLOB_SIZE; i++) {
      int led_index = (positions[blob] + i) % NUM_LEDS;
      leds[led_index] = colors[blob];
    }
  }
}

#undef BLOB_SIZE
#undef BOUNCE_SPEED
