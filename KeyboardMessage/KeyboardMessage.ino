#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

#include "USB.h"
#include "USBHIDKeyboard.h"
#define IS_LEFT

#include <Adafruit_NeoPixel.h> // Note, must use version 1.12.3, later versions at least up to 1.12.5 are broken!!!!
#ifdef __AVR__
#include <avr/power.h>
#endif
#define PIN 9
#define NUMPIXELS 21

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

USBHIDKeyboard Keyboard;

#define KEY_NONE 0x00

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

#ifdef IS_LEFT
// Left Keymap
Key Keymap[5][5] = {
  { Key(0, 0), Key(1, 0), Key(2, 0), Key(3, 0), Key(4, 0) },
  { Key(5, 0), Key(4, 1), Key(4, 2), Key(5, 2), Key(5, 1) },
  { Key(2, 2), Key(1, 1), Key(3, 2), Key(3, 1), Key(2, 1) },
  { Key(1, 2), Key(2, 3), Key(1, 3), Key(0, 1), Key(0, 2) },
  { Key(0, 3), Key(-1, -1), Key(-1, -1), Key(-1, -1), Key(-1, -1) },
};
int action_map[4][6] = {
  { 't', 'r', 'e', 'w', 'q', KEY_NONE },
  { 'g', 'f', 'd', 's', 'a', KEY_TAB },
  { 'b', 'v', 'c', 'x', 'z', KEY_NONE },
  { KEY_NONE, KEY_SPACE, KEY_LEFT_CTRL, KEY_NONE, KEY_NONE, KEY_NONE },
};
#else
// Right Keymap
Key Keymap[5][5] = {
  { Key(5, 2), Key(5, 1), Key(4, 0), Key(4, 1), Key(5, 0) },
  { Key(2, 0), Key(1, 0), Key(0, 0), Key(3, 0), Key(3, 1) },
  { Key(3, 2), Key(2, 3), Key(2, 2), Key(1, 3), Key(4, 2) },
  { Key(2, 1), Key(0, 1), Key(0, 2), Key(1, 1), Key(1, 2) },
  { Key(0, 3), Key(-1, -1), Key(-1, -1), Key(-1, -1), Key(-1, -1) },
};
#endif

void perform_action(Key key, bool down) {
  int sendcode = action_map[key.y][key.x];
  if (sendcode != 0) {
    if (down) {
      Keyboard.press(sendcode);

      pixels.setPixelColor(1, pixels.Color(150, 0, 0));
      pixels.show();
      // Keyboard.print("pressing ");
      // Keyboard.println(sendcode);

    } else {
      Keyboard.release(sendcode);
      pixels.setPixelColor(1, pixels.Color(0, 0, 0));
      pixels.show();
      // Keyboard.print("releasing ");
      // Keyboard.println(sendcode);
    }
  }
}

int cols[5] = { 6, 43, 44, 7, 8 };
int rows[5] = { 1, 2, 3, 4, 5 };

void setup() {
  Serial1.end();

  // initialize control over the Keyboard:
  Keyboard.begin();
  USB.begin();

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();
  pixels.clear();
  pixels.show();

  pinMode(1, INPUT_PULLDOWN);
  pinMode(2, INPUT_PULLDOWN);
  pinMode(3, INPUT_PULLDOWN);
  pinMode(4, INPUT_PULLDOWN);
  pinMode(5, INPUT_PULLDOWN);
  pinMode(6, OUTPUT);
  pinMode(43, OUTPUT);
  pinMode(44, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
}

void loop() {

  for (int c = 0; c < sizeof cols / sizeof cols[0]; c++) {
    digitalWrite(cols[c], HIGH);
    delayMicroseconds(5); // Slight delay for signal settling
    for (int r = 0; r < sizeof rows / sizeof rows[0]; r++) {
      int current_state = digitalRead(rows[r]);
      delay(1);

      if (last_state[r][c] != current_state) {
        last_state[r][c] = current_state;

        perform_action(Keymap[r][c], current_state);
      }
    }
    digitalWrite(cols[c], LOW);
  }
}
#endif /* ARDUINO_USB_MODE */
