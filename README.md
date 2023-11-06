# ESP32-WROOM-32-ESP-NOW
Repository created to gather all the necessary information regarding the ESP-NOW protocol.

# ESP NOW - Creating an Ad Hoc Network

[ESP NOW](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/network/esp_now.html) is a Espressif protocol that can be used to connect several ESP32 devices without the need of an Access Point. I will be exploring this protocol since I need to create an Ad Hoc Network to transmit information (MoT) between the network. I am not sure this will work, but we need to start moviment. I won't be using TCP or UDP protocols.

I created a new project according to [my ESP32 Basics repositorie](https://github.com/Rafaelatff/ESP32-WROOM-32-Basics/blob/main/README.md). Then on **menuconfig** I set the option for WiFi [CSI](https://github.com/Rafaelatff/ESP32-WROOM-32-Basics/blob/main/README.md), so I can run some small test on that, after my network is set up. 

![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/af3a1cbf-2ee7-4882-91fc-19c027e0eb52)

## MAC Discovery

To discovery the ESP MAC Address, the following code presented necessary:

```c
#include "esp_log.h"
#include "esp_wifi.h" // to use  esp_wifi_get_mac function

void app_main(void)
{
    // Get MAC Address from device
    uint8_t mac[6];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);  // MAC Address from WiFi Station (STA)
    
    // Print MAC Address
    ESP_LOGI("MAC", "Endereço MAC: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
```

Board 'B': `30:39:40:3f:00:00`.

![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/5a6c1f33-4f27-40d7-8ad0-80e5e9212089)

I did the same for all the boards, and they returned the same MAC address. To edit the MAC Address, I had to add a few more lines:

```c
#include "esp_mac.h" // to use esp_base_mac_addr_set

```

