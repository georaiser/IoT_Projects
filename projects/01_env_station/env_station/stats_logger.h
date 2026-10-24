#ifndef STATS_LOGGER_H
#define STATS_LOGGER_H

// ============================================================================
// MODULE: stats_logger.h
// Single Responsibility: Compute running Min/Max/Avg statistics, maintain
// a 24-hour circular buffer in RAM for fast charts, and persist continuous
// multi-day telemetry in non-volatile LittleFS Flash memory across reboots.
// ============================================================================
#include <Arduino.h>
#include <LittleFS.h>
#include "sensors.h"

const char* HISTORY_FILE_PATH = "/history.csv";
#define MAX_FLASH_CSV_BYTES (512 * 1024) // 512 KB (~11,500 samples @ 5 min = ~40 days)

struct MetricStats {
  float  minVal;
  float  maxVal;
  double sumVal;
  String minTime;
  String maxTime;

  void reset() {
    minVal  = 99999.0f;
    maxVal  = -99999.0f;
    sumVal  = 0.0;
    minTime = "--:--";
    maxTime = "--:--";
  }

  void update(float val, const String& timeStr) {
    if (val < minVal) { minVal = val; minTime = timeStr; }
    if (val > maxVal) { maxVal = val; maxTime = timeStr; }
    sumVal += val;
  }

  float avg(uint32_t count) const {
    return (count > 0) ? (float)(sumVal / count) : 0.0f;
  }
};

struct StationStats {
  uint32_t    sampleCount;
  MetricStats temp;
  MetricStats humidity;
  MetricStats pressure;
  MetricStats airPpm;
};

// Compact 24-hour circular buffer entry (16 bytes per record * 288 = 4.6 KB RAM)
#define MAX_HISTORY_RECORDS 288

struct HistoryRecord {
  char     timeLabel[20]; // "YYYY-MM-DD HH:MM:SS"
  int16_t  tempX10;       // °C * 10
  uint16_t humX10;        // % * 10
  uint16_t presX10;       // hPa * 10
  uint16_t airPpm;        // ppm
};

StationStats  stats;
HistoryRecord historyBuffer[MAX_HISTORY_RECORDS];
uint16_t      historyHead  = 0;
uint16_t      historyCount = 0;
bool          littleFsOk   = false;

struct StorageInfo {
  uint32_t fileBytes;
  uint32_t maxBytes;
  float    usedPct;
  uint16_t daysLeft;
  bool     warning;
};

inline StorageInfo getStorageInfo() {
  StorageInfo info;
  info.maxBytes  = MAX_FLASH_CSV_BYTES;
  info.fileBytes = 0;
  if (littleFsOk && LittleFS.exists(HISTORY_FILE_PATH)) {
    File f = LittleFS.open(HISTORY_FILE_PATH, "r");
    if (f) {
      info.fileBytes = f.size();
      f.close();
    }
  }
  info.usedPct = (info.maxBytes > 0) ? ((float)info.fileBytes / (float)info.maxBytes * 100.0f) : 0.0f;
  // ~13,000 bytes per day (288 samples @ ~45 bytes)
  uint32_t bytesLeft = (info.fileBytes < info.maxBytes) ? (info.maxBytes - info.fileBytes) : 0;
  info.daysLeft = (uint16_t)(bytesLeft / 13000);
  info.warning  = (info.daysLeft <= 2 || info.usedPct >= 95.0f);
  return info;
}

inline bool clearHistoryStorage() {
  if (littleFsOk && LittleFS.exists(HISTORY_FILE_PATH)) {
    LittleFS.remove(HISTORY_FILE_PATH);
  }
  historyHead  = 0;
  historyCount = 0;
  stats.sampleCount = 0;
  stats.temp.reset();
  stats.humidity.reset();
  stats.pressure.reset();
  stats.airPpm.reset();
  return true;
}

void appendSampleToRamBuffer(const String& fullDateTime, float t, float h, float p, uint16_t air) {
  HistoryRecord& rec = historyBuffer[historyHead];
  strncpy(rec.timeLabel, fullDateTime.c_str(), sizeof(rec.timeLabel) - 1);
  rec.timeLabel[sizeof(rec.timeLabel) - 1] = '\0';
  rec.tempX10 = (int16_t)(t * 10.0f);
  rec.humX10  = (uint16_t)(h * 10.0f);
  rec.presX10 = (uint16_t)(p * 10.0f);
  rec.airPpm  = air;

  historyHead = (historyHead + 1) % MAX_HISTORY_RECORDS;
  if (historyCount < MAX_HISTORY_RECORDS) {
    historyCount++;
  }
}

