#!/usr/bin/env bash
# ==============================================================================
# Script de instalación de librerías para Proyecto 01 (Linux Debian / Ubuntu)
# ==============================================================================

set -e

echo "=== Verificando arduino-cli en Linux ==="

# Si arduino-cli no está instalado, descargarlo en ~/.local/bin
if ! command -v arduino-cli &> /dev/null; then
    echo "arduino-cli no encontrado. Descargando versión oficial de Arduino..."
    mkdir -p ~/.local/bin
    curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=~/.local/bin sh
    export PATH="$HOME/.local/bin:$PATH"
fi

echo "Usando arduino-cli: $(which arduino-cli)"

echo -e "\n=== Actualizando índice de librerías ==="
arduino-cli lib update-index

# Lista de librerías requeridas por el Proyecto 01
LIBRARIES=(
    "Adafruit AHTX0"
    "Adafruit BMP280 Library"
    "BH1750"
    "Adafruit SSD1306"
    "Adafruit GFX Library"
    "Adafruit BusIO"
    "Adafruit Unified Sensor"
)

echo -e "\n=== Instalando librerías requeridas ==="
for lib in "${LIBRARIES[@]}"; do
    echo "-> Instalando: $lib"
    arduino-cli lib install "$lib"
done

echo -e "\n[OK] Todas las librerías del Proyecto 01 están instaladas."
echo -e "\nLibrerías instaladas en el sistema:"
arduino-cli lib list
