#include <crgb.h>

#include "patterns.h"

// Layout: 132 LEDs on each vertical side, 82 LEDs on each slope
// Left side: 0-131, Left slope: 132-213, Right slope: 214-295, Right side: 296-427
// Center point of strip: 214
#define CENTER_POINT 214

#define BLOB_SIZE 20
#define SPAWN_INTERVAL 50
#define BOUNCE_SPEED 1
#define MAX_BLOBS 20

struct Blob {
  int position;
  int size;
  int age;
  CRGB color;
  bool active;
  bool moving_left;  // true = left direction, false = right direction
};

void Toby2::render(CRGB* leds) {
  // Blobs flow outward from the center point (LED 213)
  // Spawn new blob every 50 frames, alternating red/green

  static Blob blobs[MAX_BLOBS];
  static int spawn_counter = 0;
  static bool next_is_red = true;
  static int counter = 0;

  // Initialize blobs on first call
  static bool initialized = false;
  if (!initialized) {
    for (int i = 0; i < MAX_BLOBS; i++) {
      blobs[i].active = false;
    }
    initialized = true;
  }

  // Clear all LEDs
  fill_black(leds, NUM_LEDS);

  // Update every BOUNCE_SPEED frames
  if (++counter >= BOUNCE_SPEED) {
    counter = 0;

    // Spawn new blob every SPAWN_INTERVAL frames
    spawn_counter++;
    if (spawn_counter >= SPAWN_INTERVAL) {
      spawn_counter = 0;

      // Find inactive blob slot and spawn two new blobs (one left, one right)
      for (int i = 0; i < MAX_BLOBS; i++) {
        if (!blobs[i].active) {
          // Left-moving blob (starts one to the left, alternates red/green)
          blobs[i].position = CENTER_POINT - 1;
          blobs[i].size = 0;
          blobs[i].age = 0;
          blobs[i].color = next_is_red ? CRGB(255, 0, 0) : CRGB(0, 255, 0);
          blobs[i].active = true;
          blobs[i].moving_left = true;

          // Find another slot for right-moving blob (alternates green/red)
          for (int j = i + 1; j < MAX_BLOBS; j++) {
            if (!blobs[j].active) {
              blobs[j].position = CENTER_POINT;
              blobs[j].size = 0;
              blobs[j].age = 0;
              blobs[j].color = next_is_red ? CRGB(0, 255, 0) : CRGB(255, 0, 0);
              blobs[j].active = true;
              blobs[j].moving_left = false;
              break;
            }
          }

          // Alternate color for next spawn
          next_is_red = !next_is_red;
          break;
        }
      }
    }

    // Update all active blobs
    for (int i = 0; i < MAX_BLOBS; i++) {
      if (!blobs[i].active) continue;

      blobs[i].age++;

      if (blobs[i].age < BLOB_SIZE) {
        // Still growing
        blobs[i].size = blobs[i].age;
      } else {
        // Full size - now move
        blobs[i].size = BLOB_SIZE;

        if (blobs[i].moving_left) {
          blobs[i].position--;
          // Deactivate when leading edge goes beyond the left end
          if (blobs[i].position < 0) {
            blobs[i].active = false;
          }
        } else {
          blobs[i].position++;
          // Deactivate when leading edge goes beyond the right end
          if (blobs[i].position > NUM_LEDS - 1) {
            blobs[i].active = false;
          }
        }
      }
    }
  }

  // Draw all active blobs
  for (int i = 0; i < MAX_BLOBS; i++) {
    if (!blobs[i].active) continue;

    for (int j = 0; j < blobs[i].size; j++) {
      int led_index;
      if (blobs[i].moving_left) {
        led_index = blobs[i].position - j;
      } else {
        led_index = blobs[i].position + j;
      }

      if (led_index >= 0 && led_index < NUM_LEDS) {
        leds[led_index] = blobs[i].color;
      }
    }
  }
}

#undef LEFT_SIDE_START
#undef LEFT_SIDE_END
#undef LEFT_SLOPE_START
#undef LEFT_SLOPE_END
#undef RIGHT_SLOPE_START
#undef RIGHT_SLOPE_END
#undef RIGHT_SIDE_START
#undef RIGHT_SIDE_END

#undef BLOB_SIZE
#undef BOUNCE_SPEED
