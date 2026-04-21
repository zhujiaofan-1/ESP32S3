#include <stdio.h>
#include "button.h"
#include "XL9555.h"
#include "esp_log.h"
#include "AP_WIFI.h" 
#include "nvs_flash.h"

#define TAG     "Button"

//通过在xl9555中改变这个变量的值，就能体现出按键的高低电平
static volatile uint16_t XL9555_Button_Level = 0xffff;     

// XL9555的电平变化回调函数
void XL9555_Input_callback(uint16_t Pin, int Level)
{
    if (Level == 1)
    {
        XL9555_Button_Level |= Pin;
    }else{
        XL9555_Button_Level &= ~Pin;
    }
}

//长按回调函数
void btn_long_press_callback(int gpio)
{
    ESP_LOGI(TAG," 进入AP配网模式");
    AP_WIFI_apcfg();
}

//获取gpio电平回调函数
int get_gpio_level_callback(int gpio)
{
    // 与特定的引脚相与不为0，则return  1
    return XL9555_Button_Level&gpio?1:0;
}

//button初始化
void button_Init(void)
{
    XL9555_Init(GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_17, XL9555_Input_callback);
    XL9555_IO_Cofig(0xffff);    //设置为输入模式
    //注册按键
    button_config_t btn_cfg =
    {
        .active_level = 0,
        .long_press_time = 1000,
        .gpio_num = IO0_1,
        .long_press_cb = btn_long_press_callback,
        .getlevel_cb = get_gpio_level_callback,
    };
    Button_Add(&btn_cfg);
}

//WIFI状态回调函数
void wifi_stat_callback(WIFI_STATE state)
{
    if(state == WIFI_STATE_CONNECTED)
    {
        ESP_LOGI(TAG, "WIFI已连接！");
    }
    if(state == WIFI_STATE_DISCONNECTED)
    {
        ESP_LOGI(TAG, "WIFI已断开！");
    }
}


void app_main(void)
{
    nvs_flash_init();

    button_Init();

    //初始化WIFI
    AP_WIFI_Init(wifi_stat_callback);

}
