#include <Arduino.h>
  #include <Wire.h>
  #include <Ticker.h>
  #include <ESP8266WiFi.h>
  #include <WiFiUdp.h>
  #include <ArduinoJson.h>
  #include "secrets.h"
  #include "version.h"

  Ticker soilMoistureTicker;
  Ticker wifiTicker;

  WiFiUDP udp;

  // The ESP8266 can only have 1 soil pin.
  const int soilPin = A0;
  const int led = LED_BUILTIN;

// In milliseconds
const unsigned int soil_moisture_frequency = 600;

bool already_connected = false;

void soilMoistureSensorAction();

void sendJson(const char* type, JsonVariant data) {
    StaticJsonDocument<256> doc;
    doc["type"] = type;
    doc["data"] = data;

    char buffer[256];
    size_t len = serializeJson(doc, buffer);

    udp.beginPacket(UDP_HOST, UDP_PORT);
    udp.write((uint8_t*)buffer, len);
    udp.endPacket();
}

void sendStatusMessage(const char* msg) {
    StaticJsonDocument<64> data;
    data["message"] = msg;
    sendJson("status", data.as<JsonVariant>());
}

void initWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void testWifiConnection() {
    int status = WiFi.status();
    if (status == WL_CONNECTED) {
        if (!already_connected) {
            Serial.printf("Connected to %s\n", WiFi.localIP());
            udp.begin(UDP_PORT);
            Serial.printf("UDP Started\n");
            char message[64];
            snprintf(message, sizeof(message), "Moisture Sensor %s Connected", FIRMWARE_VERSION);
            sendStatusMessage(message);
            soilMoistureSensorAction();
            already_connected = true;
        }
    } else {
        if (status == WL_CONNECT_FAILED) {
            already_connected = false;
            Serial.println("Unable to connect to WiFi.");
        } else if (status == WL_DISCONNECTED || status == WL_CONNECTION_LOST) {
            WiFi.reconnect();
            Serial.println("Connection to WiFi Lost.");
        } else {
            Serial.println("Still Connecting...");
        }
    }
}

void soilMoistureSensorAction() {
    analogRead(soilPin);
    delay(10);
    int soilMoistureValue = analogRead(soilPin);
    Serial.print("Soil Moisture: "); Serial.print(soilMoistureValue); Serial.println(" units");
    StaticJsonDocument<64> data;
    data["units"] = soilMoistureValue;
    data["sensorNum"]  = 1;
    sendJson("soilMoisture", data.as<JsonVariant>());
}

void setup() {
       Serial.begin(115200);
       initWifi();
       // Initialize the Wire
       Wire.begin();

       Serial.printf("Firmware %s started.\n Initializing tickers\n", FIRMWARE_VERSION);

       // Check wifi every 5 seconds
       wifiTicker.attach(5.0, testWifiConnection);

       // Check the moisture sensors once every soil_moisture_frequency
       soilMoistureTicker.attach_ms(soil_moisture_frequency, soilMoistureSensorAction);
}

void loop() {
	// Nothing at the moment
}
