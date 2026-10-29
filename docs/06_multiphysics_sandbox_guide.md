# 🧪 Guía Práctica de Construcción y Montaje: Terrario Multifísico de Laboratorio (ERT + GPR + Sísmica)

> **Documento:** `docs/06_multiphysics_sandbox_guide.md`  
> **Versión:** `v1.0` (28 de Octubre de 2026)  
> **Propósito:** Manual de ingeniería experimental y taller para construir, calibrar y operar un modelo físico análogo (*sandbox*) de escala reducida para prospección geofísica combinada: **Tomografía de Resistividad Eléctrica (ERT)**, **Georradar de Microondas (GPR)** y **Sísmica Activa de Refracción y Ondas Elásticas**.

---

## 📑 Tabla de Contenidos

1. [Filosofía del Modelado Físico Análogo (*Sandbox Modeling*)](#1-filosofía-del-modelado-físico-análogo-sandbox-modeling)
2. [Lista Completa de Materiales y Herramientas](#2-lista-completa-de-materiales-y-herramientas)
3. [Paso a Paso de Construcción del Terrario](#3-paso-a-paso-de-construcción-del-terrario)
4. [Montaje y Conexión de Electrodos y Sensores por Método](#4-montaje-y-conexión-de-electrodos-y-sensores-por-método)
5. [Plano de Distribución Espacial y Coordenadas (X-Y-Z)](#5-plano-de-distribución-espacial-y-coordenadas-x-y-z)
6. [Protocolo de Calibración y Checklist Previo a las Mediciones](#6-protocolo-de-calibración-y-checklist-previo-a-las-mediciones)

---

## 1. Filosofía del Modelado Físico Análogo (*Sandbox Modeling*)

En geofísica profesional, antes de perforar un pozo o interpretar una inversión tomográfica compleja, los laboratorios universitarios y centros de investigación recurren al **modelado análogo a escala**. Un recipiente controlado con arena y objetos de propiedades conocidas permite:

* **Eliminar la incertidumbre de la "caja negra":** Sabes exactamente la forma, profundidad ($z$) y posición ($x$) de los cuerpos enterrados.
* **Probar algoritmos de inversión matemática (`RES2DINV`, `pyGIMLi`, `ResIPy`):** Permite cuantificar el error cuadrático medio (RMS %) y evaluar qué arreglo de electrodos recupera la geometría real con mayor fidelidad.
* **Integración Multifísica Simultánea:** Cada método geofísico excita una propiedad física independiente del mismo medio:

```text
┌─────────────────────────────────────────────────────────────────────────────────┐
│                    EL TERRARIO MULTIFÍSICO — 3 CAMPOS FÍSICOS                   │
├───────────────────────┬─────────────────────────┬───────────────────────────────┤
│ MÉTODO GEOFÍSICO      │ PROPIEDAD FÍSICA MEDIDA │ FENÓMENO DETECTADO            │
├───────────────────────┼─────────────────────────┼───────────────────────────────┤
│ 1. Tomografía ERT 2D  │ Resistividad (ρ, Ω·m)   │ Conducción iónica de corriente│
│    (Proyecto 02)      │ y Conductividad (σ)     │ en poros saturados con agua   │
├───────────────────────┼─────────────────────────┼───────────────────────────────┤
│ 2. Georradar GPR 3GHz │ Permitividad Dieléctrica│ Reflexión y refracción de     │
│    (RCWL-0516)        │ Relativa (ε_r)          │ ondas electromagnéticas       │
├───────────────────────┼─────────────────────────┼───────────────────────────────┤
│ 3. Sísmica de Refrac. │ Velocidad de Onda P/S   │ Módulo elástico (rigidez) y   │
│    (Proyecto 06)      │ (V_P, V_S, m/s)         │ densidad del empaquetamiento  │
└───────────────────────┴─────────────────────────┴───────────────────────────────┘
```

---

## 2. Lista Completa de Materiales y Herramientas

Todos los elementos son de bajo costo y fáciles de conseguir en ferreterías (Sodimac/Easy), librerías y tiendas de modelismo en Chile:

### 2.1 Contenedor y Matriz Geológica
* **1× Caja Plástica Organizadora Transparente:**
  * Largo: **$45\text{ a }60\text{ cm}$**
  * Ancho: **$20\text{ a }25\text{ cm}$**
  * Altura / Profundidad: **$15\text{ a }20\text{ cm}$**
  * *Recomendación:* Plástico liso y rígido (polipropileno transparente). La transparencia permite verificar visualmente la posición y contacto de los cuerpos enterrados.
* **10 a 15 kg de Arena Fina:**
  * Arena de cuarzo para acuarios o arena fina de construcción lavada (seca o ligeramente húmeda).
  * Debe estar libre de tierra orgánica, piedras grandes o trozos de madera que generen ruido no controlado.
* **Plancha de Goma Eva (Foam) o Esponja Delgada ($5\text{ mm}$ de espesor):**
  * Para forrar las paredes interiores de la caja y actuar como **filtro de absorción acústica** (evita rebotes en sísmica).

### 2.2 Electrodos y Accesorios ERT
* **16× Varillas de Acero Inoxidable 316L (o Rayos de Bicicleta Inoxidables):**
  * Diámetro: $1.5\text{ a }2.0\text{ mm}$.
  * Largo cortado: **$8\text{ a }10\text{ cm}$** (se clavan $2.5\text{ cm}$ en la arena y quedan $5.5\text{ cm}$ libres para caimanes).
  * *Alternativa rápida inicial:* 16 clavos de cobre o acero galvanizado de $3"$ (aunque el inox es muy superior para evitar corrosión galvánica).
* **1× Regla Guía / Peine Espaciador:**
  * Una tira de madera, acrílico o cartón prensado de $50\text{ cm}$ de largo $\times 3\text{ cm}$ de ancho con 16 orificios taladrados a **$a = 2.0\text{ cm}$ o $2.5\text{ cm}$** exactos entre centros.
* **Cables de Prueba con Caimanes (Alligator Clips):**
  * 4 a 8 cables con pinzas de caimán pequeñas para sujetar firmemente a la cabeza de las varillas.

### 2.3 Anomalías Geológicas Controladas para Enterrar
1. **Anomalía Resistiva A (Simula Cavidad / Falla Seca / Roca Ígnea):**
   * Un trozo de **tubo de PVC de $1/2"$ o $3/4"$** de $10\text{ cm}$ de largo con tapas plásticas en ambos extremos (cilindro hueco hermético), o un bloque de madera seca barnizada de $5 \times 5 \times 8\text{ cm}$.
   * *Respuesta esperada:* Resistividad ultra alta ($\rho \gg \rho_{\text{arena}}$), reflector potente en radar GPR, zona de sombra acústica en sísmica.
2. **Anomalía Conductora B (Simula Acuífero Salino / Yacimiento Metálico):**
   * Una esponja doméstica pequeña ($4 \times 4 \times 8\text{ cm}$) empapada en salmuera (agua saturada con sal de mesa), o un perfil/barra rectangular de aluminio o cobre.
   * *Respuesta esperada:* Resistividad casi nula ($\rho \ll \rho_{\text{arena}}$), fuerte absorción electromagnética en GPR.
3. **Estrato Rápido Basal C (Para Refracción Sísmica):**
   * Una baldosa de cerámica gruesa, ladrillo macizo plano o bloque de granito/mármol de $20 \times 10\text{ cm}$ para poner en el fondo de la caja.
   * *Respuesta esperada:* $V_2 \approx 2000\text{--}3000\text{ m/s}$ frente a la arena lenta $V_1 \approx 140\text{ m/s}$.

---

## 3. Paso a Paso de Construcción del Terrario

### Paso 1: Instalación de Bordes Absorbentes Sísmicos
1. Limpia y seca el interior de la caja plástica organizadora.
2. Corta tiras de goma eva o esponja delgada de $12\text{ cm}$ de altura.
3. Pega las tiras con cinta doble faz o silicona fría en las **4 paredes perimetrales interiores** de la caja (no en el fondo).
   * *Propósito geofísico:* Simula la condición matemática de **semi-espacio infinito**, absorbiendo la energía elástica que choca contra las paredes de plástico para que no genere ecos parásitos en los sismómetros.

---

### Paso 2: Instalación del Estrato Basal (Sísmica de Refracción)
1. Si vas a realizar experimentos de refracción sísmica bicomponente, coloca la **baldosa cerámica o ladrillo plano** directamente en el centro del fondo de la caja.
2. Si vas a realizar únicamente tomografía eléctrica ERT y radar GPR en medio homogéneo, omite este paso y deja el fondo libre.

---

### Paso 3: Preparación y Humectación de la Matriz de Arena

> [!CAUTION]
> **Regla de Oro:** La arena debe humedecerse de forma **completamente homogénea antes de meterla a la caja**. Si echas agua con sal directamente en la caja sobre los electrodos, el líquido formará caminos preferenciales (canales de lixiviación) y arruinará la homogeneidad de la tomografía.

1. En un balde limpio, coloca 5 kg de arena.
2. Prepara una solución acuosa ligeramente conductora: **1 litro de agua de la llave con 1/2 cucharadita de sal fina de cocina ($2\text{ a }3\text{ gramos}$)** bien disuelta.
3. Rocía la solución gradualmente sobre la arena mientras la mezclas con una pala o con las manos.
4. **Punto óptimo de humedad ("Prueba del Puño"):**
   * Toma un puñado de arena y apriétalo con fuerza.
   * Debe formar un bloque compacto que conserve la forma sin desmoronarse, **pero no debe gotear agua entre tus dedos** ($3\%\text{ a }5\%$ de humedad gravimétrica).
   * Esto entrega una resistividad de matriz ideal y estable: $\rho_{\text{matriz}} \approx \mathbf{40\text{ a }70\ \Omega\cdot\text{m}}$.
   * **Compatibilidad con GPR (El "Punto Dulce" Dieléctrico):** Con solo $3\text{--}5\%$ de humedad, la atenuación a $3.18\text{ GHz}$ es de apenas $\approx 15\text{ dB/m}$. En el terrario ($z = 3.5\text{ cm}$, viaje total ida y vuelta de $7\text{ cm}$), la pérdida es inferior a **$1.1\text{ dB}$** (totalmente despreciable). Además, la humedad eleva la permitividad de la arena a $\varepsilon_r \approx 6\text{--}8$, lo que **triplica el contraste de reflexión ($R$) contra el tubo de PVC hueco** respecto a la arena seca.
   * *Ruta en dos etapas:* Si deseas ver la respuesta del GPR y la sísmica en estado puramente seco primero, puedes realizar los ensayos con la arena seca y luego humedecerla uniformemente para activar el ERT.
5. Vierte una primera capa de **$5\text{ cm}$ de arena** en el terrario y compáctala suavemente con una tabla plana para eliminar burbujas de aire grandes.

---

### Paso 4: Enterrado Estratégico de las Anomalías

Coloca los objetos anómalos a una profundidad controlada ($z = 3\text{ a }4\text{ cm}$ bajo la superficie final):

```text
  X = 0 cm                                                          X = 50 cm
  ┌────────────────────────────────────────────────────────────────────────┐
  │                           SUPERFICIE DE ARENA                          │
  │   e1    e2    e3    e4    e5    e6    e7    e8    e9   e10   e11   ... │
  │   ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼     ▼        │
  │                                                                        │
  │               ┌───────────────┐               ┌───────────────┐        │  Profundidad
  │               │   TUBO PVC    │               │ ESPONJA SALINA│        │  z ≈ 3.5 cm
  │               │  (Resistivo)  │               │  (Conductor)  │        │
  │               └───────────────┘               └───────────────┘        │
  │                 X = 14-18 cm                    X = 30-34 cm           │
  │                                                                        │
  │ ══════════════════════════════════════════════════════════════════════ │  Fondo
  └────────────────────────────────────────────────────────────────────────┘
```

1. **Posición de Anomalía A (Tubo PVC cerrado):** Colócalo transversalmente a la línea de electrodos, centrado en $X \approx 16\text{ cm}$ (justo bajo los electrodos $e_6\text{--}e_8$).
2. **Posición de Anomalía B (Esponja con salmuera):** Centrada en $X \approx 32\text{ cm}$ (bajo los electrodos $e_{12}\text{--}e_{14}$).
3. Cubre las anomalías con una segunda capa de arena humedecida hasta alcanzar una altura total de **$10\text{ a }12\text{ cm}$**.
4. Nivela la superficie con una regla para dejar un plano horizontal perfecto.

---

### Paso 5: Montaje y Alineación de los 16 Electrodos ERT
1. Coloca la regla guía / peine espaciador sobre la superficie de la arena, alineada con el eje longitudinal del terrario.
2. Clava las **16 varillas de acero inoxidable** a través de los orificios del peine:
   * **Espaciamiento unitario:** $a = 2.0\text{ cm}$ o $2.5\text{ cm}$.
   * **Profundidad de clavado:** Exactamente **$2.5\text{ cm}$** dentro de la arena.
   * Deja libres los restantes $5.5\text{ cm}$ superiores de cada varilla para conectar los caimanes.
3. Etiqueta con cinta adhesiva las varillas del **1 al 16** de izquierda a derecha.

---

## 4. Montaje y Conexión de Electrodos y Sensores por Método

### 4.1 Configuración ERT (Resistivímetro 2D - Proyecto 02)

Para las primeras mediciones manuales con caimanes (antes de conectar multiplexores automáticos):

```text
       [ ESP32 DevKit 30P ] ── GPIO 25 (Inj+) ──► Caimán A ──► Clavo e1
                            ── GPIO 26 (Inj-) ──► Resistencia Shunt Rs ──► Caimán B ──► Clavo e4
                                                        ▲
       [ ADS1115 (16-bit) ] ── Canales A2-A3 ───────────┘ (Mide caída en Rs -> Corriente I)
                            ── Canales A0-A1 ──► Caimán M ──► Clavo e2 (Mide ΔV_MN)
                                             ──► Caimán N ──► Clavo e3
```

* **Comprobación con multímetro:**  
  Pon el multímetro en escala de resistencia ($\Omega$) y mide entre dos clavos contiguos ($e_1$ y $e_2$). Debe marcar entre **$200\ \Omega$ y $600\ \Omega$**. Si marca $> 2\,000\ \Omega$, la arena está demasiado seca; rocía una leve bruma de agua sobre la superficie.

---

### 4.2 Configuración GPR (Radar Microondas 3.18 GHz - RCWL-0516)

El radar de microondas puede operarse en dos modalidades, según el nivel de intervención deseado:

#### Opción A: Modo Estándar de Fábrica (0% Modificación Física — Recomendado para Partir)
No requiere soldar ni alterar en absoluto el sensor:
* **Conexión:** Pines estándar `VIN` a 5V (o 3.3V), `GND` a GND y `OUT` a un pin digital con interrupción (`GPIO 34` o `GPIO 35` del ESP32).
* **Principio de Operación:** El pin digital `OUT` entrega pulsos lógicos cuando el campo de microondas detecta variaciones de fase.
  - Al deslizar el sensor a velocidad constante sobre la arena limpia, el tren de pulsos es bajo o nulo.
  - Al cruzar por encima del tubo de PVC hueco o placa metálica, la fuerte anomalía dieléctrica incrementa la **tasa de disparos por segundo ($pulsos/cm$)** o genera un nivel `HIGH` sostenido.
* **Mapeo:** El ESP32 grafica la densidad de disparos vs. posición $x$, ubicando la anomalía sin riesgo de dañar el módulo.

#### Opción B: Modo Hack Analógico (Modificación Leve de 1 Cable — Opcional)
Si se desea capturar la forma de onda senoidal Doppler continua:
* **Punto de Conexión:** Se suelda un único cable fino (30 AWG) a la **patita 12 del chip RCWL-9196** (o al pad `C-TM`), que lleva la señal de frecuencia intermedia (IF cruda) antes del comparador digital.
* **Entrada ADC:** Se conecta este cable a un canal analógico del ESP32 (`ADC1_CH6` / `GPIO 34`).
* **Operación:** Desplazando el sensor en el riel, se digitaliza la señal analógica a $1\text{ kHz}$ obteniendo la envolvente de amplitud continua de microondas.

```text
       Placa RCWL-0516 (Sin modificar o modificada)
       ┌──────────────────────────────────────┐
       │   [Antena microondas en PCB]         │
       │                                      │
       │         ┌──────────┐                 │
       │         │RCWL-9196 │ ◄── Patita 12   │
       │         └──────────┘         │       │
       │    [3V3] [GND] [OUT] [VIN]   └─── (Opcional: 1 cable a ADC ESP32)
       └──────┬─────┬─────┬─────┬─────────────┘
              │     │     │     │
         Opción A (De Fábrica): Pines normales a GPIO digital del ESP32
```

1. **Riel de Desplazamiento:** Coloca dos varillas delgadas de madera o acrílico apoyadas en los bordes superiores de la caja para que el sensor pueda deslizarse a $1\text{--}2\text{ cm}$ sobre la arena sin tocarla.
2. **Orientación de la Antena PCB:** La cara de la placa del RCWL-0516 que tiene la antena impresa debe apuntar **hacia abajo**, mirando a la arena.

---

### 4.3 Configuración Sísmica Activa (Refracción y Ondas P/S - Proyecto 06)

1. **Punto de Disparo (Fuente Sísmica $X = 0$):**
   * Clava una chapita de metal o moneda de cobre en la superficie de la arena en el extremo izquierdo ($X = 0\text{ cm}$).
   * Conéctale un cable a **GND del ESP32**.
   * Toma un clavo grande o pequeño martillo metálico conectado a un **GPIO con pull-up interno** (`GPIO 13` del ESP32).
   * Al golpear la chapita, el circuito se cierra instantáneamente a masa (`LOW`) y dispara la interrupción de microsegundos `micros()`.
2. **Receptores Sísmicos ($X_1$ y $X_2$):**
   * Pega un clavo corto de $3\text{ cm}$ con pegamento térmico o cinta de montaje por debajo de la placa del sensor `ADXL345` o del micrófono I2S `INMP441`.
   * Clava el sensor en la arena en $X_1 = 15\text{ cm}$ y el segundo en $X_2 = 35\text{ cm}$.

---

## 5. Plano de Distribución Espacial y Coordenadas (X-Y-Z)

```text
VISTA SUPERIOR EN PLANTA (Ejes X - Y):

Y (ancho = 22 cm)
 ▲
 │ ┌────────────────── Absorbedor Goma Eva (Pared) ──────────────────┐
 │ │                                                                 │
 │ │  [Fuente]                                                       │
 │ │  [Golpe ]   e1  e2  e3  e4  e5  e6  e7  e8  e9  ... e15 e16     │
 │ │    (X=0)    •   •   •   •   •   •   •   •   •        •   •      │  Línea Central ERT
 │ │                                                                 │  (Y = 11 cm)
 │ │                 [  Anomalía A: PVC  ]      [ Anomalía B: Esponja]
 │ │                 [   X = 14 a 18 cm  ]      [   X = 30 a 34 cm   ]
 │ │                                                                 │
 │ └────────────────── Absorbedor Goma Eva (Pared) ──────────────────┘
 └─────────────────────────────────────────────────────────────────────► X (largo = 50 cm)
   0 cm                                                              50 cm

VISTA EN CORTE VERTICAL (Ejes X - Z):

Z (profundidad = 15 cm)
 0 cm (Superficie) ──•───•───•───•───•───•───•───•───•───•───•───•───•───•───•───•── (Electrodos z=2.5cm)
 │                   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │   │
 │                   Arena Fina Húmeda Homogénea (ρ ≈ 50 Ω·m, V1 ≈ 140 m/s)
 │                   ┌───────────────┐                  ┌───────────────┐
 4 cm ───────────────│ Tubo PVC Seco │                  │Esponja Salada │
 │                   │ (Resistivo)   │                  │ (Conductora)  │
 │                   └───────────────┘                  └───────────────┘
 │
10 cm ───────────────[ Opcional: Baldosa Cerámica Basal (V2 ≈ 2000 m/s) ]
 │
15 cm (Fondo) ═════════════════════════════════════════════════════════════════════════
```

---

## 6. Protocolo de Calibración y Checklist Previo a las Mediciones

Antes de conectar el microcontrolador o iniciar un barrido de datos, ejecuta esta lista de comprobación con tu multímetro digital:

- [ ] **Checklist 1: Continuidad de Varillas Inox:** Verifica que cada caimán haga contacto metálico directo con la varilla (resistencia de contacto pinza-clavo $< 0.5\ \Omega$).
- [ ] **Checklist 2: Resistencia Inter-Electródica de Fondo:**
  * Mide la resistencia en arena entre los electrodos $e_1$ y $e_2$ (separación $2.5\text{ cm}$).
  * Debe medir entre **$200\ \Omega$ y $600\ \Omega$**.
  * Si mide $> 1\,500\ \Omega$, humedece ligeramente la superficie con el atomizador y espera 10 minutos a que el agua se estabilice por capilaridad.
- [ ] **Checklist 3: Prueba de Potencial Espontáneo ($SP$ Residual):**
  * Con el multímetro en milivoltios DC ($mV$), mide entre $e_2$ y $e_3$ sin inyectar corriente.
  * Debe marcar **menos de $\pm 15\text{ mV}$**. (Los electrodos de acero inoxidable 316L suelen dar $< 5\text{ mV}$). Si usaste clavos galvanizados comunes, puede marcar hasta $50\text{ mV}$; esto confirma la necesidad de usar la **inversión bipolar de polaridad** en el código.
- [ ] **Checklist 4: Conservación de la Humedad:**
  * Al terminar de armar el terrario, **cubre la caja con su tapa plástica o con film transparente de cocina**.
  * Esto evita que la arena se seque por evaporación superficial y mantiene la resistividad calibrada durante semanas de experimentos repetibles.

---

> 🚀 **Estado del Laboratorio:** Terrario físico listo para montaje de mesa. El software de control (`02_ert_meter`) se vinculará a esta geometría física ($a = 2.0\text{--}2.5\text{ cm}$, 16 electrodos).
