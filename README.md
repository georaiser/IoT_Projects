# 🌍⚛️🛰️🧠 IoT, Geophysics, Remote Sensing, Modern Physics & Edge AI Lab (ESP32)

A modular laboratory and hardware/software monorepo built around the **ESP32 ecosystem** (`ESP32 DevKit 30P`, `ESP32-C3 Super Mini`, `ESP32-S3 N16R8`, and `Seeed Studio XIAO ESP32-S3 Sense`), integrating **Environmental Telemetry**, **Applied Geophysics**, **Spectral Remote Sensing (`NDVI`)**, **Experimental Modern Physics**, and **Embedded Edge AI (`TinyML`) & Scientific Machine Learning (`CUDA`)**.

---

## 📁 Repository Structure

```text
D:\00_IoT_Projects\
├── README.md                                  # Main repository documentation (English)
├── .gitignore                                 # Git ignore rules (PlatformIO, Arduino, Python)
├── projects/
│   └── 01_env_station/                        # Project 01: Multi-Sensor Environmental & Air Quality Station
│       ├── README.md                          # Project 01 specific documentation & wiring map
│       ├── platformio.ini                     # PlatformIO configuration (esp32dev & esp32c3)
│       └── env_station/                       # Modular C++/Arduino source code (SOLID SRP & KISS)
│           ├── env_station.ino                # Main orchestrator (setup, loop & Wi-Fi lifecycle)
│           ├── config.h                       # Pin mapping, NTP config, thresholds & secrets loader
│           ├── secrets.example.h              # Public template for private Wi-Fi/API credentials
│           ├── time_service.h                 # NTP clock synchronization (ntp.shoa.cl) & formatting
│           ├── sensors.h                      # Pure sensor acquisition (AHT20, BMP280, MQ-135)
│           ├── alert_manager.h                # Threshold evaluation & passive buzzer control
│           ├── stats_logger.h                 # Running Min/Max/Avg & 24h circular RAM buffer + CSV
│           ├── oled_display.h                 # Large-text 3-slide OLED carousel + Clock & IP bar
│           ├── index_html.h                   # Isolated HTML5/CSS/JS frontend dashboard template
│           └── web_server.h                   # REST JSON API (/api/sensors), CSV export & HTTP routes
├── docs/
│   ├── 01_getting_started_esp32.md            # Step-by-step hardware & metrological validation guide
│   ├── 02_geophysics_and_environment_lab.md   # Architecture: Seismology, 2D ERT, Multi-Zone IoT & Modern Physics
│   ├── 03_edge_ai_tinyml_roadmap.md           # Edge AI (TinyML) & Scientific ML roadmap (AI-1 to AI-9)
│   ├── 04_mamba_cuda_esp32_setup.md           # Python (Mamba/CUDA) + ESP32 development environment guide
│   └── 05_cpp_for_python_java_programmers.md   # C++ for Python, Java & JS developers (Rosetta stone & API reference)
├── hardware_inventory/
│   ├── hardware_catalog.html                  # Interactive bill of materials (BOM) & sensor specs
│   └── inventory_check.xlsx                   # Hardware inventory spreadsheet
├── scripts/
│   ├── install_libraries_win.ps1              # Automated Arduino CLI library installer (Windows)
│   └── install_libraries_linux.sh             # Automated Arduino CLI library installer (Linux/Debian)
└── drivers/
    └── CP210x_Universal_Windows_Driver.zip    # USB-UART CP2102 driver for ESP32 DevKit
```

---

## 🚀 Core Instrumentation & Physics Projects

