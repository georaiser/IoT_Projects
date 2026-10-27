# 🌱 Project 01: Multi-Sensor Environmental & Air Quality Station (`01_env_station`)

A real-time IoT environmental telemetry and air quality monitoring station supporting both **ESP32 DevKit 30P** and **ESP32-C3 Super Mini**, featuring a large-text **SSD1306 OLED carousel**, an **embedded Wi-Fi web dashboard with interactive tooltips**, **acoustic threshold alerts**, **non-volatile LittleFS storage (~40 days)**, and **automated PC data synchronization**.

---

## ✨ Key Features

- **Multi-Sensor Telemetry:**
  - **`AHT20` (`0x38` I2C):** Ambient temperature ($\pm 0.3\text{ }^\circ\text{C}$) and relative humidity ($\pm 2\%\text{ RH}$).
  - **`BMP280` (`0x77` / `0x76` I2C):** Barometric pressure ($\pm 1\text{ hPa}$, configured in `MODE_FORCED` to eliminate self-heating).
  - **`MQ-135` (Analog `ADC1`):** Air quality / VOC / $\text{CO}_2$ / gas detection via a protective $10\text{ k}\Omega / 10\text{ k}\Omega$ voltage divider ($5\text{ V} \to 2.5\text{ V}$ max) and high-sensitivity `ADC_2_5db` attenuation. (When disconnected, `GPIO 0` is grounded to eliminate floating antenna noise).
- **Persistent Non-Volatile Flash Storage (`LittleFS`):**
  - Continuous circular append to on-board flash memory (`/history.csv`, 512 KB partition = **~40 days of continuous recording** at 5-minute intervals).
  - **Boot History Recovery:** On power-up or reboot, automatically restores past telemetry from flash into RAM so historical graphs survive power cycles without gaps.
  - **Memory Life Monitoring:** Real-time computation of flash storage used, percentage, and days remaining with low-storage warning banners in Web UI and OLED (`MEM LOW!` alert when $\le 2$ days left).
- **Large-Text OLED Carousel (`SSD1306 0.96"`, `0x3C` I2C):**
  - Automatically rotates every $3\text{ s}$ across 5 high-contrast views including auto-scaled real-time charts:
    1. **Slide 0:** Temperature (`T: XX.X C`) + Humidity (`H: XX.X %`)
    2. **Slide 1:** Atmospheric Pressure (`PRESSURE` / `XXXX.X hPa`)
    3. **Slide 2:** Air Quality Status (`AIR GOOD/FAIR/POOR!`) + Concentration (`XXXX ppm`)
    4. **Slide 3:** **Dynamic 48H Temperature Chart** (auto-scaled dynamically from true buffer min/max with zero-span protection)
    5. **Slide 4:** **Dynamic 48H Humidity Chart** (auto-scaled dynamically from true buffer min/max)
    - **Bottom Status Bar:** Live NTP atomic clock (`HH:MM`), station IP, and flash alert (`MEM LOW!`).
- **Wireless Over-The-Air (OTA) Updates (`v1.3`):**
  - Integrated `ArduinoOTA` listener: firmware updates can be deployed directly over Wi-Fi (`env-station at 192.168.1.5`).
- **Embedded Web Dashboard & Interactive Tooltips (`v1.3`):**
  - Frontend template in `index_html.h` in PROGMEM with 2-second asynchronous `fetch()` polling.
  - **Interactive HTML5 Canvas Hover Tooltip:** Vertical dashed guideline, target glow circle, and floating badge showing exact timestamp and measurement (`Time | Value Unit`) on both desktop (`mousemove`) and mobile touch (`touchmove`).
  - **Expanded 48-Hour Buffer:** 576 data points at 5-minute sampling intervals.
  - One-click toggle between Dynamic Zoom and Fixed Metrological Range.
  - Endpoints: `GET /` (Dashboard), `GET /api/sensors` (JSON with storage info), `GET /api/history` (576-point JSON), `GET /api/history.csv` (Flash stream), `GET /api/history/clear?confirm=true` (Remote flash purge).
- **Automated Python Synchronization Tool:**
  - Standalone script (`scripts/sync_station_data.py`) with zero pip dependencies. Deduplicates historical records, updates `data/env_station_master.csv`, and optionally purges flash memory with `--purge` after successful verification.

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

