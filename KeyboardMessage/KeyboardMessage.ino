#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

#define KEY_A 0x04 // Keyboard a and A
#define KEY_B 0x05 // Keyboard b and B
#define KEY_C 0x06 // Keyboard c and C
#define KEY_D 0x07 // Keyboard d and D
#define KEY_E 0x08 // Keyboard e and E
#define KEY_F 0x09 // Keyboard f and F
#define KEY_G 0x0a // Keyboard g and G
#define KEY_H 0x0b // Keyboard h and H
#define KEY_I 0x0c // Keyboard i and I
#define KEY_J 0x0d // Keyboard j and J
#define KEY_K 0x0e // Keyboard k and K
#define KEY_L 0x0f // Keyboard l and L
#define KEY_M 0x10 // Keyboard m and M
#define KEY_N 0x11 // Keyboard n and N
#define KEY_O 0x12 // Keyboard o and O
#define KEY_P 0x13 // Keyboard p and P
#define KEY_Q 0x14 // Keyboard q and Q
#define KEY_R 0x15 // Keyboard r and R
#define KEY_S 0x16 // Keyboard s and S
#define KEY_T 0x17 // Keyboard t and T
#define KEY_U 0x18 // Keyboard u and U
#define KEY_V 0x19 // Keyboard v and V
#define KEY_W 0x1a // Keyboard w and W
#define KEY_X 0x1b // Keyboard x and X
#define KEY_Y 0x1c // Keyboard y and Y
#define KEY_Z 0x1d // Keyboard z and Z

#include "USB.h"
#include "USBHIDKeyboard.h"
#include <Adafruit_NeoPixel.h>

// Define the number of NeoPixels and the data pin
#define NUMPIXELS 21  // Adjust this to the actual number of pixels
#define PIN 9         // Adjust this to the correct data pin on your XIAO ESP32S3

#define IS_LEFT

// Create NeoPixel object
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

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
  { Key(0, 3), Key(1, 3), Key(2, 3), Key(3, 3), Key(4, 3) },
  { Key(5, 3), Key(4, 2), Key(4, 1), Key(5, 1), Key(5, 2) },
  { Key(2, 1), Key(1, 2), Key(3, 1), Key(3, 2), Key(2, 2) },
  { Key(1, 1), Key(2, 0), Key(1, 0), Key(0, 2), Key(0, 1) },
  { Key(0, 0), Key(-1, -1), Key(-1, -1), Key(-1, -1), Key(-1, -1) },
};
int action_map[4][6] = {
  { 't', 'r', 'e', 'w', 'q', KEY_NONE },
  { KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE },
  { KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE },
  { KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE },
};
#else
// Right Keymap
Key Keymap[5][5] = {
  { Key(5, 1), Key(5, 2), Key(4, 3), Key(4, 2), Key(5, 3) },
  { Key(2, 3), Key(1, 3), Key(0, 3), Key(3, 3), Key(3, 2) },
  { Key(3, 1), Key(2, 0), Key(2, 1), Key(1, 0), Key(4, 1) },
  { Key(2, 2), Key(0, 2), Key(0, 1), Key(1, 2), Key(1, 1) },
  { Key(0, 0), Key(-1, -1), Key(-1, -1), Key(-1, -1), Key(-1, -1) },
};
#endif


int cols[5] = { 6, 43, 44, 7, 8 };
int rows[5] = { 1, 2, 3, 4, 5 };

void perform_action(Key key, bool down) {
  int sendcode = action_map[key.y][key.x];
  if (sendcode != 0)
  {
    if (down)
    {
    Keyboard.press(sendcode);

    } else
    {
    Keyboard.release(sendcode);

    }
  }
}

void setup() {
  Serial1.end();
  for (int r = 0; r < sizeof rows / sizeof rows[0]; r++) {
    pinMode(rows[r], INPUT_PULLDOWN);
  }
  for (int c = 0; c < sizeof cols / sizeof cols[0]; c++) {
    pinMode(cols[c], OUTPUT);
    digitalWrite(cols[c], LOW);
  }

  // initialize control over the Keyboard:
  Keyboard.begin();
  USB.begin();

  pixels.begin();

  // Set all pixels to red
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));  // Red
  }
  pixels.show();  // Send the updated pixel colors to the hardware
  delay(250);     // Wait for 1 second

  // Set all pixels to green
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));  // Green
  }
  pixels.show();
  delay(250);  // Wait for 1 second

  // Set all pixels to blue
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));  // Blue
  }
  pixels.show();


  // Set all pixels to White
  delay(250);  // Wait for 1 second
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));  // White
  }
  pixels.show();


  // Set all pixels off
  delay(250);  // Wait for 1 second
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));  // Off
  }
  pixels.show();
}


void loop() {
  for (int c = 0; c < sizeof cols / sizeof cols[0]; c++) {
    digitalWrite(cols[c], HIGH);
    for (int r = 0; r < sizeof rows / sizeof rows[0]; r++) {
      int current_state = digitalRead(rows[r]);
      delay(1);

      if (last_state[r][c] != current_state) {
        last_state[r][c] = current_state;
        if (current_state) {

          perform_action(Keymap[r][c],current_state);

        }
      }
    }
    digitalWrite(cols[c], LOW);
  }
}
#endif /* ARDUINO_USB_MODE */


