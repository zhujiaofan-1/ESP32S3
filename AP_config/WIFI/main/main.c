#include <stdio.h>
#include "WIFI_manager.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#define TAG     "main"

const char* ssid = "TEST";
const char* password = "12345678q";

void WIFI_manager_callback(WIFI_STATE state)
{
    if(state == WIFI_STATE_CONNECTED)
    {
        ESP_LOGI(TAG,"WIFI连接成功！");
    }else if(state == WIFI_STATE_DISCONNECTED)
    {
        ESP_LOGI(TAG,"WIFI断开连接！");
    }
}


void app_main(void)
{
    nvs_flash_init();
    WIFI_manager_Init(WIFI_manager_callback);

    WIFI_manager_connect(ssid, password);
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    
}
