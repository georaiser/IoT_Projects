# 🌱 Project 01: Multi-Sensor Environmental & Air Quality Station (`01_env_station`)

A real-time IoT environmental telemetry and air quality monitoring station supporting both **ESP32 DevKit 30P** and **ESP32-C3 Super Mini**, featuring a large-text **SSD1306 OLED carousel**, an **embedded Wi-Fi web dashboard**, and **acoustic threshold alerts**.

---

## ✨ Key Features

- **Multi-Sensor Telemetry:**
  - **`AHT20` (`0x38` I2C):** Ambient temperature ($\pm 0.3\text{ }^\circ\text{C}$) and relative humidity ($\pm 2\%\text{ RH}$).
  - **`BMP280` (`0x77` / `0x76` I2C):** Barometric pressure ($\pm 1\text{ hPa}$, configured in `MODE_FORCED` to eliminate self-heating).
  - **`MQ-135` (Analog `ADC1`):** Air quality / VOC / $\text{CO}_2$ / gas detection via a protective $10\text{ k}\Omega / 10\text{ k}\Omega$ voltage divider ($5\text{ V} \to 2.5\text{ V}$ max) and high-sensitivity `ADC_2_5db` attenuation.
- **Large-Text OLED Carousel (`SSD1306 0.96"`, `0x3C` I2C):**
  - Automatically rotates every $3\text{ s}$ across 5 high-contrast views including real-time charts:
    1. **Slide 0:** Temperature (`T: XX.X C`) + Humidity (`H: XX.X %`)
    2. **Slide 1:** Atmospheric Pressure (`PRESSURE` / `XXXX.X hPa`)
    3. **Slide 2:** Air Quality Status (`AIR GOOD/FAIR/POOR!`) + Concentration (`XXXX ppm`)
    4. **Slide 3:** **Dynamic Temperature Chart** (auto-scaled between 24h Min & Max with upper/lower bounds)
    5. **Slide 4:** **Dynamic Humidity Chart** (auto-scaled between 24h Min & Max)
    - **Bottom Status Bar:** Continuously displays live atomic clock (`HH:MM`) and IP address.
- **Wireless Over-The-Air (OTA) Updates (`v1.2`):**
  - Integrated `ArduinoOTA` listener: all future firmware builds can be uploaded over Wi-Fi without needing a USB cable or touching the board in the bedroom.
- **Embedded Wi-Fi Web Server & REST API (`v1.2`):**
  - Modular frontend template in `index_html.h` running in PROGMEM.
  - Zero-flicker asynchronous `fetch()` polling every 2 seconds.
  - Dynamic auto-scaling charts with a toggle to standard fixed metrological ranges.
  - Endpoints: `GET /` (Dashboard), `GET /api/sensors` (JSON), `GET /api/history` (JSON points), `GET /api/history.csv` (24h CSV download).
- **Acoustic Alert System (Passive Buzzer):**

---

## 🔌 Hardware Pin Mapping (`ESP32 DevKit 30P` vs. `ESP32-C3 Super Mini`)

The firmware automatically selects the pinout at compile time via `#if CONFIG_IDF_TARGET_ESP32C3` in [`config.h`](env_station/config.h):

| Signal / Peripheral | `ESP32 DevKit 30P` | `ESP32-C3 Super Mini` | Notes |
|---|:---:|:---:|---|
| **I2C `SDA`** (`AHT20`, `BMP280`, `OLED`) | `GPIO 21` (`D21`) | `GPIO 4` | $100\text{ kHz}$ bus clock |
| **I2C `SCL`** (`AHT20`, `BMP280`, `OLED`) | `GPIO 22` (`D22`) | `GPIO 5` | $100\text{ kHz}$ bus clock |
| **`MQ-135` Analog Signal (`A0`)** | `GPIO 34` (`D34`) | `GPIO 0` (`ADC1_CH0`) | Via $10\text{ k}\Omega / 10\text{ k}\Omega$ voltage divider ($\div 2$) |
| **`MQ-135` Heater Power (`VCC`)** | `VIN` ($5\text{ V}$) | `5V` ($5\text{ V}$) | Powered directly from USB $5\text{ V}$ rail |
| **Passive Buzzer (`+`)** | `GPIO 25` (`D25`) | `GPIO 3` | PWM tone alert output |
| **Digital Sensor Power (`3.3V`)** | `3V3` | `3V3` | Powers `AHT20`, `BMP280`, and `OLED` |

