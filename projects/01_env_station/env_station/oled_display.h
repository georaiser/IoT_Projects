#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

// ============================================================================
// MODULE: oled_display.h
// Single Responsibility: Render telemetry, time, and IP on SSD1306 OLED.
// Decoupled from WiFi.h and NTP internals (receives data via parameters).
// ============================================================================
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "sensors.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 100000UL, 100000UL);

bool    hasOLED      = false;
uint8_t currentSlide = 0; // 0 = Temp/Hum, 1 = Pressure, 2 = Air Quality

void initDisplay() {
  if (isI2CDevicePresent(OLED_I2C_ADDR) &&
      display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR, true, false)) {
    hasOLED = true;
    Serial.println("OLED   (0x3C): OK");
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(4, 0);
    display.print("CONNECTING");
    display.setCursor(20, 28);
    display.print("Wi-Fi...");
    display.display();
  } else {
    Serial.println("OLED   (0x3C): NOT FOUND");
  }
}

void updateDisplay(const SensorReadings& r, const String& shortTime, const String& ipStr) {
  if (!hasOLED) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2); // Large 12x16 px characters

  // y = 0..15  fits inside the top YELLOW zone
  // y = 28..43 fits inside the bottom BLUE zone
  if (currentSlide == 0) {
    display.setCursor(4, 0);
    display.printf("T: %.1f C", r.tempAHT);
    display.setCursor(4, 28);
    display.printf("H: %.1f %%", r.humidity);
  } else if (currentSlide == 1) {
    display.setCursor(10, 0);
    display.print("PRESSURE");
    display.setCursor(4, 28);
    display.printf("%.1f hPa", r.pressure);
  } else {
    display.setCursor(4, 0);
    display.printf("AIR %s", r.airStatus.c_str());
    display.setCursor(4, 28);
    display.printf("%d ppm", r.airQualityPpm);
  }

  // Bottom status bar (y = 55..63): Live Clock (HH:MM) + IP Address
  display.setTextSize(1);
  display.setCursor(0, 55);
  display.printf("%s | %s", shortTime.c_str(), ipStr.c_str());

  display.display();
}

void nextDisplaySlide(const SensorReadings& r, const String& shortTime, const String& ipStr) {
  currentSlide = (currentSlide + 1) % 3;
  updateDisplay(r, shortTime, ipStr);
}

#endif
