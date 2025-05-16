#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#error This sketch should be used when USB is in OTG mode
#else

#include "USB.h"
// Note, must use version 1.12.3, later versions at least up to 1.12.5 are broken!!!!
#include <Adafruit_NeoPixel.h>
#include "WiFi.h"
// For the MAC2STR and MACSTR macros
#include <esp_mac.h>
#include <vector>

#include "types.h"
#include "esp_now.h"

#define IS_LEFT

#ifdef IS_LEFT
#include "layout_left.h"
#else
#include "layout_right.h"
#endif

#define PIN 9
#define NUMPIXELS 21
#define ESPNOW_WIFI_CHANNEL 6
#define NUM_ROWS 5
#define NUM_COLS 5
#define PAIRING_BROADCAST_DELAY 1000

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

USBHIDKeyboard Keyboard;

int active_layer = 0;
bool LEDs_updated = false;


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

// 0: Waiting
// 1: Sending broadcasts
// 2: Paired
int pairing_state = 0;

unsigned long last_broadcast_ms = 0;

ESP_NOW_Broadcast_Peer broadcast_peer(ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);
std::vector<ESP_NOW_Peer_Class> paired_devices;

void handle_keycode(int keycode, bool down) {
  if (down) {
    Keyboard.press(keycode);
  } else {
    Keyboard.release(keycode);
  }
  if (pairing_state == 2) {
    char data[32];
    snprintf(data, sizeof(data), "%c%d", down ? 'P' : 'R', keycode);
    paired_devices.back().send_message((uint8_t *)data, sizeof(data));
  }
}

void handle_layer(int layer) {
  active_layer = layer;
  Keyboard.releaseAll();
  char data[32];
  if (pairing_state == 2) {
    snprintf(data, sizeof(data), "L%d", active_layer);
    paired_devices.back().send_message((uint8_t *)data, sizeof(data));
  }
}

void handle_macro(int macro) {
  // future work
}

void perform_action(Key key, bool down) {
  Action action = action_map[active_layer][key.y][key.x];

  switch (action.type) {
    case ACTION_NONE:
      break;
    case ACTION_KEYCODE:
      handle_keycode(action.value, down);
      break;
    case ACTION_LAYER:
      if (down) handle_layer(action.value);
      break;
    case ACTION_MACRO:
      if (down) handle_macro(action.value);
      break;
  }
  if (down && pairing_state != 2) {
    if (key.y == 1 && key.x == 1) {
      pairing_state = 1;
    }
  }

  pixels.setPixelColor(led_map[key.y][key.x], down ? pixels.Color(150, 0, 0) : pixels.Color(0, 0, 0));
  LEDs_updated = true;
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

  // Initialize the Wi-Fi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(ESPNOW_WIFI_CHANNEL);
  while (!WiFi.STA.started()) {
    delay(100);
  }

  // Register the broadcast peer
  if (!broadcast_peer.begin()) {
    delay(5000);
    ESP.restart();
  }

  // Register the new peer callback
  ESP_NOW.onNewPeer(register_new_peer, NULL);
}

void loop() {
  // Matrix scanning
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

  // Pairing / initialization logic
  if (pairing_state == 0) {
    pixels.setPixelColor(led_map[1][1], pixels.Color(0, 100, 100));
    LEDs_updated = true;
  } else if (pairing_state == 1) {
    if (last_broadcast_ms + PAIRING_BROADCAST_DELAY < millis()) {
      last_broadcast_ms = millis();
      char data[32];
      snprintf(data, sizeof(data), "Broadcast for pairing");
      broadcast_peer.send_message((uint8_t *)data, sizeof(data));
    }

    pixels.setPixelColor(led_map[1][1], pixels.Color(100, 100, 100));
    LEDs_updated = true;
  }
  if (pairing_state != 2) {
    if (paired_devices.size() > 0) {
      pairing_state = 2;
      pixels.clear();
      LEDs_updated = true;
    }
  }
  if (LEDs_updated) {
    pixels.show();
    LEDs_updated = false;
  }
}
#endif /* ARDUINO_USB_MODE */
