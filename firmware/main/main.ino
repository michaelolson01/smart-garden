#include <Arduino.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_SHT31.h>
#include <Ticker.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "secrets.h"

BH1750 lightMeter;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Ticker soilMoistureTicker;
Ticker lightLevelTicker;
Ticker wifiTicker;

WiFiUDP udp;

const int soilPin = 32;

bool already_connected = false;

void initWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void testWifiConnection() {
    int status = WiFi.status();
    if (status == WL_CONNECTED) {
        if (!already_connected) {
            Serial.print("Connected to ");
            Serial.println(WiFi.localIP());
            udp.begin(UDP_PORT);
            udp.beginPacket(UDP_HOST, UDP_PORT);
            udp.printf("Smart Garden Connected!");
            udp.endPacket();
            ArduinoOTA.begin();
            Serial.println("OTA Started");
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
        udp.beginPacket(UDP_HOST, UDP_PORT);
        udp.printf("OTA: Start updating %s\n", type);
        udp.endPacket();
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nOTA: Update complete.");
        udp.beginPacket(UDP_HOST, UDP_PORT);
        udp.printf("OTA: Update complete\n");
        udp.endPacket();
    });

	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("OTA Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");

		udp.beginPacket(UDP_HOST, UDP_PORT);
		udp.printf("OTA Error[%u]\n", error);
		udp.endPacket();
	});

	ArduinoOTA.setPassword(OTA_PASSWORD);

}

void soilMoistureSensorAction() {
    int soilMoistureValue = analogRead(soilPin);
    udp.beginPacket(UDP_HOST, UDP_PORT);
    udp.printf("Soil Moisture: %d\n", soilMoistureValue);
    udp.endPacket();
    Serial.print("Soil Moisture: "); Serial.print(soilMoistureValue); Serial.println(" units");
}

void lightLevelSensorAction() {
    uint16_t lux = lightMeter.readLightLevel();
    udp.beginPacket(UDP_HOST, UDP_PORT);
    udp.printf("Current Light: %d lx\n", lux);
    udp.endPacket();
    Serial.print("Light: "); Serial.print(lux); Serial.println(" lx");
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

    analogReadResolution(12);

    soilMoistureTicker.attach(600.0, soilMoistureSensorAction);
    lightLevelTicker.attach(60.0, lightLevelSensorAction);
    wifiTicker.attach(5.0, testWifiConnection);
}

void loop() {
    ArduinoOTA.handle();
  // float temp = sht31.readTemperature();
  // float humidity = sht31.readHumidity();

  // Serial.print("Temp: "); Serial.print(temp); Serial.print(" C\t");
  // Serial.print("Humidity: "); Serial.print(humidity); Serial.print(" %\t");

  // delay(2000);
}