---

## 🗺️ 400-Point Mini Breadboard Layout (`ESP32-C3 Super Mini`, Columns `30` $\rightarrow$ `1`)

> **Mini Breadboard Physical Coordinate Orientation:**
> - **Columns `30` down to `1`:** Left (`30`) $\rightarrow$ Right (`1`)
> - **Rows `a` to `j`:** Top (`a–e`) $\rightarrow$ Bottom (`f–j`)
> - **`ESP32-C3` with USB-C pointing Left (`Col 30`):**
>   - **Top pins (`c30`–`c23`, leaving `a, b` free):** `5V` (`30`), `GND` (`29`), `3V3` (`28`), `4` (`27`), `3` (`26`), `2` (`25`), `1` (`24`), `0` (`23`)
>   - **Bottom pins (`g30`–`g23`, leaving `h, i, j` free):** `5` (`30`), `6` (`29`), `7` (`28`), `8` (`27`), `9` (`26`), `10` (`25`), `20` (`24`), `21` (`23`)

```text
(+) ══════════ Top Red Rail    = 3.3V (continuous 30 to 1) ══════════════════════════════════
(−) ══════════ Top Blue Rail   = GND  (continuous 30 to 1) ══════════════════════════════════
COL:  30 29 28 27 26 25 24 23     20 19 18 17       14 13 12 11     7   6   5   4          2
              [Buzzer]
                 ▲          ┌───────────────────────────(Signal wire from a2 to a23)──────┐
     ┌───────────┼──────────┴─┐                                  ┌──────────────┐         │
a-e  │5V GND 3V3 4  3  2  1  0│                                  │VCC GND D0  A0│──[R1]─►(c2)
===  │◄USB-C  ESP32-C3    Ant.│ ======== (Central Channel) ===== └──────────────┘ =========═
f-j  │ 5  6   7  8  9 10 20 21│  [VDD SDA GND SCL] [GND VCC SCK SDA]  (MQ-135)
     └─┬──────────────────────┘   (AHT20+BMP280)     (OLED 0.96")
       └────────────────────────► (SCL wire from j30 to i17 & i12)
(+) ══════════ Bottom Red Rail  = 3.3V (continuous 30 to 1) ═════════════════════════════════
(−) ══════════ Bottom Blue Rail = GND  (continuous 30 to 1) ═════════════════════════════════
```

- **Voltage Divider (`10k / 10k` on top half `a–e`, Column `2`):**
  - `R1` ($10\text{ k}\Omega$): `c4` (`MQ-135 A0`) $\rightarrow$ `c2` (Midpoint)
  - `R2` ($10\text{ k}\Omega$): `b2` (Midpoint) $\rightarrow$ `b6` (`MQ-135 GND`)
  - Midpoint signal wire: `a2` $\rightarrow$ `a23` (`GPIO 0`, top-right corner of `ESP32-C3`)

---

## 📂 Source Code Architecture (SOLID / Single Responsibility & KISS)

