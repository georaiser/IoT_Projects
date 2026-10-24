# 🌋🏠 Proyectos ESP32 — Estación Geofísica + Monitor Ambiental
> **v1.1** — Dos proyectos integrados con hardware compartido. Construidos desde cero con ESP32. Corregido el 10-10-2026: sensores reales (AHT20+BMP280), asignación de nodos, mapa de pines del ESP32-S3 y compras conciliadas con facturas.

---

## Tabla de Contenidos
0. [Asignación de nodos y mapa de pines](#0--asignación-de-nodos-y-mapa-de-pines-corregido-v11)
1. [Estación Geofísica (Sísmica + Resistividad ERT)](#1-🌋-estación-geofísica)
2. [Monitor Ambiental Multi-Zona](#2-🏠-monitor-ambiental-multi-zona)
3. [Hardware Consolidado](#3-📦-hardware-consolidado-real-conciliado-con-facturas)
4. [Fases de Implementación](#4-🗓️-fases-de-implementación)
5. [Comparación con Equipos Comerciales](#5-📊-comparación-con-equipos-comerciales)
6. [Laboratorio de Física Moderna, Cuántica, Relatividad y Teledetección](#6-⚛️🛰️-laboratorio-de-física-moderna-cuántica-relatividad-y-teledetección)

---

## 0. 🔧 Asignación de nodos y mapa de pines (corregido v1.1)

> Resuelve las inconsistencias detectadas al conciliar los documentos con las facturas reales (10-10-2026).

### 0.1 Decisiones de diseño

| Tema | Decisión |
|---|---|
| Sensor de presión / humedad | **AHT20 + BMP280** (humedad y temp. del AHT20, presión del BMP280). **No** se usan BME280 ni DHT22 (no comprados). |
| Placas disponibles | 1× ESP32-S3 N16R8, 1× ESP32 DevKit 30P, 2× ESP32-C3 Super Mini, 1× XIAO ESP32-S3 Sense. |
| Hub | El ESP32-S3 es **Hub + Estación sísmica central + Salón** (una sola placa, comparte I2C). |
| DevKit 30P | **Nodo remoto sísmico** (ADXL345 #2). Ya no hace de Salón (había un conflicto: el mismo DevKit tenía dos roles). |
| AHT20+BMP280 ×3 | Hub (salón/central), Dormitorio y Exterior. El nodo remoto sísmico no lleva este sensor. |
| BH1750 ×2 | Hub y Exterior. El Dormitorio no tiene BH1750 (opcional si se compra un 3.º). |
| MQ-135 ×2 + ADS1115 ×1 | MQ-135 #1 en el Salón (Hub) vía ADS1115. MQ-135 #2 es opcional en el Dormitorio, leído por el ADC interno del C3 (12 bit, menos preciso). |
| OLED ×2 | Hub y nodo remoto sísmico. |
| MicroSD | 2 módulos (Hub y remoto); el XIAO trae ranura propia. Los C3 **no** llevan SD: el Hub registra sus datos recibidos por MQTT. Hay 1 tarjeta comprada y faltan 2 (remoto y XIAO). |
| Broker MQTT | **Mosquitto en PC o Raspberry Pi.** El Hub es cliente y suscriptor, no broker. |
| XIAO ESP32-S3 Sense | Nivel 2 (opcional): cámara de cielo y audio. |
| TFT ST7789 | **Pendiente de compra.** |

### 0.2 Mapa de pines — ESP32-S3 N16R8 (Hub)

> ⚠️ **Pines que NO se deben usar** en el N16R8: GPIO26–32 (flash), **GPIO33–37 (PSRAM octal)**, GPIO19/20 (USB), GPIO0/3/45/46 (strapping) y GPIO43/44 (UART0). GPIO48 suele tener el LED RGB.
> El mapa original (GPIO21/22 y GPIO6–15) era el del ESP32 clásico y chocaba con estos pines.

| Función | Pines | Notas |
|---|---|---|
| **I2C** | SDA = GPIO8, SCL = GPIO9 | OLED 0x3C, MPU6050 0x68, AHT20 0x38, BMP280 0x77, BH1750 0x23, ADS1115 0x48 |
| **SPI-A** (sísmico + SD) | SCK = GPIO12, MOSI = GPIO11, MISO = GPIO13 | ADXL345 CS = GPIO10 · MicroSD CS = GPIO14 |
| **SPI-B** (TFT, pendiente) | SCK = GPIO18, MOSI = GPIO17 | CS = GPIO16, DC = GPIO15, RST = GPIO21, BL a 3V3. Bus propio para que el refresco de pantalla no interfiera con el muestreo del ADXL345 |
| DS18B20 | GPIO4 | 1-Wire + resistencia 4.7 kΩ a 3.3V |
| Mic INMP441 (I2S) | SCK = GPIO5, WS = GPIO6, SD = GPIO7 | |
| Ampli MAX98357A (I2S) | BCLK = GPIO38, LRC = GPIO39, DIN = GPIO40 | |
| Buzzer pasivo | GPIO41 | PWM |
| PIR Salón | GPIO42 | |
| Potenciómetro (umbral STA/LTA) | GPIO1 | ADC1 |

### 0.3 Mapa de pines — ESP32 DevKit 30P (Nodo remoto sísmico)

| Función | Pines |
|---|---|
| SPI (VSPI) | SCK = GPIO18, MISO = GPIO19, MOSI = GPIO23 |
| ADXL345 | CS = GPIO5 |
| MicroSD | CS = GPIO4 |
| I2C (OLED 0x3C) | SDA = GPIO21, SCL = GPIO22 |
| Buzzer | GPIO25 |
| Potenciómetro | GPIO34 (solo entrada) |

> Evitar GPIO6–11 (flash) y los strapping GPIO0/2/12/15 para periféricos.

### 0.4 Mapa de pines — ESP32-C3 Super Mini (Dormitorio y Exterior)

| Función | Pines |
|---|---|
| I2C (AHT20+BMP280, BH1750) | SDA = GPIO4, SCL = GPIO5 |
| PIR (Dormitorio) | GPIO3 |
| Sensor de lluvia (Exterior) | DO = GPIO3, AO = GPIO2 (ADC1) |
| MQ-135 #2 (opcional, Dormitorio) | AO = GPIO2 (ADC1), con divisor resistivo |

> GPIO8 (LED) y GPIO9 (botón BOOT) son strapping en el C3; GPIO18/19 son USB.

> ⚠️ **MQ-135 + ADS1115:** el MQ-135 se alimenta a 5V y su salida analógica puede superar los 3.3V. Alimenta el ADS1115 a 3.3V y baja la señal con un divisor (por ejemplo 10 kΩ / 20 kΩ) antes de la entrada A0. Los "ppm de CO₂" del MQ-135 son solo una estimación.

---

## 1. 🌋 Estación Geofísica

### Objetivo
Medir y registrar fenómenos geofísicos reales: sismos, inclinación del terreno, presión atmosférica, temperatura, luz e imágenes horarias del cielo. Comparar los registros propios con los datos oficiales del [Centro Sismológico Nacional (CSN)](https://www.csn.uchile.cl).

> 💡 El proyecto comercial equivalente más cercano es el **Raspberry Shake 1D** (US$300). Esta estación cuesta ~$50 USD en componentes.

### Arquitectura del Sistema

```text
┌─────────────────────────────────────────────────────────┐
│               ESTACIÓN GEOFÍSICA — 2 NODOS              │
├────────────────────────┬────────────────────────────────┤
│  NODO CENTRAL (HUB)    │  NODO REMOTO                   │
│  ESP32-S3 N16R8        │  ESP32 DevKit 30P              │
│  ─────────────────     │  ──────────────────            │
│  ADXL345  (sísmico)    │  ADXL345  (sísmico)            │
│  MPU6050  (inclin.)    │  OLED SSD1306                  │
│  AHT20+BMP280 (presión)│  MicroSD + SD 16GB             │
│  BH1750   (luz)        │  WiFi Cliente + MQTT           │
│  DS18B20  (suelo)      │                                │
│  OLED SSD1306          │                                │
│  TFT ST7789 (pendiente)│                                │
│  MicroSD + SD 16GB     │                                │
│  ADS1115 + MQ-135      │                                │
│  WiFi + cliente MQTT   │                                │
├────────────────────────┴────────────────────────────────┤
│  XIAO ESP32-S3 Sense (Nivel 2 — opcional)               │
│  Foto JPG horaria del cielo + metadata ambiental        │
└─────────────────────────────────────────────────────────┘
```

### Sensores — Detalle Técnico

| Sensor | Bus | Dirección | Mide | Rol en el Proyecto |
|---|---|---|---|---|
| **ADXL345 GY-291** | SPI | — | Aceleración 3 ejes ±16g, 3.2 kHz ODR | Captura ondas sísmicas P y S |
| **MPU6050 GY-521** | I2C | 0x68 | Aceleración + Giroscopio | Inclinación lenta del terreno (mm/día) |
| **AHT20 + BMP280** | I2C | 0x38 (AHT20) / 0x77 (BMP280) | Presión ±1 hPa (BMP280), Temp y Humedad (AHT20) | Correlación atmosférica con sismicidad |
| **BH1750** | I2C | 0x23 | Iluminancia 0–65535 lux | Ciclo día/noche en el registro |
| **DS18B20** | 1-Wire | — | Temperatura −55°C a +125°C | Temperatura del suelo a 10 cm |
| **ADS1115** | I2C | 0x48 | ADC 16-bit, 4 canales, PGA | Sensores analógicos de precisión |

### Datos que Genera la Estación

```text
Archivo CSV continuo en MicroSD:
  timestamp, ax, ay, az, presion_hPa, temp_C, humedad_%, lux, temp_suelo_C

Servidor web local (acceso desde celular en la misma red WiFi):
  ├── Gráfico en tiempo real: a(t) triaxial
  ├── Espectrograma FFT waterfall (TFT ST7789)
  ├── Últimas 24h de presión y temperatura
  └── Log de eventos detectados

MQTT (broker en PC o Raspberry Pi):
  topic: geofisica/nodo1/accel → {ax, ay, az, timestamp}
  topic: geofisica/nodo1/ambiente → {presion, temp, humedad, lux}
  topic: geofisica/eventos → {tipo, magnitud_estimada, duracion}
```

### Algoritmo de Detección Sísmica — STA/LTA

El algoritmo estándar en sismología profesional compara el promedio de corto plazo con el de largo plazo de la señal:

$$R(t) = \frac{\text{STA}(t)}{\text{LTA}(t)} = \frac{\frac{1}{n_s}\sum_{i=t-n_s}^{t} a_i^2}{\frac{1}{n_l}\sum_{i=t-n_l}^{t} a_i^2}$$

Cuando $R(t) > \text{umbral}$ (típicamente 3–4), se declara un evento sísmico. El ESP32-S3 ejecuta esto en tiempo real a 800 Hz.

### FFT y Análisis Espectral

El ESP32-S3 N16R8 ejecuta FFT de 1024 puntos usando instrucciones DSP vectoriales del Xtensa LX7:

$$X(f) = \sum_{n=0}^{N-1} a(n) \cdot e^{-j2\pi fn/N}$$

Rango de análisis: **0.5 Hz a 400 Hz**
- Ondas sísmicas de largo período: 0.5–2 Hz
- Ruido ambiental urbano: 1–10 Hz
- Vibraciones de tráfico: 10–30 Hz
- Resonancias de estructuras: 5–15 Hz

### Mini-Resistivímetro Geofísico (Tomografía 2D ERT / SEV — Arreglo de Wenner + `RES2DINV`)

Usando dos pines de inyección con inversión de polaridad (`D25` y `D26` en el DevKit 30P), una resistencia *shunt* de precisión ($R_s = 1\text{ k}\Omega$) y el conversor **`ADS1115` (16-bit en modo diferencial)**, la estación funciona como un resistivímetro tetrapolar ($A\text{-}M\text{-}N\text{-}B$):

$$\rho_a = 2 \pi \, a \, \frac{\Delta V_{MN}}{I}, \qquad I = \frac{\Delta V_{\text{shunt}}}{R_s}$$

- **Electrodos externos ($A, B$):** Inyectan pulsos bipolares cortos ($+3.3\text{ V} / -3.3\text{ V}$ durante $20\text{ ms}$) para anular el potencial espontáneo ($\text{SP}$) y evitar la polarización electrolítica del suelo.
- **Electrodos internos ($M, N$ a separación $a$):** El canal diferencial `A0-A1` del `ADS1115` mide $\Delta V_{MN}$ con alta impedancia de entrada, mientras `A2-A3` mide la caída en $R_s$ para obtener la corriente real $I$.
- **Inversión Geofísica 2D (`RES2DINV` / `ResIPy` / `pyGIMLi`):**
  El servidor web del ESP32 exporta directamente las mediciones $(x_{\text{centro}}, a, \rho_a)$ en el formato estándar `.dat` de **`RES2DINV`** (código de arreglo `1` = Wenner) para generar la **Pseudosección de Resistividad Aparente** y el **Modelo Invertido de Resistividad Real 2D** (también compatible con software libre científico `ResIPy` y `pyGIMLi` en Python).

#### 🧪 Modelo Físico Análogo en Terrario / Sandbox (Laboratorio de Mesa ERT)
Para validar el resistivímetro antes de salir a terreno, se utiliza el montaje canónico de modelado físico análogo (*sandbox modeling*):
- **Recipiente:** Caja plástica organizadora o terrario de vidrio/acrílico de $40\text{--}60\text{ cm}$ de largo $\times 20\text{--}25\text{ cm}$ de ancho $\times 15\text{ cm}$ de profundidad.
- **Matriz de fondo:** Arena fina humedecida uniformemente con agua ligeramente mineralizada para establecer una resistividad de fondo homogénea ($\rho_{\text{matriz}} \approx 30\text{--}80\ \Omega\cdot\text{m}$).
- **Electrodos de superficie:** 16 a 24 clavos delgados de acero inoxidable clavados en línea en la arena, con separación básica $a = 2.0\text{ cm}$ o $2.5\text{ cm}$ (secuencia multi-nivel $n = 1, 2, 3, 4, 5$).
- **Anomalías enterradas controladas:**
  1. *Anomalía Resistiva ($\rho \gg \rho_{\text{matriz}}$):* Bloque de madera seca, tubo de PVC cerrado o roca ígnea densa (simula cavidades, fallas secas o túneles).
  2. *Anomalía Conductora ($\rho \ll \rho_{\text{matriz}}$):* Esponja saturada con salmuera concentrada o barra metálica (simula acuífero salino o cuerpo mineralizado de sulfuros).
- **Resultado:** La pseudosección generada por el ESP32 se invierte en `RES2DINV` / `pyGIMLi`, reconstruyendo la forma, posición horizontal $x$ y profundidad $z$ de las anomalías enterradas.

---

### 🌐 Métodos Electromagnéticos Pasivos: Magnetotelúrica (MT) y Audio-MT (AMT)

La instrumentación de campo magnético y potencial eléctrico del monorepo se conecta directamente con los métodos electromagnéticos pasivos profundos:

#### 1. Fundamento Físico (Ley de Inducción de Faraday)
$$\nabla \times \vec{E} = -\frac{\partial \vec{B}}{\partial t}$$
Las variaciones temporales naturales del campo magnético terrestre $\vec{B}(t)$ inducen corrientes eléctricas en el subsuelo (corrientes telúricas $\vec{E}(t)$). Midiendo simultáneamente ambas variables ortogonales ($E_x, H_y$), se calcula el tensor de impedancia $Z_{xy}(\omega) = E_x / H_y$ y la resistividad aparente:
$$\rho_a(\omega) = \frac{1}{\mu_0 \omega} |Z_{xy}(\omega)|^2$$

#### 2. Fuentes Naturales y Espectro de Frecuencias
- **Magnetotelúrica (MT, $f < 1\text{ Hz}$, períodos de $1\text{ s}$ a $1000\text{ s}$):**
  - *Fuente:* Interacción del viento solar con la magnetosfera y tormentas geomagnéticas (las mismas variaciones que detecta el magnetómetro triaxial digital).
  - *Profundidad:* Exploración cortical y mantélica profunda ($1\text{ km}$ hasta $> 50\text{ km}$).
- **Audio-Magnetotelúrica (AMT, $f = 1\text{ Hz}$ a $10\text{ kHz}$):**
  - *Fuente:* Actividad global de rayos y tormentas eléctricas atrapadas en la cavidad guía de onda Tierra-Ionosfera (*Resonancias de Schumann* a $7.8\text{ Hz}$, $14\text{ Hz}$, etc.).
  - *Profundidad:* Hidrogeología y prospección minera somera ($50\text{ m}$ a $1000\text{ m}$).

#### 3. Diferencia de Escala Experimental: ¿Por qué ERT sí funciona en terrario y MT no?
La profundidad de penetración de una onda electromagnética está regida por la **profundidad pelicular (*skin depth* $\delta$)**:
$$\delta \approx 503 \sqrt{\frac{\rho}{f}} \quad [\text{metros}]$$
- En un terrario de $50\text{ cm}$ ($0.5\text{ m}$) con $\rho = 50\ \Omega\cdot\text{m}$, para que la onda tenga una escala comparable al terrario se requeriría una frecuencia $f \approx 50\text{ MHz}$ (rango de microondas / Georradar GPR, no MT).
- A las frecuencias naturales de MT ($0.01\text{--}100\text{ Hz}$), la longitud de onda atraviesa el terrario sin generar corrientes telúricas localmente confinadas, y el ruido de $50\text{ Hz}$ de la red eléctrica residencial saturaría la señal.
- **Conclusión de laboratorio:** ERT (fuente artificial de corriente inyectada por electrodos) es el método perfecto para escala reducida de mesa / terrario, mientras que MT/AMT es estrictamente un método para campo abierto en terreno.

### Experimentos Posibles

1. **Registrar un sismo real** y comparar timestamp y amplitud con el boletín del CSN Chile
2. **Medir diferencia de llegada** de ondas P entre Nodo 1 y Nodo 2 → estimar distancia al epicentro
3. **Correlacionar caída de presión** (BMP280) con sismicidad superficial en el tiempo
4. **Comparar espectro de día vs noche** → efecto del ruido urbano (tráfico) en la estación
5. **Detectar microtemblores** continuos de fondo (ruido sísmico ambiental)
6. **Medir variación de inclinación** del suelo con MPU6050 durante semanas
7. **Sondeo Eléctrico Vertical (SEV) y Tomografía de Resistividad ($\rho_a$):** Medir variaciones de resistividad del suelo ($\Omega\cdot\text{m}$) por humedad, salinidad, estratigrafía somera o detección de anomalías enterradas (arreglo de Wenner con 4 electrodos + `ADS1115`)

### Conexiones Principales (Nodo Central)

```text
ESP32-S3 N16R8 (pines revisados — mapa completo en la sección 0)
├── I2C (SDA=GPIO8, SCL=GPIO9)
│   ├── MPU6050      — addr 0x68
│   ├── AHT20        — addr 0x38
│   ├── BMP280       — addr 0x77
│   ├── BH1750       — addr 0x23
│   ├── ADS1115      — addr 0x48
│   └── OLED SSD1306 — addr 0x3C
├── SPI-A (SCK=GPIO12, MOSI=GPIO11, MISO=GPIO13)
│   ├── ADXL345 : CS=GPIO10
│   └── MicroSD : CS=GPIO14
├── SPI-B dedicado TFT ST7789 (pendiente): SCK=GPIO18, MOSI=GPIO17, CS=GPIO16, DC=GPIO15, RST=GPIO21
└── DS18B20 : 1-Wire → GPIO4 + resistencia 4.7 kΩ a 3.3V

Nota: ADXL345 y MicroSD comparten el bus SPI-A con distinto CS. El TFT usa su propio bus.
      Todos los sensores I2C comparten el mismo par SDA/SCL.
      Evitar GPIO26–37 (flash/PSRAM), GPIO19/20 (USB) y GPIO0/3/45/46 (strapping).
```

---

## 2. 🏠 Monitor Ambiental Multi-Zona

### Objetivo
Monitorear temperatura, humedad, calidad del aire (CO₂), luz y movimiento en 3 zonas de una vivienda. Generar alertas automáticas por Telegram, visualizar un dashboard en tiempo real desde el celular y registrar histórico para detectar patrones.

> 💡 Equivalente comercial: **Netatmo Smart Home Weather Station** (US$180) + sensores adicionales. Este proyecto cuesta ~$40 USD en componentes por los 3 nodos.

### Arquitectura del Sistema

```text
┌──────────────────────────────────────────────────────────────┐
│     MONITOR AMBIENTAL — HUB + 2 NODOS REMOTOS (3 ZONAS)      │
├──────────────────────┬───────────────────┬───────────────────┤
│  HUB (+ SALÓN)       │  NODO DORMITORIO  │  NODO EXTERIOR    │
│  ESP32-S3 N16R8      │  ESP32-C3         │  ESP32-C3         │
├──────────────────────┼───────────────────┼───────────────────┤
│  AHT20+BMP280        │  AHT20+BMP280     │  AHT20+BMP280     │
│  MQ-135 (ADS1115)    │  PIR              │  BH1750           │
│  BH1750              │  (MQ-135 opcional)│  Sensor lluvia    │
│  PIR                 │                   │                   │
│  OLED + MicroSD      │                   │                   │
│  TFT ST7789 (pend.)  │                   │                   │
│  Buzzer + Mic INMP441│                   │                   │
│  Web Server + MQTT   │                   │                   │
└──────────────────────┴───────────────────┴───────────────────┘
            │                  │                  │
            └──────────────────┴──────────────────┘
                        WiFi 2.4 GHz
              MQTT QoS 1 → broker Mosquitto (PC / RPi)
```

### Sensores por Nodo — Detalle

#### Hub Central (ESP32-S3 N16R8)
- Rol: estación sísmica central + zona Salón + cliente MQTT (el broker Mosquitto corre en PC/RPi) + servidor web + display resumen + inferencia TinyML
- TFT ST7789 / OLED: muestra resumen de los nodos y espectrograma
- MicroSD: log histórico largo plazo de todos los nodos
- Micrófono INMP441 + Buzzer pasivo: control por voz offline y alarma acústica

#### Salón (sensores conectados al Hub S3, ya no es un nodo aparte)
| Sensor | Mide | Umbral de alerta |
|---|---|---|
| **AHT20 + BMP280** | Temperatura + Humedad + Presión (I2C) | Humedad > 70% → riesgo moho |
| **MQ-135** | CO₂ + VOC (calidad aire vía ADS1115) | CO₂ > 1000 ppm → ventila |
| **BH1750** | Lux (iluminancia I2C) | < 300 lux en horario laboral |
| **PIR HC-SR501** | Presencia/movimiento | Sin movimiento > 4h → alerta |
| **OLED 0.96"** | Display local I2C | Lecturas instantáneas del salón |

#### Nodo 2 — Dormitorio
| Sensor | Mide | Uso |
|---|---|---|
| **AHT20 + BMP280** | Temperatura + Humedad (I2C) | Calidad del sueño: 18–22°C ideal |
| **BH1750** | Luz nocturna | Detecta luz artificial después de medianoche. *Opcional: solo hay 2 BH1750 (Hub y Exterior)* |
| **PIR HC-SR501** | Presencia | ¿Hay alguien durmiendo? |

#### Nodo 3 — Exterior
| Sensor | Mide | Uso |
|---|---|---|
| **AHT20 + BMP280** | Presión + Temp + Humedad (I2C) | Predicción de lluvia (caída barométrica) |
| **BH1750** | Radiación solar (lux) | Cuánto sol hay en el día |
| **Sensor lluvia** | Precipitación (gotas) | Alerta inmediata para cerrar ventanas |

### Lógica de Alertas

```python
# Reglas configurables (pseudocódigo)
if CO2_salon > 1000:        → Telegram/Buzzer: "Ventila el salón (CO2 alto)"
if humedad_dormi > 70:      → Telegram: "Riesgo de moho en dormitorio"
if temp_exterior < 5:       → Telegram: "Helada probable esta noche"
if delta_presion_3h < -5:   → Telegram: "Lluvia en camino (presión cayendo)"
if lluvia_detectada:        → Telegram/Buzzer: "¡Está lloviendo! ¿Ventanas abiertas?"
if sin_movimiento_salon > 4h: → Telegram: "Sin actividad detectada en salón"
if lux_dormi > 50 and hora > 23: → Telegram: "Hay luz encendida en dormitorio"   # requiere un 3.er BH1750 (opcional)
```

### Datos que Genera el Sistema

```text
Por nodo cada 30 segundos → MQTT → Hub Central:
  geofisica/salon/ambiente  → {temp, hum, co2, lux, pir, ts}
  geofisica/dormi/ambiente  → {temp, hum, lux, pir, ts}
  geofisica/ext/ambiente    → {presion, temp, hum, lux, lluvia, ts}

Dashboard web (acceso desde celular):
  ├── Tabla en tiempo real: todos los nodos
  ├── Gráfico histórico 7 días por variable
  ├── Mapa de calor: hora del día vs temperatura
  ├── Indicadores de calidad de aire (semáforo)
  └── Histórico de alertas generadas

MicroSD Hub (descargable por WiFi):
  ambiente_log_YYYYMMDD.csv
```

### Conexiones — Salón (sensores en el Hub ESP32-S3)

```text
ESP32-S3 N16R8 (Hub) — periféricos del salón
├── AHT20+BMP280: I2C → GPIO8(SDA), GPIO9(SCL) — addrs 0x38 (AHT20) / 0x77 (BMP280)
├── MQ-135      : Salida analógica → divisor resistivo → Canal A0 del ADS1115 (16-bit)
├── ADS1115     : I2C → GPIO8(SDA), GPIO9(SCL) — addr 0x48 (alimentado a 3.3V)
├── BH1750      : I2C → GPIO8(SDA), GPIO9(SCL) — addr 0x23
├── OLED 0.96"  : I2C → GPIO8(SDA), GPIO9(SCL) — addr 0x3C
├── PIR HC-SR501: GPIO42 (digital, HIGH = movimiento)
└── Buzzer      : GPIO41 (PWM para alerta local)

Nota: Todos los módulos I2C comparten la misma línea SDA/SCL con diferentes direcciones.
      MQ-135 requiere calentamiento previo de 24-48h antes de calibrar.
```

### Conexiones — Nodo remoto sísmico (ESP32 DevKit 30P)

```text
ESP32 DevKit 30P
├── ADXL345 : SPI → GPIO18(SCK), GPIO19(MISO), GPIO23(MOSI), GPIO5(CS)
├── MicroSD : SPI → mismo bus, CS = GPIO4
├── OLED    : I2C → GPIO21(SDA), GPIO22(SCL) — addr 0x3C
└── Buzzer  : GPIO25
```

### Conexiones — Nodos Dormitorio y Exterior (ESP32-C3 Super Mini)

```text
Dormitorio: AHT20+BMP280 + PIR      → I2C SDA=GPIO4, SCL=GPIO5 · PIR=GPIO3
Exterior  : AHT20+BMP280 + BH1750   → I2C SDA=GPIO4, SCL=GPIO5
            Sensor de lluvia        → DO=GPIO3, AO=GPIO2
```

---

## 3. 📦 Hardware Consolidado Real (conciliado con facturas)

> **Estado Oficial (conciliado con las facturas el 10-10-2026):** compras pagadas en **Mercado Libre ($130.039)**, **MechatronicStore ($70.760)** y **AFEL ($92.970)**. Inversión total real: **$293.769 CLP**. **Pendientes:** pantalla TFT ST7789 y 2 tarjetas MicroSD.

### Microcontroladores

| ID | Componente | Cant. | Usado en | Tienda | Estado | Notas |
|---|---|:---:|---|:---:|:---:|---|
| M1 | ESP32-S3 N16R8 (44 pines, USB-C) | 1 | 🌋🏠 Hub central ambos | ML | ✅ Comprado | 8 MB PSRAM — cerebro principal sismos, web, IA y audio ($13.434) |
| M2 | ESP32 DevKit 30P USB-C (Vshop) | 1 | 🌋 Nodo remoto sísmico | ML | ✅ Comprado | Plataforma estándar ($7.160) |
| M3 | ESP32-C3 Super Mini USB-C | 2 | 🏠 Dormitorio / Exterior | ML | ✅ Comprado | Pack de 2 placas ($11.634) |
| M4 | Seeed Studio XIAO ESP32-S3 Sense | 1 | 🌋 Cámara cielo / IA visión | Mechatronic | ✅ Comprado | Cámara OV2640, micrófono PDM y ranura SD integrados ($22.990) |

### Sensores Sísmicos (Geofísica)

| ID | Componente | Cant. | Nivel | Tienda | Estado | Notas |
|---|---|:---:|:---:|:---:|:---:|---|
| S1 | ADXL345 GY-291 (3 ejes) | 2 | 1 | Mechatronic | ✅ Comprado | Uno por nodo sísmico — bus SPI alta velocidad ($6.980 total) |
| S2 | MPU6050 GY-521 (IMU 6 ejes) | 1 | 1 | AFEL | ✅ Comprado | Inclinómetro de terreno ($4.000) |
| S3 | DS18B20 Waterproof 1 metro | 1 | 3 | AFEL | ✅ Comprado | Temperatura de suelo a 10 cm ($2.500) |

### Sensores Ambientales y Detección (compartidos)

| ID | Componente | Cant. | Usado en | Tienda | Estado | Notas |
|---|---|:---:|---|:---:|:---:|---|
| A1 | AHT20 + BMP280 (I2C) | 3 | 🌋🏠 Hub + 🏠 Dormitorio + 🏠 Exterior | AFEL | ✅ Comprado | *Nota: Compradas 3 uds; el nodo remoto sísmico no lleva este sensor* ($9.000) |
| A3 | BH1750 Gy-302 (Luz lux) | 2 | 🌋 Hub / 🏠 Exterior | Mechatronic | ✅ Comprado | *Nota: Compradas 2 uds (sugerido inicial 3; suficiente para Hub y Exterior)* ($7.980) |
| A4 | MQ-135 (Calidad aire/CO₂) | 2 | 🏠 Salón (Hub) + 2.º opcional Dormitorio | AFEL | ✅ Comprado | Requiere calentamiento previo de 24-48h ($6.800) |
| A5 | PIR HC-SR501 | 2 | 🏠 Salón + Dormitorio | AFEL | ✅ Comprado | Detección de presencia infrarroja ($4.600) |
| A6 | Sensor de Gotas de Lluvia | 1 | 🏠 Nodo exterior | Mechatronic | ✅ Comprado | Detección inmediata de precipitación ($1.500) |
| A7 | VL53L0X ToF Láser | 1 | 🏠 Nivel / Distancia | Mechatronic | ✅ Comprado | Medición de distancia por tiempo de vuelo láser ($6.490) |
| A8 | Radar Microondas RCWL-0516 | 1 | 🏠 Presencia invisible / Robot | Mechatronic | ✅ Comprado | Doppler 360° que atraviesa plástico/madera ($2.290) |
| A9 | Infrarrojo HW-201 | 1 | 🤖 Robot obstáculos | Mechatronic | ✅ Comprado | Detección de proximidad cercana ($1.490) |
| A10 | Sensor Ultrasónico HC-SR04 | 1 | 🤖 Robot distancia / Nivel | AFEL | ✅ Comprado | Medición de distancia por eco ultrasónico ($1.500) |

### Adquisición, Almacenamiento, Audio y Display

| ID | Componente | Cant. | Nivel | Tienda | Estado | Notas |
|---|---|:---:|:---:|:---:|:---:|---|
| D1 | ADS1115 ADC 16-bit | 1 | 2 | AFEL | ✅ Comprado | Conversor analógico-digital 16-bit para MQ-135 ($5.000) |
| D2 | Módulo MicroSD SPI | 2 | 1 | AFEL | ✅ Comprado | *Nota: Compradas 2 uds (sugerido inicial 3; cubre Hub S3 y DevKit; XIAO ya trae ranura)* ($5.000) |
| D3 | MicroSD Maxell 16GB Clase 10 | 1 | 1 | ML | ✅ Comprado | *Nota: Comprada 1 ud inicial (sugerido inicial 3-4; lista para arrancar datasets)* ($7.990) |
| I7 | Micrófono I2S INMP441 | 1 | 2 | AFEL | ✅ Comprado | Entrada de voz nítida para el ESP32-S3 / DeepSeek ($5.990) |
| AU1 | Amplificador I2S MAX98357A 3W | 1 | 2 | ML | ✅ Comprado | Salida de audio digital I2S para voz humana del robot / alertas ($5.890) |
| AU2 | Mini Parlante 3W 4Ω | 1 | 2 | AFEL | ✅ Comprado | Altavoz de alta fidelidad para voz sintetizada ($3.000) |
| V1 | OLED SSD1306 0.96" I2C | 2 | 2 | AFEL | ✅ Comprado | 1 para el Hub + 1 para el nodo remoto sísmico o cara de Mascota Robot ($9.000) |
| V2 | Pantalla TFT ST7789 (SPI) | 1 | 2 | Mechatronic | ⏳ Pendiente | Espectrograma FFT y dashboard del Hub (ref. estimada $9.000) |
| D6 | MicroSD 16-32GB Clase 10 (adicionales) | 2 | 1 | ML | ⏳ Pendiente | Una para el nodo remoto y otra para el XIAO (ref. $7.990 c/u) |

### Conectividad, Cables y Organización

| ID | Componente | Cant. | Nivel | Tienda | Estado | Notas |
|---|---|:---:|:---:|:---:|:---:|---|
| U1 | Xiaomi Cable USB-C Datos 1m | 1 | 1 | ML | ✅ Comprado | *Nota: Comprado 1 cable datos de alta velocidad Xiaomi (sugerido 2-3)* ($6.990) |
| ORG1 | Maletín Organizador Plástico | 1 | 1 | ML | ✅ Comprado | Caja con compartimientos para resguardo ordenado ($5.661) |
| C1 | Cables Dupont (Packs x60) | 2 packs | 1 | AFEL | ✅ Comprado | *✅ Comprados 2 packs de 60 (120 cables M-M, H-H, M-H)* ($5.000) |
| C4 | Protoboard 830 puntos GL-12 | 2 | 1 | Mechatronic | ✅ Comprado | *Nota: Compradas 2 uds (sugerido inicial 3; suficiente para desarrollo)* ($7.580) |
| K3 | Mini Protoboard 400 puntos | 1 | 2 | AFEL | ✅ Comprado | Ideal para nodos compactos ESP32-C3 ($1.500) |

### Componentes Pasivos y Actuadores

| ID | Componente | Cant. | Nivel | Tienda | Estado | Notas |
|---|---|:---:|:---:|:---:|:---:|---|
| P0 | Kit Componentes Básicos AFEL | 1 kit | 1 | AFEL | ✅ Comprado | 600 resist., 340 cerámicos, 120 electrolíticos, LEDs, transistores ($17.000) |
| P4 | Push Button 4 pines (Packs x5) | 2 packs | 2 | AFEL | ✅ Comprado | 10 pulsadores para reset y configuración ($1.000) |
| P5 | Potenciómetro WH148 B10K 10kΩ | 3 | 2 | AFEL | ✅ Comprado | *✅ Compradas 3 uds (sugerido inicial 2; calibración en vivo)* ($1.500) |
| B1 | Buzzer Pasivo Solo | 2 | 1 | AFEL | ✅ Comprado | Alarmas acústicas y beeps locales ($600) |
| M5 | Micro Servomotor SG90 9g | 2 | 2 | AFEL | ✅ Comprado | Movimiento para patas de robot o indicador analógico de aguja ($3.980) |

### Herramientas y Consumibles de Laboratorio

| ID | Herramienta | Nivel | Tienda | Estado | Notas |
|---|---|:---:|:---:|:---:|---|
| H1 | Cautín LEFAVOR Digital 80W Rojo + kit 6 | 1 | ML | ✅ Comprado | Regulable digital con pantalla, soporte y puntas ($22.912) |
| H4 | Multímetro Digital AstroAI 6000 Cuentas | 1 | ML | ✅ Comprado | Auto-rango profesional para voltaje, continuidad, capacitancia ($29.611) |
| H9 | Lupa Brazo Soporte Pinzas Luz LED (Tercera Mano) | 1 | ML | ✅ Comprado | Soporte articulado para soldadura de sensores ($13.890) |
| H5 | Pinzas de Precisión HARDEN ×4 | 1 | Mechatronic | ✅ Comprado | Antimagnéticas para sujeción SMD ($3.490) |
| H7 | Alicate Cortador al Ras | 1 | AFEL | ✅ Comprado | Corte limpio de pines y cables ($2.000) |
| H3 | Pasta para soldar ZD-170 (100 gr) | 1 | Mechatronic | ✅ Comprado | Flux para soldadura limpia ($4.990) |
| H10 | Malla Desoldadora 2mm 1.5m | 2 | Mechatronic | ✅ Comprado | Extracción limpia de estaño ($2.190) |
| K1 | Alcohol Isopropílico 300cc spray | 1 | Mechatronic | ✅ Comprado | Limpieza de PCBs ($3.290) |
| K2 | 4x Placas PCB Soldables Doble Cara | 2 | ML | ✅ Comprado | Montaje definitivo de circuitos ($4.867) |

---

### 💳 Resumen Financiero Consolidado

| Tienda | Monto Real Pagado | Envíos / Descuentos | Estado |
|---|:---:|---|:---:|
| **Mercado Libre** | **$130.039 CLP** | Envíos gratis Full en ambos paquetes | 100% Pagado (4 paquetes entregados, 2 en camino) |
| **MechatronicStore** | **$70.760 CLP** | Envío gratis Bluexpress (-$500 MechaCoins) | 100% Pagado (Pedido #717057, Completado) |
| **AFEL** | **$92.970 CLP** | $88.970 en productos + $4.000 de envío | 100% Pagado (Pedido Confirmado) |
| **TOTAL GENERAL** | **$293.769 CLP** | **Laboratorio casi completo** | **Pendientes: TFT ST7789, 2× MicroSD** |

---

## 4. 🗓️ Fases de Implementación

```text
FASE 1 — Semana 1-2: Setup y tests básicos
  ├── [✅ Completado] Instalar Arduino IDE + soporte ESP32 (boards manager + arduino-cli)
  ├── [✅ Completado] Scanner I2C: verificar que todos los sensores responden (0x23, 0x38, 0x3C, 0x77)
  ├── [⏳ Pendiente]  Test ADXL345: graficar ax/ay/az en Serial Plotter al agitar
  ├── [✅ Completado] Test AHT20 + BMP280: leer temperatura, humedad y presión (validado vs DMC Quinta Normal)
  └── [✅ Completado] Test BH1750: comparar lux al tapar y destapar el sensor (0 a >60 lux)

FASE 2 — Semana 3-4: Nodo central funcionando
  ├── ADXL345 a 800 Hz con buffer circular en PSRAM del ESP32-S3
  ├── FFT básica: mostrar espectro en OLED (barras)
  ├── Log CSV a MicroSD: timestamp + todos los sensores
  ├── [✅ Completado] Servidor web simple: ver datos desde celular en red local (auto-refresh 3s)
  └── Test MQ-135: calibrar al aire libre después de 48h de calentamiento

FASE 3 — Semana 5-6: Red WiFi y MQTT
  ├── Instalar Mosquitto broker en PC (Windows) o Raspberry Pi
  ├── Cada ESP32 publica datos por MQTT cada 30 segundos
  ├── Hub central (ESP32-S3) se suscribe a todos los topics
  ├── TFT ST7789 (pendiente de compra): dashboard con los 3 nodos ambientales
  └── TFT: espectrograma waterfall sísmico en tiempo real

FASE 4 — Semana 7-8: Alertas y detección de eventos
  ├── Algoritmo STA/LTA implementado en ESP32-S3
  ├── Alertas Telegram para eventos sísmicos > umbral
  ├── Alertas ambientales: CO₂, humedad, lluvia, temp
  ├── Descarga remota del CSV vía WiFi (servidor de archivos)
  └── Primer registro de sismo real comparado con CSN

FASE 5 — Semana 9+: Optimización y expansión
  ├── Calibración cruzada de sensores (comparar entre nodos)
  ├── Dashboard web con Chart.js para gráficos históricos
  ├── Agregar XIAO ESP32-S3 Sense: foto horaria del cielo
  ├── Análisis estadístico de datos acumulados (Python/Jupyter)
  └── Contribuir datos a sensor.community o raspberryshake.org
```

---

## 5. 📊 Comparación con Equipos Comerciales

| Función | Equipo Comercial | Precio | Este Proyecto | Precio |
|---|---|:---:|---|:---:|
| Sismógrafo 1 eje | Raspberry Shake 1D | US\$300 | ESP32-S3 + ADXL345 | ~\$15.000 CLP |
| Sismógrafo 3 ejes | Raspberry Shake 3D | US\$650 | ESP32-S3 + ADXL345 | ~\$15.000 CLP |
| Estación meteo interior | Netatmo Smart | US\$110 | Nodo AHT20+BMP280+MQ-135 | ~\$12.000 CLP |
| Estación meteo exterior | Netatmo Outdoor | US\$70 | Nodo AHT20+BMP280+BH1750 | ~\$8.000 CLP |
| Monitor CO₂ | Aranet4 | US\$250 | MQ-135 + ADS1115 | ~\$4.500 CLP |
| Sistema completo 3 nodos | Airthings Wave | US\$500+ | 3 nodos ambientales | ~\$45.000 CLP |

### Comunidades de Datos Abiertos

- 🌐 **[sensor.community](https://sensor.community)** — Red global de monitores ambientales open source
- 🌐 **[raspberryshake.org/stationview](https://raspberryshake.org/stationview)** — Mapa mundial de sismógrafos ciudadanos
- 🌐 **[CSN Chile](https://www.csn.uchile.cl/sismicidad/catalogo-sismico/)** — Catálogo sísmico para comparar tus registros
- 🌐 **[IRIS Earthquake Browser](https://ds.iris.edu/ieb/)** — Base de datos mundial de sismos

### Posibilidades de Expansión Futura

| Expansión | Costo extra | Beneficio |
|---|---|---|
| **GPS NEO-6M** | ~\$5.000 | Sincronización de tiempo precisa para triangulación |
| **LoRa SX1278** | ~\$8.000 | Nodos hasta 10 km sin WiFi |
| **Panel solar 6V + TP4056** | ~\$8.000 | Nodo exterior completamente autónomo |
| **AMG8833 Termocámara** | ~\$25.000 | Mapa de calor térmico 8×8 px de habitaciones |
| **Cámara termográfica MLX90640** | ~\$45.000 | Mapa de calor 32×24 px de alta resolución |
| **Fotodiodos PIN `BPW34` (×2) + OpAmp** | ~\$3.500 | Detector de partículas ionizantes / muones cósmicos en coincidencia |

---

## 6. ⚛️🛰️ Laboratorio de Física Moderna, Cuántica, Relatividad y Teledetección

> Aprovecha el hardware ya adquirido (**`ESP32 DevKit 30P`**, **`ADS1115` 16-bit**, **`Kit Componentes AFEL P0`**, **`XIAO ESP32-S3 Sense`**, **`BH1750`**, **`BMP280`** y **`HW-201 IR`**) para realizar experimentos cuantitativos de **Mecánica Cuántica, Relatividad Especial, Cosmología/Física Estadística y Teledetección Espectral**.

### 6.1 Mecánica Cuántica: Determinación Experimental de la Constante de Planck ($h$)

- **Hardware (100% en inventario):** `ESP32 DevKit 30P` (salida analógica `DAC` en `D25`), `ADS1115` (16-bit ADC), 1 resistencia de $330\text{ }\Omega$ y LEDs de distintos colores del Kit AFEL (`P0` + `HW-201`):
  - Infrarrojo ($\lambda \approx 940\text{ nm}$), Rojo ($\approx 625\text{ nm}$), Amarillo ($\approx 590\text{ nm}$), Verde ($\approx 565\text{ nm}$), Azul ($\approx 470\text{ nm}$).
- **Física:** En una juntura semiconductor $p\text{-}n$, la recombinación radiativa ocurre cuando la energía potencial eléctrica entregada al electrón ($e V_0$) iguala la brecha energética (*bandgap*) y emite un cuanto de luz (fotón) de energía $E = h \nu$:
  $$e V_0 = h \nu - \phi \quad \Longrightarrow \quad V_0(\nu) = \left(\frac{h}{e}\right)\nu - \frac{\phi}{e}, \qquad \nu = \frac{c}{\lambda}$$
- **Experimento:** El ESP32 barre automáticamente una rampa fina de voltaje con el `DAC` (`D25`) mientras el `ADS1115` mide con resolución de $0.125\text{ mV}$ el voltaje en el LED y la corriente en la resistencia hasta detectar el umbral cuántico de encendido $V_0$ de cada color. Ajustando por regresión lineal $V_0$ vs. $\nu = c/\lambda$, la pendiente multiplicada por $e = 1.602 \times 10^{-19}\text{ C}$ entrega experimentalmente la **Constante de Planck** ($h \approx 6.63 \times 10^{-34}\text{ J}\cdot\text{s}$).

### 6.2 Efecto Fotoeléctrico y Teledetección Espectral Activa (`NDVI` Proximal)

- **Hardware (100% en inventario):** LED Rojo ($625\text{ nm}$), emisor/fotodiodo Infrarrojo Cercano $\text{NIR}$ ($940\text{ nm}$ del módulo `HW-201`), `BH1750` y `ADS1115`.
- **Física (Cuántica + Geomática/Teledetección):**
  1. **Efecto Fotoeléctrico Selectivo:** Usando LEDs como fotodiodos en circuito abierto conectados al `ADS1115`, un LED de banda ancha solo genera fotocorriente cuando inciden fotones con frecuencia superior al umbral cuántico ($\nu \ge E_g / h$).
  2. **Radiómetro `NDVI` de Laboratorio:** Iluminando secuencialmente una muestra vegetal (hoja verde vigorosa vs. hoja seca vs. suelo) con pulsos de luz Roja ($625\text{ nm}$, absorbida por la clorofila) y luz $\text{NIR}$ ($940\text{ nm}$, fuertemente reflejada por la estructura celular del mesófilo), se mide la reflectancia bidireccional y se calcula el **Índice de Vegetación de Diferencia Normalizada**:
     $$\text{NDVI} = \frac{\rho_{\text{NIR}} - \rho_{\text{Red}}}{\rho_{\text{NIR}} + \rho_{\text{Red}}}$$
     replicando en laboratorio la física exacta de los sensores satelitales **Sentinel-2 (MSI)** y **Landsat 8/9 (OLI)**.

### 6.3 Relatividad Especial y Rayos Cósmicos: Detección de Muones Relativistas ($\tau = \gamma \tau_0$)

- **Hardware:** `XIAO ESP32-S3 Sense` (sensor CMOS `OV2640` cubierto herméticamente con cinta negra/papel aluminio en modo *Dark-Frame*, o añadiendo 2 fotodiodos PIN `BPW34`).
- **Física (Relatividad Especial de Einstein):**
  Los rayos cósmicos primarios chocan en la alta atmósfera ($H \approx 15\text{ km}$) produciendo **muones ($\mu^\pm$)** que viajan a $v \approx 0.998 c$. La vida media propia del muón en reposo es apenas $\tau_0 = 2.2\text{ }\mu\text{s}$.
  - **Sin Relatividad Especial (física clásica):** Antes de desintegrarse recorrerían como máximo $d_{\text{clásica}} = c \tau_0 \approx 660\text{ m}$ y jamás llegarían a la superficie terrestre.
  - **Con Dilatación Relativista del Tiempo:** Desde el marco de referencia del laboratorio, su vida media se dilata por el factor de Lorentz $\gamma$:
    $$\gamma = \frac{1}{\sqrt{1 - v^2/c^2}} \approx 15.8 \quad \Longrightarrow \quad d_{\text{relativista}} = \gamma v \tau_0 \approx 10.4\text{ km}$$
    permitiendo que un flujo de $\sim 1\text{ muón/cm}^2/\text{min}$ atraviese nuestro escritorio.
- **Experimento:** Capturando cuadros continuos en oscuridad absoluta con el sensor CMOS del `XIAO ESP32-S3` y filtrando el ruido térmico, se registran los destellos puntuales de píxeles de alta energía provocados por partículas ionizantes / rayos cósmicos secundarios y se analiza su distribución temporal de **Poisson** (principio del proyecto ciudadano **CRAYFIS**).

### 6.4 Física Estadística y Atmósfera Planetaria: Distribución de Boltzmann y Ley de Beer-Lambert

- **Hardware (100% en inventario):** `BMP280` + `AHT20` + `BH1750` + `ESP32-C3 Super Mini` portátil.
- **Experimentos:**
  1. **Perfil Barométrico de Boltzmann ($P(z) = P_0 e^{-m g z / k_B T}$):** Subiendo por un edificio o cerro con la estación portátil y registrando $(z, P, T)$, se ajusta $\ln(P/P_0)$ vs. $z$ para medir la **altura de escala atmosférica** $H = \frac{k_B T}{m g}$ y verificar la relación entre la masa molecular media del aire ($m$) y la **Constante de Boltzmann ($k_B$)**.
  2. **Extinción Atmosférica Solar (Ley de Beer-Lambert / Fotometría Astronómica):** Midiendo la iluminancia solar directa $I(\theta_z)$ con el `BH1750` a distintos ángulos cenitales solares $\theta_z$ desde el mediodía hasta el atardecer (donde la masa de aire óptica es $X = \sec \theta_z$), se ajusta $\ln I$ vs. $\sec \theta_z$ para obtener el **espesor óptico atmosférico ($\tau$)** de Santiago en ese día:
     $$I(\theta_z) = I_0 \, e^{-\tau \sec \theta_z}$$

---

> **v1.2** — Actualizado el 21-10-2026: Incorpora Tomografía de Resistividad Eléctrica 2D (`RES2DINV` / `ResIPy` / `pyGIMLi`) y la Sección 6 de **Laboratorio de Física Moderna (Constante de Planck $h$, Relatividad Especial / Rayos Cósmicos, Física Estadística de Boltzmann y Teledetección Espectral `NDVI`)**.
