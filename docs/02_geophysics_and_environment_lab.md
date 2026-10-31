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

### Mini-Resistivímetro Geofísico (Tomografía 2D ERT / SEV — Multi-Arreglo + `RES2DINV`)

Usando dos pines de inyección con inversión de polaridad (`D25` y `D26` en el DevKit 30P), una resistencia *shunt* de precisión ($R_s = 1\text{ k}\Omega$) y el conversor **`ADS1115` (16-bit en modo diferencial)**, la estación funciona como un resistivímetro tetrapolar configurable para cualquier arreglo estándar de 4 electrodos ($A\text{-}M\text{-}N\text{-}B$):

$$\rho_a = K \cdot \frac{\Delta V_{MN}}{I}, \qquad I = \frac{\Delta V_{\text{shunt}}}{R_s}$$

Donde el **factor geométrico $K$** general en un semi-espacio homogéneo e isótropo se deduce a partir de la superposición de potenciales puntuales de inyección ($A$) y retorno ($B$):

$$K = \frac{2\pi}{\left(\frac{1}{r_{AM}} - \frac{1}{r_{BM}} - \frac{1}{r_{AN}} + \frac{1}{r_{BN}}\right)}$$

- **Electrodos de corriente ($A, B$):** Inyectan pulsos bipolares conmutados ($+3.3\text{ V} \to -3.3\text{ V} \to \text{OFF}$ durante $20\text{--}50\text{ ms}$) para:
  1. Anular el **Potencial Espontáneo ($SP$)** y ruidos telúricos continuos ($V_{\text{medido}} = \frac{V^+ - V^-}{2}$).
  2. Evitar la **polarización electrolítica** y pasivación química de los electrodos en contacto con el suelo húmedo.
- **Electrodos de potencial ($M, N$):** El canal diferencial `A0-A1` del `ADS1115` mide $\Delta V_{MN}$ con alta impedancia ($> 6\text{ M}\Omega$) y ganancia programable (PGA hasta $\pm 0.256\text{ V}$ con resolución de $7.8125\ \mu\text{V}/\text{LSB}$), mientras `A2-A3` mide la caída en $R_s$ para calcular la corriente real inyectada $I$.

---

#### 📐 Configuraciones de Electrodos Soportadas

El mismo hardware de adquisición adquiere datos para cualquiera de las siguientes geometrías de prospección:

```text
1. Wenner-Alpha (Máxima SNR, capas horizontales)
   A ────── a ────── M ────── a ────── N ────── a ────── B
  (I+)              (V+)              (V-)              (I-)

2. Dipolo-Dipolo (Máxima resolución horizontal, anomalías verticales)
   A ─── a ─── B ··········· (n · a) ··········· M ─── a ─── N
  (I+)        (I-)                              (V+)        (V-)

3. Schlumberger (Sondeo Eléctrico Vertical SEV 1D)
   A ◀────────── s ──────────▶ M ── b ── N ◀────────── s ──────────▶ B
  (I+)                        (V+)     (V-)                         (I-)

4. Wenner-Schlumberger (Híbrido 2D multielectrodo)
   A ──── (n·a) ──── M ──── a ──── N ──── (n·a) ──── B
  (I+)              (V+)          (V-)              (I-)

5. Polo-Dipolo (B remoto al "infinito")
   A ─── (n·a) ─── M ─── a ─── N          ········· [B al infinito]
  (I+)            (V+)        (V-)                  (I-)
```

| Configuración | Factor Geométrico $K$ | Profundidad Mediana ($z_e$) | Código `RES2DINV` | Ventaja Principal |
|---|:---:|:---:|:---:|---|
| **Wenner ($\alpha$)** | $2\pi a$ | $z_e \approx 0.519 a \approx \frac{L}{6}$ | `1` | Máxima relación señal/ruido; ideal para bajo voltaje (3.3V/5V) y estratificación horizontal |
| **Dipolo-Dipolo** | $\pi n(n+1)(n+2)a$ | $z_e \approx 0.416 \sqrt{n(n+1)} a$ | `3` | Mejor resolución de bordes verticales (fallas, cavidades, tuberías, raíces) |
| **Schlumberger** | $\frac{\pi (s^2 - b^2/4)}{b}$ | Depende de $s = AB/2$ | `—` (SEV) / `7` | Solo se desplazan los electrodos exteriores $A$ y $B$ para profundizar |
| **Wenner-Schlumberger** | $\pi n(n+1)a$ | $z_e \approx 0.52 \sqrt{n(n+1)} a$ | `7` | Equilibrio óptimo entre sensibilidad horizontal y vertical en tomografía 2D |
| **Polo-Dipolo** | $2\pi n(n+1)a$ | $z_e \approx 0.65 \sqrt{n(n+1)} a$ | `6` | Mayor penetración relativa con menos longitud de cable activo |

---

#### ⚡ Física de la Profundidad de Investigación vs. Voltaje de Inyección

Un principio medular de la geofísica eléctrica es:

> **La profundidad de investigación ($z_e$) depende de la GEOMETRÍA del tendido de electrodos, mientras que el VOLTAJE determina la relación SEÑAL / RUIDO (el límite de separación máxima antes de perder la señal en el ruido ambiental).**

1. **Dependencia geométrica ($z_e \approx 0.52 a$ en Wenner):**  
   Al separar los electrodos $a$, las líneas de corriente penetran más hondo en el semiespacio. Para alcanzar $1.0\text{ m}$ de profundidad, se requiere un espaciamiento $a \approx 2.0\text{ m}$ (línea total $L = 3a = 6.0\text{ m}$).