## 🔬 Metrological Validation & Thermal Characterization

Validated against the official Chilean Meteorological Directorate (DMC), physical thermodynamic laws, and empirical hardware experiments:

### 1. Barometric Pressure (`BMP280`)
- **Measured on-device:** **`957.0 hPa`**
- **Official Weather Station QFE (surface level):** **`957.8 hPa`**
- **Pressure delta:** **$\Delta P = 0.8\text{ hPa}$** (error $< 0.08\%$, corresponding to $\approx 6\text{–}7\text{ m}$ elevation difference between station and bedroom).

### 2. Comprehensive Thermal Budget & Breadboard Self-Heating
Experimental testing on hardware revealed two independent parasitic thermal sources affecting the `AHT20` when mounted on a compact breadboard:
- **`MQ-135` 5V Heater Bias:** Dissipates $\approx 0.75\text{ W}$ ($\approx 150\text{ mA}$). When mounted on the 400-point board, it contributed **$+0.7\text{ }^\circ\text{C}$** of radiative and conductive bias ($21.6\text{ }^\circ\text{C} \to 20.9\text{ }^\circ\text{C}$ upon removal).
- **ESP32-C3 CPU + Wi-Fi Radio + OLED Bias:** When seated just $7\text{ mm}$ away on the 400-point mini-board, the CPU and Wi-Fi transmit pulses added an additional **$+1.6\text{ }^\circ\text{C}$** parasitic heating ($21.0\text{ }^\circ\text{C} \to 19.4\text{ }^\circ\text{C}$).
- **Total Combined Breadboard Heating:** $\approx \mathbf{+2.3\text{ }^\circ\text{C}}$.
- **Hardware Mitigation:** Moving the entire system to an **830-point breadboard ($16.5\text{ cm}$ length)** with physical separation between the microcontroller and sensor modules immediately restored true ambient room temperature (**$19.4\text{ }^\circ\text{C}$**), matching nocturnal thermal gradients.

### 3. Psychrometric Relative Humidity Coupling (Clausius-Clapeyron)
- During the thermal transition from $21.0\text{ }^\circ\text{C}$ down to $19.4\text{ }^\circ\text{C}$ on the 830-point board, measured Relative Humidity rose from **$65.2\%$ to $69.2\%$**.
- **Thermodynamic Verification:** Evaluating the partial vapor pressure $e = \text{RH} \cdot e_s(T)$ via the Tetens/Magnus equation confirmed that $e \approx 16.2\text{ hPa}$ remained strictly constant. The apparent RH increase is a direct consequence of the temperature decrease lowering the saturation vapor pressure $e_s(T)$, validating the physical accuracy of the `AHT20`.

### 4. High-Impedance ADC Floating Antenna Effect (`GPIO 0`)
- When the `MQ-135` is unplugged, leaving `GPIO 0` unconnected turns the unbuffered internal ADC pin into an RF antenna picking up Wi-Fi burst pulses and 50 Hz mains noise, causing readings to jump erratically ($400 \to 969 \to 2000\text{ ppm}$).
- **Fix:** Connecting a jumper from `GPIO 0` directly to `GND` anchors the ADC to $0.00\text{ V}$, locking the baseline firmly at the clean-air calibration value ($400\text{ ppm}$).

---

## 🌟 Recommended Hardware Upgrades: Digital MOX Sensors

To eliminate the $0.75\text{ W}$ heating dissipation, save power, and obtain research-grade air quality metrics, two pin-compatible digital I2C alternatives are supported for station evolution:

### 1. `ENS160 + AHT21` Dual Combo (Recommended All-in-One)
- **Chips:** ScioSense `ENS160` (Multi-gas MOX) + Asair `AHT21` (High-precision Temp & RH).
- **Bus & Interface:** Pure **I2C at $3.3\text{ V}$** (`0x53` for `ENS160`, `0x38` for `AHT21`).
- **Power & Temperature:** Cold operation ($< 30\text{ mA}$ active, zero breadboard heating).
- **Metrics Provided:**
  - **Air Quality Index (AQI):** Standard 1 to 5 rating (German Federal Environmental Agency UBA).
  - **$\text{TVOC}$:** Total Volatile Organic Compounds ($0\text{ to }65,000\text{ ppb}$).
  - **$\text{eCO}_2$:** Equivalent Carbon Dioxide ($400\text{ to }65,000\text{ ppm}$).
  - **Temperature & Humidity:** $\pm 0.3\text{ }^\circ\text{C}$ and $\pm 2\%\text{ RH}$.
