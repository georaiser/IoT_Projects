// ============================================================================
// IoT ENVIRONMENTAL STATION (ESP32 DevKit 30P / ESP32-C3 Super Mini)
// Sensors: AHT20 + BMP280 (I2C) + MQ-135 (ADC) + OLED SSD1306 (Large Carousel)
// ============================================================================
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include "config.h"
#include "sensors.h"
#include "oled_display.h"
#include "web_server.h"

unsigned long lastSensorReadMs    = 0;
unsigned long lastOledSlideMs     = 0;
unsigned long lastWifiReconnectMs = 0;

void setup() {
  Serial.begin(115200);
  delay(1500);

  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(100000);

  initSensors();
  initDisplay();

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  #if CONFIG_IDF_TARGET_ESP32C3
  // Fix for ESP32-C3 Super Mini ceramic antenna RF reflection / LDO peak current
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  #endif

  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 25) {
    delay(500);
    Serial.print(".");
    wifiAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\n--> IP: http://");
    Serial.println(WiFi.localIP());
    if (MDNS.begin("env-station")) {
      MDNS.addService("http", "tcp", 80);
      Serial.println("--> mDNS: http://env-station.local");
    }
  } else {
    Serial.println("\n--> Wi-Fi not connected yet (running OLED & sensors)");
  }

  initWebServer();
  readSensors();
  updateDisplay();
}

void loop() {
  server.handleClient();

  unsigned long now = millis();

  // Auto-reconnect Wi-Fi every 15s if router restarts
  if (now - lastWifiReconnectMs >= 15000) {
    lastWifiReconnectMs = now;
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi lost, attempting reconnect...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      #if CONFIG_IDF_TARGET_ESP32C3
      WiFi.setTxPower(WIFI_POWER_8_5dBm);
      #endif
    }
  }

  // 1. Read sensors every 2 seconds
  if (now - lastSensorReadMs >= SENSOR_READ_INTERVAL_MS) {
    lastSensorReadMs = now;
    readSensors();
    updateDisplay();

    Serial.printf("Temp: %.1f C | Hum: %.1f %% | Pres: %.1f hPa | Air: %d ppm [%s] (%.2fV / raw:%d)\n",
                  readings.tempAHT, readings.humidity, readings.pressure,
                  readings.airQualityPpm, readings.airStatus.c_str(),
                  readings.mq135Voltage, readings.mq135Raw);
  }

  // 2. Rotate large-text OLED carousel every 3 seconds
  if (now - lastOledSlideMs >= OLED_SLIDE_INTERVAL_MS) {
    lastOledSlideMs = now;
    nextDisplaySlide();
  }
}
