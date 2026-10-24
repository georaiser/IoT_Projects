#ifndef SENSORS_H
#define SENSORS_H

// ============================================================================
// MODULE: sensors.h
// Single Responsibility: Hardware acquisition for AHT20, BMP280, and MQ-135.
// ============================================================================
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include "config.h"

Adafruit_AHTX0  aht20;
Adafruit_BMP280 bmp280(&Wire);

struct SensorReadings {
  bool   hasAHT;
  bool   hasBMP;
  float  tempAHT;        // °C
  float  humidity;       // %
  float  tempBMP;        // °C
  float  pressure;       // hPa
  int    mq135Raw;       // Raw ADC (0..4095)
  float  mq135Voltage;   // Real voltage at MQ-135 A0 pin (0.0..5.0 V)
  int    airQualityPpm;  // Estimated CO2/gas index in ppm (400 = clean air)
  float  airQualityPct;  // Pollution index (0..100 %)
  String airStatus;      // "GOOD", "FAIR", "POOR!"
  bool   alertActive;    // Populated by alert_manager.h
};

SensorReadings readings = {false, false, 0, 0, 0, 0, 0, 0, 400, 0, "GOOD", false};

bool isI2CDevicePresent(uint8_t address) {
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}

void initSensors() {
  pinMode(PIN_MQ135, INPUT);
  analogSetPinAttenuation(PIN_MQ135, ADC_2_5db);

  Serial.println("\n=== I2C BUS SCAN ===");
  for (byte addr = 1; addr < 127; addr++) {
    if (isI2CDevicePresent(addr)) {
      Serial.printf(" - Device found at: 0x%02X\n", addr);
    }
  }

  if (isI2CDevicePresent(0x38) && aht20.begin()) {
    readings.hasAHT = true;
    Serial.println("AHT20  (0x38): OK");
  } else {
    Serial.println("AHT20  (0x38): NOT FOUND");
  }

  if ((isI2CDevicePresent(0x77) && bmp280.begin(0x77)) ||
      (isI2CDevicePresent(0x76) && bmp280.begin(0x76))) {
    readings.hasBMP = true;
    bmp280.setSampling(Adafruit_BMP280::MODE_FORCED,
                       Adafruit_BMP280::SAMPLING_X1,
                       Adafruit_BMP280::SAMPLING_X1,
                       Adafruit_BMP280::FILTER_OFF);
    Serial.println("BMP280 (0x77): OK");
  } else {
    Serial.println("BMP280       : NOT FOUND");
  }

  Serial.printf("MQ-135 (ADC) : Ready on GPIO %d (10k/10k divider)\n", PIN_MQ135);
}

void readSensors() {
  if (readings.hasAHT) {
    sensors_event_t humEvent, tempEvent;
    aht20.getEvent(&humEvent, &tempEvent);
    readings.tempAHT  = tempEvent.temperature;
    readings.humidity = humEvent.relative_humidity;
  }

  if (readings.hasBMP) {
    bmp280.takeForcedMeasurement();
    readings.tempBMP  = bmp280.readTemperature();
    readings.pressure = bmp280.readPressure() / 100.0f;
  }

  uint32_t sumRaw = 0;
  uint32_t sumMv  = 0;
  for (int i = 0; i < 8; i++) {
    sumRaw += analogRead(PIN_MQ135);
    sumMv  += analogReadMilliVolts(PIN_MQ135);
  }
  readings.mq135Raw     = sumRaw / 8;
  readings.mq135Voltage = ((sumMv / 8.0f) * 2.0f) / 1000.0f;

  float pct = (readings.mq135Voltage - 0.22f) / (1.50f - 0.22f) * 100.0f;
  if (pct < 0.0f)   pct = 0.0f;
  if (pct > 100.0f) pct = 100.0f;
  readings.airQualityPct = pct;
  readings.airQualityPpm = 400 + (int)(pct * 16.0f);

  if (readings.airQualityPct < 25.0f) {
    readings.airStatus = "GOOD";
  } else if (readings.airQualityPct < AIR_POOR_ALERT_PCT) {
    readings.airStatus = "FAIR";
  } else {
    readings.airStatus = "POOR!";
  }
}

#endif
