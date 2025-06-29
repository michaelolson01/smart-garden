#include <Arduino.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_SHT31.h>
#include <Ticker.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "version.h"

BH1750 lightMeter;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Ticker soilMoistureTicker1;
Ticker soilMoistureTicker2;
Ticker soilMoistureTicker3;
Ticker soilMoistureTicker4;
Ticker lightLevelTicker;
Ticker wifiTicker;

WiFiUDP udp;

// One for each soil moisture monitor
const int soilPin1 = 32;
const int soilPin2 = 33;
const int soilPin3 = 34;
const int soilPin4 = 35;

// In milliseconds
const unsigned int soil_moisture_frequency = 600000;
const unsigned int light_frequency = 60000;

bool already_connected = false;

void soilMoistureSensor1Action();
void soilMoistureSensor2Action();
void soilMoistureSensor3Action();
void soilMoistureSensor4Action();
void lightLevelSensorAction();

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
			Serial.println(WiFi.macAddress());
            udp.begin(UDP_PORT);
            Serial.printf("UDP Started\n");
            char message[64];
            snprintf(message, sizeof(message), "Smart Gardent %s Connected", FIRMWARE_VERSION);
            sendStatusMessage(message);
            ArduinoOTA.begin();
            Serial.printf("OTA Started\n");
            sendStatusMessage("OTA Started");
            soilMoistureSensor1Action();
            soilMoistureSensor2Action();
            soilMoistureSensor3Action();
            soilMoistureSensor4Action();
            lightLevelSensorAction();
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

void OTAUpdateSetup() {
    ArduinoOTA.setHostname("smartGarden");

    ArduinoOTA.onStart([]() {
        const char* type = ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem";
        Serial.println("Start updating " + String(type));
        char message[64];
        snprintf(message, sizeof(message), "OTA: Start updating %s", type);
        sendStatusMessage(message);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nOTA: Update complete.");
        sendStatusMessage("OTA: Update Complete");
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("OTA Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");

        char message[64];
        snprintf(message, sizeof(message), "OTA: Error[%u]", error);
        sendStatusMessage(message);
    });

    ArduinoOTA.setPassword(OTA_PASSWORD);

}

void soilMoistureSensor1Action() {
    analogRead(soilPin1);
    delay(10);
    int soilMoistureValue = analogRead(soilPin1);
    Serial.print("Soil Moisture 1: "); Serial.print(soilMoistureValue); Serial.println(" units");
    StaticJsonDocument<64> data;
    data["units"] = soilMoistureValue;
    data["sensorNum"]  = 1;
    sendJson("soilMoisture", data.as<JsonVariant>());
}

void soilMoistureSensor2Action() {
    analogRead(soilPin2);
    delay(10);
    int soilMoistureValue = analogRead(soilPin2);
    Serial.print("Soil Moisture 2: "); Serial.print(soilMoistureValue); Serial.println(" units");
    StaticJsonDocument<64> data;
    data["units"] = soilMoistureValue;
    data["sensorNum"]  = 2;
    sendJson("soilMoisture", data.as<JsonVariant>());
}

void soilMoistureSensor3Action() {
    analogRead(soilPin3);
    delay(10);
    int soilMoistureValue = analogRead(soilPin3);
    Serial.print("Soil Moisture 3: "); Serial.print(soilMoistureValue); Serial.println(" units");
    StaticJsonDocument<64> data;
    data["units"] = soilMoistureValue;
    data["sensorNum"]  = 3;
    sendJson("soilMoisture", data.as<JsonVariant>());
}

void soilMoistureSensor4Action() {
    analogRead(soilPin4);
    delay(10);
    int soilMoistureValue = analogRead(soilPin4);
    Serial.print("Soil Moisture 4: "); Serial.print(soilMoistureValue); Serial.println(" units");
    StaticJsonDocument<64> data;
    data["units"] = soilMoistureValue;
    data["sensorNum"]  = 4;
    sendJson("soilMoisture", data.as<JsonVariant>());
}

void lightLevelSensorAction() {
    uint16_t lux = lightMeter.readLightLevel();
    Serial.print("Light: "); Serial.print(lux); Serial.println(" lx");
    StaticJsonDocument<64> data;
    data["lux"] = lux;
    sendJson("light", data.as<JsonVariant>());
}

void setup() {
       Serial.begin(115200);

       OTAUpdateSetup();
       initWifi();
       // Initialize the Wire
       Wire.begin();
       if (lightMeter.begin()) {
           Serial.println(F("BH1750 light sensor initialized"));
       } else {
           Serial.println(F("BH1750 Initialization failure!"));
       }

       analogSetAttenuation(ADC_11db);
       analogReadResolution(12);

       Serial.printf("Firmware %s started.\n Initializing tickers\n", FIRMWARE_VERSION);

       // Check wifi every 5 seconds
       wifiTicker.attach(5.0, testWifiConnection);

       // Check light once every light_frequency
       lightLevelTicker.attach_ms(light_frequency, lightLevelSensorAction);

       // Check the moisture sensors once every soil_moisture_frequency
       soilMoistureTicker1.attach_ms(soil_moisture_frequency, soilMoistureSensor1Action);
       soilMoistureTicker2.attach_ms(soil_moisture_frequency + 10, soilMoistureSensor2Action);
       soilMoistureTicker3.attach_ms(soil_moisture_frequency + 20, soilMoistureSensor3Action);
       soilMoistureTicker4.attach_ms(soil_moisture_frequency + 30, soilMoistureSensor4Action);
}

void loop() {
    ArduinoOTA.handle();
  // float temp = sht31.readTemperature();
  // float humidity = sht31.readHumidity();

  // Serial.print("Temp: "); Serial.print(temp); Serial.print(" C\t");
  // Serial.print("Humidity: "); Serial.print(humidity); Serial.print(" %\t");

  // delay(2000);
}
