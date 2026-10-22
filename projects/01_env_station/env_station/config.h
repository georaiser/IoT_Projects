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
  #define PIN_SDA     4   // GPIO4 (I2C SDA)
  #define PIN_SCL     5   // GPIO5 (I2C SCL)
  #define PIN_MQ135   0   // GPIO0 (ADC1_CH0 — avoids GPIO2 strapping pin)
  #define PIN_BUZZER  3   // GPIO3 (Passive buzzer PWM output)
#else
  #define PIN_SDA     21  // D21 (I2C SDA)
  #define PIN_SCL     22  // D22 (I2C SCL)
  #define PIN_MQ135   34  // D34 (ADC1_CH6, compatible with active Wi-Fi)
  #define PIN_BUZZER  25  // D25 (Passive buzzer PWM output)
#endif

// ============================================================================
// 3. OLED SSD1306 DISPLAY (128x64)
// ============================================================================
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT   64
#define OLED_RESET      -1
#define OLED_I2C_ADDR  0x3C

// ============================================================================
// 4. NTP CLOCK CONFIGURATION (Chile Continental: CLT4CLST / UTC-3)
// ============================================================================
#define NTP_SERVER_1   "ntp.shoa.cl"
#define NTP_SERVER_2   "pool.ntp.org"
#define TZ_INFO        "CLT4CLST,M9.1.6/24,M4.1.6/24"

// ============================================================================
// 5. ALERT THRESHOLDS & TIMING INTERVALS
// ============================================================================
#define TEMP_HIGH_ALERT_C         30.0f   // Alert if temperature >= 30.0 °C
#define TEMP_LOW_ALERT_C          10.0f   // Alert if temperature <= 10.0 °C
#define AIR_POOR_ALERT_PCT        50.0f   // Alert if pollution index >= 50% (1200 ppm)

#define SENSOR_READ_INTERVAL_MS   2000UL   // Read sensors every 2 seconds
#define OLED_SLIDE_INTERVAL_MS    3000UL   // Rotate OLED carousel every 3 seconds
#define HISTORY_LOG_INTERVAL_MS   300000UL // Log 1 history point every 5 min (288 = 24h)
#define WIFI_CHECK_INTERVAL_MS    15000UL  // Check Wi-Fi connection every 15 seconds

#endif
