# ⚡ Project 02: 2D Electrical Resistivity Tomography (ERT) Mini-Meter (`02_ert_resistivity_meter`)

An open-source, laboratory-grade **2D Electrical Resistivity Tomography (ERT)** and **Vertical Electrical Sounding (VES)** acquisition meter powered by the **ESP32 DevKit 30P** and the **ADS1115 16-bit Differential ADC**. Designed for geophysical sandbox modeling, soil moisture mapping, and direct export to **`RES2DINV`**, **`ResIPy`**, and **`pyGIMLi`**.

---

## 🔬 Theoretical Foundations

### 1. Wenner Alpha Electrode Array ($\text{A–M–N–B}$)
For four collinear, equally spaced electrodes with inter-electrode spacing $a$:
- **Current Injection ($A, B$):** Injects controlled bipolar square pulses ($\pm 3.3\text{ V}$) to neutralize electrode electrochemical polarization and cancel background Self-Potential ($\text{SP}$).
- **Potential Measurement ($M, N$):** Differential high-impedance voltage measurement $\Delta V_{MN}$ via `ADS1115` channels `A0–A1`.
- **Current Sensing ($I$):** Measured across a precision $R_s = 1\text{ k}\Omega$ shunt resistor via `ADS1115` channels `A2–A3`.

$$\rho_a = 2 \pi \, a \, \frac{\Delta V_{MN}}{I}, \qquad I = \frac{\Delta V_{\text{shunt}}}{R_s}$$

Geometric factor for Wenner array:
$$k = 2 \pi \, a$$

---

## 🏖️ Analog Laboratory Modeling: The Sandbox / Terrarium Setup

To validate 2D tomography before conducting field surveys, the system is calibrated using an **analog sandbox model**:

```text
               ┌──(A)───(M)───(N)───(B)──┐  <-- Wenner Quadripole (Spacing a)
               │   │     │     │     │   │
  ═════════════╪═══╧═════╧═════╧═════╧═══╪════════════════════════════════════
  ░░░░░░░░░░░░░│░░░░░░░░░░░░░░░░░░░░░░░░░│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░  <-- Surface
  ░░░░░░░░░░░░░│░░░░░░░░░░░░░░░░░░░░░░░░░│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
  ░░░░░░░░░░░░░▼░░░░░░░░░░░░░░░░░░░░░░░░░▼░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
  ░░░░   Homogeneous Damp Sand Matrix (ρ_matrix ≈ 50 Ω·m)                 ░░░░
  ░░░░                                                                    ░░░░
  ░░░░              ┌───────────────┐              (○) Saturated Saline   ░░░░
  ░░░░              │ Dry Wood / PVC│                  Sponge Target      ░░░░
  ░░░░              │ Resistive     │                  (Conductive,       ░░░░
  ░░░░              │ Cavity Target │                   ρ << ρ_matrix)    ░░░░
  ░░░░              │ ρ >> ρ_matrix │                                     ░░░░
  ░░░░              └───────────────┘                                     ░░░░
  ════════════════════════════════════════════════════════════════════════════
  └────────────────── Sandbox Tank Container (40 - 60 cm) ───────────────────┘
```

- **Container Dimensions:** Clear acrylic or plastic sandbox ($40\text{--}60\text{ cm}$ length $\times 20\text{--}25\text{ cm}$ width $\times 15\text{ cm}$ depth).
- **Matrix Medium:** Fine silica or construction sand uniformly dampened with tap water ($\rho_0 \approx 30\text{--}80\ \Omega\cdot\text{m}$).
- **Electrodes:** 16 to 24 stainless steel needle electrodes driven into the sand at baseline spacing $a = 2.0\text{ cm}$ or $2.5\text{ cm}$.
- **Controlled Targets:**
  - **High-Resistivity Anomaly ($\rho \gg \rho_0$):** Dry wood block, sealed PVC cylinder, or dense non-porous stone (simulates air-filled karst voids, tunnels, or resistive bedrock).
  - **High-Conductivity Anomaly ($\rho \ll \rho_0$):** Brine-saturated sponge or metallic copper bar (simulates saline aquifers, clay lenses, or massive metallic sulfide ores).

---

## 🌐 Geophysical Comparison: Active ERT vs. Passive MT / AMT

While ERT utilizes an artificial, galvanic source, the monorepo's geomagnetic and potential instrumentation relates directly to passive electromagnetic exploration:

