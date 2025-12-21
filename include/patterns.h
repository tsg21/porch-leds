#pragma once
#include <crgb.h>

#include <memory>
#include <vector>

#include "led_common.h"

// 214 on each side plus slope
// 132 on each side
// 82 on each slope

#define PATTERN_INSTANCE(NAME)          \
  class NAME : public PatternRenderer { \
   public:                              \
    void render(CRGB* leds) override;   \
  };

#define COMPLEX_PATTERN_INSTANCE(NAME)  \
  class NAME : public PatternRenderer { \
    class Impl;                         \
    Impl* impl;                         \
                                        \
   public:                              \
    NAME();                             \
    ~NAME();                            \
    void render(CRGB* leds) override;   \
  };

PATTERN_INSTANCE(Black)
PATTERN_INSTANCE(LedRace)
PATTERN_INSTANCE(TestPattern)
PATTERN_INSTANCE(Rainbow)
PATTERN_INSTANCE(Snowflakes1)
PATTERN_INSTANCE(Snowflakes2)
PATTERN_INSTANCE(TestPattern2)
PATTERN_INSTANCE(Rainbow2)
PATTERN_INSTANCE(Bounce)
PATTERN_INSTANCE(Toby)
PATTERN_INSTANCE(Toby2)
COMPLEX_PATTERN_INSTANCE(Wolfram135)
COMPLEX_PATTERN_INSTANCE(Twinkles)

#undef PATTERN_INSTANCE
#undef COMPLEX_PATTERN_INSTANCE

/*
 * List of all patterns, in the form of a parametric macro. Whenever
 * you want to list all the known patterns in other parts of the code,
 * you can invoke this with another macro name, and it will evaluate
 * that macro with each pattern as an argument.
 *
 * The second parameter SEPARATOR will be interleaved between the
 * invocations of the macro X.
 */
#define ALL_PATTERNS(X, SEPARATOR)        \
  X(Black) SEPARATOR X(LedRace)           \
  SEPARATOR                               \
  X(TestPattern) SEPARATOR X(Rainbow)     \
  SEPARATOR                               \
  X(Snowflakes1) SEPARATOR X(Snowflakes2) \
  SEPARATOR                               \
  X(TestPattern2) SEPARATOR X(Twinkles)   \
  SEPARATOR X(Rainbow2)                   \
  SEPARATOR X(Wolfram135)                 \
  SEPARATOR X(Toby)                       \
  SEPARATOR X(Toby2)
