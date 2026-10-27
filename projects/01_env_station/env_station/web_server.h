#ifndef WEB_SERVER_H
#define WEB_SERVER_H

// ============================================================================
// MODULE: web_server.h
// Single Responsibility: HTTP routes, JSON APIs (/api/sensors, /api/history)
// and CSV export. Visual template is cleanly isolated in index_html.h (SRP).
// ============================================================================
#include <WebServer.h>
#include "sensors.h"
#include "time_service.h"
#include "stats_logger.h"
#include "index_html.h"

WebServer server(80);

void handleApiSensors() {
  uint32_t n = stats.sampleCount;
  StorageInfo sInfo = getStorageInfo();
  char json[896];
  snprintf(json, sizeof(json),
    "{"
      "\"timestamp\":\"%s\","
      "\"short_time\":\"%s\","
      "\"uptime_s\":%lu,"
      "\"samples\":%lu,"
      "\"alert\":%s,"
      "\"storage\":{"
        "\"used_kb\":%.1f,"
        "\"total_kb\":%.1f,"
        "\"pct\":%.1f,"
        "\"days_left\":%u,"
        "\"warning\":%s"
      "},"
      "\"current\":{"
        "\"temp_aht\":%.1f,"
        "\"temp_bmp\":%.1f,"
        "\"humidity\":%.1f,"
        "\"pressure\":%.1f,"
        "\"air_ppm\":%d,"
        "\"air_status\":\"%s\","
        "\"mq135_v\":%.2f,"
        "\"mq135_raw\":%d"
      "},"
      "\"stats\":{"
        "\"temp\":{\"min\":%.1f,\"min_t\":\"%s\",\"avg\":%.1f,\"max\":%.1f,\"max_t\":\"%s\"},"
        "\"hum\":{\"min\":%.1f,\"min_t\":\"%s\",\"avg\":%.1f,\"max\":%.1f,\"max_t\":\"%s\"},"
        "\"pres\":{\"min\":%.1f,\"min_t\":\"%s\",\"avg\":%.1f,\"max\":%.1f,\"max_t\":\"%s\"},"
        "\"air\":{\"min\":%.0f,\"min_t\":\"%s\",\"avg\":%.0f,\"max\":%.0f,\"max_t\":\"%s\"}"
      "}"
    "}",
    clockState.dateTimeStr.c_str(),
    clockState.shortTimeStr.c_str(),
    (unsigned long)clockState.uptimeSec,
    (unsigned long)n,
    readings.alertActive ? "true" : "false",
    sInfo.fileBytes / 1024.0f,
    sInfo.maxBytes / 1024.0f,
    sInfo.usedPct,
    (unsigned int)sInfo.daysLeft,
    sInfo.warning ? "true" : "false",
    readings.tempAHT,
    readings.tempBMP,
    readings.humidity,
    readings.pressure,
    readings.airQualityPpm,
    readings.airStatus.c_str(),
    readings.mq135Voltage,
    readings.mq135Raw,
    stats.temp.minVal, stats.temp.minTime.c_str(), stats.temp.avg(n), stats.temp.maxVal, stats.temp.maxTime.c_str(),
    stats.humidity.minVal, stats.humidity.minTime.c_str(), stats.humidity.avg(n), stats.humidity.maxVal, stats.humidity.maxTime.c_str(),
    stats.pressure.minVal, stats.pressure.minTime.c_str(), stats.pressure.avg(n), stats.pressure.maxVal, stats.pressure.maxTime.c_str(),
    stats.airPpm.minVal, stats.airPpm.minTime.c_str(), stats.airPpm.avg(n), stats.airPpm.maxVal, stats.airPpm.maxTime.c_str()
  );

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleApiHistoryJson() {
  String json = buildHistoryJson();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void handleApiHistoryCsv() {
  if (littleFsOk && LittleFS.exists(HISTORY_FILE_PATH)) {
    File f = LittleFS.open(HISTORY_FILE_PATH, "r");
    if (f) {
      server.sendHeader("Content-Disposition", "attachment; filename=\"env_station_history.csv\"");
      server.streamFile(f, "text/csv");
      f.close();
      return;
    }
  }
  String csv = buildHistoryCsv();
  server.sendHeader("Content-Disposition", "attachment; filename=\"env_station_48h.csv\"");
  server.send(200, "text/csv", csv);
}

void handleApiClearHistory() {
  if (server.hasArg("confirm") && server.arg("confirm") == "true") {
    clearHistoryStorage();
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", "{\"status\":\"ok\",\"message\":\"History purged from Flash and RAM\"}");
  } else {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing confirm=true\"}");
  }
}

void handleDashboard() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void initWebServer() {
  server.on("/", handleDashboard);
  server.on("/api/sensors", handleApiSensors);
  server.on("/api/history", handleApiHistoryJson);
  server.on("/api/history.csv", handleApiHistoryCsv);
  server.on("/api/history/clear", handleApiClearHistory);
  server.begin();
}

#endif
