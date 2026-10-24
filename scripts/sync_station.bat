@echo off
title Sync ESP32 Environmental Station Data
echo ========================================================
echo   Syncing telemetry from ESP32 Station (192.168.1.5)...
echo ========================================================
python "%~dp0sync_station_data.py"
echo.
pause
