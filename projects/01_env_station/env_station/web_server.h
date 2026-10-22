#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WebServer.h>
#include "sensors.h"

WebServer server(80);

void renderDashboard() {
  String airColor = "#4ade80"; // Green
  if (readings.airStatus == "FAIR")  airColor = "#facc15"; // Yellow
  if (readings.airStatus == "POOR!") airColor = "#f87171"; // Red

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="3">
  <title>IoT Environmental Station</title>
  <style>
    body { font-family: system-ui, sans-serif; background: #0f172a; color: #f8fafc; text-align: center; margin: 0; padding: 20px; }
    h1 { font-size: 1.4rem; color: #38bdf8; margin-bottom: 20px; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(145px, 1fr)); gap: 15px; max-width: 640px; margin: 0 auto; }
    .card { background: #1e293b; padding: 18px; border-radius: 12px; border: 1px solid #334155; }
    .label { font-size: 0.85rem; color: #94a3b8; }
    .value { font-size: 1.75rem; font-weight: bold; margin-top: 8px; }
    .sub { font-size: 0.78rem; color: #94a3b8; margin-top: 4px; }
    .t1 { color: #fb923c; } .t2 { color: #facc15; } .hum { color: #38bdf8; } .pres { color: #4ade80; }
    .foot { margin-top: 20px; font-size: 0.8rem; color: #64748b; }
  </style>
</head>
<body>
  <h1>🌱 IoT Environmental Station</h1>
  <div class="grid">
    <div class="card"><div class="label">Temp (AHT20)</div><div class="value t1">)rawliteral" + String(readings.tempAHT, 1) + R"rawliteral( °C</div></div>
    <div class="card"><div class="label">Humidity</div><div class="value hum">)rawliteral" + String(readings.humidity, 1) + R"rawliteral( %</div></div>
    <div class="card"><div class="label">Pressure</div><div class="value pres">)rawliteral" + String(readings.pressure, 1) + R"rawliteral( hPa</div></div>
    <div class="card">
      <div class="label">Air Quality (MQ-135)</div>
      <div class="value" style="color:)rawliteral" + airColor + R"rawliteral(">)rawliteral" + String(readings.airQualityPpm) + R"rawliteral( ppm</div>
      <div class="sub">)rawliteral" + readings.airStatus + R"rawliteral( · )rawliteral" + String(readings.mq135Voltage, 2) + R"rawliteral( V (ADC: )rawliteral" + String(readings.mq135Raw) + R"rawliteral()</div>
    </div>
    <div class="card"><div class="label">Temp (BMP280)</div><div class="value t2">)rawliteral" + String(readings.tempBMP, 1) + R"rawliteral( °C</div></div>
  </div>
  <div class="foot">Auto-refreshing every 3 seconds</div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void initWebServer() {
  server.on("/", renderDashboard);
  server.begin();
}

#endif