```text
01_env_station/
├── README.md                  # Project 01 documentation
├── platformio.ini             # PlatformIO build environments (esp32dev & esp32c3)
└── env_station/
    ├── env_station.ino        # Main orchestrator (non-blocking timers & Wi-Fi lifecycle)
    ├── config.h               # Pin mapping, NTP config, thresholds & secrets.h loader
    ├── secrets.example.h      # Template for private Wi-Fi/API credentials
    ├── time_service.h         # SRP: NTP synchronization (ntp.shoa.cl) & time formatting
    ├── sensors.h              # SRP: Pure hardware acquisition (AHT20, BMP280, MQ-135)
    ├── alert_manager.h        # SRP: Threshold evaluation & passive buzzer control
    ├── stats_logger.h         # SRP: Running Min/Max/Avg & 24h circular RAM buffer + CSV
    ├── oled_display.h         # SRP: 3-slide large-text OLED carousel + Clock & IP bar
    ├── index_html.h           # SRP: Isolated HTML5/CSS/JS frontend template (PROGMEM)
    └── web_server.h           # SRP: REST JSON API (/api/sensors), CSV export & HTTP routes
```

---

## 🚀 Build & Flash Instructions

### 1. Using VS Code + PlatformIO
```bash
cd D:/00_IoT_Projects/projects/01_env_station
pio run -e esp32dev -t upload   # For ESP32 DevKit 30P
pio run -e esp32c3  -t upload   # For ESP32-C3 Super Mini
pio device monitor -b 115200
```

### 2. Using Arduino IDE 2.x
1. Open `env_station/env_station.ino`.
2. Select target board:
   - **ESP32 DevKit 30P:** `Tools -> Board -> esp32 -> ESP32 Dev Module`
   - **ESP32-C3 Super Mini:** `Tools -> Board -> esp32 -> ESP32C3 Dev Module` and set **`Tools -> USB CDC On Boot -> Enabled`**.
3. Click **Upload (`→`)**.

---

## 🔬 Metrological Validation Summary

Validated against the official Chilean Meteorological Directorate (DMC) **Quinta Normal Station (`330020`, Santiago)**:
- **Barometric Pressure (`BMP280`):** Tracked diurnal and frontal pressure variations (`947.3 hPa` to `955.3 hPa`) with a constant altitude offset of $-0.6\text{ to }-2.0\text{ hPa}$ ($\sim 15\text{ m}$ elevation difference).
- **Thermal Isolation:** Placing the `OLED` module (`Filas 11–8`) between `AHT20` (`Filas 20–17`) and `MQ-135` (`Filas 5–2`, dome facing outward) prevents the `MQ-135` $5\text{ V}$ internal heater from adding $+1.0\text{ }^\circ\text{C}$ radiative bias to ambient temperature readings (`AHT20 = 21.5 °C`, `BMP280 = 21.5 °C`).

---

## 🔗 REST API & Live Endpoints (`v1.2`)

- **`GET /`**: Live responsive dashboard featuring smooth 2-second `fetch()` telemetry, live NTP clock, real-time statistics (`Min / Avg / Max` with timestamps), and **adaptive charts with dynamic auto-scaling** (auto-fitting the vertical axis to real-time Min–Max bounds with a fixed-scale toggle button).
- **`GET /api/sensors`**: Structured JSON telemetry (`timestamp`, `uptime_s`, `current` readings, and running `stats` with `min`, `min_t`, `avg`, `max`, `max_t`).
- **`GET /api/history`**: Compact JSON array of 24h data points (`[timestamp_short, temp, hum, pres, air_ppm]`) powering the frontend charts.
- **`GET /api/history.csv`**: Downloadable 24-hour CSV datalog (`288` samples at 5-minute intervals stored in the `ESP32-C3` circular RAM buffer).

---

## 📊 Dual Display & Visualization Architecture

| Surface | Target Variable(s) | Scaling Type | Features |
|---|---|---|---|
| **OLED (`SSD1306`)** | Slides 3 & 4: `Temp` & `Humidity` | **Dynamic Scale** ($\text{Min}$ to $\text{Max}$) | Automatically scales to the exact 24h extreme values registered on-device, maximizing visual contrast on a 128x64 display. |
| **Web Dashboard (`HTML5`)** | Full Suite: `Temp`, `Hum`, `Pres`, `Air` | **Dynamic Scale by default** (with toggle to Fixed Range) | Dynamically zooms in on real 24h min-max range (with 15% margins) to highlight subtle microclimate variations, with a one-click toggle to standard fixed ranges. |
