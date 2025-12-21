#include <crgb.h>

#include <vector>
#include <algorithm>

#include "patterns.h"

// Layout: Left side 0-131, Left slope 132-213, Right slope 214-295, Right side 296-427
// LED 0 = left bottom, LED 131 = left top, LED 296 = right top, LED 427 = right bottom

struct Blob {
  float position;      // Current position (float for smooth movement)
  float velocity;      // LEDs per frame
  int side;           // 0 = left, 1 = right

  Blob(int startSide) : side(startSide), velocity(0) {
    // Start at bottom of chosen side
    position = (startSide == 0) ? 0.0f : 427.0f;
    // Random initial velocity: 3.0 to 7.0 LEDs per frame
    velocity = 3.0f + (rand() % 400) / 100.0f;
  }
};

void Creative::render(CRGB* leds) {
  static std::vector<Blob> blobs;
  static unsigned long lastLaunch = 0;
  static unsigned long frameCount = 0;

  // Clear LEDs
  fill_black(leds, NUM_LEDS);

  frameCount++;

  // Launch new blob every 1 second (50 frames at 50fps)
  if (frameCount - lastLaunch >= 50) {
    blobs.push_back(Blob(rand() % 2));  // Random side
    lastLaunch = frameCount;
  }

  const float gravity = -0.08f;  // LEDs per frame^2

  // Update and render blobs
  for (auto& blob : blobs) {
    // Apply gravity
    blob.velocity += gravity;

    // Update position based on side
    if (blob.side == 0) {
      // Left side: moving up means increasing position (0->131->132->213)
      blob.position += blob.velocity;

      // Check if blob crossed to right slope (213->214)
      if (blob.position > 213.0f && blob.velocity > 0) {
        blob.side = 1;
        // Map to right slope: position continues from 214
        float overshoot = blob.position - 213.0f;
        blob.position = 213.0f + overshoot;
        // Flip velocity direction (right side counts down)
        blob.velocity = -blob.velocity;
      }
    } else {
      // Right side: moving up means decreasing position (427->296->295->214)
      blob.position -= blob.velocity;

      // Check if blob crossed to left slope (214->213)
      if (blob.position < 214.0f && blob.velocity > 0) {
        blob.side = 0;
        // Map to left slope: position continues backwards from 213
        float overshoot = 214.0f - blob.position;
        blob.position = 214.0f - overshoot;
        // Flip velocity direction (left side counts up)
        blob.velocity = -blob.velocity;
      }
    }

    // Render blob (3 LEDs wide with fading)
    int ledIndex = (int)blob.position;
    if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
      leds[ledIndex] = CRGB::Red;
      if (ledIndex > 0) leds[ledIndex - 1] = CRGB(64, 0, 0);
      if (ledIndex < NUM_LEDS - 1) leds[ledIndex + 1] = CRGB(64, 0, 0);
    }
  }

  // Remove dead blobs (fallen off the bottom on either side)
  blobs.erase(
    std::remove_if(blobs.begin(), blobs.end(),
      [](const Blob& b) {
        return (b.side == 0 && b.position < 0.0f) || (b.side == 1 && b.position > 427.0f);
      }),
    blobs.end());
}
