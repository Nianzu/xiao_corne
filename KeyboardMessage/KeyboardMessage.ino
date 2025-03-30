/*
  Keyboard Message test

  For the Arduino Leonardo and Micro.

  Sends a text string when a button is pressed.

  The circuit:
  - pushbutton attached from pin 0 to ground
  - 10 kilohm resistor attached from pin 0 to +5V

  created 24 Oct 2011
  modified 27 Mar 2012
  by Tom Igoe
  modified 11 Nov 2013
  by Scott Fitzgerald

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/KeyboardMessage
*/
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
#define PIN 9       // Adjust this to the correct data pin on your XIAO ESP32S3

// Create NeoPixel object
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

USBHIDKeyboard Keyboard;

int last_state[5][5] = {
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
};

int keymap[5][5] = {
  {1, 2, 3, 4, 5},
  {6, 7, 8, 9, 10},
  {11, 12, 13, 14, 15},
  {16, 17, 18, 19, 20},
  {21, 22, 23, 24, 25},
};

int cols[5] = {6, 43, 44, 7, 8};
int rows[5] = {1, 2, 3, 4, 5};

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
    pixels.setPixelColor(i, pixels.Color(255, 0, 0)); // Red
  }
  pixels.show(); // Send the updated pixel colors to the hardware
  delay(1000); // Wait for 1 second

  // Set all pixels to green
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Green
  }
  pixels.show();
  delay(1000); // Wait for 1 second

  // Set all pixels to blue
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255)); // Blue
  }
  pixels.show();
  delay(1000); // Wait for 1 second
    for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255)); // Blue
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
          Keyboard.println(keymap[r][c]);
        }
      }

    }
    digitalWrite(cols[c], LOW);
  }
}
#endif /* ARDUINO_USB_MODE */

10
7
14
15
7
14
15
7
15
13
11
9
9
8
13
11
20
16
17
17
21
18
21
19
12
15
15
14
7
14
10
6
5
4
15
3
2
1
1
1
19
12
15
14
7
10
10
7
14
15
12
15
12
14
15
12
19
15
12
19
20
11
16
11
16
20
11
13
16
20
13
16
20
20
16
20
16
20
16
20
16
20
16
20
11
13
8
9
9
8
13
11
9
8
13
11
9
8
13
11
9
8
9
8
9
9
9
9
9
8
13
11
9
8
13
11
9
8
13
11
9
8
13
11
9
8
13
11
11
13
8
9
9
8
13
11
9
8
13
11
9
8
13
11
11
8
9
9
8
13
11
9
8
13
11
9
8
13
11
11
13
8
9
9
8
13
11
9
8
13
11
9
8
13
11
11
13
8
9
9
8
13
11
9
8
13
11
9
8
13
11
11
13
8
9
9
8
13
11
9
8
13
11
9
8
13
11
11
13
8
9
9
8
13
11
9
8
13
11
9
8
13
11
11
13
8
9
9
8
13
11
9
8
13
11
9
8
13
11
11
13
8
9
9
8
13
11
9
8
13
11
9
8
13
11
11
13
8
9
9
8
13
11
9
8
13
11
9
8
13
11
11
13
8
9
21