| ID | Project | Target MCU | Core Sensors / Hardware | Status |
|:--:|---|---|---|---|
| **01** | **Environmental & Air Quality Station** (`projects/01_env_station`) | `ESP32 DevKit 30P` / `ESP32-C3 Super Mini` | `AHT20` (Temp/RH), `BMP280` (Pressure), `MQ-135` (Air/Gas), `SSD1306 OLED`, Buzzer, NTP Clock | ✅ **Operational (v1.2)** (SOLID/KISS, REST API, Dynamic OLED & Web Charts, Metrological DMC/QFE Calibrated, $+0.7^\circ\text{C}$ Thermal Cross-Talk Characterized; Upgrade path: `ENS160+AHT21` / `SGP40`) |
| **02** | **2D Electrical Resistivity Tomography (ERT) & SEV Mini-Meter** (`projects/02_ert_meter`) | `ESP32 DevKit 30P` / `C3` | `ADS1115` (16-bit Differential ADC), Bipolar Injection (`D25`/`D26`), $1\text{ k}\Omega$ Shunt, 4 Electrodes | 🚀 **In Active Development** (5-Array Comparative Benchmark: Wenner, Dipole-Dipole, Schlumberger 1D, Wenner-Schlumberger, Pole-Dipole; depth $z_e \approx 0.52a$; `RES2DINV` `.dat` / `ResIPy` / `pyGIMLi` export) |
| **03** | **Modern Physics Lab: Planck's Constant ($h$)** | `ESP32 DevKit 30P` | 8-bit `DAC` (`D25`), `ADS1115` (16-bit), Multi-wavelength LEDs ($470\text{–}940\text{ nm}$) | 📋 Planned ($e V_0 = h\nu - \phi$ linear regression) |
| **04** | **Spectral Remote Sensing: Proximal Active `NDVI` Sensor** | `ESP32 DevKit 30P` / `C3` | Red LED ($625\text{ nm}$), NIR Emitter/Photodiode ($940\text{ nm}$, `HW-201`), `ADS1115`, `BH1750` | 📋 Planned ($\text{NDVI} = \frac{\rho_{\text{NIR}} - \rho_{\text{Red}}}{\rho_{\text{NIR}} + \rho_{\text{Red}}}$) |
| **05** | **Special Relativity & Cosmic Ray Muon Detector** | `XIAO ESP32-S3 Sense` | `OV2640` CMOS Dark-Frame Detector (or `BPW34` PIN Photodiodes) | 📋 Planned (Time dilation $\tau = \gamma\tau_0$ & Poisson statistics) |
| **06** | **3-Axis Digital Seismograph & FFT Waterfall (`STA/LTA`)** | `ESP32-S3 N16R8` | `ADXL345` (SPI, $800\text{ Hz}$), `MPU6050` (Tilt), `MicroSD` Datalogger | 📋 Planned |
| **07** | **Interactive Voice Telemetry Agent & Desktop Robot** (`projects/07_voice_assistant`) | `ESP32-S3 N16R8` | `INMP441` (I2S Mic), `MAX98357A` (3W Amp), `SG90` Servos, `RCWL-0516` | 📋 Planned (`ESP-SR` KWS + Local TTS + DeepSeek LLM) |

---

## 🧠 Edge AI (`TinyML`) & Scientific Machine Learning Projects

Detailed architecture and workflows are documented in [`docs/03_edge_ai_tinyml_roadmap.md`](docs/03_edge_ai_tinyml_roadmap.md).

| ID | AI / ML Module | Target Hardware | Model / Technique | Scientific & Practical Goal |
|:--:|---|---|---|---|
| **AI-1** | **Seismic P/S Wave vs. Urban Noise Classifier** | `ESP32-S3 N16R8` + `ADXL345` (SPI) | `STA/LTA` Trigger + FFT Spectral Features $\rightarrow$ `int8` MLP / 1D-CNN (`ESP-NN`) | Filter footsteps, door slams, and traffic vibrations from real seismic events |
| **AI-2** | **Local Barometric Rain Nowcasting ($1\text{–}3\text{ h}$)** | `ESP32-C3` / `S3` + `BMP280` + `AHT20` + `BH1750` + Rain Sensor | Embedded `RandomForest` in C (`emlearn`) trained on $\Delta P_{1h/3h}$, Dew Point spread ($T - T_d$), and Lux | Hyper-local short-term precipitation probability without internet |
| **AI-3** | **Sky & Cloud Cover Vision Classifier** | `XIAO ESP32-S3 Sense` (`OV2640` + `PSRAM`) | `96×96` quantized MobileNet / FOMO CNN | Classify sky state (`clear`, `scattered`, `overcast`, `rain/fog`, `night`) and feed cloud fraction to `AI-2` & `AI-7` |
| **AI-4** | **Environmental Acoustic Event Classifier** | `XIAO ESP32-S3 Sense` (PDM Mic) / `INMP441` (I2S) | $16\text{ kHz}$ Audio $\rightarrow$ Mel-Filterbank Energy (`MFE`) $\rightarrow$ 2D-CNN | Privacy-preserving acoustic detection of rain on roof, strong wind, and smoke/security alarms |
| **AI-5** | **Air Quality Plume Classifier ($\text{d}V/\text{d}t$) & Thermal Cross-Talk Compensation** | `ESP32 DevKit` / `C3` / `S3` + `MQ-135` + `AHT20` + `BMP280` | Thermal state regression + Autoencoder / MLP on $[V, \text{d}V/\text{d}t, \text{d}^2V/\text{d}t^2, \Delta T, \Delta RH]$ | Compensate the $+1.0\text{ }^\circ\text{C}$ `MQ-135` heater cross-talk on `AHT20` and distinguish butane leaks, VOCs/smoke, and metabolic $\text{CO}_2$ buildup |
| **AI-6** | **Geophysical 2D ERT Neural Inversion & Subsurface Anomaly Classifier** | `ESP32-S3` (Edge) + PC GPU `CUDA` (`PyTorch` / `pyGIMLi`) | Physics-Informed Neural Network (`PINN`) / Surrogate Inversion + Edge 1D-CNN on Wenner $\rho_a(x,a)$ | Millisecond 2D resistivity inversion benchmarked against `RES2DINV` / `ResIPy`, plus field classification of moisture bulbs vs. resistive cavities |
| **AI-7** | **Multispectral Remote Sensing Fusion (`NDVI` + Canopy Vision)** | `ESP32-S3` / `XIAO S3` + Active `Red/NIR` + `ADS1115` + `BH1750` | Multimodal `RandomForest` / `MLP` fusing active `NDVI`, `SAVI`, ambient `Lux`, and canopy RGB histogram | Correct bidirectional illumination (BRDF/shadows) and detect early plant water stress vs. healthy foliage |
| **AI-8** | **Relativistic Cosmic-Ray Muon CMOS Track Classifier** | `XIAO ESP32-S3 Sense` (`OV2640` Dark-Frame) | $16\times 16$ Pixel Patch Morphological CNN / Classifier | Separate genuine ionizing particle tracks (`vertical spot`, `oblique muon streak`) from CMOS thermal hot pixels and readout noise |
| **AI-9** | **Interactive Voice Telemetry Agent & Desktop Robot** | `ESP32-S3 N16R8` + `INMP441` (I2S Mic) + `MAX98357A` (3W I2S Amp) + `SG90` Servos + `RCWL-0516` | Local Wake-Word (`ESP-SR` / Edge Impulse KWS) + Local WAV TTS / Wi-Fi LLM (`DeepSeek`) | Hands-free spoken laboratory assistant reporting environmental, seismic, and geophysical status |

