#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#error This sketch should be used when USB is in OTG mode
#else

#include "USB.h"
#include "USBHIDKeyboard.h"
// Note, must use version 1.12.3, later versions at least up to 1.12.5 are broken!!!!
#include <Adafruit_NeoPixel.h>
#include "ESP32_NOW.h"
#include "WiFi.h"
// For the MAC2STR and MACSTR macros
#include <esp_mac.h>
#include <vector>
f
#define IS_LEFT

#define PIN 9
#define NUMPIXELS 21

#define ESPNOW_WIFI_CHANNEL 6

#define NUM_ROWS 5
#define NUM_COLS 5

#define PAIRING_BROADCAST_DELAY 1000

// Additional keyboard keycodes
#define ___ 0x00
#define LT0 -1
#define LT1 -2
#define LT2 -3
#define LT3 -4
#define LT4 -5

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

USBHIDKeyboard Keyboard;

int active_layer = 0;
bool LEDs_updated = false;

class Key {
public:
  int x;
  int y;
  Key(int x, int y) {
    this->x = x;
    this->y = y;
  }
};

enum ActionType {
  ACTION_NONE,
  ACTION_KEYCODE,
  ACTION_LAYER,
  ACTION_MACRO
};

struct Action {
  ActionType type;
  int value;

  constexpr Action()
    : type(ACTION_NONE), value(0) {}
  constexpr Action(ActionType type, int value)
    : type(type), value(value) {}
};

#define ____ Action()
#define KEY(k) Action(ACTION_KEYCODE, k)
#define LAY(l) Action(ACTION_LAYER, l)
#define MACRO(m) Action(ACTION_MACRO, m)

class ESP_NOW_Broadcast_Peer : public ESP_NOW_Peer {
public:
  // Constructor of the class using the broadcast address
  ESP_NOW_Broadcast_Peer(uint8_t channel, wifi_interface_t iface, const uint8_t *lmk)
    : ESP_NOW_Peer(ESP_NOW.BROADCAST_ADDR, channel, iface, lmk) {}

  // Destructor of the class
  ~ESP_NOW_Broadcast_Peer() {
    remove();
  }

  // Function to properly initialize the ESP-NOW and register the broadcast peer
  bool begin() {
    if (!ESP_NOW.begin() || !add()) {
      log_e("Failed to initialize ESP-NOW or register the broadcast peer");
      return false;
    }
    return true;
  }

  // Function to send a message to all devices within the network
  bool send_message(const uint8_t *data, size_t len) {
    if (!send(data, len)) {
      log_e("Failed to broadcast message");
      return false;
    }
    return true;
  }
};

class ESP_NOW_Peer_Class : public ESP_NOW_Peer {
public:
  // Constructor of the class
  ESP_NOW_Peer_Class(const uint8_t *mac_addr, uint8_t channel, wifi_interface_t iface, const uint8_t *lmk)
    : ESP_NOW_Peer(mac_addr, channel, iface, lmk) {}

  // Destructor of the class
  ~ESP_NOW_Peer_Class() {}

  // Function to register the master peer
  bool add_peer() {
    if (!add()) {
      log_e("Failed to register the broadcast peer");
      return false;
    }
    return true;
  }

  // Function to print the received messages from the master
  void onReceive(const uint8_t *data, size_t len, bool broadcast) {

    // Make a copy of the data to ensure null-termination
    char message[33];  // 32 + 1 for null terminator
    len = len > 32 ? 32 : len;
    memcpy(message, data, len);
    message[len] = '\0';

    // Parse message: format is 'P<num>' or 'R<num>' or 'L<num>'
    if (message[0] == 'P' || message[0] == 'R' || message[0] == 'L') {
      int keycode = atoi(&message[1]);  // Convert from string to int
      if (message[0] == 'P') {
        Keyboard.press(keycode);
      } else if (message[0] == 'R') {
        Keyboard.release(keycode);

      } else if (message[0] == 'L') {
        active_layer = keycode;
        Keyboard.releaseAll();
      }
    }
  }

