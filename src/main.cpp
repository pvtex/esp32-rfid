/*
MIT License

Copyright (c) 2024 pvtex
Copyright (c) 2018 esp-rfid Community
Copyright (c) 2017 Ömer Şiar Baysal

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#define VERSION "1.1.5"

bool eth_connected = false;

#include "Arduino.h"
#include <WiFi.h>
#include "WiFiEventHandler.h"
#include <SPI.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include "esp_flash.h" 
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <TimeLib.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <Bounce2.h>
#include <esp_task_wdt.h>
#include <Update.h>
#include "magicnumbers.h"
#include "config.h"

Config config;

#include <WiegandNG.h>

File fsUploadFile;                      //Hält den aktuellen Upload

WiegandNG wg;

// relay specific variables
bool activateRelay[MAX_NUM_RELAYS] = {false, false, false, false};
bool deactivateRelay[MAX_NUM_RELAYS] = {false, false, false, false};

// these are from vendors
#include "webh/glyphicons-halflings-regular.woff.gz.h"
#include "webh/required.css.gz.h"
#include "webh/required.js.gz.h"

// these are from us which can be updated and changed
#include "webh/esprfid.js.gz.h"
#include "webh/esprfid.htm.gz.h"
#include "webh/index.html.gz.h"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
Ticker wifiReconnectTimer;
Ticker wsMessageTicker;
Bounce openLockButton;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#define LEDoff HIGH
#define LEDon LOW

#define BEEPERoff HIGH
#define BEEPERon LOW

// Variables for whole scope
unsigned long cooldown = 0;
unsigned long currentMillis = 0;
unsigned long deltaTime = 0;
bool doEnableWifi = false;
bool doEnableEth = false;
bool formatreq = false;
const char *httpUsername = "admin";
unsigned long keyTimer = 0;
uint8_t lastDoorbellState = 0;
uint8_t lastDoorState = 0;
uint8_t lastTamperState = 0;
unsigned long nextbeat = 0;
time_t epoch;
time_t lastNTPepoch;
unsigned long lastNTPSync = 0;
unsigned long openDoorMillis = 0;
unsigned long previousLoopMillis = 0;
unsigned long previousMillis = 0;
bool shouldReboot = false;
tm timeinfo;
unsigned long uptimeSeconds = 0;
unsigned long wifiPinBlink = millis();
unsigned long wiFiUptimeMillis = 0;


#include "led.esp"
#include "beeper.esp"
#include "log.esp"
#include "mqtt.esp"
#include "helpers.esp"
#include "wsResponses.esp"
#include "rfid.esp"
#include "wifi.esp"
#ifdef ETHERNET
#include "ethernet.esp"
#endif
#include "config.esp"
#include "websocket.esp"
#include "webserver.esp"
#include "door.esp"
#include "doorbell.esp"

char* numberToHexStr(char* out, unsigned char* in, size_t length)
{
        char* ptr = out;
        for (int i = length-1; i >= 0 ; i--)
            ptr += sprintf(ptr, "%02X", in[i]);
        return ptr;
}

void ICACHE_FLASH_ATTR setup()
{
#ifdef DEBUG
	Serial.begin(115200);
	Serial.println();

	Serial.print(F("[ INFO ] ESP RFID v"));
	Serial.println(VERSION);

	uint32_t realSize;
    esp_flash_get_size(NULL, &realSize);
	uint32_t ideSize = ESP.getFlashChipSize();
	FlashMode_t ideMode = ESP.getFlashChipMode();
	
	Serial.print("ESP32 Model:      ");
	Serial.print(ESP.getChipModel());
	Serial.print(" rev");
	Serial.println(ESP.getChipCores());
	Serial.print("ESP32 Cores:       ");
	Serial.println(ESP.getChipCores());
	uint32_t chipID = 0;
	for (int i = 0; i < 17; i = i + 8) {
    	chipID |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  	}
	Serial.printf("ESP32 Chip ID:   %d\n", chipID);

	Serial.printf("Flash real size: %u\n\n", realSize);
	Serial.printf("Flash ide  size: %u\n", ideSize);
	Serial.printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed());
	Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT"
																	: ideMode == FM_DIO	   ? "DIO"
																	: ideMode == FM_DOUT   ? "DOUT"
																						   : "UNKNOWN"));
	if (ideSize != realSize)
	{
		Serial.println(F("Flash Chip configuration wrong!\n"));
	}
	else
	{
		Serial.println(F("Flash Chip configuration ok.\n"));
	}
#endif
	if (!LittleFS.begin(true))
	{
#ifdef DEBUG
		Serial.println(F("[ ERROR ] Filesystem ERROR!"));
#endif
	} else
	{
#ifdef DEBUG
			Serial.println(F("[ INFO ] Filesystem OK"));
#endif
	}

	File root = LittleFS.open("/P");
	if(!root.isDirectory())
	{
        LittleFS.mkdir("/P");
    }
	
	bool configured = false;
	configured = loadConfiguration(config);
	eth_connected = false;
	setupMqtt();
	setupWifi(configured);
#ifdef ETHERNET
	setupEth(configured);
	/*
	config.ipAddressEth = ETH.localIP();
	config.gatewayIpEth = ETH.gatewayIP();
	config.subnetIpEth = ETH.subnetMask();
	config.dnsIpEth = ETH.dnsIP();
	config.ethmac = ETH.macAddress();
	
    String linkduplex = "HD";
	if (ETH.fullDuplex() == true) 
	{
		linkduplex = "FD";
	}
	char spd[12]; 
	sprintf(spd, "%dMbps %s", ETH.linkSpeed(), linkduplex);
	config.ethlink = (String)spd;
	*/
