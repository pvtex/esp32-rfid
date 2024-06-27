@ECHO OFF
cls

echo - [1] Erase the Flash on ESP32
echo - [2] Flash Generic Firmware
echo - [3] Flash Generic DEBUG Firmware
echo - [4] Flash APWIKOGER Firmware
echo - [5] Flash APWIKOGER DEBUG Firmware
echo - [6] Flash DTWONDER Firmware
echo - [7] Flash DTWONDER DEBUG Firmware
echo - [8] Flash LILYGO Firmware
echo - [9] Flash LILYGO DEBUG Firmware

set /p opt=Please choose an option eg. 1: 

2>NUL CALL :%opt%
IF ERRORLEVEL 1 CALL :DEFAULT_CASE

:1
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 erase_flash
  GOTO EXIT_CASE
:2
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 write_flash 0x00010000 wifi.bin
  GOTO EXIT_CASE 
:3
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 write_flash 0x00010000 wifi-debug.bin
  GOTO EXIT_CASE
:4
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 write_flash 0x00010000 wifi-apwikoger.bin
  GOTO EXIT_CASE 
:5
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 write_flash 0x00010000 wifi-apwikoger-debug.bin
  GOTO EXIT_CASE
:6
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 write_flash 0x00010000 ethernet-dtwonder.bin
  GOTO EXIT_CASE 
:7
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 write_flash 0x00010000 ethernet-dtwonder-debug.bin
  GOTO EXIT_CASE
:8
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 write_flash 0x00010000 ethernet-lilygo.bin
  GOTO EXIT_CASE 
:9
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 write_flash 0x00010000 ethernet-lilygo-debug.bin
  GOTO EXIT_CASE
:DEFAULT_CASE
  ECHO Unknown option "%opt%"
  GOTO END_CASE
:END_CASE
  VER > NUL # reset ERRORLEVEL
  GOTO :EOF # return from CALL
:EXIT_CASE
  exit


