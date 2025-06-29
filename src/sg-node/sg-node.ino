#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <time.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include "secrets.h"
#include "version.h"

Ticker wifiTicker;

bool already_connected = false;

void OnDataRecv(const uint8_t *, const uint8_t *, int);

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
    char recvData[250];

    memcpy(&recvData, incomingData, sizeof(recvData));
    Serial.printf("Bytes Received: %d\n", len);
    Serial.printf("Json: %s\n", recvData);
}

void initWifi() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void initESPNow() {
    if (esp_now_init() != ESP_OK) {
        Serial.printf("Error initializting ESP-NOW.\n");
    } else {
        esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
        Serial.printf("ESP-NOW initialized for Receiving.\n");
    }
}

void testWifiConnection() {
    int status = WiFi.status();
    if (status == WL_CONNECTED) {
        if (!already_connected) {
            Serial.printf("Connected to %s\n", WiFi.localIP());
            Serial.printf("MAC: %s\n", WiFi.macAddress());
            ArduinoOTA.begin();
            Serial.printf("OTA Started\n");
            already_connected = true;
        }
    } else {
        if (status == WL_CONNECT_FAILED) {
            already_connected = false;
            Serial.printf("Unable to connect to WiFi.\n");
        } else if (status == WL_DISCONNECTED || status == WL_CONNECTION_LOST) {
            WiFi.reconnect();
            Serial.printf("Connection to WiFi Lost.\n");
        } else {
            Serial.printf("Still Connecting...\n");
        }
    }
}

void OTAUpdateSetup() {
    ArduinoOTA.setHostname("smartGarden");

    ArduinoOTA.onStart([]() {
        const char* type = ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem";
        Serial.printf("Start updating %s\n", String(type));
    });

    ArduinoOTA.onEnd([]() {
        Serial.printf("\nOTA: Update complete.");
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("OTA Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.printf("Auth Failed\n");
        else if (error == OTA_BEGIN_ERROR) Serial.printf("Begin Failed\n");
        else if (error == OTA_CONNECT_ERROR) Serial.printf("Connect Failed\n");
        else if (error == OTA_RECEIVE_ERROR) Serial.printf("Receive Failed\n");
        else if (error == OTA_END_ERROR) Serial.printf("End Failed\n");

        char message[64];
        snprintf(message, sizeof(message), "OTA: Error[%u]", error);
    });

    ArduinoOTA.setPassword(OTA_PASSWORD);
}

void setup() {
       Serial.begin(115200);

       OTAUpdateSetup();
       initWifi();

       // Check wifi every 5 seconds
       wifiTicker.attach(5.0, testWifiConnection);
}

void loop() {
    ArduinoOTA.handle();
}
