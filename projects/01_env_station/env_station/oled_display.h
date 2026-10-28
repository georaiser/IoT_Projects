#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

// ============================================================================
// MODULE: oled_display.h
// Single Responsibility: Render telemetry, dynamic Min-Max graphs (Temp & Hum),
// and status bar on SSD1306 OLED.
// ============================================================================
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"
#include "sensors.h"
#include "stats_logger.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 100000UL, 100000UL);

bool    hasOLED      = false;
uint8_t currentSlide = 0; // 0=T/H, 1=Press, 2=Air, 3=Temp Graph, 4=Hum Graph

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

// Render dynamic graph with dynamic Min-Max scaling for OLED
void renderOledGraph(const char* label, float minV, float maxV, const char* unit, bool isTemp) {
  if (historyCount < 2) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.printf("%s 72H", label);
    display.setCursor(15, 28);
    display.print("Collecting data...");
    return;
  }

  uint16_t startIdx = (historyCount < MAX_HISTORY_RECORDS) ? 0 : historyHead;

  // Auto-scan the actual buffer to determine the true Min and Max of the plotted data
  float actualMin = 9999.0f;
  float actualMax = -9999.0f;
  for (uint16_t i = 0; i < historyCount; i++) {
    uint16_t idx = (startIdx + i) % MAX_HISTORY_RECORDS;
    float val = isTemp ? (historyBuffer[idx].tempX10 / 10.0f) : (historyBuffer[idx].humX10 / 10.0f);
    if (val < actualMin) actualMin = val;
    if (val > actualMax) actualMax = val;
  }

  float span = actualMax - actualMin;
  if (span < 0.5f) span = 1.0f; // Prevent division by zero and provide minimum visible span

  // 1. Top header (y = 0..14) with true dynamic Min and Max
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("%s 72H", label);

  display.setCursor(68, 0);
  display.printf("MX:%.1f", actualMax);

  display.setCursor(68, 8);
  display.printf("MN:%.1f", actualMin);

  // 2. Graph plot region: x = 0..127, y = 18..51 (height = 33 px)
  display.drawLine(0, 16, 127, 16, SSD1306_WHITE); // Upper separator line
  display.drawLine(0, 52, 127, 52, SSD1306_WHITE); // Lower separator line

  // Draw vertical dotted day separators ONLY for the first measurement of each new calendar day (matches Web)
  int lastSeenDateKey = -1;
  for (uint16_t i = 0; i < historyCount; i++) {
    uint16_t idx = (startIdx + i) % MAX_HISTORY_RECORDS;
    const HistoryRecord& rec = historyBuffer[idx];
    if (strlen(rec.timeLabel) >= 10 && rec.timeLabel[4] == '-' && rec.timeLabel[7] == '-' &&
        isdigit(rec.timeLabel[8]) && isdigit(rec.timeLabel[9])) {
      int dateKey = ((rec.timeLabel[5] - '0') * 10 + (rec.timeLabel[6] - '0')) * 100 +
                    (rec.timeLabel[8] - '0') * 10 + (rec.timeLabel[9] - '0');
      if (lastSeenDateKey == -1) {
        lastSeenDateKey = dateKey; // First recorded day (point 0), no divider line needed
      } else if (dateKey != lastSeenDateKey) {
        lastSeenDateKey = dateKey; // New day boundary!
        int x = (int)((i * 127.0f) / (historyCount - 1));
        // Alternating pixel dotted line from y=18 to y=50
        for (int dy = 18; dy <= 50; dy += 2) {
          display.drawPixel(x, dy, SSD1306_WHITE);
        }
      }
    }
  }

  int prevX = -1;
  int prevY = -1;

  for (uint16_t i = 0; i < historyCount; i++) {
    uint16_t idx = (startIdx + i) % MAX_HISTORY_RECORDS;
    float val = isTemp ? (historyBuffer[idx].tempX10 / 10.0f) : (historyBuffer[idx].humX10 / 10.0f);

    int x = (int)((i * 127.0f) / (historyCount - 1));
    float norm = (val - actualMin) / span;
    if (norm < 0.0f) norm = 0.0f;
    if (norm > 1.0f) norm = 1.0f;
    int y = 50 - (int)(norm * 32.0f); // 50 is bottom, 18 is top

    if (prevX >= 0) {
      display.drawLine(prevX, prevY, x, y, SSD1306_WHITE);
    } else {
      display.drawPixel(x, y, SSD1306_WHITE);
    }
    prevX = x;
    prevY = y;
  }
}

void updateDisplay(const SensorReadings& r, const String& shortTime, const String& ipStr) {
  if (!hasOLED) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  if (currentSlide == 0) {
    display.setTextSize(2);
    display.setCursor(4, 0);
    display.printf("T: %.1f C", r.tempAHT);
    display.setCursor(4, 28);
    display.printf("H: %.1f %%", r.humidity);
  } else if (currentSlide == 1) {
    display.setTextSize(2);
    display.setCursor(10, 0);
    display.print("PRESSURE");
    display.setCursor(4, 28);
    display.printf("%.1f hPa", r.pressure);
  } else if (currentSlide == 2) {
    display.setTextSize(2);
    display.setCursor(4, 0);
    display.printf("AIR %s", r.airStatus.c_str());
    display.setCursor(4, 28);
    display.printf("%d ppm", r.airQualityPpm);
  } else if (currentSlide == 3) {
    // Dynamic Temperature Graph scaled between 24h min and max
    renderOledGraph("TEMP", stats.temp.minVal, stats.temp.maxVal, "C", true);
  } else {
    // Dynamic Humidity Graph scaled between 24h min and max
    renderOledGraph("HUM", stats.humidity.minVal, stats.humidity.maxVal, "%", false);
  }

  // Bottom status bar (y = 55..63): Live Clock (HH:MM) + IP Address or Storage Warning
  StorageInfo sInfo = getStorageInfo();
  display.setTextSize(1);
  display.setCursor(0, 55);
  if (sInfo.warning) {
    display.printf("%s | MEM LOW!", shortTime.c_str());
  } else {
    display.printf("%s | %s", shortTime.c_str(), ipStr.c_str());
  }

  display.display();
}

void nextDisplaySlide(const SensorReadings& r, const String& shortTime, const String& ipStr) {
  currentSlide = (currentSlide + 1) % 5;
  updateDisplay(r, shortTime, ipStr);
}

#endif
