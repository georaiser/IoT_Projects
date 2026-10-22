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
│       └── env_station/                       # Modular C++/Arduino source code
│           ├── env_station.ino                # Main entry point (setup & loop)
│           ├── config.h                       # Pin mapping, Wi-Fi config & alert thresholds
│           ├── sensors.h                      # AHT20, BMP280, MQ-135 (ADC) & Buzzer drivers
│           ├── oled_display.h                 # Large-text 3-slide carousel for SSD1306 OLED
│           └── web_server.h                   # Embedded real-time HTML/CSS web dashboard
├── docs/
│   ├── 01_getting_started_esp32.md            # Step-by-step hardware & metrological validation guide
│   ├── 02_geophysics_and_environment_lab.md   # Architecture: Seismology, 2D ERT, Multi-Zone IoT & Modern Physics
│   ├── 03_edge_ai_tinyml_roadmap.md           # Edge AI (TinyML) & Scientific ML roadmap (AI-1 to AI-9)
│   └── 04_mamba_cuda_esp32_setup.md           # Python (Mamba/CUDA) + ESP32 development environment guide
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
| **01** | **Environmental & Air Quality Station** (`projects/01_env_station`) | `ESP32 DevKit 30P` / `ESP32-C3 Super Mini` | `AHT20` (Temp/RH), `BMP280` (Barometric Pressure), `MQ-135` (Air Quality/Gas), `SSD1306 OLED`, Passive Buzzer | ✅ **Validated** (Calibrated vs. DMC Quinta Normal `330020`) |
| **02** | **2D Electrical Resistivity Tomography (ERT) Mini-Meter** | `ESP32 DevKit 30P` | `ADS1115` (16-bit Differential ADC), Bipolar Injection (`D25`/`D26`), $1\text{ k}\Omega$ Shunt, 4 Wenner Electrodes | 🔜 Next (`RES2DINV` `.dat` / `ResIPy` / `pyGIMLi` export) |
| **03** | **Modern Physics Lab: Planck's Constant ($h$)** | `ESP32 DevKit 30P` | 8-bit `DAC` (`D25`), `ADS1115` (16-bit), Multi-wavelength LEDs ($470\text{–}940\text{ nm}$) | 📋 Planned ($e V_0 = h\nu - \phi$ linear regression) |
| **04** | **Spectral Remote Sensing: Proximal Active `NDVI` Sensor** | `ESP32 DevKit 30P` / `C3` | Red LED ($625\text{ nm}$), NIR Emitter/Photodiode ($940\text{ nm}$, `HW-201`), `ADS1115`, `BH1750` | 📋 Planned ($\text{NDVI} = \frac{\rho_{\text{NIR}} - \rho_{\text{Red}}}{\rho_{\text{NIR}} + \rho_{\text{Red}}}$) |
| **05** | **Special Relativity & Cosmic Ray Muon Detector** | `XIAO ESP32-S3 Sense` | `OV2640` CMOS Dark-Frame Detector (or `BPW34` PIN Photodiodes) | 📋 Planned (Time dilation $\tau = \gamma\tau_0$ & Poisson statistics) |
| **06** | **3-Axis Digital Seismograph & FFT Waterfall (`STA/LTA`)** | `ESP32-S3 N16R8` | `ADXL345` (SPI, $800\text{ Hz}$), `MPU6050` (Tilt), `MicroSD` Datalogger | 📋 Planned |

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
