#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// 1. WI-FI & PRIVATE CREDENTIALS (Loaded from Git-ignored "secrets.h")
// ============================================================================
#if __has_include("secrets.h")
  #include "secrets.h"
  const char* WIFI_SSID = SECRET_WIFI_SSID;
  const char* WIFI_PASS = SECRET_WIFI_PASS;
#else
  const char* WIFI_SSID = "YOUR_WIFI_SSID";
  const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
#endif

// ============================================================================
// 2. AUTOMATIC PIN MAPPING (ESP32 DevKit 30P vs ESP32-C3 Super Mini)
// ============================================================================
#if CONFIG_IDF_TARGET_ESP32C3
  // ESP32-C3 Super Mini pins
  #define PIN_SDA     4   // Col 4 (5V row)
  #define PIN_SCL     5   // Col 8 (opposite row, antenna corner)
  #define PIN_MQ135   0   // GPIO0 (ADC1_CH0, Col 8 on 5V row — avoids GPIO2 strapping pin)
  #define PIN_BUZZER  3   // GPIO3 (Col 5 on 5V row)
#else
  // ESP32 DevKit 30P pins
  #define PIN_SDA     21
  #define PIN_SCL     22
  #define PIN_MQ135   34  // D34 (ADC1, works while Wi-Fi is active)
  #define PIN_BUZZER  25  // D25 for passive buzzer
#endif

// ============================================================================
// 3. OLED SSD1306 DISPLAY (128x64)
// ============================================================================
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT   64
#define OLED_RESET      -1
#define OLED_I2C_ADDR  0x3C

// ============================================================================
// 4. ALERT THRESHOLDS & TIMING INTERVALS
// ============================================================================
#define TEMP_HIGH_ALERT_C     30.0f  // Beep if temperature >= 30.0 °C
#define TEMP_LOW_ALERT_C      10.0f  // Beep if temperature <= 10.0 °C
#define AIR_POOR_ALERT_PCT    50.0f  // Beep if gas/pollution index >= 50%

#define SENSOR_READ_INTERVAL_MS   2000  // Read sensors every 2 seconds
#define OLED_SLIDE_INTERVAL_MS    3000  // Rotate OLED carousel every 3 seconds

#endif
