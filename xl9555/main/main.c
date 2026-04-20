#include <stdio.h>
#include "XL9555.h"
#include "esp_log.h"

#define TAG     "main"

void XL9555_callback(uint16_t Pin, int Level)
{
    switch (Pin)
    {
        case IO0_1:
            ESP_LOGI(TAG, "Button 1 :%d",Level);
            gpio_set_level(GPIO_NUM_15, !Level);
            break;
        
        case IO0_2:
            ESP_LOGI(TAG, "Button 2 :%d",Level);
            break;
        
        case IO0_3:
            ESP_LOGI(TAG, "Button 3 :%d",Level);
            break;

        case IO0_4:
            ESP_LOGI(TAG, "Button 4 :%d",Level);
            break;
    
        default:break;
    }
}
void app_main(void)
{
    XL9555_Init(GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_17, XL9555_callback);
    
    XL9555_IO_Cofig(0xFFFF);

    gpio_config_t LED_config = 
    {
        .pin_bit_mask = (1 << GPIO_NUM_15),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&LED_config);

}
