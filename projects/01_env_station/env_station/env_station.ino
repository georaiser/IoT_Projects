// ============================================================================
// IoT ENVIRONMENTAL STATION v1.2 (ESP32 DevKit 30P / ESP32-C3 Super Mini)
// Architecture: SOLID (Single Responsibility Principle) & KISS
// Features: NTP Clock, REST API, Smooth Web Dashboard, Dynamic Charts & OTA
// ============================================================================
#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include "config.h"
#include "time_service.h"
#include "sensors.h"
#include "alert_manager.h"
#include "stats_logger.h"
#include "oled_display.h"
#include "web_server.h"

unsigned long lastSensorReadMs    = 0;
unsigned long lastOledSlideMs     = 0;
unsigned long lastHistoryLogMs    = 0;
unsigned long lastWifiReconnectMs = 0;

String getStationIpString() {
  return (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : String("Reconnecting");
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  #if CONFIG_IDF_TARGET_ESP32C3
  // Prevent RF reflection saturation on ESP32-C3 Super Mini ceramic antenna
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  #endif

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 25) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\n--> IP: http://");
    Serial.println(WiFi.localIP());
    if (MDNS.begin("env-station")) {
      MDNS.addService("http", "tcp", 80);
      Serial.println("--> mDNS: http://env-station.local");
    }
  } else {
    Serial.println("\n--> Wi-Fi offline mode (OLED & sensors active)");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1500);

  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(100000);

  initSensors();
  initAlertManager();
  initStatsLogger();
  initDisplay();

  connectWiFi();
  initTimeService();
  initWebServer();

  // Initialize Wireless Over-The-Air (OTA) flashing
  ArduinoOTA.setHostname("env-station");
  ArduinoOTA.begin();
  Serial.println("--> ArduinoOTA: Ready for wireless Wi-Fi flashing");

  // Wait up to 2.5 seconds for NTP sync so the first history record has a valid timestamp
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Synchronizing NTP time");
    int ntpWait = 0;
    while (!clockState.synced && ntpWait < 25) {
      delay(100);
      updateTimeService();
      Serial.print(".");
      ntpWait++;
    }
    Serial.println(clockState.synced ? " OK!" : " Pending (will sync in background)");
  }

  // Initial acquisition and first history record
  updateTimeService();
  readSensors();
  evaluateAlerts(readings);
  updateStats(readings, clockState.shortTimeStr);
  logHistorySample(readings, clockState.dateTimeStr);
  updateDisplay(readings, clockState.shortTimeStr, getStationIpString());
}

void loop() {
  ArduinoOTA.handle(); // Listen for wireless firmware uploads
  server.handleClient();

  unsigned long now = millis();

  // 1. Auto-reconnect Wi-Fi every 15s if router restarts
  if (now - lastWifiReconnectMs >= WIFI_CHECK_INTERVAL_MS) {
    lastWifiReconnectMs = now;
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi lost, reconnecting...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      #if CONFIG_IDF_TARGET_ESP32C3
      WiFi.setTxPower(WIFI_POWER_8_5dBm);
      #endif
    }
  }

  // 2. Read sensors, evaluate alerts & update statistics every 2 seconds
  if (now - lastSensorReadMs >= SENSOR_READ_INTERVAL_MS) {
    lastSensorReadMs = now;
    updateTimeService();
    readSensors();
    evaluateAlerts(readings);
    updateStats(readings, clockState.shortTimeStr);
    updateDisplay(readings, clockState.shortTimeStr, getStationIpString());

    Serial.printf("[%s] Temp: %.1f C | Hum: %.1f %% | Pres: %.1f hPa | Air: %d ppm [%s] (%.2fV / raw:%d)\n",
                  clockState.dateTimeStr.c_str(),
                  readings.tempAHT, readings.humidity, readings.pressure,
                  readings.airQualityPpm, readings.airStatus.c_str(),
                  readings.mq135Voltage, readings.mq135Raw);
  }

  // 3. Rotate large-text OLED carousel every 3 seconds
  if (now - lastOledSlideMs >= OLED_SLIDE_INTERVAL_MS) {
    lastOledSlideMs = now;
    nextDisplaySlide(readings, clockState.shortTimeStr, getStationIpString());
  }

  // 4. Append sample to 24-hour circular buffer every 5 minutes
  if (now - lastHistoryLogMs >= HISTORY_LOG_INTERVAL_MS) {
    lastHistoryLogMs = now;
    logHistorySample(readings, clockState.dateTimeStr);
  }
}
