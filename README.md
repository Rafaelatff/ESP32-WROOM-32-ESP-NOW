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

```c
void recv_cb(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len){
    ESP_LOGI(TAG, "Data received: " MACSTR "%s", MAC2STR(esp_now_info->src_addr), data);
}
```
Where, `MACSTR` and `MAC2STR` are defined as:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/4e1dc9ac-67af-48bd-b297-c20e54d9687e)


The same for the `esp_now_register_send_cb(send_cb))`:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/2311773c-7404-4a56-b21e-9c56cc629d33)
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/6b76ef74-347b-468f-95c3-bd8bb3a0d6d9)

```c
void send_cb(const uint8_t *mac_addr, esp_now_send_status_t status){
    if(status == ESP_NOW_SEND_SUCCESS){
        ESP_LOGI(TAG, "ESP_NOW_SEND_SUCCESS");
    }
    else{
        ESP_LOGI(TAG, "ESP_NOW_SEND_FAIL");
    }
}
```

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

We start by declaring a variable with the name `peer_mac`, with value of 0xFFFFFFFFFF, that will broadcast the messages. Later, when we discover the MAC address of the boards, we will change it to the desired address that we want to register as peer.

```c
static uint8_t peer_mac [ESP_NOW_ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
```

We also define the channel that we want to use, at the begin of the code. In this way, is easy to change the channel later, instead of changing manually in each place a channel is configured.

```c
#define ESP_CHANNEL 1
```

And again, on the `void app_main(void)`, we call the `register_peer()` and pass as argument the global variable that we just defined `peer_mac`. We do all that by using the defined function of `ESP_ERROR_CHECK`and returning a `ESP_OK` at the end.

```c
#include "esp_now.h"

void app_main(void)
{
    ESP_ERROR_CHECK(init_wifi()); // Already explained
    ESP_ERROR_CHECK(init_esp_now()); // Already explained
    ESP_ERROR_CHECK(register_peer(peer_mac));
} 
```

The function will receive as argument `uint8_t *peer_addr` to the function `register_peer()`, 

We start by creating a struct of type `esp_now_peer_info_t`:
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/591abaa8-8382-4018-82b8-c15e442aae5a)

Then we use the function `memcpy()` that copies a block of memory from a source address to a destination address. We also need to include the `<string.h>` to use memcpy function. The general syntax of the memcpy() function is:

```c
void *memcpy(void *dest, const void *src, size_t n);
```
Where: 

* dest: A pointer to the destination array where the content is to be copied.
* src: A pointer to the source of data to be copied.
* n: The number of bytes to copy.

In our case we will be using:

```c
memcpy(esp_now_peer_info.peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
```
To:

* esp_now_peer_info.peer_addr will be the destination, where the content is to be copied.
* peer_mac is the source of data to be copied, in our case, the global variable that we just declared.
* ESP_NOW_ETH_ALEN, as the number of bytes to copy.

![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/6b50b008-a6ce-4ff2-b9e6-55653aeee840)

We pass the channel to the struct that we just created, `esp_now_peer_info.channel = ESP_CHANNEL;`.
And we pass the `ESP_IF_WIFI_STA` `enum type` to the `ifidx` part of this same struct, `esp_now_peer_info.ifidx = ESP_IF_WIFI_STA;`.
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/5080f21e-89e8-4602-8451-b4e5c777e639)

To finish, we pass the struct to the `esp_now_add_peer(&esp_now_peer_info)` function.
![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/7e67f70f-2fa4-4b47-8911-70001b43363a)

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

## ESP NOW - Sending Data

To send data, we declared the following function:

```c
static esp_err_t esp_now_send_data(const uint8_t *peer_addr, const uint8_t *data, uint8_t len){
    esp_now_send(peer_addr, data, len);
    return ESP_OK;
}
```

That basically will call the `esp_now_send()`, defined in the `esp_now.h` library

![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/8a25abce-ef6b-4dd0-9f5f-bebad5d02b33)