2. **Límite del voltaje (3.3V / 5V vs. 200V comerciales):**  
   Al aumentar $a$, la señal de potencial decae con $\frac{\rho I}{2\pi a}$ y la corriente está limitada por la resistencia de contacto de las picas ($I = \frac{V_{\text{inj}}}{R_{\text{contacto}} + R_s}$).
   * **Piso de ruido ambiental:** En campo típico existe un ruido de fondo de $\sim 0.5\text{--}1.5\text{ mV}$ (telúrico, electroquímico y 50 Hz).
   * **Con 3.3V / 5V en suelo húmedo / jardín ($\rho \approx 60\ \Omega\cdot\text{m}$, $R_{\text{contacto}} \approx 1.5\text{ k}\Omega$):**  
     $I \approx 1.5\text{--}2.0\text{ mA}$. Con $a = 2.0\text{ m}$ ($z_e \approx 1.0\text{ m}$), la señal es $\Delta V_{MN} \approx 8\text{ mV}$, plenamente separable del ruido gracias al PGA de 16 bits del ADS1115.  
     **Alcance práctico confiable: hasta $1.0\text{--}1.5\text{ metros}$ de profundidad.**
   * **En terrario o sandbox de laboratorio ($a = 2.0\text{--}5.0\text{ cm}$):**  
     $\Delta V_{MN} \approx 100\text{--}300\text{ mV}$. La señal es gigantesca y permite resoluciones milimétricas a $1\text{--}3\text{ cm}$ de profundidad.
   * **¿Por qué los equipos industriales usan 200V–400V?**  
     Para forzar $200\text{--}500\text{ mA}$ en suelos áridos o rocosos y alcanzar profundidades de $10\text{--}50\text{ m}$ con tendidos de cientos de metros. Para laboratorio, agricultura de precisión y geotecnia somera, **3.3V / 5V es completamente funcional y 100% seguro contra descargas**.

---

#### 💾 Exportación y Comparativa de Inversión 2D/3D (`RES2DINV` vs. Software Libre `pyGIMLi` y `ResIPy`)

El sistema implementa una arquitectura de exportación agnóstica de software. Dado que licencias comerciales como `RES2DINV` o `RES3DINV` no siempre están disponibles o son cerradas, el monorepo prioriza una **comparativa dual**: contrastar las inversiones comerciales clásicas de Gauss-Newton contra el ecosistema científico libre de última generación en Python:

```text
                                  ESP32 ERT MINI-METER
                                           │
                    ┌──────────────────────┴──────────────────────┐
                    ▼                                             ▼
          Formato RES2DINV (.dat)                       Formato CSV Unificado (x, y, z)
                    │                                             │
         ┌──────────┴──────────┐                       ┌──────────┴──────────┐
         ▼                     ▼                       ▼                     ▼
     RES2DINV               ResIPy                  pyGIMLi               ParaView
  (Gauss-Newton)        (GUI Open Source)      (Python FEM Inversion)    (Render 3D VTK)
```

1. **Formato Estándar `RES2DINV` / `RES3DINV` (`.dat`):**
   El servidor web genera en memoria el formato estándar con código de geometría (`1` = Wenner, `3` = Dipolo-Dipolo, `7` = Wenner-Schlumberger, `11` = General 3D).
2. **Ecosistema Científico Libre (`pyGIMLi` + `ResIPy` en Python):**
   - **`pyGIMLi` (Python Geophysical Inversion and Modelling Library):** Estándar de la comunidad geofísica abierta. Resuelve la ecuación directa por **Elementos Finitos (FEM)** en mallas triangulares no estructuradas (2D) y tetraédricas (3D), permitiendo controlar con precisión matemática el parámetro de regularización ($\lambda$), la rugosidad del modelo y la matriz de covarianza de datos.
   - **`ResIPy` (GUI Open-Source para R2 / R3t):** Desarrollado por la Universidad de Lancaster. Ofrece una interfaz gráfica potente para cargar directamente los archivos del ESP32, realizar control de calidad (filtro de errores recíprocos y pseudosección en vivo), e invertir tanto perfiles 2D como mallas 3D sin escribir código.
   - **Visualización 3D en `ParaView`:** `pyGIMLi` exporta el volumen invertido en formato `.vtk`, permitiendo generar isosuperficies de resistividad volumétrica, cortes interactivos ortogonales ($XY$, $XZ$, $YZ$) y transparencias volumétricas de las anomalías enterradas.

##### Estructura de Exportación 2D y 3D (Formato Nativo RES2DINV / RES3DINV como Principal)

El flujo de trabajo genera **primariamente el formato estándar `.dat` de `RES2DINV` y `RES3DINV`**, garantizando compatibilidad industrial inmediata, y permitiendo luego la conversión a formatos abiertos (`pyGIMLi` / `ResIPy` / VTK) según se requiera:

