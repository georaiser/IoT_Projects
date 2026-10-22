# ==============================================================================
# Script de instalación de librerías para Proyecto 01 (Windows PowerShell)
# ==============================================================================

Write-Host "Verificando ejecutable arduino-cli..." -ForegroundColor Cyan

$cliPath = "C:\Program Files\Arduino IDE\resources\app\lib\backend\resources\arduino-cli.exe"

if (-not (Test-Path $cliPath)) {
    # Intentar buscarlo en PATH si no está en la ruta estándar de Arduino IDE 2.x
    $cmd = Get-Command arduino-cli -ErrorAction SilentlyContinue
    if ($cmd) {
        $cliPath = $cmd.Source
    } else {
        Write-Error "No se encontró arduino-cli.exe. Asegúrate de tener instalado Arduino IDE 2.x"
        exit 1
    }
}

Write-Host "Usando: $cliPath" -ForegroundColor Green
Write-Host "Actualizando índice de librerías..." -ForegroundColor Cyan
& $cliPath lib update-index

$libraries = @(
    "Adafruit AHTX0",
    "Adafruit BMP280 Library",
    "BH1750",
    "Adafruit SSD1306",
    "Adafruit GFX Library",
    "Adafruit BusIO",
    "Adafruit Unified Sensor"
)

Write-Host "`nInstalando librerías requeridas para Proyecto 01..." -ForegroundColor Cyan

foreach ($lib in $libraries) {
    Write-Host "-> Instalando: $lib" -ForegroundColor Yellow
    & $cliPath lib install $lib
}

Write-Host "`n[OK] Todas las librerías del Proyecto 01 han sido instaladas con éxito." -ForegroundColor Green
Write-Host "Librerías instaladas actualmente:" -ForegroundColor Cyan
& $cliPath lib list
