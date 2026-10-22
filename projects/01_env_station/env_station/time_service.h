#ifndef TIME_SERVICE_H
#define TIME_SERVICE_H

// ============================================================================
// MODULE: time_service.h
// Single Responsibility: NTP time synchronization and formatted time strings.
// ============================================================================
#include <Arduino.h>
#include <time.h>
#include "config.h"

struct TimeState {
  bool   synced;
  String dateTimeStr; // "YYYY-MM-DD HH:MM:SS"
  String shortTimeStr;// "HH:MM"
  uint32_t uptimeSec;
};

TimeState clockState = {false, "Syncing NTP...", "--:--", 0};

void initTimeService() {
  configTzTime(TZ_INFO, NTP_SERVER_1, NTP_SERVER_2);
}

void updateTimeService() {
  clockState.uptimeSec = millis() / 1000UL;

  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10)) {
    char fullBuf[24];
    char shortBuf[8];
    strftime(fullBuf, sizeof(fullBuf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    strftime(shortBuf, sizeof(shortBuf), "%H:%M", &timeinfo);
    clockState.synced       = true;
    clockState.dateTimeStr  = String(fullBuf);
    clockState.shortTimeStr = String(shortBuf);
  } else if (!clockState.synced) {
    uint32_t m = (clockState.uptimeSec / 60) % 60;
    uint32_t h = (clockState.uptimeSec / 3600);
    char upBuf[16];
    snprintf(upBuf, sizeof(upBuf), "Up %02lu:%02lu", (unsigned long)h, (unsigned long)m);
    clockState.shortTimeStr = String(upBuf);
  }
}

#endif
