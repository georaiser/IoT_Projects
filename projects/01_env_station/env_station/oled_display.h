#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "sensors.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 100000UL, 100000UL);

bool hasOLED         = false;
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

void updateDisplay() {
  if (!hasOLED) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2); // Large 12x16 px characters

  // y = 0..15  fits inside the top YELLOW zone
  // y = 28..43 fits inside the bottom BLUE zone
  if (currentSlide == 0) {
    // SLIDE 0: Temperature + Humidity
    display.setCursor(4, 0);
    display.printf("T: %.1f C", readings.tempAHT);

    display.setCursor(4, 28);
    display.printf("H: %.1f %%", readings.humidity);

  } else if (currentSlide == 1) {
    // SLIDE 1: Atmospheric Pressure
    display.setCursor(10, 0);
    display.print("PRESSURE");

    display.setCursor(4, 28);
    display.printf("%.1f hPa", readings.pressure);

  } else {
    // SLIDE 2: Air Quality (MQ-135) with numerical ppm + status
    display.setCursor(4, 0);
    display.printf("AIR %s", readings.airStatus.c_str());

    display.setCursor(4, 28);
    display.printf("%d ppm", readings.airQualityPpm);
  }

  // Bottom status bar (y = 55..63): Live Wi-Fi IP address
  display.setTextSize(1);
  display.setCursor(4, 55);
  if (WiFi.status() == WL_CONNECTED) {
    display.printf("IP: %s", WiFi.localIP().toString().c_str());
  } else {
    display.print("Wi-Fi: Reconnecting");
  }

  display.display();
}

void nextDisplaySlide() {
  currentSlide = (currentSlide + 1) % 3;
  updateDisplay();
}

#endif