```text
# 1. Archivo 2D Estándar RES2DINV (perfil_linea_Y1.dat)
Terrario ERT 2D - Linea Y1 (ESP32 ADS1115)
2.5                          # Espaciamiento unitario a (cm)
1                            # Código de arreglo (1 = Wenner)
15                           # Número de mediciones
1                            # Coordenadas relativas (x_centro, a)
0                            # Sin datos de polarización inducida (IP)
# x_centro   separacion_a   resistividad_aparente(ohm-m)
3.75         2.50           54.20
6.25         2.50           58.10
...
0                            # Terminador de archivo

# 2. Archivo 3D Nativo RES3DINV (terrario_bloque_3d.dat)
Terrario ERT 3D - Grilla 4x4 (ESP32 ADS1115)
4.0                          # Espaciamiento en X (cm)
4.0                          # Espaciamiento en Y (cm)
11                           # Código de arreglo 3D (11 = Arreglo libre / Cross-line cuadripolos)
64                           # Total de puntos de medición volumétricos 3D
1                            # Tipo de formato (1 = Coordenadas de electrodos X-Y)
0                            # Indicador IP (0 = Solo resistividad aparente)
# Ax   Ay   Bx   By   Mx   My   Nx   Ny   rho_a(ohm-m)
0.0  0.0  4.0  0.0  8.0  0.0 12.0  0.0  50.32
0.0  4.0  4.0  4.0  8.0  4.0 12.0  4.0  48.15
0.0  0.0  4.0  0.0  4.0  4.0  8.0  4.0  62.40   # Medición cruzada (Cross-line 3D)
...
0                            # Terminador de archivo 3D
```

---

#### 🧊 Estrategia de Prospección: 3 a 4 Perfiles 2D Paralelos vs. Inversión Volumétrica 3D

Para evaluar experimentalmente el salto de dimensión (2.5D vs. 3D real), el protocolo de laboratorio contempla **adquirir 3 o 4 perfiles 2D paralelos contiguos** en el terrario y compararlos cara a cara contra el modelo invertido 3D:

```text
VISTA SUPERIOR — PROSPECCIÓN MULTI-PERFIL 2D PARALELO vs. CUBO 3D:

Y (ancho = 22 cm)
 ▲
 │ ┌─────────────────────────────────────────────────────────────────┐
 │ │                                                                 │
 │ │  Y = 5 cm   ───[ Perfil 2D #1: e1  a  e16 (Línea Norte)  ]───   │  dat #1
 │ │  Y = 9 cm   ───[ Perfil 2D #2: e1  a  e16 (Sobre Anomalía)]───   │  dat #2
 │ │  Y = 13 cm  ───[ Perfil 2D #3: e1  a  e16 (Sobre Anomalía)]───   │  dat #3
 │ │  Y = 17 cm  ───[ Perfil 2D #4: e1  a  e16 (Línea Sur)    ]───   │  dat #4
 │ │                                                                 │
 │ └─────────────────────────────────────────────────────────────────┘
 └─────────────────────────────────────────────────────────────────────► X (largo = 50 cm)

METODOLOGÍA DE COMPARACIÓN:
1. Inversión 2D Individual (RES2DINV / ResIPy):
   - Se invierten por separado perfil_1.dat, perfil_2.dat, perfil_3.dat y perfil_4.dat.
   - Enfoque pseudo-3D (Corte en rebanadas): Se alinean verticalmente los 4 cortes 2D para observar la extensión de la anomalía en el eje Y.
2. Inversión Volumétrica Verdadera 3D (RES3DINV / pyGIMLi):
   - Se compilan los 4 perfiles paralelos más las mediciones diagonales cruzadas (cross-line) en un único archivo primario terrario_3d.dat (formato RES3DINV).
   - Se corre la inversión 3D completa: resuelve los efectos de borde tridimensionales (foco de corriente fuera del plano 2D) y elimina los artefactos de deformación ("smearing") que sufren los perfiles 2D aislados.
```

---

#### 🧊 Extensión a Tomografía 3D (Malla $4 \times 4$ de 16 Electrodos)

El mismo hardware de 16 electrodos y multiplexores puede conmutar de una línea 2D ($1 \times 16$) a una **grilla volumétrica 3D ($4 \times 4$)**:

```text
       X1 (0 cm)       X2 (4 cm)       X3 (8 cm)       X4 (12 cm)
Y1 (0 cm)   e1 ───────────── e2 ───────────── e3 ───────────── e4
            │                │                │                │
Y2 (4 cm)   e5 ───────────── e6 ───────────── e7 ───────────── e8
            │                │                │                │
Y3 (8 cm)   e9 ───────────── e10 ──────────── e11 ──────────── e12
            │                │                │                │
Y4 (12 cm)  e13 ──────────── e14 ──────────── e15 ──────────── e16

                      ESPACIAMIENTO 3D: ax = ay = 4.0 cm
```

##### 1. Adquisición Multi-Direccional 3D
Para iluminar el volumen subyacente en el semiespacio, el firmware ejecuta una secuencia tridimensional completa:
- **Perfiles X (En línea):** Cuadripolos en cada una de las 4 filas horizontales.
- **Perfiles Y (Transversales):** Cuadripolos en cada una de las 4 columnas verticales.
- **Cuadripolos Cruzados (Cross-line 3D):** Inyección en fila $Y_1$ y medición de potencial en fila contigua $Y_2$ (e.g., inyectar en $e_1\text{-}e_2$ y medir en $e_6\text{-}e_7$), resolviendo la continuidad lateral de la anomalía en el espacio $Y$.

##### 2. Factor Geométrico $K$ General 3D
En configuraciones arbitrarias o tridimensionales, el factor geométrico se calcula punto a punto a partir de las distancias euclidianas 3D entre electrodos en superficie $z = 0$:

$$r_{AM} = \sqrt{(x_A - x_M)^2 + (y_A - y_M)^2}$$

$$K_{3D} = \frac{2\pi}{\left(\frac{1}{r_{AM}} - \frac{1}{r_{BM}} - \frac{1}{r_{AN}} + \frac{1}{r_{BN}}\right)}$$

