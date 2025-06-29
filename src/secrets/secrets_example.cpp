// secrets.cpp to hold the values of the secrets

#include "secrets.h"

// Note - esp32 boards can only connect to 2.4MHz networks.
const char *WIFI_SSID = "Your wifi SSID";
const char *WIFI_PASSWORD = "Your wifi password";
const char *UDP_HOST = "Host address";
const int UDP_PORT = 0000; // Insert port here
const char *OTA_PASSWORD = "An OTA Password"
// The MAC address of the Receiver ESP32 module
const uint8_t *HUB_MAC = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99};