![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/a41e9eeb-67b9-48dd-8bdc-f79ebe2d691c)

At this point, we can have an ESP32 programmed and sending data (THIS CODE CAN ALSO BE USED TO DISCOVER THE MAC ADDRESS OF THE ESP BOARD).

Inside the `app_main(void)`, we will add the following code:

```c
    uint8_t data_LED_ON[] = "1";
    uint8_t data_LED_OFF[] = "0";

    while(1){
        gpio_set_level(BLUE_LED,1); // #define BLUE_LED GPIO_NUM_2 at the begin of the code
        esp_now_send_data(peer_mac, data_LED_ON, 32); // 32 is way more len than needed,
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 sec
         gpio_set_level(BLUE_LED,0);
        esp_now_send_data(peer_mac, data_LED_OFF, 32);
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 sec
    }
```
After `build`, `flash` and `monitor`:

![image](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/0b074db7-a947-490d-84b1-2b8076e2905c)

Then I recober the MAC address of two boards:

* `a0:b7:65:63:96:04`
* `48:e7:29:ca:f9:78`

To test the code, I changed the MAC address of broadcast to one of these MAC, and programmed the board with the other MAC with all the functions presented in this repositorie, plus the **ESP NOW - Receiving Data** part (and different `app_main(void)` of course!).

The MAC modification:

```c
static uint8_t peer_mac [ESP_NOW_ETH_ALEN] = {0xa0, 0xb7, 0x65, 0x63, 0x96, 0x04};
//Broadcast = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
```

All this code is inside this repositorie, on the `esp-now-idf` folder.

## ESP NOW - Receiving Data

The ESP32 that will receive the message doesn't need to have a peer_mac configured. The code doesn't show the GPIO configuration, only the WiFi and ESP NOW needed configuration. We will init the WiFi by `init_wifi()` and init the ESP NOW `init_esp_now()` and then stay forever inside a `while(1)` loop.

```c
void app_main(void)
{
    ESP_ERROR_CHECK(init_wifi());
    ESP_ERROR_CHECK(init_esp_now());
    while(1){
    }
}
```    
The received message will be received on the already written, `recv_cb()` function. We will just implement a few things.

Once the send data is "1" or "0" (`uint8_t data_LED_ON[] = "1"; uint8_t data_LED_OFF[] = "0";`), the receive data will be a string that can be either the char "1" or "0". To convert the char to an integer, we use the `atoi()` function. The atoi function stands for ASCII to Integer. We will pass the result of the `atoi()` function to ah variable named `value` of `uint8_t` type.

We will pass data, typecasted as char, `uint8_t value = atoi((char *) data);`. If the value is 1, it lid its own blue LED. If the received value is 0, then the board will turn off its own LED.

Function will be like:

```c
void recv_cb(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len){
    ESP_LOGI(TAG, "Data received: " MACSTR "%s", MAC2STR(esp_now_info->src_addr), data);

    uint8_t value = atoi((char *) data);
    if(value == 1){
        gpio_set_level(BLUE_LED,1);
    }
    else{
        gpio_set_level(BLUE_LED,0);
    }
}
```

After `build`, `flash` and `monitor`:

![WhatsApp Video 2023-11-13 at 23 19 29](https://github.com/Rafaelatff/ESP32-WROOM-32-ESP-NOW/assets/58916022/509cb131-bc4f-4e67-a58d-231ea3cb21a3)

All this code is inside this repositorie, on the `esp-now-idf-responder` folder.

# Bibliography

All the links that help me through the process of ESP32 learning.

* [ESP32 IDF SDK 35: ESP NOW unidireccional - initiator](https://www.youtube.com/watch?v=-FFhONBzRZ4&list=PL-Hb9zZP9qC65SpXHnTAO0-qV6x5JxCMJ&index=36).
* [ESP32 IDF SDK 36: ESP NOW unidireccional - responder](https://www.youtube.com/watch?v=HIJFqvpriqg&list=PL-Hb9zZP9qC65SpXHnTAO0-qV6x5JxCMJ&index=37).