##### 3. Comparativa Metodológica: 2D vs. 3D
| Criterio | Modo 2D (Línea $1 \times 16$) | Modo 3D (Grilla $4 \times 4$) |
|---|---|---|
| **Apertura y Cobertura** | Perfil longitudinal estrecho ($L = 37.5\text{ cm}$) | Cubo volumétrico de análisis ($12 \times 12 \times 6\text{ cm}$) |
| **Profundidad Mediana ($z_e$)** | Hasta $6\text{ cm}$ (Wenner $a = 2.5\text{ cm}$, $n=1\text{--}4$) | Hasta $4\text{--}5\text{ cm}$ ($ax = ay = 4\text{ cm}$) |
| **Asunción Geológica** | Asume estructuras cilíndricas 2D infinitas en el eje $Y$ | No asume simetría: resuelve cuerpos finitos aislados en $X, Y, Z$ |
| **Visualización** | Corte vertical 2D de resistividad real ($\rho(x,z)$) | Bloque volumétrico interactivo en `pyGIMLi` / ParaView ($\rho(x,y,z)$) |

#### 🔬 Protocolo Experimental: Comparación Cruzada de Arreglos (Benchmark Multi-Arreglo)

Uno de los objetivos científicos centrales del Proyecto 02 es **comparar directamente los 5 arreglos sobre el mismo medio o modelo análogo** (terrario/sandbox o perfil de terreno), contrastando su sensibilidad, relación señal/ruido y resolución:

```text
                                  LÍNEA DE MEDIDA (16-24 ELECTRODOS)
       e1    e2    e3    e4    e5    e6    e7    e8    e9    e10   e11   e12   ...   e24
       ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼           ▼
═══════╧═════╧═════╧═════╧═════╧═════╧═════╧═════╧═════╧═════╧═════╧═════╧═══════════╧══════ (Superficie)
       │                                                         │
       │                   ┌───────────────────────────┐         │
       │                   │   ANOMALÍA RESISTIVA      │         │
       │                   │  (Tubo PVC / Bloque seco) │         │
       │                   └───────────────────────────┘         │
       │                                                         │
       │          ┌───────────────────────────┐                  │
       │          │   ANOMALÍA CONDUCTORA     │                  │
       │          │ (Esponja salina / Metal)  │                  │
       │          └───────────────────────────┘                  │
       ▼                                                         ▼
```

##### Matriz Comparativa de Comportamiento Geofísico

| Arreglo | Relación Señal/Ruido (SNR) | Sensibilidad a Capas Horizontales | Resolución de Bordes Verticales | Sensibilidad al Buzamiento (Inclinación) | Rol en el Laboratorio Comparativo |
|---|:---:|:---:|:---:|:---:|---|
| **1. Wenner ($\alpha$)** | ⭐⭐⭐⭐⭐ (Máxima) | ⭐⭐⭐⭐⭐ (Excelente) | ⭐⭐ (Pobre, difumina) | Simétrica | **Línea base de alta estabilidad:** Mapea el nivel freático y estratos planos con señal limpia aun a 3.3V. |
| **2. Dipolo-Dipolo** | ⭐⭐ (Baja en $n>3$) | ⭐⭐ (Pobre) | ⭐⭐⭐⭐⭐ (Excelente) | Simétrica | **Detector de anomalías localizadas:** Resuelve con máxima nitidez tubos, cavidades, fallas verticales y raíces. |
| **3. Schlumberger** | ⭐⭐⭐⭐ (Alta) | ⭐⭐⭐⭐ (Muy buena) | ⭐⭐⭐ (Intermedia) | Simétrica | **Calibración 1D (SEV):** Curva bilogarítmica clásica ($\rho_a$ vs. $AB/2$) en el centro del perfil sin mover electrodos $M, N$. |
| **4. Wenner-Schlumb.** | ⭐⭐⭐⭐ (Alta) | ⭐⭐⭐⭐ (Muy buena) | ⭐⭐⭐⭐ (Muy buena) | Simétrica | **Compromiso óptimo 2D:** Cobertura densa con excelente balance entre profundidad y resolución lateral. |
| **5. Polo-Dipolo** | ⭐⭐⭐ (Intermedia) | ⭐⭐⭐ (Intermedia) | ⭐⭐⭐⭐ (Buena) | ⭐⭐⭐⭐⭐ (Máxima) | **Detector de contactos inclinados:** Arreglo asimétrico ($B$ al infinito); detecta buzamiento midiendo *forward* y *reverse*. |

##### Metodología de Validación Experimental:
1. **Adquisición sobre el mismo modelo análogo:** Se clava una línea fija de electrodos en el sandbox con una anomalía resistiva (madera/PVC) y una conductora (esponja salina).
2. **Barrido secuencial en el firmware:** El ESP32 ejecuta sucesivamente los patrones de conmutación para Wenner, Dipolo-Dipolo, Schlumberger, Wenner-Schlumberger y Polo-Dipolo.
3. **Inversión y Contraste 2D:** Se descargan los archivos `.dat` (`wenner.dat`, `dipole.dat`, etc.) y se procesan en **`ResIPy`** o **`pyGIMLi`**:
   - Se compara la función de sensibilidad Fréchet $F(x,z)$ de cada arreglo.
   - Se evalúa cuantitativamente el error cuadrático medio (RMS %) de inversión y la fidelidad geométrica de las anomalías recuperadas.

---

