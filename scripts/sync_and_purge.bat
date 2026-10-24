@echo off
title Sync and Purge ESP32 Station Storage
echo ========================================================
echo   BACKING UP AND PURGING ESP32 STORAGE
echo   All data will be saved to your PC before purging device.
echo ========================================================
python "%~dp0sync_station_data.py" --purge
echo.
pause
