struct Config {

#ifdef GENERIC
    #define MAX_NUM_RELAYS 4
    int relayPin[MAX_NUM_RELAYS];
    uint8_t accessdeniedpin = 255;
    uint8_t beeperpin = 255;
    uint8_t doorstatpin = 255;
    uint8_t ledwaitingpin = 255;
    uint8_t openlockpin = 255;
    uint8_t wifipin = 255;
    int wgd0pin = 255;
    int wgd1pin = 255;
#endif

#ifdef APWIKOGER
    #define MAX_NUM_RELAYS 1
    int relayPin[MAX_NUM_RELAYS] = {16};
    uint8_t accessdeniedpin = 255;
    uint8_t beeperpin = 13;
    uint8_t doorstatpin = 255;
    uint8_t ledwaitingpin = 14;
    uint8_t openlockpin = 255;
    uint8_t wifipin = 255;
    int wgd0pin = 33;
    int wgd1pin = 26;
#endif

#ifdef DTWONDER
    #define MAX_NUM_RELAYS 2
    int relayPin[MAX_NUM_RELAYS] = {16, 2};
    uint8_t accessdeniedpin = 255;
    uint8_t beeperpin = 1;
    uint8_t doorstatpin = 39;
    uint8_t ledwaitingpin = 3;
    uint8_t openlockpin = 36;
    uint8_t wifipin = 255;
    int wgd0pin = 4;
    int wgd1pin = 5;
#endif

#ifdef LILYGO
    #define MAX_NUM_RELAYS 1
    int relayPin[MAX_NUM_RELAYS] = {32};
    uint8_t accessdeniedpin = 255;
    uint8_t beeperpin = 15;
    uint8_t doorstatpin = 33;
    uint8_t ledwaitingpin = 13;
    uint8_t openlockpin = 35;
    uint8_t wifipin = 255;
    int wgd0pin = 14;
    int wgd1pin = 2;
#endif

#ifdef ETHERNET
    IPAddress ipAddressEth = (192,168,5,1);
    IPAddress gatewayIpEth = (0,0,0,0);
    IPAddress subnetIpEth = (255,255,255,0);
    IPAddress dnsIpEth = (0,0,0,0);
    String ethlink = "not connected";
    String ethmac = "";
#endif

    bool accessPointMode = false;
    IPAddress accessPointIp;
    IPAddress accessPointSubnetIp;
    unsigned long activateTime[MAX_NUM_RELAYS];
    unsigned long autoRestartIntervalSeconds = 0;
    unsigned long beeperInterval = 0;
    unsigned long beeperOffTime = 0;
    byte bssid[6] = {0, 0, 0, 0, 0, 0};
    char *deviceHostname = NULL;
    bool dhcpEnabled = true;
    bool dhcpEnabledEth = true;
    IPAddress dnsIp;
    uint8_t doorbellpin = 255;
    char *doorName[MAX_NUM_RELAYS];
    bool fallbackMode = false;
    IPAddress gatewayIp;
    char *httpPass = NULL;
    IPAddress ipAddress;
    int lockType[MAX_NUM_RELAYS];
    uint8_t maxOpenDoorTime = 0;
    bool mqttAutoTopic = false;
    bool mqttEnabled = false;
    bool mqttEvents = false;	  // Sends events over MQTT disables LittleFS file logging
    bool mqttHA = false; // Sends events over simple MQTT topics and AutoDiscovery
    char *mqttHost = NULL;
    unsigned long mqttInterval = 180; // Add to GUI & json config
    char *mqttPass = NULL;
    int mqttPort;
    char *mqttTopic = NULL;
    char *mqttUser = NULL;
    bool networkHidden = false;
    char *ntpServer = NULL;
	int ntpInterval = 0;
    int numRelays = 1;
    char *openingHours[7];
    char *openingHours2[7];
    bool pinCodeRequested = false;
    bool pinCodeOnly = false;
    bool wiegandReadHex = true;
    bool present = false;
    int readertype = 1;
    int relayType[MAX_NUM_RELAYS];
    bool removeParityBits = true;
    IPAddress subnetIp;
    const char *ssid;
    char *tzInfo = (char *)"";
    const char *wifiApIp = NULL;
    const char *wifiApSubnet = NULL;
    const char *wifiPassword = NULL;
    unsigned long wifiTimeout = 0;
    int wiegandbits = 58;
};
