# ESP32-RFID - Access Control with ESP32 and Wiegand

[This Project is a Port for ESP32 from ESP-RFID](https://github.com/esprfid/esp-rfid)


Access Control system using a Wiegand RFID readers and Espressif's ESP32 Microcontroller. 

![Screenshot Status](./demo/screenshot_status.png)

## Features
### For Users
* Minimal effort for setting up your Access Control system, just flash and everything can be configured via Web UI
* Capable of managing up to 1.000 Users (even more is possible)
* Great for Maker Spaces, Labs, Schools, etc
* Cheap to build and easy to maintain
### For Tinkerers
* Open Source (minimum amount of hardcoded variable, this means more freedom)
* Using WebSocket protocol to exchange data between Hardware and Web Browser
* Data is encoded as JSON object
* Records are Timestamped (Time synced from a NTP Server)
* MQTT enabled
* Bootstrap, jQuery, FooTables for beautiful Web Pages for both Mobile and Desktop Screens
* Thanks to ESPAsyncWebServer Library communication is Asynchronous

## Getting Started
This project still in its development phase. New features (and also bugs) are introduced often and some functions may become deprecated. Please feel free to comment or give feedback.

* Get the latest release from [here](https://github.com/pvtex/esp32-rfid/releases).
* See [Known Issues](https://github.com/pvtex/esp32-rfid#known-issues) before starting right away.
* See [Security](https://github.com/pvtex/esp32-rfid#security) for your safety.
* See [ChangeLog](https://github.com/pvtex/esp32-rfid/blob/dev/CHANGELOG.md)

### What You Will Need
* An ESP32 module or a development board with at least **32Mbit Flash (equals to 4MBytes)**
* 4 port Level Shifter for connecting the Wiegand reader to ESP32 (needed for D0, D1, Buzzer and LED)
* Wiegand based RFID reader
* A Relay Module (or you can build your own circuit)
* n quantity of RFID Tags (for example Mifare Classic 1KB , or any other Tag with a 4 or 7byte ID) equivalent to User Number

### ESP32-Board
curently ESP32-RFID support 3 boards:

* without Ethernet
    * [APWIKOGER-Relay-X1](boards/APWIKOGER-Relay-X1)
* with Ethernet
    * [DTWonder DT-R002](boards/dtwonder-DT-R002/) (with debug settings: no extras like Doorstatus, Exit-Button, ..., only Wiegand)
    * [lilygo t-internet-com](boards/t-internet-com/)

You can find gerber files and pictures for each board in the board subfolder.     
In each subfolder there is a gerber file for an adapter board to wiegand interface, door status and exit button.      

### Software

#### Using Compiled Binaries
Download compiled binaries from GitHub Releases page
https://github.com/pvtex/esp32-rfid/releases

On Windows you can use **"flash.bat"**, it will ask you which COM port that ESP is connected and then flashes it. You can use any flashing tool and do the flashing manually. The flashing process itself has been described at numerous places on Internet.

#### Building With PlatformIO

The build environment is based on [PlatformIO](http://platformio.org). Follow the instructions found here: http://platformio.org/#!/get-started for installing it but skip the ```platform init``` step as this has already been done, modified and it is included in this repository. In summary:

```
sudo pip install -U pip setuptools
sudo pip install -U platformio
git clone https://github.com/pvtex/esp32-rfid.git
cd esp32-rfid
platformio run
```

When you run ```platformio run``` for the first time, it will download the toolchains and all necessary libraries automatically.

##### Useful commands:

* ```platformio run``` - process/build all targets
* ```platformio run -e generic -t upload``` - process/build and flash just the ESP12e target (the NodeMcu v2)
* ```platformio run -t clean``` - clean project (remove compiled files)

The resulting (built) image(s) can be found in the directory ```/bin``` created during the build process.

### Pin Layout

The following table shows the typical pin layout used for connecting readers hardware to ESP:

| ESP32   | generic | DTWonder | lilygo | APWIKOGER |
|--------:|:-------:|:--------:|:------:|:---------:|
| GPIO-1  |         | LED      |        |           |
| GPIO-2  |         | Relay 2  |        |           |
| GPIO-3  |         | Buzzer   |        |           |
| GPIO-4  | Relay   | D0       |        |           |
| GPIO-5  |         | D1       |        |           |
| GPIO-12 | D1      |          |        |           |
| GPIO-13 | D0      |          |        | Buzzer    |
| GPIO-14 |         |          |        | LED       |
| GPIO-15 | Buzzer  |          |        |           |
| GPIO-16 | LED     | Relay 1  |        | Relay 1   |  
| GPIO-18 |         |          |        | Exit      |
| GPIO-26 |         |          |        | D0        |
| GPIO-33 |         |          |        | D1        |
| GPIO-36 |         | Door     |        |           |
| GPIO-39 |         | Exit     |        |           |




For Wiegand based readers, you can configure D0 and D1 pins via settings page. By default, D0 is GPIO-4 and D1 is GPIO-5

### Steps
* First, flash firmware (you can use /bin/flash.bat on Windows) to your ESP either using Arduino IDE or with your favourite flash tool
* (optional) Fire up your serial monitor to get informed
* Search for Wireless Network "esp32-rfid-xxxxxx" and connect to it (It should be an open network and does not require password)
* Open your browser and visit either "http://192.168.4.1" or "http://esp32-rfid.local" (.local needs Bonjour installed on your computer).
* Log on to ESP32, default password is "admin"
* Go to "Settings" page
* Configure your amazing access control device. Push "Scan" button to join your wireless network, configure RFID hardware, Relay Module.
* Save settings, when rebooted your ESP32 will try to join your wireless network.
* Check your new IP address from serial monitor and connect to your ESP again. (You can also connect to "http://esp32-rfid.local")
* Go to "Users" page
* Scan a PICC (RFID Tag) then it should glimpse on your Browser's screen.
* Type "User Name" or "Label" for the PICC you scanned.
* Choose "Allow Access" if you want to
* Click "Add"
* Congratulations, everything went well, if you encounter any issue feel free to ask help on GitHub.

### MQTT
You can integrate ESP32-RFID with other systems using MQTT. Read the [additional documentation](./README-MQTT.md) for all the details.

### Known Issues
* Please also check [GitHub issues](https://github.com/pvtex/esp32-rfid/issues).

#### Time
We are syncing time from a NTP Server (in Client -aka infrastructure- Mode). This will require ESP to have an Internet connection. Additionally your ESP32 can also work without Internet connection (Access Point -aka Ad-Hoc- Mode), without giving up functionality.
This will require you to sync time manually. ESP32 can store and hold time for you approximately 51 days without major issues, device time can drift from actual time depending on usage, temperature, etc. so you have to login to settings page and sync it in a timely fashion.
Timezones are supported with automatic switch to and from daylight saving time.

## **Security**
We assume **ESP32-RFID** project -as a whole- does not offer strong security. There are PICCs available that their UID (Unique Identification Numbers) can be set manually (Currently esp-rfid relies only UID to identify its users). Also there may be a bug in the code that may result free access to your belongings. And also, like every other network connected device es32p-rfid is vulnerable to many attacks including Man-in-the-middle, Brute-force, etc.

This is a simple, hobby grade project, do not use it where strong security is needed.

What can be done to increase security? (by you and by us)

* We are working on more secure ways to Authenticate RFID Tags.
* You can disable wireless network to reduce attack surface. (This can be configured in Web UI Settings page)
* Choose a strong password for the Web UI

### my Projects that are based on esp-rfid

* [ESP-RFID v2](https://github.com/pvtex/esp-rfid) ESP-RFID v2 - Access Control with ESP8266 and Wiegand
* [ESP32-RFID](https://github.com/pvtex/esp32-rfid) ESP32-RFID - Access Control with ESP32 and Wiegand

### Changelog

See [ChangeLog](./CHANGELOG.md)

## License
The code parts written by ESP-RFID project's authors are licensed under [MIT License](https://github.com/pvtex/esp32-rfid/blob/stable/LICENSE), 3rd party libraries that are used by this project are licensed under different license schemes, please check them out as well.
