# 📘 Guía de C++ Embebido (ESP32/Arduino) para Programadores de Python, Java y JavaScript

> **v1.0** — Guía de referencia rápida y puente conceptual. Si ya sabes programar en **Python, Java o JavaScript**, aquí aprenderás exactamente cómo se traducen esos conceptos a **C++ para microcontroladores ESP32**, desglosando todas las funciones, métodos, librerías y patrones de diseño utilizados en este monorepo.

---

## Tabla de Contenidos
1. [La Piedra Rosetta: Equivalencias entre Python, Java, JS y C++](#1-la-piedra-rosetta-equivalencias-clave)
2. [El Modelo Mental de Microcontroladores (Lo que cambia respecto a la PC)](#2-el-modelo-mental-de-microcontroladores)
3. [Tipos de Datos y Memoria en C++](#3-tipos-de-datos-y-memoria-en-c)
4. [Estructura del Programa: `setup()`, `loop()` y Timers no bloqueantes](#4-estructura-del-programa-setup-loop-y-timers-no-bloqueantes)
5. [Desglose de Librerías y Métodos Utilizados en el Proyecto](#5-desglose-de-librerías-y-métodos-utilizados)
   - [5.1 Sistema y Temporización](#51-sistema-y-temporización)
   - [5.2 Pines GPIO y ADC (Conversor Analógico-Digital)](#52-pines-gpio-y-adc)
   - [5.3 Bus I2C (`Wire.h`)](#53-bus-i2c-wireh)
   - [5.4 Sensores de Hardware (`Adafruit_AHTX0`, `Adafruit_BMP280`)](#54-sensores-de-hardware)
   - [5.5 Pantalla OLED (`Adafruit_SSD1306`)](#55-pantalla-oled-adafruit_ssd1306)
   - [5.6 Conectividad Wi-Fi, NTP y OTA (`WiFi.h`, `time.h`, `ArduinoOTA.h`)](#56-conectividad-wi-fi-ntp-y-ota)
   - [5.7 Servidor Web Embebido (`WebServer.h`)](#57-servidor-web-embebido-webserverh)
   - [5.8 Memoria Flash Persistente (`LittleFS.h`)](#58-memoria-flash-persistente-littlefsh)
6. [Patrones de Diseño Aplicados: SOLID (SRP) y KISS](#6-patrones-de-diseño-aplicados-solid-y-kiss)

---

## 1. La Piedra Rosetta: Equivalencias Clave

| Concepto | Python | Java | JavaScript | C++ (ESP32/Arduino) |
|---|---|---|---|---|
| **Imprimir en consola** | `print(f"X: {x}")` | `System.out.println("X: " + x);` | `console.log(\`X: ${x}\`);` | `Serial.printf("X: %d\n", x);` |
| **Estructura de datos** | `dict` / `dataclass` | `class` con atributos | `Object` (`{}`) | `struct Nombre { ... };` |
| **Colección de datos** | `list` (`[1, 2, 3]`) | `ArrayList<Integer>` | `Array` (`[1, 2, 3]`) | Arreglo fijo: `int arr[3];` |
| **Tiempo actual** | `time.time()` | `System.currentTimeMillis()` | `Date.now()` | `millis()` (milisegundos desde boot) |
| **Pausar ejecución** | `time.sleep(1)` | `Thread.sleep(1000)` | `await sleep(1000)` | `delay(1000)` (⚠️ ¡bloqueante!) |
| **Manejo de texto** | `str` | `String` | `string` | `char[]` (rápido) o `String` (objeto) |
| **Constante fija** | `PI = 3.14` | `final double PI = 3.14;` | `const PI = 3.14;` | `const float PI = 3.14;` o `#define` |
| **Modularidad / Import** | `import modulo` | `import paquete.*;` | `import { x } from './m';` | `#include "modulo.h"` |

---

## 2. El Modelo Mental de Microcontroladores

En una PC con Python, Java o Node.js tienes **gigabytes de RAM**, un Sistema Operativo (Windows/Linux) que gestiona la memoria, y un recolector de basura (*Garbage Collector*) que limpia variables no usadas.

En un **ESP32**:
1. **La RAM es finita y pequeña:** Tienes unos $300\text{ a }400\text{ KB}$ de RAM disponible.
2. **No hay Garbage Collector:** Si creas y destruyes muchos objetos de texto grandes (`String`), la memoria se fragmenta (*memory leak / heap fragmentation*) y el microcontrolador se reinicia solo.
3. **Por qué usamos arreglos estáticos circulares:**
   En vez de un `list.append()` de Python que crece infinitamente hasta agotar la RAM, creamos un arreglo de tamaño fijo en RAM:
   ```cpp
   #define MAX_HISTORY_RECORDS 288
   HistoryRecord historyBuffer[MAX_HISTORY_RECORDS]; // Ocupa exactamente 4.6 KB fijos
   ```
   Al llegar al elemento 288, vuelve al índice 0 con el operador módulo: `(head + 1) % MAX_HISTORY_RECORDS`.

---

## 3. Tipos de Datos y Memoria en C++

En microcontroladores es vital saber exactamente cuántos bits ocupa cada variable:

| Tipo C++ | Tamaño | Rango | Equivalente en Python/Java |
|---|:---:|:---:|---|
| **`bool`** | 1 byte | `true` o `false` | `bool` / `boolean` |
| **`int8_t`** / **`uint8_t`** | 1 byte (8 bits) | $-128..127$ / $0..255$ | `byte` |
| **`int16_t`** / **`uint16_t`** | 2 bytes (16 bits) | $-32.768..32.767$ / $0..65.535$ | `short` |
| **`int`** / **`int32_t`** | 4 bytes (32 bits) | $\approx \pm 2.147.000.000$ | `int` |
| **`uint32_t`** / **`unsigned long`** | 4 bytes (32 bits) | $0 \text{ a } 4.294.967.295$ | Entero positivo (ideal para `millis()`) |
| **`float`** | 4 bytes | Decimal simple precisión ($\sim 6$ dígitos) | `float` |
| **`double`** | 8 bytes | Decimal doble precisión ($\sim 15$ dígitos) | `float` (Python) / `double` (Java) |

### ¿Qué significan los símbolos `*` y `&`?
* **El operador `&` (Referencia o Dirección):**
  En Python y Java todos los objetos se pasan por referencia automáticamente. En C++, si pasas un objeto grande como parámetro sin `&`, C++ hace una copia completa en memoria.
  Escribir `void updateStats(const SensorReadings& r)` significa: *"Léeme la estructura original directamente, no gastes RAM haciendo una copia"*.
* **El operador `*` (Puntero):**
  Guarda la dirección de memoria de una variable o periférico (por ejemplo `Adafruit_BMP280 bmp280(&Wire);` significa *"dale a la librería el acceso al bus I2C físico `Wire`"*).

---

## 4. Estructura del Programa: `setup()`, `loop()` y Timers no bloqueantes

Todo sketch de Arduino/ESP32 tiene exactamente dos funciones principales:

```cpp
void setup() {
  // Se ejecuta UNA SOLA VEZ al encender o reiniciar el microcontrolador.
  // Aquí se inicializan sensores, puertos, Wi-Fi y memoria.
}

void loop() {
  // Se ejecuta EN BUCLE INFINITO, millones de veces por segundo, mientras tenga energía.
}
```

### El Gran Pecado: Usar `delay()`
En Python harías `time.sleep(5)`. Pero si haces `delay(5000)` en un ESP32:
* El procesador se congela completamente durante 5 segundos.
* El servidor web **deja de responder peticiones**.
* La pantalla OLED **no se actualiza**.
* Las alertas del buzzer **se bloquean**.

### La Solución Profesional: La técnica de `millis()` (Timers no bloqueantes)
Es exactamente equivalente a un `setInterval()` de JavaScript:

```cpp
unsigned long lastReadMs = 0;
const unsigned long INTERVAL = 2000; // 2 segundos

void loop() {
  server.handleClient(); // Atiende la web en cada milisegundo

  unsigned long now = millis();
  if (now - lastReadMs >= INTERVAL) {
    lastReadMs = now;
    readSensors(); // Solo se ejecuta cuando han pasado 2 segundos
  }
}
```

---

## 5. Desglose de Librerías y Métodos Utilizados

A continuación tienes el catálogo de todas las funciones que dan vida a tu estación:

### 5.1 Sistema y Temporización

* **`Serial.begin(115200);`**
  Inicializa la comunicación serie digital por USB a 115.200 baudios.
* **`Serial.printf("Temp: %.1f C\n", temp);`**
  Imprime texto formateado con marcadores (`%d` para enteros, `%.1f` para decimales, `%s` para texto).
* **`millis();`**
  Retorna un número entero (`uint32_t`) con los milisegundos transcurridos desde que la placa encendió. Tarda $\approx 50$ días en desbordarse a cero.
* **`snprintf(buffer, sizeof(buffer), "formato", ...);`**
  Construye cadenas de texto de forma segura sin peligro de desbordar la memoria RAM.

---

### 5.2 Pines GPIO y ADC (Conversor Analógico-Digital)

* **`pinMode(pin, modo);`**
  Configura un pin físico: `INPUT` (leer sensores), `OUTPUT` (actuadores/buzzer).
* **`digitalWrite(pin, estado);`**
  Escribe `HIGH` (3.3V) o `LOW` (0V) en un pin digital.
* **`analogRead(PIN_MQ135);`**
  Lee el conversor de 12 bits del ESP32. Entrega un valor entero entre $0$ ($0\text{ V}$) y $4095$ ($3.3\text{ V}$).
* **`analogReadMilliVolts(pin);`**
  Función avanzada del ESP32: lee la calibración interna de fábrica grabada en los fusibles eFuse y entrega el valor directamente en **milivoltios reales** ($\text{mV}$), eliminando la no-linealidad clásica del ADC.
* **`analogSetPinAttenuation(pin, ADC_2_5db);`**
  Ajusta la ganancia del amplificador interno del ADC. Con `ADC_2_5db`, el rango óptimo de medición es de $0\text{ a }1.5\text{ V}$ con máxima resolución.

---

### 5.3 Bus I2C (`Wire.h`)

El bus I2C permite conectar decenas de sensores usando solo dos cables: **SDA** (datos) y **SCL** (reloj).

* **`Wire.begin(SDA_PIN, SCL_PIN);`**
  Inicia el hardware del bus I2C en los pines asignados (`GPIO 4` y `GPIO 5` en el ESP32-C3).
* **`Wire.setClock(100000);`**
  Fija la velocidad del bus a $100\text{ kHz}$ (estándar I2C, ideal para evitar ruido en protoboards).
* **`isI2CDevicePresent(address);`**
  Envía un pulso `Wire.beginTransmission(address)` y comprueba con `Wire.endTransmission() == 0` si el chip físico respondió. Es lo que hace el escáner I2C.

---

### 5.4 Sensores de Hardware

#### AHT20 (Temperatura y Humedad - Asair)
* `#include <Adafruit_AHTX0.h>`
* **`aht20.begin()`:** Despierta el sensor y calibra los registros internos. Retorna `true` si se comunica en la dirección `0x38`.
* **`aht20.getEvent(&humEvent, &tempEvent);`:** Llena simultáneamente dos estructuras con la temperatura en $^\circ\text{C}$ (`tempEvent.temperature`) y humedad relativa en $\%$ (`humEvent.relative_humidity`).

#### BMP280 (Presión Barométrica - Bosch)
* `#include <Adafruit_BMP280.h>`
* **`bmp280.begin(0x77)`:** Inicia el barómetro en la dirección `0x77` (o `0x76`).
* **`bmp280.setSampling(...)`:** Configura el sobremuestreo y filtros.
  * **Modo utilizado: `MODE_FORCED`:** El sensor duerme a $0\text{ mA}$ y solo despierta por unos microsegundos cuando se le pide una lectura, **eliminando el autocalentamiento**.
* **`bmp280.takeForcedMeasurement()`:** Ordena tomar una medición instantánea.
* **`bmp280.readPressure() / 100.0f`:** Lee la presión barométrica en Pascales y la divide por 100 para entregar **hectopascales ($\text{hPa}$)**.

---

### 5.5 Pantalla OLED (`Adafruit_SSD1306`)

* `#include <Adafruit_SSD1306.h>`
* **`display.begin(SSD1306_SWITCHCAPVCC, 0x3C)`:** Enciende la bomba de carga interna del OLED para alimentar los pixeles orgánicos.
* **`display.clearDisplay()`:** Borra el búfer gráfico en la RAM.
* **`display.setTextSize(1 o 2)`:** Tamaño de fuente ($1 = 6\times 8\text{ px}$, $2 = 12\times 16\text{ px}$).
* **`display.setCursor(x, y)`:** Ubica el cursor en coordenadas cartesianas ($x=0..127$, $y=0..63$).
* **`display.drawLine(x0, y0, x1, y1, SSD1306_WHITE)`:** Dibuja líneas para los gráficos dinámicos del carrusel.
* **`display.display()`:** **¡El método clave!** Envía todos los píxeles calculados en la RAM hacia el chip físico del OLED por I2C de un solo golpe.

---

### 5.6 Conectividad Wi-Fi, NTP y OTA

* `#include <WiFi.h>`
  * **`WiFi.mode(WIFI_STA);`:** Modo estación (se conecta a tu router, no crea red propia).
  * **`WiFi.setTxPower(WIFI_POWER_8_5dBm);`:** Reduce la potencia de transmisión RF en el ESP32-C3 para evitar saturación de antena por reflexión en la protoboard.
  * **`WiFi.status() == WL_CONNECTED`:** Retorna si la conexión Wi-Fi está activa.
* `#include <time.h>` (Sincronización NTP oficial de Chile)
  * **`configTzTime("CLT4CLST,M9.1.0/0,M4.1.0/0", "ntp.shoa.cl", "pool.ntp.org");`:**
    Configura la zona horaria chilena con cambio de hora automático (verano/invierno) consultando los servidores atómicos del Servicio Hidrográfico y Oceanográfico de la Armada (SHOA).
  * **`getLocalTime(&timeinfo, timeout)`:** Obtiene año, mes, día, hora, minuto y segundo exactos.
* `#include <ArduinoOTA.h>` (Actualización inalámbrica por el aire)
  * **`ArduinoOTA.setHostname("env-station");`**
  * **`ArduinoOTA.begin();`**
  * **`ArduinoOTA.handle();`:** Debe llamarse en cada iteración del `loop()` para escuchar si la PC le está enviando un nuevo firmware por Wi-Fi.

---

### 5.7 Servidor Web Embebido (`WebServer.h`)

* `#include <WebServer.h>`
* **`WebServer server(80);`:** Instancia el servidor HTTP en el puerto estándar 80.
* **`server.on("/ruta", funcionHandler);`:** El equivalente exacto a `@app.route('/ruta')` de Flask (Python) o `app.get('/ruta', callback)` de Express (Node.js).
* **`server.send(codigoHttp, tipoContenido, cuerpo);`:** Envía la respuesta al cliente (ej. `200`, `"application/json"`, `jsonString`).
* **`server.send_P(200, "text/html", INDEX_HTML);`:** Envía el archivo HTML almacenado en la memoria de programa (`PROGMEM`), sin gastar un solo byte de memoria RAM.
* **`server.streamFile(f, "text/csv");`:** Lee un archivo de la memoria Flash en bloques y lo transmite directo por el socket de red sin cargarlo todo en RAM.
* **`server.handleClient();`:** Procesa las peticiones entrantes.

---

### 5.8 Memoria Flash Persistente (`LittleFS.h`)

* `#include <LittleFS.h>`
* **`LittleFS.begin(true);`:** Monta el sistema de archivos de la Flash interna. Si está virgen o corrupto, el argumento `true` lo formatea automáticamente.
* **`LittleFS.exists("/history.csv");`:** Comprueba si el archivo existe.
* **`LittleFS.open("/history.csv", "a");`:** Abre el archivo en modo *Append* (anexar al final sin borrar lo anterior).
* **`LittleFS.open("/history.csv", "r");`:** Abre el archivo en modo lectura (*Read*).
* **`LittleFS.remove("/history.csv");`:** Borra físicamente el archivo para vaciar la memoria.
* **`f.size();`:** Retorna el tamaño exacto del archivo en bytes.

---

## 6. Patrones de Diseño Aplicados: SOLID y KISS

Para evitar que el código sea un monolito desordenado de 2.000 líneas en un solo archivo `.ino`, la arquitectura se dividió bajo el principio de **Responsabilidad Única (SRP)**:

```text
env_station.ino        --> Solo orquesta el setup y el loop temporal
├── config.h           --> Solo define pines y constantes de hardware
├── time_service.h     --> Solo sincroniza NTP y entrega strings horarios
├── sensors.h          --> Solo lee los chips (AHT20, BMP280, MQ-135)
├── alert_manager.h    --> Solo evalúa umbrales y hace sonar el buzzer
├── stats_logger.h     --> Solo calcula Mín/Máx/Prom y guarda en LittleFS
├── oled_display.h     --> Solo dibuja en la pantalla física
├── index_html.h       --> Solo contiene la plantilla web HTML5/CSS/JS
└── web_server.h       --> Solo gestiona las rutas HTTP y la API REST
```

**Beneficio práctico:** Si el día de mañana cambias el sensor `MQ-135` por el `ENS160+AHT21`, **únicamente modificas `sensors.h`**. La pantalla OLED, la web, las alertas y el guardado en Flash siguen funcionando sin tocar una sola línea de su código. Eso es programación modular limpia.