#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#error This sketch should be used when USB is in OTG mode
#else

#include "USB.h"
#include "USBHIDKeyboard.h"
// Note, must use version 1.12.3, later versions at least up to 1.12.5 are broken!!!!
#include <Adafruit_NeoPixel.h>

#define IS_LEFT

#define PIN 9
#define NUMPIXELS 21

#define NUM_ROWS 5
#define NUM_COLS 5

// Keyboard keycodes
#define ___ 0x00

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

USBHIDKeyboard Keyboard;

class Key {
public:
  int x;
  int y;
  Key(int x, int y) {
    this->x = x;
    this->y = y;
  }
};

int last_state[5][5] = {
  { 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0 },
};

int led_map[4][6] = {
  { 0, 1, 2, 3, 4, 5 },
  { 15, 14, 13, 10, 9, 6 },
  { 16, 17, 12, 11, 8, 7 },
  { 20, 19, 18, 0, 0, 0 },
};

// GPIO pins used for the matrix rows and columns
int gpio_cols[NUM_COLS] = { 6, 43, 44, 7, 8 };
int gpio_rows[NUM_ROWS] = { 1, 2, 3, 4, 5 };

// Left maps
#ifdef IS_LEFT
Key key_map[5][5] = {
  { Key(0, 0), Key(1, 0), Key(2, 0), Key(3, 0), Key(4, 0) },
  { Key(5, 0), Key(4, 1), Key(4, 2), Key(5, 2), Key(5, 1) },
  { Key(2, 2), Key(1, 1), Key(3, 2), Key(3, 1), Key(2, 1) },
  { Key(1, 2), Key(2, 3), Key(1, 3), Key(0, 1), Key(0, 2) },
  { Key(0, 3), Key(-1, -1), Key(-1, -1), Key(-1, -1), Key(-1, -1) },
};

int action_map[4][6] = {
  { 't', 'r', 'e', 'w', 'q', ___ },
  { 'g', 'f', 'd', 's', 'a', KEY_TAB },
  { 'b', 'v', 'c', 'x', 'z', ___ },
  { ___, KEY_SPACE, KEY_LEFT_CTRL, ___, ___, ___ },
};

// Right maps
#else
Key key_map[5][5] = {
  { Key(5, 2), Key(5, 1), Key(4, 0), Key(4, 1), Key(5, 0) },
  { Key(2, 0), Key(1, 0), Key(0, 0), Key(3, 0), Key(3, 1) },
  { Key(3, 2), Key(2, 3), Key(2, 2), Key(1, 3), Key(4, 2) },
  { Key(2, 1), Key(0, 1), Key(0, 2), Key(1, 1), Key(1, 2) },
  { Key(0, 3), Key(-1, -1), Key(-1, -1), Key(-1, -1), Key(-1, -1) },
};

int action_map[4][6] = {
  { 'y', 'u', 'i', 'o', 'p', ___ },
  { 'h', 'j', 'k', 'l', ';', KEY_BACKSPACE },
  { 'n', 'm', ',', '.', ___, ___ },
  { ___, KEY_KP_ENTER, KEY_LEFT_SHIFT, ___, ___, ___ },
};
#endif

void perform_action(Key key, bool down) {
  int sendcode = action_map[key.y][key.x];
  if (sendcode != 0) {
    if (down) {
      Keyboard.press(sendcode);

      pixels.setPixelColor(led_map[key.y][key.x], pixels.Color(150, 0, 0));
      pixels.show();

    } else {
      Keyboard.release(sendcode);
      pixels.setPixelColor(led_map[key.y][key.x], pixels.Color(0, 0, 0));
      pixels.show();
    }
  }
}

void setup() {
  // Initialize control over the Keyboard
  Keyboard.begin();
  USB.begin();

  // Startup the neopixel driver
  pixels.begin();
  pixels.clear();
  pixels.show();

  // Set the gpio direction
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(gpio_cols[c], OUTPUT);
    digitalWrite(gpio_cols[c], LOW);
  }
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(gpio_rows[r], INPUT_PULLDOWN);
  }
}

void loop() {
  for (int c = 0; c < NUM_COLS; c++) {
    digitalWrite(gpio_cols[c], HIGH);
    delayMicroseconds(5);  // Slight delay for signal settling
    for (int r = 0; r < NUM_ROWS; r++) {
      int current_state = digitalRead(gpio_rows[r]);
      delay(1);

      if (last_state[r][c] != current_state) {
        last_state[r][c] = current_state;

        perform_action(key_map[r][c], current_state);
      }
    }
    digitalWrite(gpio_cols[c], LOW);
  }
}
#endif /* ARDUINO_USB_MODE */