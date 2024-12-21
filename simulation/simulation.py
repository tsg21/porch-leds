import argparse
import ctypes
import pygame
import time
import sys

parser = argparse.ArgumentParser(
    description="Simulate the porch LEDs in a pygame window")
parser.add_argument("--library", default="./simulation.so",
                    help="Path to the simulation.so library to load")
parser.add_argument("--led-size", type=int, default=4,
                    help="Size of an LED in the simulation")
parser.add_argument("pattern", nargs="?", default="Snowflakes2",
                    help="Name of the pattern class to run")
args = parser.parse_args()

# Load the compiled C library
target = ctypes.CDLL(args.library)

target.setup.argtypes = [ctypes.c_char_p]
target.setup.restype = ctypes.c_bool
pattern_buffer = ctypes.create_string_buffer(args.pattern.encode())
if not target.setup(pattern_buffer):
    sys.exit(f"Unrecognised pattern '{args.pattern}'")

# Constants
LEDS_SIDE = 132
LEDS_ARCH = 82
LED_SIZE = args.led_size
SPACING = 0
R2O2 = 0.70710678
NUM_LEDS = LEDS_SIDE * 2 + LEDS_ARCH * 2
WINDOW_WIDTH = LEDS_ARCH * 2 * (LED_SIZE+SPACING)
WINDOW_HEIGHT = R2O2 * LEDS_ARCH * (LED_SIZE+SPACING) + LEDS_SIDE * (LED_SIZE+SPACING) + 20
BACKGROUND_COLOR = (0, 0, 0)
LED_OFF_COLOR = (0, 0, 0)

# Initialize pygame
pygame.init()
screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
pygame.display.set_caption("LED Simulator")

# LED buffer to hold RGB values
ledsArray = (ctypes.c_uint8 * (NUM_LEDS * 3))()
offsetPattern = ctypes.c_int(0)
lastUpdate = ctypes.c_ulong(0)

# Function to draw LEDs
def draw_leds():
    screen.fill(BACKGROUND_COLOR)

    position = (10.0, 2.0)

    for i in range(0, NUM_LEDS):
        r = ledsArray[i * 3 + 0]
        g = ledsArray[i * 3 + 1]
        b = ledsArray[i * 3 + 2]
        color = (r, g, b)

        if i < 132:
            position = (position[0], position[1]+1)
        elif i < 214:
            position = (position[0]+R2O2, position[1]+R2O2)
        elif i < 215:
            position = (position[0]+1, position[1])
        elif i < 296:
            position = (position[0]+R2O2, position[1]-R2O2)
        else:
            position = (position[0], position[1]-1)

        x = round(position[0] * (LED_SIZE + SPACING))
        y = WINDOW_HEIGHT - round(position[1] * (LED_SIZE + SPACING))

        pygame.draw.circle(screen, color, (x + LED_SIZE // 2, y + LED_SIZE // 2), LED_SIZE // 2)
        
    pygame.display.flip()

try:
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()

        target.render(ledsArray)
        draw_leds()
        time.sleep(0.02)  # Match the 20 ms update interval

except KeyboardInterrupt:
    pygame.quit()
    sys.exit()