- **Key Advantage:** Replaces **both** the `AHT20` and `MQ-135` in a single tiny $1.2\times 1.2\text{ cm}$ footprint, completely freeing the analog `GPIO 0` and removing the $10\text{ k}\Omega / 10\text{ k}\Omega$ voltage divider.

### 2. `SGP40` (Sensirion Swiss Precision TVOC)
- **Chip:** Sensirion `SGP40` multipixel MOX sensor.
- **Bus & Interface:** Pure **I2C at $3.3\text{ V}$** (`0x59`).
- **Power & Temperature:** Ultra-low power (**$2.6\text{ mA}$**, strictly cold).
- **Metrics Provided:**
  - **VOC Index (0 to 500):** Processed via Sensirion's VOC Index Algorithm, with automatic real-time compensation using ambient temperature and humidity from the onboard `AHT20`.
- **Key Advantage:** Factory-calibrated with traceable gas mixtures, exceptional long-term baseline stability, and miniature breakout form factor.

---

## 🔗 REST API & Live Endpoints (`v1.3`)

- **`GET /`**: Live responsive dashboard featuring 2-second asynchronous `fetch()` telemetry, live NTP clock, real-time statistics (`Min / Avg / Max` with timestamps), **interactive canvas tooltip**, and **adaptive 48h charts with dynamic auto-scaling**.
- **`GET /api/sensors`**: Structured JSON telemetry including sensor readings, session stats, and on-board storage health:
  ```json
  {
    "uptime_s": 86400,
    "current": { "temp": 19.4, "humidity": 69.2, "pressure": 958.6, "air_ppm": 400 },
    "storage": { "used_kb": 12.4, "total_kb": 512.0, "pct": 2.4, "days_left": 39.1, "warning": false }
  }
  ```
- **`GET /api/history`**: Compact JSON array of up to 576 48-hour data points (`[timestamp_short, temp, hum, pres, air_ppm]`).
- **`GET /api/history.csv`**: Direct stream of `/history.csv` from LittleFS flash memory (5-minute resolution).
- **`GET /api/history/clear?confirm=true`** (or `POST`): Remote purge of the LittleFS historical log, recreating a fresh header.

---

## 🐍 Automated PC Synchronization & Master Archiving (`scripts/`)

To prevent flash overflow without losing data, the Python CLI tool [`sync_station_data.py`](../../scripts/sync_station_data.py) runs on any computer connected to the same LAN (zero pip dependencies):

```bash
# 1. Standard sync: downloads 48h history, deduplicates rows, appends to master CSV
python scripts/sync_station_data.py --ip 192.168.1.5

# 2. Automated sync & purge: verifies master CSV is saved before clearing ESP32 Flash
python scripts/sync_station_data.py --ip 192.168.1.5 --purge
```

- **Double-Click Runners:** Windows users can double-click [`sync_station.bat`](../../scripts/sync_station.bat) or [`sync_and_purge.bat`](../../scripts/sync_and_purge.bat).
- **Master Archive:** Consolidated locally into `D:\00_IoT_Projects\data\env_station_master.csv`.

---

## 📊 Dual Display & Visualization Architecture (`v1.3`)

| Surface | Target Variable(s) | Scaling Type | Features |
|---|---|---|---|
| **OLED (`SSD1306`)** | Slides 3 & 4: `Temp` & `Humidity` | **Dynamic Scale (48H)** ($\text{Min}$ to $\text{Max}$) | Auto-scans true min/max directly from active points in `historyBuffer` with `span < 0.5f` division-by-zero protection. Displays `MX` and `MN` badges and low storage alert (`MEM LOW!`). |
| **Web Dashboard (`HTML5`)** | Full Suite: `Temp`, `Hum`, `Pres`, `Air` | **Dynamic Scale (48H)** (with toggle to Fixed Range) | **Interactive Canvas Tooltip:** Vertical dashed crosshair with dynamic tooltip box displaying `HH:MM | Value Unit` on mouse hover or mobile touch. Includes low flash storage banner. |
