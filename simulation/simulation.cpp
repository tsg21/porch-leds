#include <string>
#include <crgb.h>
#include "simulation.h"
#include "common_led.h"

// Define an instance of each pattern class
#define MAKE_INSTANCE(classname) classname instance_of_##classname
ALL_PATTERNS(MAKE_INSTANCE, ;);
#undef MAKE_INSTANCE

PatternRenderer* patternRenderer;

extern "C" bool setup(const char *name_asciz) {
  std::string name = name_asciz;

  // Translate each pattern name as a string into the instance of the class
#define CHECK_STRING(classname)                         \
  if (name == #classname) {                             \
    patternRenderer = &instance_of_##classname;         \
    return true;                                        \
  }
  ALL_PATTERNS(CHECK_STRING, else)
#undef CHECK_STRING

  return false;
}

extern "C" void render(CRGB* leds) {
  patternRenderer->render(leds);
}
