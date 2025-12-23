#include <crgb.h>

#include <algorithm>
#include <iterator>
#include <vector>

#include "patterns.h"

class Twinkle2 : public MultiPatternItem {
 public:
  int pos;

 private:
  CRGB color;
  int seq;
  int scaleUpRate;
  int scaleDownRate;
  int scaleDownFromSeq;
  int deathSeq;

 public:
  Twinkle2(int randValue) {
    seq = 0;

    pos = randValue % NUM_LEDS;

    // 50% chance red, 50% chance green
    if (randValue % 2 == 0) {
      color = CRGB(255, 0, 0);  // red
    } else {
      color = CRGB(0, 255, 0);  // green
    }

    scaleUpRate = 1;
    scaleDownRate = 1;

    scaleDownFromSeq = 255 / scaleUpRate / 2;  // 2x longer duration
    deathSeq =
        scaleDownFromSeq + (scaleUpRate * scaleDownFromSeq) / scaleDownRate;
  }

  void update() override { seq++; }

  bool isDead() override { return seq >= deathSeq; }

  void render(CRGB* leds) override {
    int fadeFactor = seq < scaleDownFromSeq
                         ? seq * scaleUpRate
                         : (scaleDownFromSeq * scaleUpRate) -
                               ((seq - scaleDownFromSeq) * scaleDownRate);

    if (fadeFactor > 0) {
      // Render blob of ~20 LEDs centered at pos with edge fade
      for (int offset = -10; offset <= 10; offset++) {
        int ledPos = pos + offset;
        if (ledPos >= 0 && ledPos < NUM_LEDS) {
          // Calculate edge fade: full brightness at center, fades to edges
          int distance = abs(offset);
          float edgeFade = 1.0 - (distance / 10.0);
          int combinedFade = fadeFactor * edgeFade;
          leds[ledPos] = color.scale8(combinedFade);
        }
      }
    }
  }
};

class TobyTwinkles::Impl {
 private:
  std::vector<std::unique_ptr<Twinkle2>> patterns;

  bool isPositionAvailable(int pos) {
    for (const auto& pattern : patterns) {
      // Check if blobs would overlap (within 21 LEDs of each other)
      if (abs(pos - pattern->pos) < 21) {
        return false;
      }
    }
    return true;
  }

 public:
  Impl();
  void render(CRGB* leds);
};

TobyTwinkles::Impl::Impl() {
  // Initialise
}

void TobyTwinkles::Impl::render(CRGB* leds) {
  int randValue = rand();

  // Spawn new twinkle if position is available
  if (randValue % 100 < 5) {
    int pos = randValue % NUM_LEDS;
    if (isPositionAvailable(pos)) {
      patterns.push_back(std::make_unique<Twinkle2>(randValue));
    }
  }

  // Update and render all patterns
  for (auto& pattern : patterns) {
    pattern->update();
    pattern->render(leds);
  }

  // Remove dead patterns
  patterns.erase(
      std::remove_if(patterns.begin(), patterns.end(),
                     [](const auto& p) { return p->isDead(); }),
      patterns.end());
}

TobyTwinkles::TobyTwinkles() : impl(new Impl) {}
TobyTwinkles::~TobyTwinkles() { delete impl; }
void TobyTwinkles::render(CRGB* leds) { impl->render(leds); }
