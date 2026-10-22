#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include "config.h"

Adafruit_AHTX0 aht20;
Adafruit_BMP280 bmp280(&Wire);

bool hasAHT20  = false;
bool hasBMP280 = false;

struct SensorReadings {
  float tempAHT;        // °C
  float humidity;       // %
  float tempBMP;        // °C
  float pressure;       // hPa
  int   mq135Raw;       // Raw ADC value (0 to 4095)
  float mq135Voltage;   // Real voltage at MQ-135 A0 pin (0.0 to 5.0 V)
  int   airQualityPpm;  // Estimated CO2/gas index in ppm (400 = clean air)
  float airQualityPct;  // Gas/pollution index (0 to 100 %)
  String airStatus;     // "GOOD", "FAIR", "POOR!"
  bool alertActive;     // true if extreme temperature or poor air quality
};

SensorReadings readings = {0, 0, 0, 0, 0, 0, 400, 0, "GOOD", false};

bool isI2CDevicePresent(uint8_t address) {
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}

void triggerAlertBeep() {
  tone(PIN_BUZZER, 2500, 120);
  delay(160);
  tone(PIN_BUZZER, 2500, 120);
  delay(140);
  noTone(PIN_BUZZER);
}

void initSensors() {
  pinMode(PIN_MQ135, INPUT);
  analogSetPinAttenuation(PIN_MQ135, ADC_2_5db); // High sensitivity for low voltages (0.05V - 2.2V)
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  // Short startup beep to confirm buzzer connection
  tone(PIN_BUZZER, 2000, 80);
  delay(100);
  noTone(PIN_BUZZER);

  Serial.println("\n=== I2C BUS SCAN ===");
  for (byte addr = 1; addr < 127; addr++) {
    if (isI2CDevicePresent(addr)) {
      Serial.printf(" - Device found at: 0x%02X\n", addr);
    }
  }

  // 1. AHT20 (0x38)
  if (isI2CDevicePresent(0x38) && aht20.begin()) {
    hasAHT20 = true;
    Serial.println("AHT20  (0x38): OK");
  } else {
    Serial.println("AHT20  (0x38): NOT FOUND");
  }

  // 2. BMP280 (0x77 or 0x76)
  if ((isI2CDevicePresent(0x77) && bmp280.begin(0x77)) ||
      (isI2CDevicePresent(0x76) && bmp280.begin(0x76))) {
    hasBMP280 = true;
    bmp280.setSampling(Adafruit_BMP280::MODE_FORCED,
                       Adafruit_BMP280::SAMPLING_X1,
                       Adafruit_BMP280::SAMPLING_X1,
                       Adafruit_BMP280::FILTER_OFF);
    Serial.println("BMP280 (0x77): OK");
  } else {
    Serial.println("BMP280       : NOT FOUND");
  }

  Serial.printf("MQ-135 (ADC) : Ready on GPIO %d (10k/10k divider)\n", PIN_MQ135);
  Serial.printf("Buzzer       : Ready on GPIO %d\n", PIN_BUZZER);
}

void readSensors() {
  // 1. Read AHT20
  if (hasAHT20) {
    sensors_event_t humEvent, tempEvent;
    aht20.getEvent(&humEvent, &tempEvent);
    readings.tempAHT  = tempEvent.temperature;
    readings.humidity = humEvent.relative_humidity;
  }

  // 2. Read BMP280
  if (hasBMP280) {
    bmp280.takeForcedMeasurement();
    readings.tempBMP  = bmp280.readTemperature();
    readings.pressure = bmp280.readPressure() / 100.0f;
  }

  // 3. Read MQ-135 (average 8 ADC samples for stability)
  uint32_t sumRaw = 0;
  uint32_t sumMv  = 0;
  for (int i = 0; i < 8; i++) {
    sumRaw += analogRead(PIN_MQ135);
    sumMv  += analogReadMilliVolts(PIN_MQ135);
  }
  readings.mq135Raw = sumRaw / 8;
  float pinMv = sumMv / 8.0f;

  // Multiply by 2.0 to compensate for the 10k/10k voltage divider
  readings.mq135Voltage = (pinMv * 2.0f) / 1000.0f;

  // Calibrated to real hardware baseline (~0.28V in clean air, >0.90V with gas)
  float pct = (readings.mq135Voltage - 0.27f) / (1.50f - 0.27f) * 100.0f;
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

  // 4. Check alert conditions
  bool tempAlert = hasAHT20 && (readings.tempAHT >= TEMP_HIGH_ALERT_C || readings.tempAHT <= TEMP_LOW_ALERT_C);
  bool airAlert  = (readings.airQualityPct >= AIR_POOR_ALERT_PCT);
  readings.alertActive = (tempAlert || airAlert);

  if (readings.alertActive) {
    triggerAlertBeep();
  }
}

#endif
