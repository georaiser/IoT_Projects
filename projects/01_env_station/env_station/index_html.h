#ifndef INDEX_HTML_H
#define INDEX_HTML_H

// ============================================================================
// MODULE: index_html.h
// Single Responsibility: Static HTML/CSS/JS dashboard template (stored in PROGMEM).
// Features: Dynamic auto-scaling (with Fixed toggle), multi-metric tabs.
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
    body { font-family: system-ui, -apple-system, sans-serif; background: #0f172a; color: #f8fafc; text-align: center; margin: 0; padding: 16px; }
    h1 { font-size: 1.35rem; color: #38bdf8; margin: 0 0 4px 0; }
    .clock { font-size: 0.92rem; color: #cbd5e1; margin-bottom: 14px; font-variant-numeric: tabular-nums; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 10px; max-width: 680px; margin: 0 auto; }
    .card { background: #1e293b; padding: 12px; border-radius: 12px; border: 1px solid #334155; }
    .label { font-size: 0.8rem; color: #94a3b8; }
    .value { font-size: 1.65rem; font-weight: 700; margin: 4px 0; font-variant-numeric: tabular-nums; }
    .sub { font-size: 0.72rem; color: #94a3b8; line-height: 1.3; }
    .stat-bar { margin-top: 6px; padding-top: 6px; border-top: 1px solid #334155; font-size: 0.7rem; color: #cbd5e1; display: flex; justify-content: space-between; }
    .t1 { color: #fb923c; } .t2 { color: #facc15; } .hum { color: #38bdf8; } .pres { color: #4ade80; }
    
    /* Chart Section */
    .chart-box { background: #1e293b; border: 1px solid #334155; border-radius: 12px; max-width: 680px; margin: 14px auto 0 auto; padding: 12px; }
    .tabs-row { display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px; flex-wrap: wrap; gap: 8px; }
    .tabs { display: flex; gap: 5px; flex-wrap: wrap; }
    .tab-btn { background: #0f172a; color: #94a3b8; border: 1px solid #334155; border-radius: 6px; padding: 5px 9px; font-size: 0.75rem; cursor: pointer; }
    .tab-btn.active { background: #38bdf8; color: #0f172a; font-weight: 700; border-color: #38bdf8; }
    .mode-btn { background: #334155; color: #38bdf8; border: 1px solid #475569; border-radius: 6px; padding: 5px 9px; font-size: 0.72rem; cursor: pointer; font-weight: 600; }
    canvas { width: 100%; height: 180px; display: block; }
    .scale-info { font-size: 0.72rem; color: #94a3b8; margin-top: 6px; }

    .actions { margin-top: 14px; display: flex; gap: 8px; justify-content: center; flex-wrap: wrap; }
    .btn { background: #1e293b; color: #38bdf8; border: 1px solid #38bdf8; padding: 7px 12px; border-radius: 8px; text-decoration: none; font-size: 0.8rem; }
    .foot { margin-top: 12px; font-size: 0.72rem; color: #64748b; }
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
  </div>

  <!-- Historical Charts with Dynamic/Fixed Scale Toggle -->
  <div class="chart-box">
    <div class="tabs-row">
      <div class="tabs">
        <button class="tab-btn active" onclick="setChart('temp')">Temperature</button>
        <button class="tab-btn" onclick="setChart('hum')">Humidity</button>
        <button class="tab-btn" onclick="setChart('pres')">Pressure</button>
        <button class="tab-btn" onclick="setChart('air')">Air Quality</button>
      </div>
      <button class="mode-btn" id="modeBtn" onclick="toggleScale()">Mode: Dynamic (Auto Zoom)</button>
    </div>
    <canvas id="cv" width="650" height="180"></canvas>
    <div class="scale-info" id="scaleInfo">Dynamic Scale: Auto-fitting Min to Max</div>
  </div>

  <div class="actions">
    <a class="btn" href="/api/history.csv">📥 Download 24h CSV</a>
    <a class="btn" href="/api/sensors" target="_blank">🔗 JSON API</a>
  </div>
  <div class="foot">Live updates every 2s · Smooth auto-sync</div>

  <script>
    let activeMetric = 'temp';
    let isDynamic = true; // Default: Dynamic scale for high detail
    let historyData = [];

    const SCALES = {
      temp: { fixedMin: 10.0,  fixedMax: 40.0,   unit: '°C',  color: '#fb923c', title: 'Temperature' },
      hum:  { fixedMin: 20.0,  fixedMax: 90.0,   unit: '%',   color: '#38bdf8', title: 'Humidity' },
      pres: { fixedMin: 940.0, fixedMax: 970.0,  unit: 'hPa', color: '#4ade80', title: 'Barometric Pressure' },
      air:  { fixedMin: 400.0, fixedMax: 2000.0, unit: 'ppm', color: '#facc15', title: 'Air Quality (CO2 eq)' }
    };

    function setChart(m) {
      activeMetric = m;
      document.querySelectorAll('.tab-btn').forEach((b, i) => {
        b.classList.toggle('active', ['temp','hum','pres','air'][i] === m);
      });
      drawChart();
    }

    function toggleScale() {
      isDynamic = !isDynamic;
      document.getElementById('modeBtn').textContent = isDynamic ? 'Mode: Dynamic (Auto Zoom)' : 'Mode: Fixed Scale';
      drawChart();
    }

    function drawChart() {
      const cv = document.getElementById('cv');
      const ctx = cv.getContext('2d');
      const W = cv.width, H = cv.height;
      ctx.clearRect(0, 0, W, H);

      const cfg = SCALES[activeMetric];
      const mIdx = { temp: 1, hum: 2, pres: 3, air: 4 }[activeMetric];
      const padL = 46, padR = 15, padT = 15, padB = 25;
      const plotW = W - padL - padR;
      const plotH = H - padT - padB;

      // Determine scale range
      let yMin = cfg.fixedMin;
      let yMax = cfg.fixedMax;

      if (isDynamic && historyData && historyData.length > 0) {
        let vals = historyData.map(d => d[mIdx]);
        let dMin = Math.min(...vals);
        let dMax = Math.max(...vals);
        let margin = (dMax - dMin) * 0.15;
        if (margin < 0.2) margin = 0.5; // Minimum margin if values are flat
        yMin = dMin - margin;
        yMax = dMax + margin;
      }

      document.getElementById('scaleInfo').textContent = (isDynamic ? 'Dynamic Zoom: ' : 'Fixed Range: ') +
        yMin.toFixed(1) + (activeMetric==='air'?'':cfg.unit) + ' to ' + yMax.toFixed(1) + (activeMetric==='air'?'':cfg.unit);

      // Draw Grid & Y Labels
      ctx.strokeStyle = '#334155';
      ctx.fillStyle = '#94a3b8';
      ctx.font = '11px sans-serif';
      ctx.textAlign = 'right';

      const steps = 4;
      for (let s = 0; s <= steps; s++) {
        const yVal = yMin + ((yMax - yMin) * (steps - s) / steps);
        const py = padT + (plotH * s / steps);
        ctx.beginPath();
        ctx.moveTo(padL, py);
        ctx.lineTo(W - padR, py);
        ctx.stroke();
        ctx.fillText(yVal.toFixed(activeMetric === 'pres' || activeMetric === 'air' ? 0 : 1) + (activeMetric==='air'?'':cfg.unit), padL - 6, py + 4);
      }

      if (!historyData || historyData.length === 0) {
        ctx.fillStyle = '#64748b';
        ctx.textAlign = 'center';
        ctx.fillText('Collecting 24h data samples...', W / 2, H / 2);
        return;
      }

      // Plot data line
      ctx.beginPath();
      ctx.strokeStyle = cfg.color;
      ctx.lineWidth = 2.5;

      const n = historyData.length;
      for (let i = 0; i < n; i++) {
        const val = historyData[i][mIdx];
        const px = padL + (n > 1 ? (plotW * i / (n - 1)) : (plotW / 2));
        let norm = (val - yMin) / (yMax - yMin);
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;
        const py = padT + plotH * (1.0 - norm);

        if (i === 0) ctx.moveTo(px, py);
        else ctx.lineTo(px, py);
      }
      ctx.stroke();

      // Highlight latest point with a filled dot
      if (n > 0) {
        const lastVal = historyData[n - 1][mIdx];
        const lastX = padL + (n > 1 ? plotW : (plotW / 2));
        let norm = (lastVal - yMin) / (yMax - yMin);
        if (norm < 0) norm = 0; if (norm > 1) norm = 1;
        const lastY = padT + plotH * (1.0 - norm);

        ctx.fillStyle = cfg.color;
        ctx.beginPath();
        ctx.arc(lastX, lastY, 4, 0, Math.PI * 2);
        ctx.fill();
      }

      // Draw first and last time labels
      ctx.fillStyle = '#94a3b8';
      ctx.textAlign = 'left';
      ctx.fillText(historyData[0][0], padL, H - 6);
      ctx.textAlign = 'right';
      ctx.fillText(historyData[n - 1][0], W - padR, H - 6);
    }

    async function fetchHistory() {
      try {
        const res = await fetch('/api/history');
        historyData = await res.json();
        drawChart();
      } catch (e) {}
    }

    async function refreshData() {
      try {
        const res = await fetch('/api/sensors');
        const d = await res.json();
        document.getElementById('clock').textContent = '🕒 ' + d.timestamp;
        document.getElementById('t_aht').textContent = d.current.temp_aht.toFixed(1) + ' °C';
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
      } catch (e) {}
    }

    refreshData();
    fetchHistory();
    setInterval(refreshData, 2000);
    setInterval(fetchHistory, 30000);
  </script>
</body>
</html>
)rawliteral";

#endif
