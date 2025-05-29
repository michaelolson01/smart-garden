#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_SHT31.h>

BH1750 lightMeter;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

#define SOIL_PIN 34  // Analog pin for soil sensor

void setup() {
  Serial.begin(115200);
  Wire.begin();

  lightMeter.begin();
  if (!sht31.begin(0x44)) {
    Serial.println("SHT31 not found!");
  }

  pinMode(SOIL_PIN, INPUT);
}

void loop() {
  float light = lightMeter.readLightLevel();
  float temp = sht31.readTemperature();
  float humidity = sht31.readHumidity();
  int soil = analogRead(SOIL_PIN);

  Serial.print("Light: "); Serial.print(light); Serial.println(" lx");
  Serial.print("Temp: "); Serial.print(temp); Serial.println(" °C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Soil Moisture: "); Serial.println(soil);

  delay(5000);
}
