# Guía Integral de Entorno: Mamba, CUDA y Toolchain ESP32 / Arduino

Este documento detalla paso a paso la configuración de entornos reproducibles para **Python (Mamba / PyTorch / CUDA)** y el toolchain de desarrollo embebido para **ESP32 / Arduino**, contemplando tres escenarios:
1. **Windows Nativo (sin WSL)**
2. **Windows con WSL2 (Debian)**
3. **Debian GNU/Linux Nativo (Dual Boot)**

---

## 0. Resumen de Versiones y Especificaciones de Hardware

## 0. Matriz de Versiones Críticas y Compatibilidad (Evitar Conflictos)

> ⚠️ **Atención Crítica con la GPU (GTX 960M / Maxwell `sm_50` / 2 GB VRAM):**
> La GTX 960M pertenece a la arquitectura **Maxwell (Compute Capability 5.0)**. 
> * **El conflicto habitual:** Las versiones oficiales estándar de PyTorch v2.1+ compiladas para CUDA 12.x **eliminaron el soporte por defecto para arquitecturas inferiores a `sm_60` (Pascal) o `sm_70` (Volta)**. Si se instala un binario genérico reciente sin flags de compatibilidad, `torch.cuda.is_available()` puede dar `True` pero fallar al ejecutar operaciones de tensores (`CUDA error: no kernel image is available for execution on the device`).
> * **Solución probada:** Usar la combinación exacta de canales `-c pytorch -c nvidia -c conda-forge` con `--channel-priority flexible` y PyTorch con runtime CUDA 12.4 / 11.8.

### Tabla de Paquetes Principales y Compatibilidad

| Componente / Paquete | Versión Exacta Probada | Versión Compatible Alternativa | Notas de Conflicto / Dependencias |
| :--- | :--- | :--- | :--- |
| **Python** | `3.11.15` | `3.10.x` - `3.11.x` | Evitar Python `>= 3.12` con versiones antiguas de PyTorch/CUDA por incompatibilidad con ruedas CPython. |
| **Driver NVIDIA (Windows)** | `582.78` | `>= 535.x` | Soporta CUDA Runtime hasta 13.0. |
| **Driver NVIDIA (Debian)** | `>= 535.x` / `550.x` | `nvidia-driver` (non-free) | En Debian 12 usar repositorio `non-free` y compilar módulo con DKMS. |
| **Compute Capability** | `sm_50` | `sm_50` | Flag de arquitectura de la GTX 960M. |
| **PyTorch (`torch`)** | `2.5.1` (o `2.8.0` build conda) | `2.1.2` (vía CUDA 11.8) | Si hay error de kernel en `sm_50`, la alternativa infalible es: `pytorch=2.1.2 pytorch-cuda=11.8`. |
| **`torchvision`** | `0.20.1` / `0.24.0` | `0.16.2` | Debe mantenerse en sincronía estricta con la versión de PyTorch instalada. |
| **`torchaudio`** | `2.5.1` / `2.8.0` | `2.1.2` | Debe sincronizarse estrictamente con `torch`. |
| **`cuda-runtime` / `pytorch-cuda`** | `12.4` | `11.8` | Administrado directamente por Mamba sin tocar variables globales del SO. |
| **`cudnn`** | `9.10.2` | `8.9.x` | Incluido dentro del entorno de Conda/Mamba. |
| **`pyside6`** | `6.11.1` | `>= 6.5.x` | En Linux nativo requiere librerías del sistema: `libgl1`, `libglib2.0-0`, `libegl1`. |
| **`numpy`** | `2.4.6` (o `1.26.4`) | `1.26.4` | ⚠️ **Cuidado con NumPy 2.x:** Si utilizas librerías científicas más antiguas que compilaron con C-API de NumPy 1.x, fijar `numpy<2` (ej. `1.26.4`). |
| **`scipy`** | `1.17.1` | `1.12.x` | Requiere compatibilidad con la versión de NumPy elegida. |
| **`pyserial`** | `3.5` | `3.5` | Crítico para comunicación con ESP32 / Arduino por puerto COM / `/dev/ttyUSB*`. |
| **`esp32:esp32` (Core Arduino)** | `2.0.17` o `3.0.x` | `2.0.17` | En ESP32, el paso a Core 3.x (basado en ESP-IDF 5.x) cambió funciones del ADC y WiFi. Para compatibilidad máxima con proyectos existentes se recomienda `2.0.17`. |


---

## 1. Escenario 1: Windows Nativo (Sin WSL)

