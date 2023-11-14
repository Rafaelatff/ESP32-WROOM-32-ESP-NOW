#include <stdio.h>
#include <string.h> // to use memcpy functions
#include "sdkconfig.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/Task.h"
#include "esp_log.h"
#include "esp_now.h"
#include "esp_wifi.h" // to use  esp_wifi_get_mac function
#include "esp_mac.h" // to use esp_base_mac_addr_set
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"


#define TAG "my_tag"
#define ESP_CHANNEL 1
#define BLUE_LED GPIO_NUM_2

static uint8_t peer_mac [ESP_NOW_ETH_ALEN] = {0xa0, 0xb7, 0x65, 0x63, 0x96, 0x04};
//Broadcast = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}



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

void recv_cb(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len){
    ESP_LOGI(TAG, "Data received: " MACSTR "%s", MAC2STR(esp_now_info->src_addr), data);
}

void send_cb(const uint8_t *mac_addr, esp_now_send_status_t status){
    if(status == ESP_NOW_SEND_SUCCESS){
        ESP_LOGI(TAG, "ESP_NOW_SEND_SUCCESS");
    }
    else{
        ESP_LOGI(TAG, "ESP_NOW_SEND_FAIL");
    }
}

static esp_err_t init_esp_now(void){
    esp_now_init();
    esp_now_register_recv_cb(recv_cb);
    esp_now_register_send_cb(send_cb);

    ESP_LOGI(TAG, "ESP NOW initialization completed.");
    return ESP_OK;
}

static esp_err_t register_peer(uint8_t *peer_addr){
    esp_now_peer_info_t esp_now_peer_info = {};
    memcpy(esp_now_peer_info.peer_addr, peer_mac, ESP_NOW_ETH_ALEN);
    esp_now_peer_info.channel = ESP_CHANNEL;
    esp_now_peer_info.ifidx = ESP_IF_WIFI_STA;

    esp_now_add_peer(&esp_now_peer_info);
    return ESP_OK;
}

static esp_err_t esp_now_send_data(const uint8_t *peer_addr, const uint8_t *data, uint8_t len){
    esp_now_send(peer_addr, data, len);
    return ESP_OK;
}

void app_main(void)
{
    gpio_set_direction(GPIO_NUM_2,GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_2,1); // To test - GPIO2 is connected to the anode of blue led.
    printf("Iniciando o bagulho. \n\n");

    ESP_ERROR_CHECK(init_wifi());
    ESP_ERROR_CHECK(init_esp_now());
    ESP_ERROR_CHECK(register_peer(peer_mac));

    uint8_t data_LED_ON[] = "1";
    uint8_t data_LED_OFF[] = "0";

    while(1){
        gpio_set_level(BLUE_LED,1);
        esp_now_send_data(peer_mac, data_LED_ON, 32);
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 sec
         gpio_set_level(BLUE_LED,0);
        esp_now_send_data(peer_mac, data_LED_OFF, 32);
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 sec
    }

}