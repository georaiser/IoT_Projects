#ifndef INDEX_HTML_H
#define INDEX_HTML_H

// ============================================================================
// MODULE: index_html.h
// Single Responsibility: Static HTML/CSS/JS dashboard template (stored in PROGMEM).
// ============================================================================
#include <pgmspace.h>

static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <title>IoT Environmental Station</title>
  <style>
    body { font-family: system-ui, -apple-system, sans-serif; background: #0f172a; color: #f8fafc; text-align: center; margin: 0; padding: 18px; }
    h1 { font-size: 1.35rem; color: #38bdf8; margin: 0 0 6px 0; }
    .clock { font-size: 0.95rem; color: #cbd5e1; margin-bottom: 16px; font-variant-numeric: tabular-nums; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(155px, 1fr)); gap: 12px; max-width: 680px; margin: 0 auto; }
    .card { background: #1e293b; padding: 14px; border-radius: 12px; border: 1px solid #334155; }
    .label { font-size: 0.82rem; color: #94a3b8; }
    .value { font-size: 1.7rem; font-weight: 700; margin: 6px 0; font-variant-numeric: tabular-nums; }
    .sub { font-size: 0.74rem; color: #94a3b8; line-height: 1.35; }
    .stat-bar { margin-top: 8px; padding-top: 6px; border-top: 1px solid #334155; font-size: 0.72rem; color: #cbd5e1; display: flex; justify-content: space-between; }
    .t1 { color: #fb923c; } .t2 { color: #facc15; } .hum { color: #38bdf8; } .pres { color: #4ade80; }
    .actions { margin-top: 18px; display: flex; gap: 10px; justify-content: center; flex-wrap: wrap; }
    .btn { background: #1e293b; color: #38bdf8; border: 1px solid #38bdf8; padding: 8px 14px; border-radius: 8px; text-decoration: none; font-size: 0.82rem; }
    .foot { margin-top: 14px; font-size: 0.75rem; color: #64748b; }
  </style>
</head>
<body>
  <h1>🌱 IoT Environmental Station</h1>
  <div class="clock" id="clock">Syncing clock...</div>

  <div class="grid">
    <div class="card">
      <div class="label">Temp (AHT20)</div>
      <div class="value t1" id="t_aht">--.- °C</div>
      <div class="stat-bar"><span id="t_min">Min: --</span><span id="t_avg">Avg: --</span><span id="t_max">Max: --</span></div>
    </div>
    <div class="card">
      <div class="label">Humidity</div>
      <div class="value hum" id="hum">--.- %</div>
      <div class="stat-bar"><span id="h_min">Min: --</span><span id="h_avg">Avg: --</span><span id="h_max">Max: --</span></div>
    </div>
    <div class="card">
      <div class="label">Pressure</div>
      <div class="value pres" id="pres">----.- hPa</div>
      <div class="stat-bar"><span id="p_min">Min: --</span><span id="p_avg">Avg: --</span><span id="p_max">Max: --</span></div>
    </div>
    <div class="card">
      <div class="label">Air Quality (MQ-135)</div>
      <div class="value" id="air_ppm">--- ppm</div>
      <div class="sub" id="air_sub">--</div>
      <div class="stat-bar"><span id="a_min">Min: --</span><span id="a_avg">Avg: --</span><span id="a_max">Max: --</span></div>
    </div>
    <div class="card">
      <div class="label">Temp (BMP280)</div>
      <div class="value t2" id="t_bmp">--.- °C</div>
      <div class="sub" id="samples_info">Samples: 0</div>
    </div>
  </div>

  <div class="actions">
    <a class="btn" href="/api/history.csv">📥 Download 24h CSV</a>
    <a class="btn" href="/api/sensors" target="_blank">🔗 JSON API</a>
  </div>
  <div class="foot">Live smooth updates every 2s (No page reload)</div>

  <script>
    async function refreshData() {
      try {
        const res = await fetch('/api/sensors');
        const d = await res.json();
        document.getElementById('clock').textContent = '🕒 ' + d.timestamp;
        document.getElementById('t_aht').textContent = d.current.temp_aht.toFixed(1) + ' °C';
        document.getElementById('t_bmp').textContent = d.current.temp_bmp.toFixed(1) + ' °C';
        document.getElementById('hum').textContent   = d.current.humidity.toFixed(1) + ' %';
        document.getElementById('pres').textContent  = d.current.pressure.toFixed(1) + ' hPa';

        const airEl = document.getElementById('air_ppm');
        airEl.textContent = d.current.air_ppm + ' ppm';
        airEl.style.color = d.current.air_status === 'GOOD' ? '#4ade80' : (d.current.air_status === 'FAIR' ? '#facc15' : '#f87171');
        document.getElementById('air_sub').textContent = d.current.air_status + ' · ' + d.current.mq135_v.toFixed(2) + ' V (ADC: ' + d.current.mq135_raw + ')';

        document.getElementById('t_min').textContent = 'Min ' + d.stats.temp.min.toFixed(1) + ' (' + d.stats.temp.min_t + ')';
        document.getElementById('t_avg').textContent = 'Avg ' + d.stats.temp.avg.toFixed(1);
        document.getElementById('t_max').textContent = 'Max ' + d.stats.temp.max.toFixed(1) + ' (' + d.stats.temp.max_t + ')';

        document.getElementById('h_min').textContent = 'Min ' + d.stats.hum.min.toFixed(0) + '%';
        document.getElementById('h_avg').textContent = 'Avg ' + d.stats.hum.avg.toFixed(0) + '%';
        document.getElementById('h_max').textContent = 'Max ' + d.stats.hum.max.toFixed(0) + '%';

        document.getElementById('p_min').textContent = 'Min ' + d.stats.pres.min.toFixed(1);
        document.getElementById('p_avg').textContent = 'Avg ' + d.stats.pres.avg.toFixed(1);
        document.getElementById('p_max').textContent = 'Max ' + d.stats.pres.max.toFixed(1);

        document.getElementById('a_min').textContent = 'Min ' + d.stats.air.min.toFixed(0);
        document.getElementById('a_avg').textContent = 'Avg ' + d.stats.air.avg.toFixed(0);
        document.getElementById('a_max').textContent = 'Max ' + d.stats.air.max.toFixed(0) + ' (' + d.stats.air.max_t + ')';

        document.getElementById('samples_info').textContent = 'Samples: ' + d.samples + ' | Uptime: ' + Math.floor(d.uptime_s / 60) + 'm';
      } catch (e) {}
    }
    refreshData();
    setInterval(refreshData, 2000);
  </script>
</body>
</html>
)rawliteral";

#endif