| Parameter | 2D ERT (This Project) | Audio-Magnetotellurics (AMT) | Magnetotellurics (MT) |
|---|---|---|---|
| **Source Type** | Active, controlled galvanic current ($\pm 3.3\text{ V}$) | Passive natural EM waves | Passive natural EM waves |
| **Physical Origin** | Bipolar square-wave DC pulses | Global lightning activity (Schumann Resonances: $7.8\text{ Hz}, 14\text{ Hz}$) | Solar wind, magnetospheric hydromagnetic waves, geomagnetic storms |
| **Frequency ($f$)** | Quasi-DC ($5\text{--}20\text{ Hz}$ switching) | $1\text{ Hz} \text{ to } 10\text{ kHz}$ | $0.0001\text{ Hz} \text{ to } 1\text{ Hz}$ |
| **Exploration Depth ($\delta$)** | Shallow ($0.05\text{ m} \text{ to } 2\text{ m}$ in lab/field) | Intermediate ($50\text{ m} \text{ to } 1000\text{ m}$) | Deep Crustal / Mantle ($1\text{ km} \text{ to } 50+\text{ km}$) |
| **Measured Fields** | $\Delta V_{MN}$ (Electric potential difference) | $\vec{E}(t)$ (Telluric) + $\vec{H}(t)$ (Magnetic) | $\vec{E}(t)$ (Telluric) + $\vec{H}(t)$ (Magnetic) |
| **Governing Law** | Ohm's Law: $\vec{J} = \sigma \vec{E}$ | Faraday's Law: $\nabla \times \vec{E} = -\frac{\partial \vec{B}}{\partial t}$ | Faraday's Law: $\nabla \times \vec{E} = -\frac{\partial \vec{B}}{\partial t}$ |
| **Sandbox Feasibility** | **100% Feasible** (Scale-independent geometry) | **Unfeasible** (Requires outdoor dipole length) | **Unfeasible** (Skin depth exceeds lab dimensions) |

### Why ERT Works in a Sandbox while MT Does Not:
The electromagnetic penetration depth follows the **Skin Depth formula**:
$$\delta \approx 503 \sqrt{\frac{\rho}{f}} \quad [\text{meters}]$$

In a $50\text{ cm}$ sandbox with $\rho = 50\ \Omega\cdot\text{m}$, an MT signal at $1\text{ Hz}$ has a skin depth of $\delta \approx 3.5\text{ km}$—the wave passes straight through the box without interacting. Conversely, galvanic ERT lines of current are geometrically confined by the boundary conditions of the tank, making sandbox inversion directly mathematically scalable to field ERT.

---

## 🔌 Hardware Pin Mapping (`ESP32 DevKit 30P`)

| Peripheral / Line | `ESP32 DevKit 30P` Pin | Signal / Function |
|---|:---:|---|
| **I2C SDA** | `GPIO 21` (`D21`) | Data line for `ADS1115` ADC ($100\text{ kHz}$) |
| **I2C SCL** | `GPIO 22` (`D22`) | Clock line for `ADS1115` ADC ($100\text{ kHz}$) |
| **Injection Phase + (`INJ_A`)** | `GPIO 25` (`D25`) | Push-pull driver for positive half-cycle ($+3.3\text{ V}$) |
| **Injection Phase - (`INJ_B`)** | `GPIO 26` (`D26`) | Push-pull driver for negative half-cycle ($-3.3\text{ V}$) |
| **`ADS1115` Differential A0–A1** | Analog Inputs `A0`, `A1` | High-impedance potential measurement $\Delta V_{MN}$ |
| **`ADS1115` Differential A2–A3** | Analog Inputs `A2`, `A3` | Shunt resistor drop $\Delta V_{Rs}$ for current calculation ($I$) |
| **`ADS1115` Power (`VDD` / `GND`)** | `3V3` / `GND` | Clean 3.3V power rail |

---

## 📁 Standard `RES2DINV` Data Export Format (`.dat`)

The ESP32 web server serves `/api/ert.dat` formatted for direct drag-and-drop into **`RES2DINV`**:

```text
Sandbox ERT Survey Profile 01
2.0                                # Unit electrode spacing (cm)
1                                  # Array code: 1 = Wenner
36                                 # Total number of data points
1                                  # 1 = Apparent resistivity format
0                                  # 0 = No IP (chargeability) data
2.0    1.0    48.25                # x_midpoint, n_level, apparent_resistivity (Ohm-m)
4.0    1.0    49.10
...
```

---

## 📂 Source Code Architecture (SOLID / SRP)

```text
02_ert_resistivity_meter/
├── README.md                  # Project 02 comprehensive documentation
├── platformio.ini             # PlatformIO configuration
└── ert_meter/
    ├── ert_meter.ino          # Orchestrator & sequence controller
    ├── config.h               # Hardware pins, electrode geometries, shunt value
    ├── injection_driver.h     # SRP: Bipolar current pulse generator & SP neutralization
    ├── ads1115_reader.h       # SRP: Differential high-resolution acquisition (V_mn & I)
    ├── pseudosection_math.h   # SRP: Geometric factor k, apparent resistivity rho_a calculation
    ├── res2dinv_exporter.h    # SRP: RES2DINV .dat file formatting & buffer management
    └── web_server.h           # SRP: REST API, live pseudo-section SVG plot, & .dat file download
```
