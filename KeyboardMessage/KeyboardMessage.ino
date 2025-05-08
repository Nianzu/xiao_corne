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

#define IS_LEFT

#define PIN 9
#define NUMPIXELS 21

#define ESPNOW_WIFI_CHANNEL 6

#define NUM_ROWS 5
#define NUM_COLS 5

#define PAIRING_BROADCAST_DELAY 1000

// Additional keyboard keycodes
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
    Serial.printf("Received a message from master " MACSTR " (%s)\n", MAC2STR(addr()), broadcast ? "broadcast" : "unicast");
    Serial.printf("  Message: %s\n", (char *)data);

      // Make a copy of the data to ensure null-termination
  char message[33]; // 32 + 1 for null terminator
  len = len > 32 ? 32 : len;
  memcpy(message, data, len);
  message[len] = '\0';

  Serial.printf("  Message: %s\n", message);

  // Parse message: format is 'P<num>' or 'R<num>'
  if (message[0] == 'P' || message[0] == 'R') {
    int keycode = atoi(&message[1]); // Convert from string to int
    if (keycode > 0 && keycode <= 255) { // Basic sanity check
      if (message[0] == 'P') {
        Keyboard.press(keycode);
        Serial.printf("Pressed keycode: %d\n", keycode);
      } else {
        Keyboard.release(keycode);
        Serial.printf("Released keycode: %d\n", keycode);
      }
    } else {
      Serial.println("Invalid keycode");
    }
  } else {
    Serial.println("Unknown message format");
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

// 0: Waiting
// 1: Sending broadcasts
// 2: Paired
int paring_state = 0;

unsigned long last_broadcast_ms = 0;

ESP_NOW_Broadcast_Peer broadcast_peer(ESPNOW_WIFI_CHANNEL, WIFI_IF_STA, NULL);
std::vector<ESP_NOW_Peer_Class> paired_devices;

void perform_action(Key key, bool down) {
  int sendcode = action_map[key.y][key.x];
  if (sendcode != 0) {
    if (paring_state == 2) {
      if (down) {
        Keyboard.press(sendcode);

        char data[32];
        snprintf(data, sizeof(data), "P%d", sendcode);
        paired_devices.back().send_message((uint8_t *)data, sizeof(data));

        pixels.setPixelColor(led_map[key.y][key.x], pixels.Color(150, 0, 0));
        pixels.show();

      } else {
        Keyboard.release(sendcode);

        char data[32];
        snprintf(data, sizeof(data), "R%d", sendcode);
        paired_devices.back().send_message((uint8_t *)data, sizeof(data));

        pixels.setPixelColor(led_map[key.y][key.x], pixels.Color(0, 0, 0));
        pixels.show();
      }
    } else {
      if (down) {
        if (key.y == 1 && key.x == 1) {
          paring_state = 1;
        }
      }
    }
  }
}

void register_new_peer(const esp_now_recv_info_t *info, const uint8_t *data, int len, void *arg) {
  Serial.println("New peer found");
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
    Serial.println("Sent reply");
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

  Serial.begin(9600);
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
    pixels.show();
  } else if (paring_state == 1) {
    if (last_broadcast_ms + PAIRING_BROADCAST_DELAY < millis()) {
      last_broadcast_ms = millis();
      char data[32];
      snprintf(data, sizeof(data), "Broadcast for pairing");
      broadcast_peer.send_message((uint8_t *)data, sizeof(data));
      Serial.println("Broadcasting");
    }

    pixels.setPixelColor(led_map[1][1], pixels.Color(100, 100, 100));
    pixels.show();
  }
  if (paring_state != 2) {
    if (paired_devices.size() > 0) {
      paring_state = 2;
      pixels.clear();
      pixels.show();
    }
  }
}
#endif /* ARDUINO_USB_MODE */