#### 🧪 Modelo Físico Análogo en Terrario / Sandbox (Laboratorio de Mesa ERT)
Para validar el resistivímetro antes de salir a terreno, se utiliza el montaje canónico de modelado físico análogo (*sandbox modeling*):
- **Recipiente:** Caja plástica organizadora o terrario de vidrio/acrílico de $40\text{--}60\text{ cm}$ de largo $\times 20\text{--}25\text{ cm}$ de ancho $\times 15\text{ cm}$ de profundidad.
- **Matriz de fondo:** Arena fina humedecida uniformemente con agua ligeramente mineralizada para establecer una resistividad de fondo homogénea ($\rho_{\text{matriz}} \approx 30\text{--}80\ \Omega\cdot\text{m}$).
- **Electrodos de superficie:** 16 a 24 varillas delgadas de acero inoxidable clavadas en línea en la arena, con separación básica $a = 2.0\text{ cm}$ o $2.5\text{ cm}$ (secuencia multi-nivel $n = 1, 2, 3, 4, 5$).
- **Anomalías enterradas controladas:**
  1. *Anomalía Resistiva ($\rho \gg \rho_{\text{matriz}}$):* Bloque de madera seca, tubo de PVC cerrado o roca ígnea densa (simula cavidades, fallas secas o túneles).
  2. *Anomalía Conductora ($\rho \ll \rho_{\text{matriz}}$):* Esponja saturada con salmuera concentrada o barra metálica (simula acuífero salino o cuerpo mineralizado de sulfuros).
- **Resultado:** La pseudosección generada por el ESP32 se invierte en `RES2DINV` / `pyGIMLi`, reconstruyendo la forma, posición horizontal $x$ y profundidad $z$ de las anomalías enterradas.
- **Las 2 Reglas de Oro en el Terrario:**
  1. **Efecto Escala ($\Delta V_{MN} \propto 1/a$) y Humectación Homogénea:** En el terrario ($a = 2\text{ cm}$), la señal es naturalmente **$100\text{ veces mayor}$** que en el campo ($a = 2\text{ m}$) para la misma corriente $I$. A diferencia del campo abierto (donde se vierte salmuera en las picas $A$ y $B$ para bajar $R_{\text{contacto}}$ de $3\,000\ \Omega$ a $150\ \Omega$), **en el terrario jamás se debe echar agua con sal en un clavo individual**, pues a $2\text{ cm}$ de distancia la sal se difunde hacia los electrodos vecinos y contamina la matriz homogénea. La arena debe humedecerse toda por igual con atomizador.
  2. **Prevención de Electrólisis y Burbujas Aislantes:** En electrodos delgados, corrientes continuas altas ($> 30\text{ mA}$) disocian el agua ($2\text{H}_2\text{O} \to 2\text{H}_2\uparrow + \text{O}_2\uparrow$), recubriendo el metal con microburbujas aislantes. Por ello se emplean **pulsos bipolares cortos ($+V$ por $25\text{ ms}$, $-V$ por $25\text{ ms}$ y corte a `OFF`)**.

---

#### 🔧 Evolución de Hardware: Del Modo Base (Resistencias AFEL) al Sistema Automatizado (`INA226` + `TB6612FNG` + `4× CD74HC4067`)

Por la Ley de Ohm ($I = \frac{V_{\text{inj}}}{R_{\text{contacto}} + R_{\text{suelo}} + R_s}$), aumentar la corriente inyectada $I$ multiplica directamente la señal medida $\Delta V_{MN} = \frac{\rho I}{K}$. El proyecto soporta dos niveles de implementación:

##### Nivel 1 — Modo Base en Protoboard (100% en Inventario con Resistencias del Kit AFEL `P0`)
Sin comprar ningún componente adicional, se aprovecha la capacidad de los pines GPIO del ESP32 (hasta $15\text{--}20\text{ mA}$ seguros) implementando un **selector de doble resistencia *shunt* con jumper** en la protoboard y lectura diferencial en el `ADS1115` (`A0-A1` para $\Delta V_{MN}$ y `A2-A3` para $\Delta V_{\text{shunt}}$):

| Rango en Protoboard | Resistencia *Shunt* ($R_s$) | Corriente Inyectada ($I$) | Aplicación Ideal |
|---|:---:|:---:|---|
| **Modo Estándar** | **$1\,000\ \Omega$** ($1\text{ k}\Omega$) | $1.0\text{ a }3.0\text{ mA}$ | Arreglos de alta señal (**Wenner**, **Schlumberger**) y niveles someros ($n=1, 2$). |
| **Modo Alta Corriente (`High-SNR`)** | **$220\ \Omega$** | **$8.0\text{ a }14.0\text{ mA}$** | Multiplica por $\times 4.5$ la señal para **Dipolo-Dipolo** y **Polo-Dipolo** en niveles profundos del terrario ($n=3, 4, 5, 6$, donde la señal decae con $1/n^3$). |

##### Nivel 2 — Upgrade Kit Semiprofesional y Conmutación Automática Multielectrodo
Para escalar desde el cambio manual de cables hacia una estación tomográfica automatizada comparable a equipos comerciales (*Syscal* / *SuperSting*), se incorporan cuatro mejoras modulares de bajo costo:

```text
                       ┌───────────────────────────────────────────────────────┐
                       │         ESP32 DevKit 30P / ESP32-S3                   │
                       └───────┬───────────────────┬───────────────────┬───────┘
                     D25 / D26 │           Bus I2C │         S0..S3    │ Control
                               ▼                   ▼                   ▼
                       ┌───────────────┐   ┌───────────────┐   ┌───────────────────────────────┐
                       │  TB6612FNG    │   │ INA226 (16b)  │   │ 4× Multiplexores CD74HC4067   │
                       │ Puente H MOS  ├──►│ Medidor I (mA)├──►│ MUX_A, MUX_B (Inyección)      │
                       │ (±3.3V a ±12V)│   │ + ADS1115 (V) │   │ MUX_M, MUX_N (Potencial)      │
                       └───────────────┘   └───────▲───────┘   └───────────────┬───────────────┘
                                                   │                           │
                                                   └───────── ΔV_MN ───────────┤
                                                                               ▼
                                                               [ 16 Electrodos Inox 316L en Terrario ]
```