---

## 🛠️ Quick Start — Project 01 (`01_env_station`)

### Option A: VS Code + PlatformIO
1. Open `D:\00_IoT_Projects\projects\01_env_station` in **VS Code**.
2. Select target environment in `platformio.ini`:
   - `esp32dev` for **ESP32 DevKit 30P**
   - `esp32c3` for **ESP32-C3 Super Mini**
3. Build and upload with `pio run -t upload`.

### Option B: Arduino IDE 2.x
1. Open `D:\00_IoT_Projects\projects\01_env_station\env_station\env_station.ino`.
2. Select board:
   - **`ESP32 Dev Module`** (for DevKit 30P), or
   - **`ESP32C3 Dev Module`** with **`USB CDC On Boot: Enabled`** (for ESP32-C3 Super Mini).
3. Click **Upload (`→`)** and open Serial Monitor at **`115200 baud`**.

---

## ⚡ Project 02 Overview — 2D ERT & SEV Resistivity Meter (`02_ert_meter`)

A low-cost, high-precision geophysical resistivity meter using **ESP32 DevKit 30P** (or **ESP32-C3**) and the **ADS1115 16-bit differential ADC** for shallow subsurface imaging and sandbox analog modeling:

- **Mathematical Core:** $\rho_a = K \cdot \frac{\Delta V_{MN}}{I}$, where $I = \frac{\Delta V_{\text{shunt}}}{R_s}$ with $R_s = 1\text{ k}\Omega$.
- **Bipolar Injection (`D25`/`D26`):** Reverses $+3.3\text{ V} \to -3.3\text{ V}$ to eliminate natural Self-Potential ($SP$) and cancel electrochemical polarization.
- **5-Array Comparative Benchmark (Multi-Array Suite):**
  - **Wenner ($\alpha$):** $K = 2\pi a$, $z_e \approx 0.52 a$ (highest SNR, baseline for horizontal layering and water table).
  - **Dipole-Dipole:** $K = \pi n(n+1)(n+2)a$ (maximum horizontal resolution for vertical dykes, faults, cavities, and buried pipes).
  - **Schlumberger 1D:** $K = \frac{\pi (s^2 - b^2/4)}{b}$ (classic Vertical Electrical Sounding calibration without moving potential electrodes $M, N$).
  - **Wenner-Schlumberger:** $K = \pi n(n+1)a$ (hybrid balance for 2D multi-electrode tomography).
  - **Pole-Dipole:** $K = 2\pi n(n+1)a$ (remote $B$ at infinity; maximum depth penetration and sensitive to dipping structures).
- **Physical Depth of Investigation:**
  - Governed geometrically by array aperture ($z_e \approx 0.52a$ in Wenner).
  - At $3.3\text{--}5\text{ V}$ injection, signal-to-noise ratio supports up to $1.0\text{--}1.5\text{ m}$ depth in typical damp garden/agricultural soils ($a \approx 2\text{--}3\text{ m}$), and millimeter-scale profiling in laboratory sandbox models ($a = 2\text{--}5\text{ cm}$).
- **Standard Geophysical Export:** Direct browser download of `.dat` files for **`RES2DINV`** (standard array codes `1`, `3`, `6`, `7`), `.csv` datasets for Python **`pyGIMLi`** and **`ResIPy`**.
- **Field & IoT Interface:** Onboard SSD1306 OLED for instant field readouts, embedded Web Dashboard with real-time 2D pseudosection preview and multi-array comparison mode.
