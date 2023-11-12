# ESP32-WROOM-32-ESP-NOW
Repository created to gather all the necessary information regarding the ESP-NOW protocol.

# ESP NOW - Creating an Ad Hoc Network

[ESP NOW](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/api-reference/network/esp_now.html) is a Espressif protocol that can be used to connect several ESP32 devices without the need of an Access Point. I will be exploring this protocol since I need to create an Ad Hoc Network to transmit information (MoT) between the network. I am not sure this will work, but we need to start moviment. I won't be using TCP or UDP protocols.

I created a new project according to [my ESP32 Basics repositorie](https://github.com/Rafaelatff/ESP32-WROOM-32-Basics/blob/main/README.md). Then on **menuconfig** I set the option for WiFi [CSI](https://github.com/Rafaelatff/ESP32-WROOM-32-Basics/blob/main/README.md), so I can run some small test on that, after my network is set up. 

![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/af3a1cbf-2ee7-4882-91fc-19c027e0eb52)

To work with the ESP now, we need:

* Init WiFi.
* Init ESP NOW.
* Register peers (devices).
* Communicate (send and receive the callbacks to registered peers or a broadcast transmittion).

Here we are considering that all the necessary code to work with LOGs, GPIOs and time (FreeRTOS and Tasks) are already set.

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

I did the same for all the boards, and they returned the same MAC address. Later I discovery that when correctly configured a different and unique MAC value is set for the ESP32 board.

## ESP NOW - WiFi Init

The WiFi programming model can be depicted as following picture, as presented in the **esp_wifi.h**, an Espressif library.

![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/f30edc86-8e08-48f6-ae62-a94da62a6cea)

We start by adding some necessary libraries, the WiFi itself, libaries for the mac (esp_mac.h), network stack (esp_netif.h), to handling the events (esp_event.h) and also for the Non-Volatile Memory (nvs_flash.h), necessary to work with the WiFi.

```c
#include "esp_wifi.h" // to use  esp_wifi_get_mac function
#include "esp_mac.h" // to use esp_base_mac_addr_set
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
```

```
static esp_err_t init_wifi(void){
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT(); //  // usa macro default

    esp_netif_init();
    esp_event_loop_create_default();
    nvs_flash_init();
    esp_wifi_init(&wifi_init_config);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    esp_wifi_start();

    ESP_LOGI(TAG, "WiFi initialization completed.");
    return ESP_OK;
}
```

## ESP NOW - Esp Now Init

```c
#include "esp_now.h"
```

# Bibliography

All the links that help me through the process of ESP32 learning.

* []().