#endif
	setupWebServer();
	writeEvent("INFO", "sys", "System setup completed, running", "");
#ifdef DEBUG
	Serial.println(F("[ INFO ] System setup completed, running"));
#endif
}

void ICACHE_RAM_ATTR loop()
{
	currentMillis = millis();
	deltaTime = currentMillis - previousLoopMillis;
	uptimeSeconds = currentMillis / 1000;
	previousLoopMillis = currentMillis;
	
	trySyncNTPtime(10);
	
	
	if (config.openlockpin != 255)
	{
		openLockButton.update();
		if (openLockButton.fell())
		{
			writeLatest(" ", "Button", 1);
			mqttPublishAccess(epoch, "true", "Always", "Button", " ", " ");
			activateRelay[0] = true;
			beeperValidAccess();
			// TODO: handle other relays
		}
	}

	ledWifiStatus();
	ledAccessDeniedOff();
	beeperBeep();
	doorStatus();
	doorbellStatus();

	if (currentMillis >= cooldown)
	{
		rfidLoop();
	}

	for (int currentRelay = 0; currentRelay < config.numRelays; currentRelay++)
	{
		if (config.lockType[currentRelay] == LOCKTYPE_CONTINUOUS) // Continuous relay mode
		{
			if (activateRelay[currentRelay])
			{
				if (digitalRead(config.relayPin[currentRelay]) == !config.relayType[currentRelay]) // currently OFF, need to switch ON
				{
					mqttPublishIo("lock" + String(currentRelay), "UNLOCKED");
#ifdef DEBUG
					Serial.print(F("mili : "));
					Serial.println(millis());
					Serial.printf("activating relay %d now\n", currentRelay);
#endif
					digitalWrite(config.relayPin[currentRelay], config.relayType[currentRelay]);
				}
				else // currently ON, need to switch OFF
				{
					mqttPublishIo("lock" + String(currentRelay), "LOCKED");
#ifdef DEBUG
					Serial.print(F("mili : "));
					Serial.println(millis());
					Serial.printf("deactivating relay %d now\n", currentRelay);
#endif
					digitalWrite(config.relayPin[currentRelay], !config.relayType[currentRelay]);
				}
				activateRelay[currentRelay] = false;
			}
		}
		else if (config.lockType[currentRelay] == LOCKTYPE_MOMENTARY) // Momentary relay mode
		{
			if (activateRelay[currentRelay])
			{
				mqttPublishIo("lock" + String(currentRelay), "UNLOCKED");
#ifdef DEBUG
				Serial.print(F("mili : "));
				Serial.println(millis());
				Serial.printf("activating relay %d now\n", currentRelay);
#endif
				digitalWrite(config.relayPin[currentRelay], config.relayType[currentRelay]);
				previousMillis = millis();
				activateRelay[currentRelay] = false;
				deactivateRelay[currentRelay] = true;
#ifdef DEBUG
				Serial.printf("relay %d active\n", currentRelay);
#endif
			}
			else if (((currentMillis - previousMillis) >= config.activateTime[currentRelay]) && (deactivateRelay[currentRelay]))
			{
				mqttPublishIo("lock" + String(currentRelay), "LOCKED");
#ifdef DEBUG
				Serial.println(currentMillis);
				Serial.println(previousMillis);
				Serial.println(config.activateTime[currentRelay]);
				Serial.println(activateRelay[currentRelay]);
				Serial.println(F("deactivate relay after this"));
				Serial.print("mili : ");
				Serial.println(millis());
#endif
				digitalWrite(config.relayPin[currentRelay], !config.relayType[currentRelay]);
				deactivateRelay[currentRelay] = false;
			}
		}
	}
	if (formatreq)
	{
#ifdef DEBUG
		Serial.println(F("[ WARN ] Factory reset initiated..."));
#endif
		LittleFS.end();
		ws.enable(false);
		LittleFS.format();
		ESP.restart();
	}

	if (config.autoRestartIntervalSeconds > 0 && uptimeSeconds > config.autoRestartIntervalSeconds)
	{
		writeEvent("WARN", "sys", "Auto restarting...", "");
#ifdef DEBUG
		Serial.println(F("[ WARN ] Auto retarting..."));
#endif
		shouldReboot = true;
	}

	if (shouldReboot)
	{
		writeEvent("INFO", "sys", "System is going to reboot", "");
#ifdef DEBUG
		Serial.println(F("[ INFO ] System is going to reboot..."));
#endif
		LittleFS.end();
		ESP.restart();
	}

	if (WiFi.isConnected())
	{
		wiFiUptimeMillis += deltaTime;
	}

	if (config.wifiTimeout > 0 && wiFiUptimeMillis > (config.wifiTimeout * 1000) && WiFi.isConnected())
	{
		writeEvent("INFO", "wifi", "WiFi is going to be disabled", "");
#ifdef DEBUG
		Serial.println(F("[ INFO ] WiFi is going to be disabled..."));
#endif
		disableWifi();

	}

	// don't try connecting to WiFi when waiting for pincode
	if (doEnableWifi == true && keyTimer == 0 && activateRelay[0] == true)
	{
		if (!WiFi.isConnected())
		{
			enableWifi();
			writeEvent("INFO", "wifi", "Enabling WiFi", "");
			doEnableWifi = false;
#ifdef DEBUG
		Serial.println(F("[ INFO ] Enabling WiFi..."));
#endif
		}
	}

	if (config.mqttEnabled && mqttClient.connected())
	{
		if ((unsigned)epoch > nextbeat)
		{
			mqttPublishHeartbeat(epoch, uptimeSeconds);
			nextbeat = (unsigned)epoch + config.mqttInterval;
#ifdef DEBUG
			Serial.print("[ INFO ] Nextbeat=");
			Serial.println(nextbeat);
#endif
		}
		processMqttQueue();
	}

	processWsQueue();

	// clean unused websockets
	ws.cleanupClients();
}