| Módulo de Upgrade | Qué Reemplaza / Mejora | Beneficio Técnico y Científico | Costo Ref. (Chile) |
|---|---|---|:---:|
| **1× `INA226` (I2C 16-bit)** | Resistencia *shunt* discreta y canal `A2-A3` del ADS1115 | Mide la corriente $I$ con *shunt* SMD de precisión sin deriva térmica y **libera los 4 canales del `ADS1115`** para medir potencial. | ~\$3.500 CLP |
| **1× `TB6612FNG` (o `DRV8833`)** | Inyección directa desde pines GPIO del ESP32 | **Puente H MOSFET** de bajísima caída (muy superior al `L298N`). Conmuta $\pm 3.3\text{ V}$, $\pm 5\text{ V}$ o $\pm 12\text{ V}$ (hasta $1.2\text{ A}$) sin cargar el microcontrolador. | ~\$3.000 CLP |
| **4× `CD74HC4067` (16ch MUX)** | Mover los 4 caimanes a mano (~100 veces por experimento) | **Conmutación automática de 16 electrodos** (`MUX A`, `MUX B`, `MUX M`, `MUX N`). Ejecuta el **Benchmark completo de los 5 arreglos en $< 10\text{ s}$** con un clic desde la web. | ~\$8.000 CLP (los 4) |
| **Varillas Inox 316L (TIG / Rayos)** | Clavos de fierro o acero galvanizado | Elimina la corrosión rápida y los voltajes galvánicos parásitos en contacto con la humedad. | ~\$3.000 CLP |

---

### 📡 Georradar Experimental (GPR de Mesa / Terrario) con Radar Doppler RCWL-0516

El sensor **RCWL-0516** adquirido para el laboratorio integra un transmisor/receptor de microondas en banda S que opera a **$f \approx 3.18\text{ GHz}$**. Aunque de fábrica viene configurado con un comparador digital para detección de presencia binaria (`OUT` HIGH/LOW), su electrónica interna puede hackearse para convertirlo en un **escáner electromagnético de reflexión somera (GPR análogo)**:

#### 1. Fundamentos Físicos de Microondas y Longitud de Onda
- **Frecuencia de emisión:** $f_0 \approx 3.18\text{ GHz}$.
- **Longitud de onda en el aire:** $\lambda_0 = \frac{c}{f_0} \approx 9.4\text{ cm}$.
- **Longitud de onda en arena seca ($\varepsilon_r \approx 4$):**
  $$\lambda = \frac{\lambda_0}{\sqrt{\varepsilon_r}} = \frac{9.4\text{ cm}}{2} \approx 4.7\text{ cm}$$
- **Comparación industrial:** Los georradares comerciales de alta resolución para inspección de estructuras, concreto y canalizaciones superficiales (ej. *Hilti PS 1000*, *Proceq GPR*) operan justamente entre $2.0\text{ GHz}$ y $3.5\text{ GHz}$.

#### 2. Penetración Dieléctrica: Suelo Seco vs. Suelo Húmedo
- **En suelo húmedo o salino:** ❌ Las moléculas de agua líquida poseen alta permitividad ($\varepsilon_r \approx 80$) y pérdidas dieléctricas masivas a gigahertz (absorción por relajación molecular de Debye, principio del microondas doméstico). La onda de $3.2\text{ GHz}$ se extingue en $2\text{--}5\text{ cm}$.
- **En arena seca, yeso, ladrillo, madera, PVC o terrario seco:** ✅ La atenuación es despreciable y la microonda penetra **$20\text{ a }50\text{ cm}$**, reflejándose fuertemente en contrastes dieléctricos (metales $\varepsilon_r \to \infty$, cavidades de aire $\varepsilon_r = 1$, tuberías o rocas).

#### 3. El Hack de la Señal Analógica y B-Scan Sintético
El chip integrado **RCWL-9196** procesa la señal de microondas mezclando la onda emitida con la reflejada. La señal de frecuencia intermedia (IF cruda) previa al comparador está disponible en el circuito:
- **Punto de conexión:** Pin 12 del chip RCWL-9196 (o el nodo del capacitor de integración C-TM).
- **Adquisición con ESP32:** Se conecta esta señal IF analógica directamente a un canal ADC del ESP32.
- **Escaneo por Movimiento Relativo (Synthetic B-Scan):** Al deslizar el sensor a velocidad constante sobre un riel encima del terrario con arena seca, la aproximación y alejamiento de una anomalía enterrada (tubo metálico o cavidad) genera un tren de oscilación Doppler ($5\text{--}200\text{ Hz}$). Graficando la envolvente de amplitud en función de la posición $x$, el ESP32 mapea la posición horizontal exacta del objeto.

#### 4. Sinergia Geofísica: ERT + GPR
ERT y GPR son los dos métodos geofísicos superficiales más complementarios de la ciencia:
- **ERT (Corriente Continua / Proyecto 02):** Sensible a la resistividad ($\rho$); ideal en **suelos húmedos, arcillas y acuíferos**. Falla en arena ultra seca.
- **GPR (Microondas 3.2 GHz / RCWL-0516):** Sensible a la permitividad dieléctrica ($\varepsilon_r$); ideal en **medios secos, arenosos, rocas secas y concreto**. Falla en arcillas saturadas.

---