### 1.1 Driver NVIDIA
1. Descargar e instalar el driver oficial Game Ready o Studio correspondiente a la GTX 960M desde:
   * [NVIDIA Driver Downloads](https://www.nvidia.com/Download/index.aspx)
2. Verificar en PowerShell:
   ```powershell
   nvidia-smi
   ```

### 1.2 Instalación de Miniforge (Mamba)
1. Descargar el instalador de Windows:
   * [Miniforge3-Windows-x86_64.exe](https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Windows-x86_64.exe)
   * O instalarlo mediante `winget`:
     ```powershell
     winget install -e --id CondaForge.Miniforge3
     ```
2. Reiniciar la terminal de PowerShell o abrir **Miniforge Prompt**.

### 1.3 Creación del Entorno `py311-cuda`
Ejecutar en PowerShell / Miniforge Prompt:
```powershell
# Crear el entorno base con Python 3.11 y PyTorch con CUDA 12.4
mamba create -n py311-cuda python=3.11 pytorch=2.5.1 torchvision=0.20.1 torchaudio=2.5.1 pytorch-cuda=12.4 -c pytorch -c nvidia -c conda-forge --channel-priority flexible -y

# Instalar librerías de análisis y serial
mamba install -n py311-cuda scipy pandas matplotlib jupyterlab pyserial openpyxl pyside6 -c conda-forge -y
```

### 1.4 Verificación en Windows
```powershell
mamba run -n py311-cuda python -c "import torch; print('PyTorch:', torch.__version__, '| CUDA:', torch.cuda.is_available(), '| GPU:', torch.cuda.get_device_name(0) if torch.cuda.is_available() else 'N/A')"
```

### 1.5 Toolchain Arduino & ESP32 en Windows
1. **Arduino IDE 2.x o Arduino CLI:**
   * Descargar Arduino IDE: [arduino.cc/en/software](https://www.arduino.cc/en/software)
   * O instalar `arduino-cli` directamente vía `winget`:
     ```powershell
     winget install Arduino.ArduinoCLI
     ```
2. **Configuración de tarjeta ESP32:**
   ```powershell
   arduino-cli config init
   arduino-cli config set board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
   arduino-cli core update-index
   arduino-cli core install esp32:esp32
   ```
3. **Instalación de librerías del proyecto:**
   * Ejecutar el script provisto en la carpeta:
     ```powershell
     powershell -ExecutionPolicy Bypass -File .\instalar_librerias_proyecto01.ps1
     ```
4. **Drivers USB (CP2102 / CH340):**
   * Instalar drivers según el chip USB-UART de la placa ESP32 (disponibles en la subcarpeta `driver/`).

---

## 2. Escenario 2: Windows con WSL2 (Debian)

### 2.1 Instalación y preparación de WSL2
1. En Windows PowerShell como Administrador:
   ```powershell
   wsl --install -d Debian
   ```
2. Una vez dentro de Debian WSL, actualizar el gestor de paquetes del sistema:
   ```bash
   sudo apt update && sudo apt upgrade -y
   sudo apt install -y build-essential curl wget git libgl1 libglib2.0-0
   ```

### 2.2 Instalación de Miniforge (Mamba) en Linux WSL
```bash
# Descargar e instalar Miniforge
curl -fsSL -O "https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh"
bash Miniforge3-Linux-x86_64.sh -b -p "$HOME/miniforge3"
rm -f Miniforge3-Linux-x86_64.sh

# Inicializar shell
"$HOME/miniforge3/bin/conda" init bash
"$HOME/miniforge3/bin/mamba" init bash
source ~/.bashrc
```

### 2.3 Creación del entorno exacto `py311-cuda`
```bash
# Crear entorno idéntico
mamba create -n py311-cuda python=3.11 pytorch=2.5.1 torchvision=0.20.1 torchaudio=2.5.1 pytorch-cuda=12.4 -c pytorch -c nvidia -c conda-forge --channel-priority flexible -y

# Instalar paquetes adicionales
mamba install -n py311-cuda scipy pandas matplotlib jupyterlab pyserial openpyxl pyside6 -c conda-forge -y
```

### 2.4 Verificación de CUDA en WSL
```bash
mamba run -n py311-cuda python -c "import torch; print('PyTorch:', torch.__version__, '| CUDA:', torch.cuda.is_available(), '| Device:', torch.cuda.get_device_name(0) if torch.cuda.is_available() else 'None')"
```

### 2.5 Compilación Arduino / ESP32 en WSL2
* **Instalación de arduino-cli:**
  ```bash
  mkdir -p ~/.local/bin
  curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=~/.local/bin sh
  echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
  source ~/.bashrc
  ```
* **Soporte ESP32:**
  ```bash
  arduino-cli config init
  arduino-cli config set board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
  arduino-cli core update-index
  arduino-cli core install esp32:esp32
  ```
* **Librerías del Proyecto:**
  ```bash
  chmod +x ./instalar_librerias_proyecto01.sh
  ./instalar_librerias_proyecto01.sh
  ```
* **Paso USB a WSL (Flashing físico):**
  * Para poder programar el puerto COM/USB desde WSL, Windows requiere `usbipd-win`:
    ```powershell
    # En Windows (Admin)
    winget install usbipd
    usbipd list
    usbipd bind --busid <BUSID>
    usbipd attach --wsl --busid <BUSID>
    ```

---

## 3. Escenario 3: Debian Nativo (Dual Boot)

> 💡 **¿Por qué compilar en Linux nativo?** 
> En compilaciones de C/C++ (ESP32/Arduino), Linux crea y destruye procesos (`fork`) a una velocidad hasta 3-5 veces superior a Windows, además de no sufrir la penalización de análisis en tiempo real de Windows Defender sobre miles de archivos temporales (`.o`, `.d`). Además, la gestión de puertos USB `/dev/ttyUSB*` es nativa.

### 3.1 Instalación del Driver NVIDIA Propietario en Debian 12 / Testing
1. Habilitar componentes `contrib` y `non-free` en `/etc/apt/sources.list`.
2. Instalar encabezados del kernel y driver:
   ```bash
   sudo apt update
   sudo apt install -y linux-headers-$(uname -r) build-essential dkms
   sudo apt install -y nvidia-driver nvidia-smi nvidia-cuda-toolkit
   ```
3. Reiniciar el sistema y verificar:
   ```bash
   nvidia-smi
   ```

### 3.2 Miniforge y Entorno `py311-cuda`
Los pasos son exactamente idénticos al Escenario 2.2 y 2.3:
```bash
# 1. Instalar Miniforge
curl -fsSL -O "https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh"
bash Miniforge3-Linux-x86_64.sh -b -p "$HOME/miniforge3"
rm -f Miniforge3-Linux-x86_64.sh
"$HOME/miniforge3/bin/conda" init bash
"$HOME/miniforge3/bin/mamba" init bash
source ~/.bashrc

# 2. Crear py311-cuda
mamba create -n py311-cuda python=3.11 pytorch=2.5.1 torchvision=0.20.1 torchaudio=2.5.1 pytorch-cuda=12.4 -c pytorch -c nvidia -c conda-forge --channel-priority flexible -y
mamba install -n py311-cuda scipy pandas matplotlib jupyterlab pyserial openpyxl pyside6 -c conda-forge -y
```

### 3.3 Toolchain Arduino & ESP32 Nativo
1. **Permisos de Puerto Serie (Crucial para ESP32/Arduino):**
   ```bash
   sudo usermod -a -G dialout $USER
   ```
   *(Cerrar sesión y volver a iniciar para aplicar los permisos de grupo).*

2. **Instalación de Arduino CLI y Core ESP32:**
   ```bash
   mkdir -p ~/.local/bin
   curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=~/.local/bin sh
   export PATH="$HOME/.local/bin:$PATH"

   arduino-cli config init
   arduino-cli config set board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
   arduino-cli core update-index
   arduino-cli core install esp32:esp32
   ```

3. **Librerías del Proyecto:**
   ```bash
   chmod +x instalar_librerias_proyecto01.sh
   ./instalar_librerias_proyecto01.sh
   ```

4. **Compilar y Subir Sketch desde Terminal:**
   ```bash
   # Compilar
   arduino-cli compile --fqbn esp32:esp32:esp32 sketches/Proyecto_01/

   # Subir a la placa (detecta automáticamente el puerto /dev/ttyUSB0 o /dev/ttyACM0)
   arduino-cli upload -p /dev/ttyUSB0 --fqbn esp32:esp32:esp32 sketches/Proyecto_01/
   ```

---

## 4. Gestión de Espacio y Eliminación de WSL (Si decides pasar a Dual Boot)

Si finalmente optas por la instalación nativa en Dual Boot y quieres recuperar el espacio en Windows:

1. **Respaldar archivos personales de WSL (opcional):**
   ```powershell
   # En Windows PowerShell
   wsl --export Debian D:\backup_debian_wsl.tar
   ```
2. **Eliminar Debian y liberar de inmediato el disco virtual (`~25 GB`):**
   ```powershell
   wsl --unregister Debian
   ```
3. **Desinstalar el Subsistema WSL de Windows:**
   * Ir a *Configuración > Aplicaciones > Aplicaciones instaladas* y desinstalar **Subsistema de Windows para Linux**.
