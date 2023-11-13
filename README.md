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

On `void app_main(void)`, the fist function we are going to call will be the `init_wifi()`. But while creating this function, we will be considering that this will be a function that must return the status, if an error happened or if it went okay. When we call the `init_wifi()`, we will be calling by using the defined function of `ESP_ERROR_CHECK` (part of the esp_err.h, indirectly declared). Considering that, the last line of the `init_wifi()` will be a `return ESP_OK;` with type `esp_err_t`.

![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/1edaaee0-7493-4664-83e5-8c59d4dba730)

Then, we will be initializing a struct of type `wifi_init_config_t`, tha is used to pass the WiFi parameters to the esp function `esp_wifi_init()`. To easy this configuration, we will be using the default macro `WIFI_INIT_CONFIG_DEFAULT()`.
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/821cd984-8598-4a82-8b3b-d0883165363e)
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/b85d598e-f832-499b-8123-be760b64c1a0)

Then we will be calling the `esp_netif_init()`:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/155dfca1-8d55-41db-8de1-53c14c9236a1)

Also the `esp_event_loop_create_default()`:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/13a7eec1-acee-4aa4-a2e1-f2140ec9d7f3)

And the `nvs_flash_init()`:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/018d9509-6d37-4a38-993a-75a49906c87c)
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/b86d035a-36de-4969-8fb4-236bfe4d8fda)

Then we pass the struct with the default configuration to the `esp_wifi_init(&wifi_init_config)`:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/d33dd127-8930-4385-bb71-09465b7d886a)
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/13a9e1cd-3a56-4642-86a4-a29773791fee)

Then we set the WiFi mode to Station by `esp_wifi_set_mode(WIFI_MODE_STA)`:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/09925326-4662-45fa-9bb1-543ed84de1f3)
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/88c76dc6-f9e6-4308-bbe0-bfc1e113d5d5)

We set the storage memory for the WiFi as Flash, by `esp_wifi_set_storage(WIFI_STORAGE_FLASH)`:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/8d8c039a-55c4-4c1b-b132-79b0c50ff6c7)
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/6be1055b-34c7-4f40-a12e-c6dc479e8c4f)

And to finish the WiFi initialization we call `esp_wifi_start()`:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/f72ce596-85e7-4a10-bd3c-ca0c7b9c06c0)

The complete code stays as:

```c
static esp_err_t init_wifi(void){
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT(); // macro default

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

Again, on the `void app_main(void)`, we call the `init_esp_now()` by using the defined function of `ESP_ERROR_CHECK`and returning a `ESP_OK` at the end. We also call the `esp_now.h` library.

```c
#include "esp_now.h"

void app_main(void)
{
    ESP_ERROR_CHECK(init_wifi()); // Already explained
    ESP_ERROR_CHECK(init_esp_now());
} 
```
The function begins with a `esp_now_init()`, to initializate the Esp Now.
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/406ef94d-26b4-4c7b-ada7-54d915e91e44)

We call the `esp_now_register_recv_cb()` to register callback function of receiving ESPNOW dataand pass the data `recv_cb` and set a LOGI:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/a1af1c05-2274-4cc5-a43c-37548349e356)
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/c89e4d14-5acb-484c-b129-849652292efb)
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/3fa7ff61-b66f-4044-a59f-b9e82894bae2)

The same for the `esp_now_register_send_cb()`:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/2311773c-7404-4a56-b21e-9c56cc629d33)
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/6b76ef74-347b-468f-95c3-bd8bb3a0d6d9)


The entire `init_esp_now()` code stays as:
```c
static esp_err_t init_esp_now(void){
    esp_now_init();
    esp_now_register_recv_cb(recv_cb);
    esp_now_register_send_cb(send_cb);

    ESP_LOGI(TAG, "ESP NOW initialization completed.");
    return ESP_OK;
}
```

## ESP NOW - Peer register

The entire `register_peer()` code stays as:
```c
static esp_err_t register_peer(uint8_t *peer_addr){
    esp_now_peer_info_t esp_now_peer_info = {};
    memcpy(esp_now_peer_info.peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
    esp_now_peer_info.channel = ESP_CHANNEL;
    esp_now_peer_info.ifidx = ESP_IF_WIFI_STA;

    esp_now_add_peer(&esp_now_peer_info);
    return ESP_OK;
}
```

# Bibliography

All the links that help me through the process of ESP32 learning.

* [ESP32 IDF SDK 35: ESP NOW unidireccional - initiator](https://www.youtube.com/watch?v=-FFhONBzRZ4&list=PL-Hb9zZP9qC65SpXHnTAO0-qV6x5JxCMJ&index=36).
* [ESP32 IDF SDK 36: ESP NOW unidireccional - responder](https://www.youtube.com/watch?v=HIJFqvpriqg&list=PL-Hb9zZP9qC65SpXHnTAO0-qV6x5JxCMJ&index=37).
