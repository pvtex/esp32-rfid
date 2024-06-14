# ESP32-RFID - Access Control with ESP32 and Wiegand

[This Project is a Port for ESP32 from ESP-RFID](https://github.com/esprfid/esp-rfid)


Access Control system using a Wiegand RFID readers and Espressif's ESP32 Microcontroller. 

![Showcase Gif](https://raw.githubusercontent.com/esprfid/esp-rfid/stable/demo/showcase.gif)

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

* Get the latest release from [here](https://github.com/espvtexprfid/esp32-rfid/releases).
* See [Known Issues](https://github.com/pvtex/esp32-rfid#known-issues) before starting right away.
* See [Security](https://github.com/pvtex/esp32-rfid#security) for your safety.
* See [ChangeLog](https://github.com/pvtex/esp32-rfid/blob/dev/CHANGELOG.md)

### What You Will Need
* An ESP32 module or a development board with at least **32Mbit Flash (equals to 4MBytes)**
* Level Shifter for connecting the Wiegand reader to ESP32
* Wiegand based RFID reader
* A Relay Module (or you can build your own circuit)
* n quantity of Mifare Classic 1KB (recommended due to available code base) PICCs (RFID Tags) equivalent to User Number

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

##### How to modify the project

If you want to modify the code, you can read more info in the [CONTRIBUTING](./CONTRIBUTING.md) file.


### Pin Layout

The following table shows the typical pin layout used for connecting readers hardware to ESP:

| ESP32   | Wiegand |
|--------:|:-------:|
| GPIO-16 | LED     | 
| GPIO-15 | Buzzer  |
| GPIO-13 | D0      |
| GPIO-12 | D1      |
| GPIO-14 |         | 
| GPIO-04 |         | 
| GPIO-05 |         | 
| GPIO-03 |         |

For Wiegand based readers, you can configure D0 and D1 pins via settings page. By default, D0 is GPIO-4 and D1 is GPIO-5

### Steps
* First, flash firmware (you can use /bin/flash.bat on Windows) to your ESP either using Arduino IDE or with your favourite flash tool
* (optional) Fire up your serial monitor to get informed
* Search for Wireless Network "esp32-rfid-xxxxxx" and connect to it (It should be an open network and does not require password)
* Open your browser and visit either "http://192.168.4.1" or "http://esp32-rfid.local" (.local needs Bonjour installed on your computer).
* Log on to ESP, default password is "admin"
* Go to "Settings" page
* Configure your amazing access control device. Push "Scan" button to join your wireless network, configure RFID hardware, Relay Module.
* Save settings, when rebooted your ESP will try to join your wireless network.
* Check your new IP address from serial monitor and connect to your ESP again. (You can also connect to "http://esp32-rfid.local")
* Go to "Users" page
* Scan a PICC (RFID Tag) then it should glimpse on your Browser's screen.
* Type "User Name" or "Label" for the PICC you scanned.
* Choose "Allow Access" if you want to
* Click "Add"
* Congratulations, everything went well, if you encounter any issue feel free to ask help on GitHub.

### MQTT
You can integrate ESP-RFID with other systems using MQTT. Read the [additional documentation](./README-MQTT.md) for all the details.

### Known Issues
* Please also check [GitHub issues](https://github.com/pvtex/esp32-rfid/issues).

#### Time
We are syncing time from a NTP Server (in Client -aka infrastructure- Mode). This will require ESP to have an Internet connection. Additionally your ESP can also work without Internet connection (Access Point -aka Ad-Hoc- Mode), without giving up functionality.
This will require you to sync time manually. ESP can store and hold time for you approximately 51 days without major issues, device time can drift from actual time depending on usage, temperature, etc. so you have to login to settings page and sync it in a timely fashion.
Timezones are supported with automatic switch to and from daylight saving time.

## **Security**
We assume **ESP32-RFID** project -as a whole- does not offer strong security. There are PICCs available that their UID (Unique Identification Numbers) can be set manually (Currently esp-rfid relies only UID to identify its users). Also there may be a bug in the code that may result free access to your belongings. And also, like every other network connected device esp-rfid is vulnerable to many attacks including Man-in-the-middle, Brute-force, etc.

This is a simple, hobby grade project, do not use it where strong security is needed.

What can be done to increase security? (by you and by us)

* We are working on more secure ways to Authenticate RFID Tags.
* You can disable wireless network to reduce attack surface. (This can be configured in Web UI Settings page)
* Choose a strong password for the Web UI

## Scalability
Since we are limited on both flash and ram size things may get ugly at some point in the future. You can find out some test results below.

### Tests

#### 1) How many RFID Tag can be handled?
Restore some randomly generated user data on File System worth:

* 1000 separate "userfile"
* random 4 Bytes long UID and
* random User Names and
* 4 bytes random Unix Time Stamp
* each have "access type" 1 byte integer "1" or "0".

Total 122,880 Bytes

At least 1000 unique User (RFID Tag) can be handled, the test were performed on WeMos D1 mini.

#### Additional testing is needed:

* Logging needs testing. How long should it need to log access? What if a Boss needs whole year log?
* Reliability on Flash (these NOR Flash have limited write cycle on their cells). It depends on manufacturer choice of Flash Chip and usage.

### Projects that are based on esp-rfid

* [ESP-IO](https://github.com/Pako2/EventGhostPlugins/tree/master/ESP-IO) Project to manipulate GPIOs with EventGhost
* [ESP-RCM](https://github.com/Pako2/esp-rcm) Room Climate Monitor with ESP8266, HTU21D, Si7021, AM2320
* [ESP-RFID-PY](https://github.com/esprfid/esp-rfid-py) Micro-Python implementation of esp-rfid is also made available by @iBobik

### Acknowledgements

- @rneurink
- @thunderace
- @zeraien
- @nardev
- @romanzava
- @arduino12
- @Pako2
- @marelab
- @pvtex

See [ChangeLog](./CHANGELOG.md)

## Donations
[![OC](https://opencollective.com/esp-rfid/tiers/esp-rfid-user.svg?avatarHeight=56)](https://opencollective.com/esp-rfid)

Developing fully open, extensively tested embedded software is hard and time consuming work. Please consider making donations to support developers behind this beautiful software.

Donations **transparently** processed by **[Open Collective](https://opencollective.com/how-it-works)** and expenses are being made public by OC's open ledger.

* 2017-10-03 [steinar-t](https://github.com/steinar-t)
* 2017-12-10 [saschaludwig](https://github.com/saschaludwig)
* 2018-10-02 Dennis Parsch
* 2019-01-12 Chris-topher Slater
* 2019-04-23 Klaus Blum
* 2019-04-25 Andre Dieteich

## Contributors

This project exists thanks to all the people who contribute. 
<a href="https://github.com/pvtex/esp32-rfid/graphs/contributors"><img src="https://opencollective.com/esp-rfid/contributors.svg?width=890&button=false" /></a>

## License
The code parts written by ESP-RFID project's authors are licensed under [MIT License](https://github.com/pvtex/esp32-rfid/blob/stable/LICENSE), 3rd party libraries that are used by this project are licensed under different license schemes, please check them out as well.
