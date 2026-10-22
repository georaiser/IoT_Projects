#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

// ============================================================================
// MODULE: alert_manager.h
// Single Responsibility: Evaluate threshold rules and drive the buzzer output.
// ============================================================================
#include <Arduino.h>
#include "config.h"
#include "sensors.h"

void initAlertManager() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  // Short startup confirmation beep (2000 Hz, 80 ms)
  tone(PIN_BUZZER, 2000, 80);
  delay(100);
  noTone(PIN_BUZZER);
  Serial.printf("Buzzer       : Ready on GPIO %d\n", PIN_BUZZER);
}

void triggerAlertBeep() {
  tone(PIN_BUZZER, 2500, 120);
  delay(160);
  tone(PIN_BUZZER, 2500, 120);
  delay(140);
  noTone(PIN_BUZZER);
}

void evaluateAlerts(SensorReadings& data) {
  bool tempAlert = data.hasAHT && (data.tempAHT >= TEMP_HIGH_ALERT_C || data.tempAHT <= TEMP_LOW_ALERT_C);
  bool airAlert  = (data.airQualityPct >= AIR_POOR_ALERT_PCT);

  data.alertActive = (tempAlert || airAlert);
  if (data.alertActive) {
    triggerAlertBeep();
  }
}

#endif