### 🔨 Métodos Sísmicos con Hardware ESP32: Pasivos y Activos

Aprovechando el hardware adquirido (**`ESP32-S3 N16R8`**, **`ESP32 DevKit 30P`**, **`2× ADXL345 SPI`**, **`MPU6050`**, **`Micrófono I2S INMP441`** y **`MicroSD`**), se pueden implementar cuatro técnicas de prospección y caracterización sísmica:

#### A. Sismología Pasiva y Algoritmo STA/LTA (Proyecto 06)
- **Hardware:** `ESP32-S3` + `ADXL345` (SPI a $800\text{ Hz}$ / $1600\text{ Hz}$) + buffer circular en 8 MB PSRAM.
- **Detección Automática de Eventos (STA/LTA):**
  $$R(t) = \frac{\text{STA}(t)}{\text{LTA}(t)} = \frac{\frac{1}{n_s}\sum_{i=t-n_s}^t a_i^2}{\frac{1}{n_l}\sum_{i=t-n_l}^t a_i^2}$$
  Discrimina en tiempo real sismos reales de vibraciones urbanas continuas (tráfico, pisadas).
- **Distancia Epicentral ($S\text{-}P$):** A partir del desfase temporal entre la llegada de la onda compresional P y la de cizalla S ($\Delta t = t_S - t_P$), se calcula la distancia al foco:
  $$d \approx \frac{V_P \cdot V_S}{V_P - V_S} \Delta t \approx 8.0 \cdot \Delta t \quad [\text{km}]$$

#### B. Efectos de Sitio y Resonancia del Suelo: Razón Espectral H/V (Técnica de Nakamura / HVSR)
- **Principio:** Con el sismógrafo triaxial registrando el **ruido sísmico ambiental continuo** (microtremores generados por oleaje marino, viento y actividad humana), se divide el espectro de amplitud de los ejes horizontales entre el eje vertical:
  $$\text{HVSR}(f) = \frac{\sqrt{S_x(f)^2 + S_y(f)^2}}{S_z(f)}$$
- **Resultado Geofísico:** El pico principal de la curva $\text{HVSR}(f)$ revela la **frecuencia fundamental de resonancia ($f_0$) del suelo** del sitio y su factor de amplificación sísmica, crucial para diseño sismorresistente.

#### C. Sísmica Activa de Refracción Somera (Seismic Refraction)
- **Hardware:** Placa metálica de impacto en el suelo + martillo con contacto eléctrico (*trigger* digital conectado a una interrupción GPIO de resolución de microsegundos con pull-up) + 2 acelerómetros `ADXL345` (SPI) o micrófono I2S `INMP441` a distancias conocidas ($x_1$ y $x_2$).
- **Circuito de Disparo (Trigger Instantáneo):**
  ```text
       [ ESP32 GPIO con Pull-up (3.3V) ] ─── Cable al Martillo metálico
                                                       │
                                                       ▼ (Impacto mecánico)
       [ GND del ESP32 ] ─────────────────── Placa metálica clavada en el suelo
  ```
  Al hacer contacto el martillo con la placa, el pin cae instantáneamente a `LOW` y dispara la interrupción de hardware con `micros()`, iniciando el cronómetro sísmico con error inferior a $2\ \mu\text{s}$.
- **Física (Ley de Snell Sísmica y Dromocrónica):**
  ```text
         Tiempo t
            ▲
            │                 Onda Directa: Pendiente = 1/V1
            │                 ─────────────────────────────►
            │                              Onda Refractada: Pendiente = 1/V2
            │                   ti ───────/────────────────►
            │                    │       /
            │                    │      /
            │                    │     / (xcrit, tcrit)
            │                    │    /
            └────────────────────┴───/──────────────────────► Distancia x
                                     xcrit
  ```
  Al golpear la placa, se detecta el primer arribo (*first break*) de la onda P:
  - Onda directa en estrato superficial: $t = \frac{x}{V_1}$.
  - Onda refractada crítica en el estrato basal más rápido ($V_2 > V_1$):
    $$t = \frac{x}{V_2} + t_i, \qquad t_i = \frac{2 z_1 \sqrt{V_2^2 - V_1^2}}{V_1 V_2}$$
  - Distancia crítica de cruce:
    $$x_{\text{crit}} = 2 z_1 \sqrt{\frac{V_2 + V_1}{V_2 - V_1}}$$
- **Parámetros Geotécnicos Obtenidos:** Velocidad de onda P del suelo vegetal/relleno ($V_1 \approx 300\text{--}600\text{ m/s}$), velocidad del estrato competente/roca basal ($V_2 \approx 1500\text{--}3500\text{ m/s}$) y profundidad exacta del contacto geológico ($z_1$).

#### D. Análisis de Ondas Superficiales (MASW / Dispersión de Rayleigh) y Rigidez Dinámica
- **Clasificación Geotécnica del Suelo (Norma Chilena NCh433 / DS61):** La velocidad de onda de corte en los primeros 30 metros ($V_{S30}$) define si el suelo es Tipo A (roca), B (roca blanda/suelo muy denso), C (suelo denso) o D/E (suelo blando/arenas licuables).
- **Diferencia de Fase Bicanal:** Con dos acelerómetros separados a distancia $\Delta x$, la diferencia de fase $\Delta \phi(f)$ en el espectro cruzado calcula la curva de dispersión de velocidad de fase de Rayleigh:
  $$V_R(f) = \frac{2\pi f \cdot \Delta x}{\Delta \phi(f)}, \qquad V_S \approx 1.1 \cdot V_R$$