// Restore previous 24h history from LittleFS Flash into RAM buffer on startup
void restoreHistoryFromFlash() {
  if (!LittleFS.exists(HISTORY_FILE_PATH)) {
    Serial.println("LittleFS: No previous history file found (starting fresh)");
    return;
  }

  File f = LittleFS.open(HISTORY_FILE_PATH, "r");
  if (!f) return;

  Serial.println("LittleFS: Restoring history buffer from Flash memory...");
  String header = f.readStringUntil('\n'); // skip CSV header

  // We read lines and keep the last 288 in RAM
  uint32_t lineCount = 0;
  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() < 18) continue;

    // Parse CSV line: timestamp,temp,hum,pres,air
    int c1 = line.indexOf(',');
    int c2 = (c1 > 0) ? line.indexOf(',', c1 + 1) : -1;
    int c3 = (c2 > 0) ? line.indexOf(',', c2 + 1) : -1;
    int c4 = (c3 > 0) ? line.indexOf(',', c3 + 1) : -1;

    if (c1 > 0 && c2 > 0 && c3 > 0 && c4 > 0) {
      String tStr = line.substring(0, c1);
      float  tVal = line.substring(c1 + 1, c2).toFloat();
      float  hVal = line.substring(c2 + 1, c3).toFloat();
      float  pVal = line.substring(c3 + 1, c4).toFloat();
      uint16_t aVal = (uint16_t)line.substring(c4 + 1).toInt();

      appendSampleToRamBuffer(tStr, tVal, hVal, pVal, aVal);
      lineCount++;
    }
  }
  f.close();
  Serial.printf("LittleFS: Successfully restored %u points into RAM buffer\n", historyCount);
}

void initStatsLogger() {
  stats.sampleCount = 0;
  stats.temp.reset();
  stats.humidity.reset();
  stats.pressure.reset();
  stats.airPpm.reset();

  if (LittleFS.begin(true)) {
    littleFsOk = true;
    Serial.println("LittleFS: Flash filesystem mounted OK");
    restoreHistoryFromFlash();
  } else {
    littleFsOk = false;
    Serial.println("LittleFS: Mount failed (operating in RAM-only mode)");
  }
}

void updateStats(const SensorReadings& r, const String& shortTime) {
  stats.sampleCount++;
  stats.temp.update(r.tempAHT, shortTime);
  stats.humidity.update(r.humidity, shortTime);
  stats.pressure.update(r.pressure, shortTime);
  stats.airPpm.update((float)r.airQualityPpm, shortTime);
}

void logHistorySample(const SensorReadings& r, const String& fullDateTime) {
  // 1. Add to RAM circular buffer for fast web charts
  appendSampleToRamBuffer(fullDateTime, r.tempAHT, r.humidity, r.pressure, (uint16_t)r.airQualityPpm);

  // 2. Persist to LittleFS Flash memory (survives power off & reboots)
  if (littleFsOk) {
    bool fileExists = LittleFS.exists(HISTORY_FILE_PATH);
    File f = LittleFS.open(HISTORY_FILE_PATH, "a");
    if (f) {
      if (!fileExists || f.size() == 0) {
        f.println("timestamp,temp_c,humidity_pct,pressure_hpa,air_ppm");
      }
      f.printf("%s,%.1f,%.1f,%.1f,%u\n",
               fullDateTime.c_str(),
               r.tempAHT,
               r.humidity,
               r.pressure,
               (uint16_t)r.airQualityPpm);
      f.close();
    }
  }
}

String buildHistoryCsv() {
  String csv = "timestamp,temp_c,humidity_pct,pressure_hpa,air_ppm\n";
  csv.reserve(historyCount * 42 + 64);

  uint16_t startIdx = (historyCount < MAX_HISTORY_RECORDS) ? 0 : historyHead;
  for (uint16_t i = 0; i < historyCount; i++) {
    uint16_t idx = (startIdx + i) % MAX_HISTORY_RECORDS;
    const HistoryRecord& rec = historyBuffer[idx];
    char line[64];
    snprintf(line, sizeof(line), "%s,%.1f,%.1f,%.1f,%u\n",
             rec.timeLabel,
             rec.tempX10 / 10.0f,
             rec.humX10 / 10.0f,
             rec.presX10 / 10.0f,
             rec.airPpm);
    csv += line;
  }
  return csv;
}

// Build lightweight JSON array of history points for live web charts
String buildHistoryJson() {
  String json = "[";
  json.reserve(historyCount * 36 + 16);

  uint16_t startIdx = (historyCount < MAX_HISTORY_RECORDS) ? 0 : historyHead;
  for (uint16_t i = 0; i < historyCount; i++) {
    uint16_t idx = (startIdx + i) % MAX_HISTORY_RECORDS;
    const HistoryRecord& rec = historyBuffer[idx];

    // Extract HH:MM from timeLabel (e.g. "2026-10-22 20:17:00" -> "20:17")
    char tShort[6] = "--:--";
    if (strlen(rec.timeLabel) >= 16) {
      tShort[0] = rec.timeLabel[11];
      tShort[1] = rec.timeLabel[12];
      tShort[2] = ':';
      tShort[3] = rec.timeLabel[14];
      tShort[4] = rec.timeLabel[15];
      tShort[5] = '\0';
    }

    char item[48];
    snprintf(item, sizeof(item), "%s[\"%s\",%.1f,%.1f,%.1f,%u]",
             (i > 0) ? "," : "",
             tShort,
             rec.tempX10 / 10.0f,
             rec.humX10 / 10.0f,
             rec.presX10 / 10.0f,
             rec.airPpm);
    json += item;
  }
  json += "]";
  return json;
}

#endif
