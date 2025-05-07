#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

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

class key {
public:
  int x;
  int y;
  key(int x, int y) {
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
// Left keymap
key keymap[5][5] = {
  { key(0, 3), key(1, 3), key(2, 3), key(3, 3), key(4, 3) },
  { key(5, 3), key(4, 2), key(4, 1), key(5, 1), key(5, 2) },
  { key(2, 1), key(1, 2), key(3, 1), key(3, 2), key(2, 2) },
  { key(1, 1), key(2, 0), key(1, 0), key(0, 2), key(0, 1) },
  { key(0, 0), key(-1, -1), key(-1, -1), key(-1, -1), key(-1, -1) },
};
#else
// Right keymap
key keymap[5][5] = {
  { key(5, 1), key(5, 2), key(4, 3), key(4, 2), key(5, 3) },
  { key(2, 3), key(1, 3), key(0, 3), key(3, 3), key(3, 2) },
  { key(3, 1), key(2, 0), key(2, 1), key(1, 0), key(4, 1) },
  { key(2, 2), key(0, 2), key(0, 1), key(1, 2), key(1, 1) },
  { key(0, 0), key(-1, -1), key(-1, -1), key(-1, -1), key(-1, -1) },
};
#endif




int cols[5] = { 6, 43, 44, 7, 8 };
int rows[5] = { 1, 2, 3, 4, 5 };

void setup() {
  Serial1.end();
  for (int r = 0; r < sizeof rows / sizeof rows[0]; r++) {
    pinMode(rows[r], INPUT_PULLDOWN);
  }
  for (int c = 0; c < sizeof cols / sizeof cols[0]; c++) {
    pinMode(cols[c], OUTPUT);
    digitalWrite(cols[c], LOW);
  }

  // initialize control over the keyboard:
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

          Keyboard.print(keymap[r][c].x);
          Keyboard.print(",");
          Keyboard.println(keymap[r][c].y);
        }
      }
    }
    digitalWrite(cols[c], LOW);
  }
}
#endif /* ARDUINO_USB_MODE */