  // Function to send a message to a device
  bool send_message(const uint8_t *data, size_t len) {
    if (!send(data, len)) {
      log_e("Failed to broadcast message");
      return false;
    }
    return true;
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

Action action_map[5][4][6] = {
  {
    { KEY('t'), KEY('r'), KEY('e'), KEY('w'), KEY('q'), ____ },
    { KEY('g'), KEY('f'), KEY('d'), KEY('s'), KEY('a'), KEY(KEY_TAB) },
    { KEY('b'), KEY('v'), KEY('c'), KEY('x'), KEY('z'), ____ },
    { LAY(1), KEY(KEY_SPACE), KEY(KEY_LEFT_CTRL), ____, ____, ____ },
  },
  {
    { ____, KEY('9'), KEY('8'), KEY('7'), ____, ____ },
    { ____, KEY('6'), KEY('5'), KEY('4'), KEY('0'), ____ },
    { ____, KEY('3'), KEY('2'), KEY('1'), ____, ____ },
    { LAY(0), KEY(KEY_SPACE), KEY(KEY_LEFT_CTRL), ____, ____, ____ },
  },
  {
    { KEY('^'), KEY(']'), KEY('['), KEY('_'), KEY('\"'), ____ },
    { KEY('*'), KEY('}'), KEY('{'), KEY('-'), KEY('/'), ____ },
    { KEY('`'), KEY('~'), KEY('|'), KEY('$'), KEY('#'), ____ },
    { LAY(3), KEY(KEY_SPACE), KEY(KEY_LEFT_CTRL), ____, ____, ____ },
  },
  {
    { KEY('r'), KEY('e'), KEY('w'), KEY('q'), ____, ____ },
    { KEY('f'), KEY('d'), KEY('s'), KEY('a'), KEY(KEY_LEFT_SHIFT), KEY(KEY_TAB) },
    { KEY('v'), KEY('c'), KEY('x'), KEY('z'), ____, ____ },
    { LAY(2), KEY(KEY_SPACE), KEY(KEY_LEFT_CTRL), ____, ____, ____ },
  },
  {
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
  }
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

Action action_map[5][4][6] = {
  {
    { KEY('y'), KEY('u'), KEY('i'), KEY('o'), KEY('p'), ____ },
    { KEY('h'), KEY('j'), KEY('k'), KEY('l'), KEY(';'), KEY(KEY_BACKSPACE) },
    { KEY('n'), KEY('m'), KEY(','), KEY('.'), ____, ____ },
    { LAY(2), KEY(KEY_KP_ENTER), KEY(KEY_LEFT_SHIFT), ____, ____, ____ },
  },
  {
    { ____, KEY(KEY_LEFT_ALT), KEY(KEY_TAB), ____, ____, ____ },
    { KEY(KEY_LEFT_GUI), KEY(KEY_LEFT_ARROW), KEY(KEY_DOWN_ARROW), KEY(KEY_UP_ARROW), KEY(KEY_RIGHT_ARROW), KEY(KEY_BACKSPACE) },
    { ____, KEY(KEY_HOME), ____, KEY(KEY_PRINT_SCREEN), KEY(KEY_END), ____ },
    { LAY(3), KEY(KEY_KP_ENTER), KEY(KEY_LEFT_SHIFT), ____, ____, ____ },
  },
  {
    { KEY('!'), KEY('<'), KEY('>'), KEY('='), KEY('&'), ____ },
    { KEY('?'), KEY('('), KEY(')'), KEY('\''), KEY(':'), KEY(KEY_BACKSPACE) },
    { KEY('+'), KEY('%'), KEY('\\'), KEY('@'), ____, ____ },
    { LAY(0), KEY(KEY_KP_ENTER), KEY(KEY_LEFT_SHIFT), ____, ____, ____ },
  },
  {
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { LAY(1), ____, ____, ____, ____, ____ },
  },
  {
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
    { ____, ____, ____, ____, ____, ____ },
  },
};

#endif

// 0: Waiting
// 1: Sending broadcasts
// 2: Paired
int paring_state = 0;

unsigned long last_broadcast_ms = 0;

ESP_NOW_Broadcast_Peer broadcast_peer(ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);
std::vector<ESP_NOW_Peer_Class> paired_devices;

void handle_keycode(int keycode, bool down) {
  if (down) {
    Keyboard.press(keycode);
  } else {
    Keyboard.release(keycode);
  }
  if (paring_state == 2) {
    char data[32];
    snprintf(data, sizeof(data), "%c%d", down ? 'P' : 'R', keycode);
    paired_devices.back().send_message((uint8_t *)data, sizeof(data));
  }
}

void handle_layer(int layer) {
  active_layer = layer;
  Keyboard.releaseAll();
  char data[32];
  if (paring_state == 2) {
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
  if (down && paring_state != 2) {
    if (key.y == 1 && key.x == 1) {
      paring_state = 1;
    }
  }

  pixels.setPixelColor(led_map[key.y][key.x], down ? pixels.Color(150, 0, 0) : pixels.Color(0, 0, 0));
  LEDs_updated = true;
}

void register_new_peer(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg) {
  if (paired_devices.size() == 0) {

    ESP_NOW_Peer_Class new_peer(info->src_addr, ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);

    paired_devices.push_back(new_peer);
    if (!paired_devices.back().add_peer()) {
      log_v("Failed to register the new master");
      return;
    }
    char data[32];
    snprintf(data, sizeof(data), "Unicast reply for pairing");
    broadcast_peer.send_message((uint8_t *)data, sizeof(data));
  } else {
    // The slave will only receive broadcast messages
    log_v("Received a unicast message from " MACSTR, MAC2STR(info->src_addr));
    log_v("Igorning the message");
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

  if (paring_state == 0) {
    pixels.setPixelColor(led_map[1][1], pixels.Color(0, 100, 100));
    LEDs_updated = true;
  } else if (paring_state == 1) {
    if (last_broadcast_ms + PAIRING_BROADCAST_DELAY < millis()) {
      last_broadcast_ms = millis();
      char data[32];
      snprintf(data, sizeof(data), "Broadcast for pairing");
      broadcast_peer.send_message((uint8_t *)data, sizeof(data));
    }

    pixels.setPixelColor(led_map[1][1], pixels.Color(100, 100, 100));
    LEDs_updated = true;
  }
  if (paring_state != 2) {
    if (paired_devices.size() > 0) {
      paring_state = 2;
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
