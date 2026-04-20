#include <stdio.h>
#include "button.h"
#include "XL9555.h"
#include "esp_log.h"

#define TAG     "Button"

//通过在xl9555中改变这个变量的值，就能体现出按键的高低电平
volatile uint16_t XL9555_Button_Level = 0xffff;     
// XL9555回调函数
void XL9555_Input_callback(uint16_t Pin, int Level)
{
    if (Level == 1)
    {
        XL9555_Button_Level |= Pin;
    }else{
        XL9555_Button_Level &= ~Pin;
    }
}

//按键回调函数
void btn_short_press_callback(int gpio)
{
    ESP_LOGI(TAG," 检测到短按,Button:%d",gpio);
}

void btn_long_press_callback(int gpio)
{
    ESP_LOGI(TAG," 检测到长按,Button:%d",gpio);
}

//获取gpio电平回调函数
int get_gpio_level_callback(int gpio)
{
    // 与特定的引脚相与不为0，则return  1
    return XL9555_Button_Level&gpio?1:0;
}

void app_main(void)
{
    XL9555_Init(GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_17, XL9555_Input_callback);
    XL9555_IO_Cofig(0xffff);    //设置为输入模式
    //注册按键
    button_config_t btn_cfg =
    {
        .active_level = 0,
        .long_press_time = 1000,
        .gpio_num = IO0_1,
        .short_press_cb = btn_short_press_callback,
        .long_press_cb = btn_long_press_callback,
        .getlevel_cb = get_gpio_level_callback,
    };
    Button_Add(&btn_cfg);
    btn_cfg.gpio_num = IO0_2;
    Button_Add(&btn_cfg);
    btn_cfg.gpio_num = IO0_3;
    Button_Add(&btn_cfg);
    btn_cfg.gpio_num = IO0_4;
    Button_Add(&btn_cfg);
}
