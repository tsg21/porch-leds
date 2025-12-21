# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP32-based LED controller for 428 NeoPixel LEDs arranged in a porch-shaped arch (two vertical sides + two slopes). The system uses PlatformIO with Arduino framework and integrates with Home Assistant via MQTT.

## Build and Upload Commands

```bash
# Build firmware
pio run

# Upload to device (OTA)
pio run --target upload

# Upload via serial (comment out upload_protocol in platformio.ini first)
pio run --target upload

# Monitor serial output
pio device monitor
```

## Simulation Environment

The simulation allows testing patterns without hardware:

```bash
cd simulation
pipenv install
make && pipenv run python simulation.py
```

The pattern being simulated is controlled in `simulation/simulation.cpp`.

## Architecture

### Pattern System

All patterns inherit from `PatternRenderer` (defined in `include/led_common.h`):
- `render(CRGB* leds)` is called every 20ms (see `INTERVAL_MICROS` in `src/main.cpp`)
- Pattern instances are created globally in `src/main.cpp` using the `ALL_PATTERNS` macro

### Adding a New Pattern

1. Create implementation in `src/patterns/<name>.cpp`
2. Declare pattern class in `include/patterns.h`:
   - Use `PATTERN_INSTANCE(Name)` for simple patterns
   - Use `COMPLEX_PATTERN_INSTANCE(Name)` for patterns with private implementation (pimpl idiom)
3. Add to `ALL_PATTERNS` macro in `include/patterns.h` - this automatically:
   - Creates instance in `main.cpp`
   - Registers with MQTT configuration
   - Adds to Home Assistant select options

### Hardware Configuration

- Board: Seeed XIAO ESP32-C3
- LED strip: 428 NeoPixels on pin D4
- Layout: 132 LEDs per vertical side, 82 LEDs per slope
- Total LEDs defined as `NUM_LEDS` in `include/led_common.h`

### MQTT Integration

The system (`src/mqtt.cpp`) automatically:
- Connects to WiFi and MQTT broker (credentials in `include/secrets.h`)
- Publishes Home Assistant discovery config on connect
- Subscribes to pattern change commands
- Supports OTA updates on port 3232

Pattern changes come via `homeassistant/select/porch_lights_mode/set` topic.

### Multi-Pattern System

For patterns composed of multiple independent items (e.g., snowflakes):
- Inherit from `MultiPatternItem` instead of `PatternRenderer`
- Implement `update()`, `isDead()`, and `render(CRGB* leds)`
- Use `MultiPatternRenderer` to manage the collection
- Dead items are automatically cleaned up each frame

## Code Standards

- C++17 (`-std=gnu++2a` in platformio.ini)
- FastLED 3.9.7 for LED control
- Use `CRGB` type for color values
