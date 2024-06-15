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
#define VERSION "1.1.0"

#include "Arduino.h"
#include <WiFi.h>
#include "WiFiEventHandler.h"
#include <SPI.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <TimeLib.h>
#include <Ticker.h>
#include <time.h>
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

	uint32_t realSize = spi_flash_get_chip_size();
	uint32_t ideSize = ESP.getFlashChipSize();
	FlashMode_t ideMode = ESP.getFlashChipMode();
	
	char chipID[15];
  	uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
  	uint16_t chip = (uint16_t)(chipid >> 32);
  	snprintf(chipID, 15, "%04X%08X", chip, (uint32_t)chipid);
	Serial.printf("Flash real id:   %s\n", chipID);

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
	if (!SPIFFS.begin())
	{
		if (SPIFFS.format())
		{
			writeEvent("WARN", "sys", "Filesystem formatted", "");
#ifdef DEBUG
			Serial.println(F("[ WARN ] Filesystem formatted!"));
#endif
		}
		else
		{
#ifdef DEBUG
			Serial.println(F(" failed!"));
			Serial.println(F("[ WARN ] Could not format filesystem!"));
#endif
		}
	}

	bool configured = false;
	configured = loadConfiguration(config);
	setupWifi(configured);
	setupWebServer();
	setupMqtt();
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
	
	openLockButton.update();
	if (config.openlockpin != 255 && openLockButton.fell())
	{
		writeLatest(" ", "Button", 1);
		mqttPublishAccess(epoch, "true", "Always", "Button", " ", " ");
		activateRelay[0] = true;
		beeperValidAccess();
		// TODO: handle other relays
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
			}
			else if ((currentMillis - previousMillis >= config.activateTime[currentRelay]) && (deactivateRelay[currentRelay]))
			{
				mqttPublishIo("lock" + String(currentRelay), "LOCKED");
#ifdef DEBUG
				Serial.println(F(currentMillis));
				Serial.println(F(previousMillis));
				Serial.println(F(config.activateTime[currentRelay]));
				Serial.println(F(activateRelay[currentRelay]));
				Serial.println(F("deactivate relay after this"));
				Serial.print(F("mili : "));
				Serial.println(F(millis()));
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
		SPIFFS.end();
		ws.enable(false);
		SPIFFS.format();
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
		SPIFFS.end();
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
