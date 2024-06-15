@ECHO OFF
cls

echo - [1] Flash Generic Firmware
echo - [2] Erase the Flash on ESP32
echo - [3] Flash Generic DEBUG Firmware

set /p opt=Please choose an option eg. 1: 

2>NUL CALL :%opt%
IF ERRORLEVEL 1 CALL :DEFAULT_CASE

:1
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 write_flash 0x00010000 generic.bin
  GOTO EXIT_CASE   
:2
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 erase_flash
  GOTO EXIT_CASE
:3
  set /p com=Enter which COM Port your ESP is connected eg. COM1 COM2 COM7: 
  esptool.exe --chip auto --port COM19 write_flash 0x00010000 debug.bin
  GOTO EXIT_CASE
:DEFAULT_CASE
  ECHO Unknown option "%opt%"
  GOTO END_CASE
:END_CASE
  VER > NUL # reset ERRORLEVEL
  GOTO :EOF # return from CALL
:EXIT_CASE
  exit


