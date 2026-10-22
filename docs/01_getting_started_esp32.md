# 🌱 Proyecto 1 — Mi primera estación ambiental con ESP32
> **v1.0** · Guía paso a paso para principiantes absolutos. No necesitas saber electrónica ni programar: solo seguir los pasos en orden.
> Usa **solo hardware que ya compraste**. Lo que hagas aquí servirá después en la Estación Geofísica y el Monitor Ambiental.

---

## Tabla de Contenidos
0. [Qué vas a construir y qué vas a aprender](#0-qué-vas-a-construir-y-qué-vas-a-aprender)
1. [Qué necesitas (todo ya comprado)](#1-qué-necesitas-todo-ya-comprado)
2. [Conceptos mínimos (5 minutos)](#2-conceptos-mínimos-5-minutos)
3. [Paso 1 — Preparar el computador](#paso-1--preparar-el-computador)
4. [Paso 2 — Probar la placa: LED parpadeante](#paso-2--probar-la-placa-led-parpadeante)
5. [Paso 3 — Conectar un sensor y buscarlo (escáner I2C)](#paso-3--conectar-un-sensor-y-buscarlo-escáner-i2c)
6. [Paso 4 — Temperatura, humedad y presión](#paso-4--temperatura-humedad-y-presión-aht20--bmp280)
7. [Paso 5 — Luz (BH1750)](#paso-5--luz-bh1750)
8. [Paso 6 — Mostrar todo en la pantalla OLED](#paso-6--mostrar-todo-en-la-pantalla-oled)
9. [Paso 7 — Repetir en la placa mini (ESP32-C3)](#paso-7--repetir-en-la-placa-mini-esp32-c3-super-mini)
10. [Problemas comunes y soluciones](#problemas-comunes-y-soluciones)
11. [Hoja de ruta: de simple a complejo](#hoja-de-ruta-de-simple-a-complejo)
12. [Intercambios de hardware respecto a los proyectos anteriores](#intercambios-de-hardware-respecto-a-los-proyectos-anteriores)

---

## 0. Qué vas a construir y qué vas a aprender

**Construirás** un pequeño aparato que mide **temperatura, humedad, presión y luz**, y lo muestra en una pantalla y en el computador.

| Paso | Qué haces | Qué aprendes |
|:---:|---|---|
| 1 | Instalar programas | Entorno de trabajo (Arduino IDE) |
| 2 | Hacer parpadear un LED | Subir un programa a la placa |
| 3 | Escanear el bus I2C | Cómo "conversa" un sensor con la placa |
| 4 | Leer AHT20 + BMP280 | Leer sensores con librerías |
| 5 | Leer BH1750 | Conectar varios sensores a los mismos cables |
| 6 | Pantalla OLED | Mostrar datos sin computador |
| 7 | Repetir en ESP32-C3 | La misma idea en una placa mini |

⏱️ **Tiempo estimado:** 1 a 2 tardes. Detente cuando quieras; cada paso funciona solo.

---

## 1. Qué necesitas (todo ya comprado)

### Placas

| Placa | Rol en este proyecto |
|---|---|
| **ESP32 DevKit 30P** (WROOM-32) | 🟢 **Tu placa principal para aprender** (Pasos 1–6) |
| **ESP32-C3 Super Mini** | 🟡 Placa mini (Paso 7) |

> 💡 Se eligió el **DevKit 30P** porque es la más estándar: la mayoría de tutoriales en internet usan una igual. Las **ESP32-S3** y **XIAO** se dejan para más adelante.

### Sensores y accesorios

| Componente | Cant. a usar | Para qué |
|---|:---:|---|
| **AHT20 + BMP280** (módulo) | 1 | Temperatura, humedad y presión |
| **BH1750 GY-302** | 1 | Luz (lux) |
| **OLED 0.96" SSD1306 (I2C)** | 1 | Pantalla |
| **Protoboard 830 puntos** | 1 | Conectar sin soldar |
| **Cables Dupont** (macho-macho y macho-hembra) | ~12 | Conexiones |
| **Cable USB de datos** (Xiaomi USB-C) | 1 | Conectar la placa al PC |
| **Multímetro** (opcional) | 1 | Revisar voltajes si algo falla |

### ⚠️ Reglas de oro (léelas una vez)

1. **Nunca conectes o cambies cables con la placa enchufada.** Primero desenchufa el USB.
2. **Alimenta todos los sensores con `3V3`**, no con `5V` ni `VIN`, salvo que se indique.
3. **Revisa dos veces `VCC` y `GND`.** Invertirlos puede quemar el sensor.
4. **Usa un cable USB de datos.** Los cables solo de carga no permiten programar.
5. Si algo huele a quemado o se calienta: **desenchufa de inmediato**.
6. Si tus módulos traen los pines sueltos (en bolsita), hay que **soldarlos** primero. Si aún no sabes soldar, pide ayuda a alguien o practica con una placa PCB de repuesto.

---

## 2. Conceptos mínimos (5 minutos)

**Microcontrolador (ESP32):** un mini computador que ejecuta *un solo programa*. Tiene Wi‑Fi y pines para conectar sensores.

**Pin:** un "agujero" de la placa por donde entra o sale electricidad o información.

**GND / 3V3:** `GND` es el "0 voltios" (tierra) y `3V3` es la alimentación de 3.3 voltios. Todo sensor necesita ambos.

**I2C:** una forma de conectar **muchos sensores con solo 2 cables de datos**:

| Cable | Nombre | Qué hace |
|---|---|---|
| **SDA** | Datos | Aquí viaja la información |
| **SCL** | Reloj | Sincroniza la comunicación |

Cada sensor tiene una **dirección** (como una dirección de casa), por eso se pueden compartir los mismos 2 cables:

| Dispositivo | Dirección I2C típica |
|---|:---:|
| AHT20 (humedad y temperatura) | `0x38` |
| BMP280 (presión) | `0x77` (a veces `0x76`) |
| BH1750 (luz) | `0x23` |
| OLED SSD1306 | `0x3C` |

**Sketch:** así se llama un programa de Arduino. **Librería:** código ya hecho que hace fácil usar un sensor.

**Monitor Serie:** una ventana del Arduino IDE donde la placa "escribe" mensajes en tu computador.

---

## Paso 1 — Preparar el computador

### 1.1 Instalar Arduino IDE
1. Descarga **Arduino IDE 2.x** desde <https://www.arduino.cc/en/software> e instálalo (acepta todo por defecto).

### 1.2 Agregar soporte para ESP32
1. Abre Arduino IDE → **Archivo → Preferencias**.
2. En **"Gestor de URLs Adicionales de Tarjetas"** pega:
   ```text
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
3. Pulsa **OK**.
4. Menú **Herramientas → Placa → Gestor de tarjetas…**
5. Busca `esp32` e instala **"esp32 by Espressif Systems"** (puede tardar varios minutos).

### 1.3 Conectar la placa
1. Enchufa el **DevKit 30P** con el cable USB de datos.
2. Menú **Herramientas → Placa → esp32 → `ESP32 Dev Module`**.
3. Menú **Herramientas → Puerto** → elige el que aparezca (en Windows, algo como `COM3`).

> ❓ **¿No aparece ningún puerto?** Prueba otro cable USB (debe ser de datos), otro puerto USB y, si sigue igual, instala el controlador **CP2102** o **CH340** (según el chip que tenga tu placa; búscalo en internet con ese nombre).

✅ **Listo cuando:** ves el puerto seleccionado en el menú Herramientas.

---

## Paso 2 — Probar la placa: LED parpadeante

Aún **sin sensores**. Solo comprobamos que puedes programar la placa. El DevKit tiene un LED azul propio en el **GPIO 2**.

1. **Archivo → Nuevo sketch**, borra todo y pega:

```cpp
// Paso 2: LED parpadeante
const int PIN_LED = 2;   // LED azul integrado del DevKit

void setup() {
  pinMode(PIN_LED, OUTPUT);        // el pin será una salida
  Serial.begin(115200);            // inicia el "chat" con el PC
  Serial.println("Hola, soy tu ESP32!");
}

void loop() {
  digitalWrite(PIN_LED, HIGH);     // enciende
  Serial.println("LED encendido");
  delay(1000);                     // espera 1 segundo
  digitalWrite(PIN_LED, LOW);      // apaga
  Serial.println("LED apagado");
  delay(1000);
}
```

2. Pulsa el botón **→ Subir** (flecha arriba a la izquierda).
3. Si aparece `Connecting........` y no avanza, **mantén pulsado el botón `BOOT`** de la placa hasta que empiece a subir.
4. Abre el **Monitor Serie** (lupa arriba a la derecha) y selecciona **115200 baud** abajo a la derecha.

✅ **Listo cuando:** el LED azul parpadea y ves los mensajes en el Monitor Serie.

> 🎉 Acabas de programar un microcontrolador. Todo lo demás es variar este patrón.

---

## Paso 3 — Conectar un sensor y buscarlo (escáner I2C)

Empezamos con el sensor de **temperatura/humedad/presión (AHT20 + BMP280)**.

### 3.1 Cableado

> 🔌 **Desenchufa el USB antes de conectar.**

| Módulo AHT20+BMP280 | → | DevKit 30P |
|---|:---:|---|
| `VCC` (o `VIN`) | → | `3V3` |
| `GND` | → | `GND` |
| `SCL` | → | `GPIO 22` (rotulado `D22` o `22`) |
| `SDA` | → | `GPIO 21` (rotulado `D21` o `21`) |

```text
  DevKit 30P                 AHT20+BMP280
 ┌──────────┐              ┌─────────────┐
 │      3V3 ├──────────────┤ VCC         │
 │      GND ├──────────────┤ GND         │
 │  GPIO 22 ├──────────────┤ SCL         │
 │  GPIO 21 ├──────────────┤ SDA         │
 └──────────┘              └─────────────┘
```

> 💡 **Truco:** pon la placa y el sensor en la protoboard y usa cables Dupont. Las dos filas laterales (rojo `+` y azul `-`) sirven de "regleta": una para `3V3` y otra para `GND`, así luego conectas más sensores.

### 3.2 Programa: escáner I2C
Este programa pregunta "¿quién está ahí?" a todas las direcciones.

```cpp
// Paso 3: Escáner I2C
#include <Wire.h>

#define SDA_PIN 21
#define SCL_PIN 22

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(1000);
  Serial.println("Escaneando bus I2C...");
}

void loop() {
  int encontrados = 0;
  for (byte dir = 1; dir < 127; dir++) {
    Wire.beginTransmission(dir);
    if (Wire.endTransmission() == 0) {
      Serial.print("Dispositivo encontrado en 0x");
      if (dir < 16) Serial.print("0");
      Serial.println(dir, HEX);
      encontrados++;
    }
  }
  if (encontrados == 0) Serial.println("No se encontró nada. Revisa el cableado.");
  Serial.println("---");
  delay(3000);
}
```

### 3.3 Resultado esperado

```text
Dispositivo encontrado en 0x38     ← AHT20 ✅
Dispositivo encontrado en 0x77     ← BMP280 ✅  (o 0x76)
```

✅ **Listo cuando:** aparecen `0x38` y `0x77` (o `0x76`).

> ❌ **No aparece nada:** revisa que `SDA` y `SCL` no estén cruzados, que `VCC` vaya a `3V3` y que los pines estén bien soldados. Ver [Problemas comunes](#problemas-comunes-y-soluciones).
>
> 📝 **Anota** la dirección del BMP280 que te salga (`0x76` o `0x77`): la usarás en el siguiente paso.

---

## Paso 4 — Temperatura, humedad y presión (AHT20 + BMP280)

### 4.1 Instalar librerías
**Herramientas → Administrar bibliotecas…** (o `Ctrl+Shift+I`). Busca e instala:

| Busca | Instalar |
|---|---|
| `Adafruit AHTX0` | *Adafruit AHTX0* (acepta instalar dependencias → **Install all**) |
| `Adafruit BMP280` | *Adafruit BMP280 Library* (también **Install all**) |

### 4.2 Programa

```cpp
// Paso 4: AHT20 + BMP280 (version validada en hardware)
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp(&Wire);

void setup() {
  Serial.begin(115200);
  delay(1500);                       // tiempo para abrir el Serial Monitor
  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.println("\n=== ESTACION AMBIENTAL: AHT20 + BMP280 ===");

  // Si un sensor no responde, el programa se DETIENE y lo avisa
  // (asi nunca muestra valores basura como 0.0 C o 727 hPa)
  if (!aht.begin()) {
    Serial.println("ERROR: AHT20 no responde");
    while (true) delay(1000);
  }
  // Prueba primero 0x77 y luego 0x76 (segun lo que viste en el escaner)
  if (!bmp.begin(0x77) && !bmp.begin(0x76)) {
    Serial.println("ERROR: BMP280 no responde");
    while (true) delay(1000);
  }

  // Configuracion recomendada por Bosch para monitoreo del clima:
  // mide solo cuando se le pide (modo forzado) -> no se auto-calienta
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,
                  Adafruit_BMP280::SAMPLING_X1,   // temperatura
                  Adafruit_BMP280::SAMPLING_X1,   // presion
                  Adafruit_BMP280::FILTER_OFF);

  Serial.println("Sensores listos!");
}

void loop() {
  sensors_event_t humedad, temp;
  aht.getEvent(&humedad, &temp);              // AHT20: temperatura y humedad

  bmp.takeForcedMeasurement();                // BMP280: mide ahora
  float temp_bmp = bmp.readTemperature();
  float presion  = bmp.readPressure() / 100.0F; // Pa -> hPa

  Serial.printf("Temp AHT20: %.1f C | Temp BMP280: %.1f C | Humedad: %.1f %% | Presion: %.1f hPa\n",
                temp.temperature, temp_bmp, humedad.relative_humidity, presion);
  delay(2000);
}
```

### 4.3 Resultado esperado y pruebas

```text
Temp AHT20: 22.1 C | Temp BMP280: 22.1 C | Humedad: 77.4 % | Presion: 949.1 hPa
```

> 📌 Lectura real obtenida en Santiago (~550 m s.n.m.), día nublado antes de lluvia. Los dos termómetros (AHT20 y BMP280) deben coincidir dentro de ~0.5 °C.

| Prueba | Qué debería pasar |
|---|---|
| Soplar suavemente sobre el AHT20 | La **humedad sube** rápido |
| Tocar el sensor con el dedo (unos segundos) | La **temperatura sube** unos grados |
| Comparar con el pronóstico del clima de tu ciudad | La **presión** debería estar cerca (en Santiago ~ 930–950 hPa por la altura; en la costa ~ 1010 hPa) |

✅ **Listo cuando:** los tres valores aparecen y reaccionan a las pruebas.

> 💡 La presión que mide el BMP280 es la presión **de tu ubicación** (depende de la altura). Los pronósticos suelen informar presión "a nivel del mar", por eso no coinciden exactamente. Lo importante es **cómo cambia con las horas**: si baja rápido, suele venir mal tiempo.

---

## Paso 5 — Luz (BH1750)

### 5.1 Cableado y Código de Colores Estándar
Conecta el BH1750 **en paralelo** con el sensor anterior (comparten los mismos nodos I2C `SDA` y `SCL`). Puedes puentear desde los agujeros libres de la tira del AHT20+BMP280 cruzando el canal central de la protoboard.

> 🔌 Desenchufa el USB primero.
>
> ⚠️ **¡Cuidado con el orden de pines del GY-302 (BH1750)!** A diferencia del AHT20+BMP280 (`VDD - SDA - GND - SCL`), el módulo GY-302 tiene las letras impresas **por la cara trasera** (`VCC - GND - SCL - SDA - ADDR`). Al darlo vuelta para clavarlo con el sensor de luz mirando hacia arriba, el orden queda invertido verticalmente (`ADDR` en un extremo y `VCC` en el opuesto). Verifica siempre mirando las letras por debajo antes de energizar.

| BH1750 GY-302 | → | Va a… | Color Dupont sugerido |
|---|:---:|---|---|
| `VCC` | → | Riel `+` (`3V3`) | 🔴 **Rojo** |
| `GND` | → | Riel `-` (`GND`) | ⚫ **Negro** |
| `SCL` | → | `GPIO 22` (mismo nodo `SCL` que el AHT20+BMP280) | 🟡 **Amarillo** |
| `SDA` | → | `GPIO 21` (mismo nodo `SDA` que el AHT20+BMP280) | 🟢 **Verde** |
| `ADDR` | → | **No conectar** (dejar libre → dirección `0x23`) | ❌ *Sin cable* |

### 5.2 Escanear de nuevo
Con el escáner del Paso 3 ahora aparecen **tres** direcciones: `0x23` (BH1750), `0x38` (AHT20) y `0x77` (BMP280). 🎉 Tres chips distintos compartiendo los mismos 2 cables de datos.

### 5.3 Librería
Instala: `BH1750` de *Christopher Laws*.

### 5.4 Programa (Sensores + Servidor Web Wi-Fi + Serial Monitor)

```cpp
// Paso 5 + Bonus Wi-Fi: AHT20 + BMP280 + BH1750 + Panel Web en Vivo
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>

const char* WIFI_SSID = "TU_NOMBRE_WIFI";
const char* WIFI_PASS = "TU_CLAVE_WIFI";

WebServer server(80);
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp(&Wire);
BH1750 luxometro;

unsigned long ultimaLectura = 0;

void mostrarPaginaWeb() {
  sensors_event_t humedad, temp;
  aht.getEvent(&humedad, &temp);
  bmp.takeForcedMeasurement();
  float temp_bmp = bmp.readTemperature();
  float presion  = bmp.readPressure() / 100.0F;
  float lux      = luxometro.readLightLevel();

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="3">
  <title>Estación Ambiental IoT</title>
  <style>
    body { font-family: system-ui, sans-serif; background: #0f172a; color: #f8fafc; text-align: center; margin: 0; padding: 20px; }
    h1 { font-size: 1.4rem; color: #38bdf8; margin-bottom: 20px; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 15px; max-width: 600px; margin: 0 auto; }
    .card { background: #1e293b; padding: 18px; border-radius: 12px; border: 1px solid #334155; }
    .label { font-size: 0.85rem; color: #94a3b8; }
    .value { font-size: 1.8rem; font-weight: bold; margin-top: 8px; }
    .t1 { color: #fb923c; } .t2 { color: #facc15; } .hum { color: #38bdf8; } .pres { color: #4ade80; } .lux { color: #e879f9; }
    .foot { margin-top: 20px; font-size: 0.8rem; color: #64748b; }
  </style>
</head>
<body>
  <h1>🌱 Estación Ambiental IoT</h1>
  <div class="grid">
    <div class="card"><div class="label">Temp (AHT20)</div><div class="value t1">)rawliteral" + String(temp.temperature, 1) + R"rawliteral( °C</div></div>
    <div class="card"><div class="label">Humedad</div><div class="value hum">)rawliteral" + String(humedad.relative_humidity, 1) + R"rawliteral( %</div></div>
    <div class="card"><div class="label">Presión</div><div class="value pres">)rawliteral" + String(presion, 1) + R"rawliteral( hPa</div></div>
    <div class="card"><div class="label">Luz Ambiental</div><div class="value lux">)rawliteral" + String(lux, 0) + R"rawliteral( lux</div></div>
    <div class="card"><div class="label">Temp (BMP280)</div><div class="value t2">)rawliteral" + String(temp_bmp, 1) + R"rawliteral( °C</div></div>
  </div>
  <div class="foot">Actualizando automáticamente cada 3 segundos</div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Wire.begin(21, 22);

  if (!aht.begin())                         { Serial.println("ERROR AHT20");   while (true) delay(1000); }
  if (!bmp.begin(0x77) && !bmp.begin(0x76)) { Serial.println("ERROR BMP280");  while (true) delay(1000); }
  if (!luxometro.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) { Serial.println("ERROR BH1750"); while (true) delay(1000); }

  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,
                  Adafruit_BMP280::SAMPLING_X1,
                  Adafruit_BMP280::SAMPLING_X1,
                  Adafruit_BMP280::FILTER_OFF);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }

  Serial.print("\n--> Abre en tu navegador: http://");
  Serial.println(WiFi.localIP());

  server.on("/", mostrarPaginaWeb);
  server.begin();
}

void loop() {
  server.handleClient();
  if (millis() - ultimaLectura >= 2000) {
    ultimaLectura = millis();
    sensors_event_t humedad, temp;
    aht.getEvent(&humedad, &temp);
    bmp.takeForcedMeasurement();
    float presion = bmp.readPressure() / 100.0F;
    float lux     = luxometro.readLightLevel();

    Serial.printf("Temp: %.1f C | Hum: %.1f %% | Pres: %.1f hPa | Luz: %.0f lux\n",
                  temp.temperature, humedad.relative_humidity, presion, lux);
  }
}
```

### 5.5 Pruebas y Validación de Campo Real (Santiago de Chile)

| Ambiente probado | Temp (AHT20) | Humedad | Presión | Luz (BH1750) |
|---|:---:|:---:|:---:|:---:|
| **Escritorio interior (noche)** | `24.5 °C` | `63.5 %` | `947.3 hPa` | `60 lux` |
| **Patio exterior (02:20 AM)** | `18.8 °C` | `81.5 %` | `950.4 hPa` | `0 lux` |
| **Dormitorio a oscuras (03:55 AM)** | `21.3 °C` | `79.0 %` | `950.3 hPa` | `0 lux` |

> 🔬 **Lección Metrológica Real (Calibración Cruzada contra Estación Oficial DMC Quinta Normal `330020`):**
> - **Presión (`BMP280`):** Comparado contra la presión de estación (QFE) de Quinta Normal en distintos momentos del día (`953.1`, `950.1`, `948.1 hPa`), el sensor mantuvo una diferencia constante de **~−1.0 a −2.0 hPa** (correspondiente a ~15 m de diferencia de altitud), siguiendo la marea barométrica diurna al décimo de hPa.
> - **Efecto térmico de soldadura en `BMP280`:** Inmediatamente después de repasar una soldadura, el calor desplazó la lectura de presión unos `-6 hPa` (`945.2 hPa`), recuperándose sola en ~15 minutos a medida que el encapsulado MEMS se enfrió y relajó mecánicamente.
> - **Efecto de alcohol / flux en `AHT20`:** El primer sensor limpiado con alcohol isopropílico quedó con un *offset* positivo de **+11.5 % de humedad** (`75.0 %` vs `63.5 %` medido con el segundo módulo AHT20 nuevo soldado en seco). **Nunca aplicar alcohol ni líquidos sobre la ventanilla blanca del AHT20.**

---

## Paso 6 — Mostrar todo en la pantalla OLED (Estación Completa)

### 6.1 Cableado
La pantalla OLED SSD1306 (0.96", 128×64) también va al **mismo bus I2C**. Puedes puentear `SDA` y `SCL` directamente desde los agujeros libres del sensor de luz BH1750.

> 🔌 Desenchufa el USB primero y verifica el orden impreso en tu pantalla (`GND - VCC - SCL - SDA`).

| OLED SSD1306 | → | Va a… | Color Dupont sugerido |
|---|:---:|---|---|
| `GND` | → | Riel `-` (`GND`) | ⚫ **Negro** |
| `VCC` | → | Riel `+` (`3V3`) | 🔴 **Rojo** |
| `SCL` | → | `GPIO 22` (puente desde `SCL` del BH1750) | 🟡 **Amarillo** |
| `SDA` | → | `GPIO 21` (puente desde `SDA` del BH1750) | 🟢 **Verde** |

Con el escáner ahora participan **cuatro** dispositivos en el mismo bus: `0x23` (BH1750), `0x38` (AHT20), `0x3C` (OLED) y `0x77` (BMP280).

### 6.2 Librerías
Instala (con *Install all*):
- `Adafruit SSD1306`
- `Adafruit GFX Library`

### 6.3 Programa Final del Proyecto 1 (OLED + Web Wi-Fi + Serial Monitor)

```cpp
// Paso 6: Estación Ambiental Completa (OLED + Web Wi-Fi + Serial Monitor)
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <BH1750.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA   64
#define RESET_OLED      -1
#define DIRECCION_OLED 0x3C

const char* WIFI_SSID = "TU_NOMBRE_WIFI";
const char* WIFI_PASS = "TU_CLAVE_WIFI";

WebServer server(80);
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp(&Wire);
BH1750 luxometro;
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, RESET_OLED);

unsigned long ultimaActualizacion = 0;

void mostrarPaginaWeb() {
  sensors_event_t humedad, temp;
  aht.getEvent(&humedad, &temp);
  bmp.takeForcedMeasurement();
  float temp_bmp = bmp.readTemperature();
  float presion  = bmp.readPressure() / 100.0F;
  float lux      = luxometro.readLightLevel();

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="3">
  <title>Estación Ambiental IoT</title>
  <style>
    body { font-family: system-ui, sans-serif; background: #0f172a; color: #f8fafc; text-align: center; margin: 0; padding: 20px; }
    h1 { font-size: 1.4rem; color: #38bdf8; margin-bottom: 20px; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 15px; max-width: 600px; margin: 0 auto; }
    .card { background: #1e293b; padding: 18px; border-radius: 12px; border: 1px solid #334155; }
    .label { font-size: 0.85rem; color: #94a3b8; }
    .value { font-size: 1.8rem; font-weight: bold; margin-top: 8px; }
    .t1 { color: #fb923c; } .t2 { color: #facc15; } .hum { color: #38bdf8; } .pres { color: #4ade80; } .lux { color: #e879f9; }
    .foot { margin-top: 20px; font-size: 0.8rem; color: #64748b; }
  </style>
</head>
<body>
  <h1>🌱 Estación Ambiental IoT</h1>
  <div class="grid">
    <div class="card"><div class="label">Temp (AHT20)</div><div class="value t1">)rawliteral" + String(temp.temperature, 1) + R"rawliteral( °C</div></div>
    <div class="card"><div class="label">Humedad</div><div class="value hum">)rawliteral" + String(humedad.relative_humidity, 1) + R"rawliteral( %</div></div>
    <div class="card"><div class="label">Presión</div><div class="value pres">)rawliteral" + String(presion, 1) + R"rawliteral( hPa</div></div>
    <div class="card"><div class="label">Luz Ambiental</div><div class="value lux">)rawliteral" + String(lux, 0) + R"rawliteral( lux</div></div>
    <div class="card"><div class="label">Temp (BMP280)</div><div class="value t2">)rawliteral" + String(temp_bmp, 1) + R"rawliteral( °C</div></div>
  </div>
  <div class="foot">Actualizando automáticamente cada 3 segundos</div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  delay(1500);
  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, DIRECCION_OLED)) {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 25);
  display.println("Iniciando...");
  display.display();

  if (!aht.begin())      Serial.println("ERROR: AHT20");
  if (!bmp.begin(0x77) && !bmp.begin(0x76)) Serial.println("ERROR: BMP280");
  if (!luxometro.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) Serial.println("ERROR: BH1750");

  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,
                  Adafruit_BMP280::SAMPLING_X1,
                  Adafruit_BMP280::SAMPLING_X1,
                  Adafruit_BMP280::FILTER_OFF);

  display.clearDisplay();
  display.setCursor(15, 25);
  display.println("Conectando WiFi...");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }

  Serial.print("\n--> IP: http://");
  Serial.println(WiFi.localIP());

  server.on("/", mostrarPaginaWeb);
  server.begin();
}

void loop() {
  server.handleClient();

  if (millis() - ultimaActualizacion >= 2000) {
    ultimaActualizacion = millis();

    sensors_event_t humedad, temp;
    aht.getEvent(&humedad, &temp);
    bmp.takeForcedMeasurement();
    float presion = bmp.readPressure() / 100.0F;
    float lux     = luxometro.readLightLevel();

    Serial.printf("Temp: %.1f C | Hum: %.1f %% | Pres: %.1f hPa | Luz: %.0f lux\n",
                  temp.temperature, humedad.relative_humidity, presion, lux);

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("ESTACION METEO ESP32");
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    display.setCursor(0, 15); display.printf("Temp: %.1f C", temp.temperature);
    display.setCursor(0, 27); display.printf("Hum : %.1f %%", humedad.relative_humidity);
    display.setCursor(0, 39); display.printf("Pres: %.1f hPa", presion);
    display.setCursor(0, 51); display.printf("Luz : %.0f lux", lux);
    display.display();
  }
}
```

✅ **Proyecto 1 terminado cuando:** la pantalla OLED, el Serial Monitor y la página web en tu celular muestran las 4 mediciones actualizándose en tiempo real.

---

## Herramientas de Simulación y Red Aprendidas

### Simuladores Web Gratuitos Recomendados
1. **Wokwi (`wokwi.com`):** El mejor simulador para **ESP32**, sensores I2C, pantallas OLED y Wi-Fi en tiempo real.
2. **Tinkercad Circuits (`tinkercad.com/circuits`):** Ideal para practicar armado en protoboard 3D y circuitos clásicos (como el temporizador NE555).
3. **Falstad (`falstad.com/circuit`):** Simulador físico interactivo que muestra el flujo de corriente y voltajes en vivo sobre el diagrama esquemático.

### ¿Qué hacer si se reinicia el módem Wi-Fi y cambia la IP del ESP32?
Cuando el router se apaga y enciende, suele asignarle una nueva IP por DHCP al ESP32. Para encontrar la nueva IP desde Windows sin reconectar el ESP32 al PC:
1. En **CMD**, barre las IPs locales para despertar la tabla ARP:
   ```cmd
   for /L %i in (1,1,30) do @ping -n 1 -w 60 192.168.1.%i | findstr /i "TTL="
   ```
2. O consulta la tabla de vecinos actualizada:
   ```cmd
   arp -a | findstr "192.168.1."
   ```

---

## Paso 7 — Repetir en la placa mini (ESP32-C3 Super Mini)

La **ESP32-C3 Super Mini** es mucho más pequeña, ideal para nodos fijos (dormitorio, exterior). La idea es la misma, pero cambian **3 cosas**.

### 7.1 Diferencias con el DevKit

| Tema | DevKit 30P | ESP32-C3 Super Mini |
|---|---|---|
| Placa en Arduino IDE | `ESP32 Dev Module` | **`ESP32C3 Dev Module`** |
| Mensajes por USB | Funcionan solos | Activar **Herramientas → USB CDC On Boot → Enabled** (si no, el Monitor Serie queda en blanco) |
| Pines I2C | SDA = 21, SCL = 22 | **SDA = GPIO 4, SCL = GPIO 5** |
| LED integrado | GPIO 2 | GPIO 8 (se enciende con `LOW`, lógica invertida) |
| Modo programación | Botón `BOOT` | Mantén `BOOT`, pulsa `RESET`, suelta `BOOT` |

### 7.2 Mapa de Coordenadas en Mini Protoboard de 400 puntos (`30 columnas`)

En la mini protoboard de 400 puntos (`columnas 1 a 30`, `filas a–e` arriba y `f–j` abajo, con los rieles `+` = `3.3V` y `−` = `GND` en el borde inferior):

```text
COL:  1  2  3  4  5  6  7  8    11 12 13 14    17 18 19 20      24  25  26  27    29
    ┌────────────────────────┐                                 ┌──────────────┐
a-e │      [ESP32-C3]   (SCL)│                                 │VCC GND D0  A0│──[R1]─►(c29)
=== │◄USB-C   (Canal)    Ant.│ =============================== └──────────────┘ ======= │ ===
f-j │5V GND 3V3 4  3  2  1  0│  [VDD SDA GND SCL] [GND VCC SCK SDA]   MQ-135            │
    └───────────┼──────────┬─┘   (AHT20+BMP280)     (OLED 0.96")                        │
                ▼          └──────────────────────────(Cable de señal desde a29)────────┘
             [Buzzer]
(+) ══════════ 3.3V (Riel Rojo inferior continuo de 1 a 30) ═════════════════════════════════
(−) ══════════ GND  (Riel Azul inferior continuo de 1 a 30) ═════════════════════════════════
```

| Componente | Columnas | Conexiones exactas |
|---|---|---|
| **`ESP32-C3 Super Mini`** | `1 a 8` (`c` y `h`) | En la fila de `5V`: `Col 1 (5V)` → `a24` · `Col 2 (GND)` → Riel `−` · `Col 3 (3V3)` → Riel `+` · `Col 4 (GPIO 4)` → `SDA` (`i12`) · `Col 5 (GPIO 3)` → `Buzzer (+)` (`j5`) · `Col 8 (GPIO 0)` → `a29` (divisor `MQ-135`). En la fila opuesta: `Col 8 (GPIO 5)` → `SCL` (`i14`). |
| **`AHT20 + BMP280`** | `11 a 14` (`f`) | `j11 (VDD)` → Riel `+` · `i12/h12 (SDA)` → `GPIO 4` y `h20` · `j13 (GND)` → Riel `−` · `i14/h14 (SCL)` → `GPIO 5` y `h19` |
| **`OLED 0.96"`** | `17 a 20` (`f`) | `j17 (GND)` → Riel `−` · `j18 (VCC)` → Riel `+` · `h19 (SCK)` → `h14` · `h20 (SDA)` → `h12` *(actúa además como barrera térmica central)* |
| **`MQ-135` + Divisor `10k/10k`** | `24 a 29` (`a–e`) | `a24 (VCC)` → `5V (Col 1)` · `a25 (GND)` → Riel `−` · `R1 (10k)`: `c27 (A0)` a `c29` · `R2 (10k)`: `b29` a `b25 (GND)` · Señal: `a29` → `GPIO 0 (Col 8)` |
| **`Buzzer Pasivo`** | `Col 5` (`j5`) | Patita `+` en `j5` (`GPIO 3`) y patita `−` directo al Riel Azul `−` (`GND`) |

> ⚠️ En el C3, **evita conectar divisores resistivos a `GPIO 2`, `GPIO 8` y `GPIO 9`** (pines de arranque *strapping*). Por eso el `MQ-135` va en **`GPIO 0`** (`ADC1_CH0`).

### 7.3 Proyecto Modular en Inglés (`d:\00_FisicaModerna\Proyecto_IoT\env_station`)
El código definitivo está modularizado en inglés dentro de `d:\00_FisicaModerna\Proyecto_IoT\env_station\` (compatible con **VS Code + PlatformIO** y con **Arduino IDE**):
- `env_station.ino`: `setup()` y `loop()` principales.
- `config.h`: Selección automática de pines con `#if CONFIG_IDF_TARGET_ESP32C3` (sin costo en memoria compilada).
- `sensors.h`: Lectura de `AHT20`, `BMP280`, `MQ-135` (`ADC_2_5db` + compensación $\times 2$ del divisor `10k/10k` en `ppm`) y alarma sonora en `PIN_BUZZER`.
- `oled_display.h`: Carrusel en **letra grande (`TextSize 2`)** que rota cada 3 s entre `T/H`, `PRESSURE` y `AIR (ppm)`.
- `web_server.h`: Dashboard web auto-refrescable cada 3 s.

### 7.4 Pasos
1. Desenchufa el DevKit. Mueve los cables del sensor a la placa C3.
2. Selecciona **Herramientas → Placa → `ESP32C3 Dev Module`** y activa **USB CDC On Boot**.
3. Elige el nuevo **Puerto**.
4. Sube el programa del Paso 6 con el bloque de pines modificado.

✅ **Listo cuando:** obtienes las mismas lecturas que en el DevKit. 🎉

---

## Problemas comunes y soluciones

| Síntoma | Causa probable | Solución |
|---|---|---|
| No aparece el puerto COM | Cable solo de carga / falta controlador | Otro cable de datos; instalar CP2102 o CH340 |
| `Failed to connect to ESP32` | La placa no entra en modo carga | Mantener `BOOT` mientras sube; probar otro puerto USB |
| Monitor Serie con símbolos raros | Velocidad equivocada | Seleccionar **115200 baud** |
| Monitor Serie vacío (C3) | Falta USB CDC | Activar *USB CDC On Boot* y volver a subir |
| El escáner no encuentra nada | SDA/SCL cruzados, mal contacto, pines sin soldar | Revisar cables, probar otro cable Dupont, revisar soldaduras |
| Encuentra `0x76` y el código falla | Dirección distinta | El código ya prueba ambas (`0x77` y `0x76`) |
| `ERROR BH1750` | Pin `ADDR` conectado o mal contacto | Dejar `ADDR` sin conectar |
| El escáner encuentra `0x38` y `0x77` (y el ID `0x58` se lee bien), pero sale `Temp 0.0` / `Hum 0 %` / `727 hPa` / `nan` | **Un pin suelto o mal soldado (típicamente `GND`)**: alcanza para mensajes cortos, pero se corta cuando el chip mide o envía varios bytes | Revisar cada pin moviéndolo suavemente; repasar la soldadura de ese pin (cautín 2 s + 1 mm de estaño). No basta con medir 3.3 V con el multímetro: la punta presiona el pin y oculta el falso contacto |
| Pines del sensor **sin soldar** (Dupont metido en el agujero) | No hay contacto firme en los 4 pines a la vez | Soldar la regleta de pines; no se puede usar sin soldar |
| Humedad ~10–15 % más alta de lo real tras limpiar con alcohol | El polímero capacitivo del AHT20 absorbió alcohol isopropílico o vapores de flux | No aplicar alcohol sobre la cara del chip; esperar días a que evapore o aplicar corrección de *offset* en código (`humedad - 11.5`) tras comparar con un segundo módulo nuevo |
| La presión del BMP280 baja 4–7 hPa justo después de soldar | Estrés térmico y mecánico temporal en la membrana MEMS del barómetro | Esperar 15–20 minutos sin tocarlo: vuelve solo a su valor a medida que se enfría |
| `undefined reference to setup()` y `loop()` al compilar | El archivo `.ino` de la ventana activa de Arduino IDE está vacío (0 bytes) | Pegar el código completo en la ventana activa, presionar `Ctrl+S` y volver a darle a `Upload` |
| La página web Wi-Fi deja de cargar tras reiniciar el módem (`ERR_CONNECTION_TIMED_OUT`) | El router le asignó una nueva IP por DHCP al ESP32 | Buscar la nueva IP con el barrido `ping` + `arp -a`, o implementar `mDNS` (`http://estacion.local`) |
| Humedad siempre `0` o `100` | Sensor húmedo o dañado | Esperar a que se seque; probar otro AHT20 |
| Temperatura 2–4 °C más alta de lo real | Calor de la placa o de la mano | Separar el sensor de la placa con cables más largos y usar `MODE_FORCED` en el BMP280 |
| Un sensor se calienta mucho | `VCC` mal conectado o invertido | **Desenchufar de inmediato** y revisar |
| La pantalla OLED queda en blanco o `NO DETECTADA (0x3C)` pese a estar conectada al riel `+`/`−` | **Rieles de alimentación divididos a la mitad en protoboards largas (830 puntos GL-12)**: el riel metálico interno se interrumpe en la columna ~30 (no hay continuidad entre la mitad izquierda y la derecha) | Medir continuidad con el multímetro entre ambos extremos del riel; colocar **2 puentes** en el centro del riel (`+` con `+` y `−` con `−`) o conectar `VCC`/`GND` en la misma mitad donde entra la energía del ESP32 |
| Un sensor (`BH1750`) se calienta mucho y luego da `Device is not configured! (-2 lux)` | `VCC` (3.3V) conectado por error al pin `ADDR` o polaridad invertida, dañando el chip interno | **Desenchufar USB de inmediato**; siempre cambiar cables con el USB desconectado y dejar `ADDR` libre |
| Error de compilación `No such file` | Falta una librería | Instalar la librería que nombra el error |

> 🔍 **Regla para depurar:** cuando algo falla, vuelve al **escáner I2C** (Paso 3). Si el escáner no ve el sensor, el problema es físico (cables). Si lo ve, el problema es el programa.

---

## Hoja de ruta: de simple a complejo

Cada proyecto reutiliza lo que ya aprendiste. Haz uno a la vez.

| # | Proyecto | Placa | Sensores nuevos | Qué aprendes | Dificultad |
|:---:|---|---|---|---|:---:|
| **1** | ✅ Mini estación ambiental (esta guía) | DevKit 30P | AHT20+BMP280, BH1750, OLED | I2C, librerías, pantalla, validación metrológica | ⭐ |
| **2** | Lo mismo en miniatura | **ESP32-C3** | Los mismos | Soldar C3, cambiar pines I2C (`GPIO 4/5`) | ⭐ |
| **3** | ✅ **Estación Wi‑Fi:** ver datos desde el celular | DevKit 30P | — | Servidor web embebido (`WebServer.h`) + arquitectura `LittleFS` en VS Code | ⭐⭐ |
| **4** | **Detector de lluvia + alarma** | ESP32-C3 (exterior) | Sensor de gotas + buzzer | Entradas digitales/analógicas, alertas | ⭐⭐ |
| **5** | **Registro de datos en MicroSD** (datalogger) | DevKit 30P | Módulo MicroSD SPI | Guardar CSV, reloj por Internet (NTP) | ⭐⭐ |
| **6** | ✅ **Monitor de calidad de aire + Alarma** | DevKit / C3 | MQ-135 (divisor 10k/10k) + Buzzer | Sensores analógicos, Ley de Ohm, carrusel OLED letra grande | ⭐⭐⭐ |
| **6B** | **Tomografía de Resistividad 2D (Wenner + `RES2DINV`)** | DevKit 30P | ADS1115 (16-bit) + resistencia 1 kΩ + 4 electrodos | Inyección bipolar (`D25`/`D26`), $\rho_a = 2\pi a \Delta V/I$, exportación `.dat` para `RES2DINV` / `ResIPy` / `pyGIMLi` | ⭐⭐⭐ |
| **6C** | **Lab. Física Moderna y Teledetección (`Planck h`, `NDVI`, `Muones`)** | DevKit / XIAO | ADS1115 + LEDs ($470\text{–}940\text{ nm}$) + `HW-201` + `OV2640` | Medición experimental de la Constante de Planck ($h$), radiómetro espectral `NDVI` ($\text{NIR}/\text{Red}$) y rayos cósmicos relativistas ($\tau = \gamma\tau_0$) | ⭐⭐⭐⭐ |
| **7** | **Red de 2–3 nodos con MQTT** | DevKit + 2× C3 | — | Comunicar placas entre sí | ⭐⭐⭐ |
| **8** | **Sismógrafo básico** | ESP32-S3 | ADXL345 (SPI) | Muestreo rápido, señal, STA/LTA | ⭐⭐⭐⭐ |
| **9** | **IA en el borde (TinyML)** | ESP32-S3 / XIAO | Los anteriores | Entrenar un modelo y correrlo en la placa | ⭐⭐⭐⭐⭐ |

Los proyectos **7** y **8** corresponden a los documentos `proyectos_GeofisicaAmbiental_v1.md` y `proyecto_IA_TinyML_v1.md`. Los proyectos **1 a 6** son la escalera para llegar ahí sin frustrarte.

### Recomendación de próximo paso
Con el Proyecto 1 + Servidor Web Wi-Fi validados, el siguiente paso profesional es:
1. Migrar el entorno a **VS Code + PlatformIO** y separar el HTML/CSS/JS en la carpeta `data/` usando **LittleFS** y **mDNS** (`http://estacion.local`).
2. Pasar al **Proyecto 2 (ESP32-C3 Super Mini)** o **Proyecto 4 (Detector de lluvia)**.

---

## Intercambios de hardware respecto a los proyectos anteriores

En los documentos previos algunos componentes tenían un destino fijo. Para aprender se **reasignan temporalmente**, sin comprar nada:

| Componente | Destino original | Uso en esta guía | Impacto |
|---|---|---|---|
| **ESP32 DevKit 30P** | Nodo remoto sísmico | Placa principal de aprendizaje | Ninguno: se puede reasignar cuando llegue el momento |
| **ESP32-C3 Super Mini** (1 de 2) | Dormitorio | Paso 7 y proyecto 4 (exterior) | Ninguno |
| **AHT20+BMP280** (2 de 3) | Hub / Nodos | Proyecto 1 (Sensor #1 con offset +11.5% RH documentado; Sensor #2 validado como patrón) | Queda 1 virgen adicional en caja |
| **BH1750** (2 uds) | Hub / Exterior | Sensor #1 dañado por sobretemperatura accidental; Sensor #2 pendiente de soldar | Se usará el #2 en la estación |
| **OLED** (1 de 2) | Hub | Proyecto 1 (validada mostrando datos en vivo en `0x3C`) | Queda 1 libre |
| **ESP32-S3 N16R8** | Hub central | Sin tocar hasta el proyecto 8 | — |

> 📌 Nada se pierde ni se daña: al terminar un proyecto desenchufas, guardas los módulos en el maletín y los reusas.

### Compras pendientes que todavía **no** son necesarias
- **TFT ST7789** y las **2 MicroSD adicionales** solo hacen falta desde el proyecto 5 en adelante.

---

> **v1.3 (21-Oct-2026)** — Estación ambiental + `MQ-135` (divisor 10k/10k) + carrusel OLED en letra grande (`env_station`) validados; agregado mapa de coordenadas para `ESP32-C3 Super Mini` en mini protoboard (400 pts), Tomografía ERT 2D (`RES2DINV`) y Laboratorio de Física Moderna y Teledetección (`Constante de Planck h`, `NDVI`, `Rayos Cósmicos`).
