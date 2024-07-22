# Patch for JL1101 Ethernet PHY

## Add the driver to the ESP-IDF Framework

copy this file [esp_eth_phy_jl1101.c](https://github.com/esphome/esphome/blob/dev/esphome/components/ethernet/esp_eth_phy_jl1101.c) to   

C:\Espressif\frameworks\esp-idf-v5.1\components\esp_eth\src [^1]   

## Add Source to CMakeLists.txt

edit the file CMakeLists.txt in the directory C:\Espressif\frameworks\esp-idf-v5.1\components\esp_eth [^1].    
Change the section beginning from line 22 [^2] to:
'''
    if(CONFIG_ETH_USE_ESP32_EMAC)
        list(APPEND srcs "src/esp_eth_mac_esp.c"
                         "src/esp_eth_phy_dp83848.c"
                         "src/esp_eth_phy_ip101.c"
                         "src/esp_eth_phy_ksz80xx.c"
                         "src/esp_eth_phy_lan87xx.c"
                         "src/esp_eth_phy_rtl8201.c"
			             "src/esp_eth_phy_jl1101.c")
'''
## Add prototype of esp_eth_phy_new_jl1101() to esp_eth_phy.h

edit the file esp_eth_phy.h in directory C:\Espressif\frameworks\esp-idf-v5.1\components\esp_eth\include [^1].

add the following in line 302 [^2] after the definition of RTL8201.
'''
/**
* @brief Create a PHY instance of JL1101
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_new_jl1101()
'''

## Configuration

| PIN | Function |
|:---:|:--------:|
| 0   | power |
| 17  | ref_cl |
| 18  | mdio |
| 23  | mdc |

phy_addr = 1     
ref_clk_mode = Pin.OUT


[^1]: Version number of the ESP-IDF framework could be different      
[^2]: Line number could be different due to other version