#### E. Modelado Físico Sísmico Análogo en Terrario / Sandbox

La sísmica de refracción y dispersión puede reproducirse a escala reducida en el terrario de laboratorio gracias a un fenómeno físico ventajoso: **en arena suelta no confinada, la velocidad de onda P es extremadamente lenta ($V_P \approx 120\text{--}200\text{ m/s}$)**:

```text
       [Impacto con bolita de acero + Trigger]                  [Sensor #1]         [Sensor #2]
                          │                                          │                   │
                          ▼ (x=0)                                    ▼ (x1)              ▼ (x2)
══════════════════════════█══════════════════════════════════════════▲═══════════════════▲════════ (Superficie)
       Capa 1: Arena suelta (Lenta: V1 ≈ 140 m/s)                    │                   │
  ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─│─ ─ ─ ─ ─ ─ ─ ─ ─ ─│─ ─ (Profundidad z1)
       Capa 2: Baldosa / Ladrillo macizo (Rápida: V2 ≈ 2000 m/s)     │                   │
```

1. **Resolución Temporal del ESP32:** En $d = 30\text{ cm}$ de arena, la onda tarda $t = \frac{0.30\text{ m}}{150\text{ m/s}} = 2\,000\ \mu\text{s}$ ($2.0\text{ ms}$). Con el reloj de $1\ \mu\text{s}$ (`micros()`) del ESP32 o el micrófono I2S `INMP441` muestreando a $44.1\text{ kHz}$ ($22.6\ \mu\text{s}$ entre puntos), se adquieren casi 100 muestras digitales de la traza de onda completa.
2. **Experimento 1 — Refracción Bicomponente:**
   - Se coloca en el fondo del terrario una baldosa gruesa o bloque de ladrillo macizo ($V_2 \approx 2000\text{ m/s}$) cubierto por $5\text{ cm}$ de arena suelta ($V_1 \approx 140\text{ m/s}$).
   - A distancias cortas ($x < 15\text{ cm}$) llega primero la onda directa lenta; a distancias mayores ($x > 15\text{ cm}$), la onda que bajó al ladrillo viaja $14\times$ más rápido por el fondo y adelanta a la directa. El quiebre en la dromocrónica entrega la profundidad exacta del estrato arenoso.
3. **Experimento 2 — Sombra Sísmica y Difracción:**
   - Un tubo de PVC cerrado o bloque de espuma enterrado atenúa drásticamente la amplitud de la onda $P$ y genera retrasos por difracción, correlacionándose directamente con la anomalía resistiva de alta impedancia detectada por ERT.
4. **Experimento 3 — Frecuencia de Resonancia ($f_0 = V_S / 4H$):**
   - Para un espesor de arena de $10\text{ cm}$ ($H = 0.1\text{ m}$) con $V_S \approx 80\text{ m/s}$, la frecuencia propia es $f_0 \approx 200\text{ Hz}$, detectable mediante la FFT del acelerómetro o micrófono.
5. **Pautas de Construcción del Terrario Sísmico:**
   - **Bordes Absorbentes:** Para evitar reflexiones parásitas en las paredes rígidas de la caja plástica, se pegan tiras de goma eva o esponja en las 4 paredes interiores.
   - **Acoplamiento Mecánico:** Los sensores (`ADXL345` o micrófono) se fijan con un clavo o pica corta clavada en la arena para asegurar transferencia elástica directa.

#### F. Laboratorio Acústico de Mesa: Velocidad de Onda y Módulo de Young ($E$)
- En barras de material (madera, concreto, roca o tubo de PVC) o dentro del terrario:
- Un golpe leve en un extremo y el `INMP441` o `ADXL345` en el otro extremo miden el tiempo de vuelo $\Delta t$ con precisión de microsegundos ($V_P = L / \Delta t$).
- A partir de la densidad conocida $\rho$, se calcula experimentalmente el **Módulo de Elasticidad de Young ($E$)** del material:
  $$V_P = \sqrt{\frac{E}{\rho} \frac{1 - \nu}{(1 + \nu)(1 - 2\nu)}}$$

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
2. **Medir diferencia de llegada** de ondas P entre Nodo 1 y Nodo 2 → estimar distancia al epicentro ($d \approx 8 \cdot \Delta t$)
3. **Correlacionar caída de presión** (BMP280) con sismicidad superficial en el tiempo
4. **Comparar espectro de día vs noche** → efecto del ruido urbano (tráfico) en la estación
5. **Detectar microtemblores** continuos de fondo (ruido sísmico ambiental)
6. **Medir variación de inclinación** del suelo con MPU6050 durante semanas
7. **Resonancia del Suelo (HVSR Nakamura):** Curva espectral $H/V$ para medir la frecuencia fundamental $f_0$ y factor de amplificación sísmica del sitio
8. **Sísmica Activa de Refracción:** Medir con martillo-trigger y 2 acelerómetros la dromocrónica ($t$ vs. $x$), velocidades $V_1, V_2$ y profundidad del estrato competente ($z_1$)
9. **Dispersión de Ondas Rayleigh (MASW):** Curva $V_R(f)$ y perfil $V_S$ para clasificar el suelo según NCh433 / DS61
10. **Sondeo Eléctrico Vertical (SEV) y Tomografía de Resistividad 2D ($\rho_a$):** Medir variaciones de resistividad del suelo ($\Omega\cdot\text{m}$) con 5 arreglos + exportación `RES2DINV`
11. **GPR de Mesa / Terrario (RCWL-0516):** Escaneo por microondas a 3.18 GHz para detectar tuberías y cavidades en arena seca con perfil de reflexión analógica IF

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
