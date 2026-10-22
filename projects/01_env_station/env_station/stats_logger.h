#ifndef STATS_LOGGER_H
#define STATS_LOGGER_H

// ============================================================================
// MODULE: stats_logger.h
// Single Responsibility: Compute running Min/Max/Avg statistics and maintain
// a 24-hour circular history buffer in RAM (288 records @ 5-min intervals).
// ============================================================================
#include <Arduino.h>
#include "sensors.h"

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

void initStatsLogger() {
  stats.sampleCount = 0;
  stats.temp.reset();
  stats.humidity.reset();
  stats.pressure.reset();
  stats.airPpm.reset();
}

void updateStats(const SensorReadings& r, const String& shortTime) {
  stats.sampleCount++;
  stats.temp.update(r.tempAHT, shortTime);
  stats.humidity.update(r.humidity, shortTime);
  stats.pressure.update(r.pressure, shortTime);
  stats.airPpm.update((float)r.airQualityPpm, shortTime);
}

void logHistorySample(const SensorReadings& r, const String& fullDateTime) {
  HistoryRecord& rec = historyBuffer[historyHead];
  strncpy(rec.timeLabel, fullDateTime.c_str(), sizeof(rec.timeLabel) - 1);
  rec.timeLabel[sizeof(rec.timeLabel) - 1] = '\0';
  rec.tempX10 = (int16_t)(r.tempAHT * 10.0f);
  rec.humX10  = (uint16_t)(r.humidity * 10.0f);
  rec.presX10 = (uint16_t)(r.pressure * 10.0f);
  rec.airPpm  = (uint16_t)r.airQualityPpm;

  historyHead = (historyHead + 1) % MAX_HISTORY_RECORDS;
  if (historyCount < MAX_HISTORY_RECORDS) {
    historyCount++;
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

#endif
