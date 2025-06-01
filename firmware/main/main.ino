#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_SHT31.h>
#include <Ticker.h>
#include <WiFi.h>
#include "secrets.h"

BH1750 lightMeter;
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Ticker soilMoistureTicker;
Ticker lightLevelTicker;
Ticker wifiTicker;

const int soilPin = 32;

bool already_connected = false;

void initWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void testWifiConnection() {
    int status = WiFi.status();
    if ((status == WL_CONNECTED) && (!already_connected)) {
        Serial.print("Connected to ");
        Serial.println(WiFi.localIP());
        already_connected = true;
    } else if (status == WL_CONNECT_FAILED) {
        already_connected = false;
        Serial.println("Unable to connect to WiFi.");
    } else if (status == WL_DISCONNECTED) {
        already_connected = false;
        Serial.println("Disconnected from WiFi.");
    } else if (status == WL_CONNECTION_LOST) {
        already_connected = false;
        Serial.println("Connection to WiFi Lost.");
    } else if (status != WL_CONNECTED){
        Serial.println("Still Connecting...");
    }
}

void soilMoistureSensorAction() {
    int soilMoistureValue = analogRead(soilPin);
    Serial.print("Soil Moisture: "); Serial.print(soilMoistureValue); Serial.println(" units");
}

void lightLevelSensorAction() {
    uint16_t lux = lightMeter.readLightLevel();
    Serial.print("Light: "); Serial.print(lux); Serial.println(" lx");
}

void setup() {
    initWifi();
    Serial.begin(9600);
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

  // float temp = sht31.readTemperature();
  // float humidity = sht31.readHumidity();

  // Serial.print("Temp: "); Serial.print(temp); Serial.print(" C\t");
  // Serial.print("Humidity: "); Serial.print(humidity); Serial.print(" %\t");

  // delay(2000);
}
