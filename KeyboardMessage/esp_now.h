#include "ESP32_NOW.h"

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
    log_v("Received a unicast message from " MACSTR, MAC2STR(info->src_addr));
    log_v("Igorning the message");
  }
}