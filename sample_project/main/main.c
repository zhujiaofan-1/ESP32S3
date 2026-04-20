#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/ledc.h"

void LED_Flash_Init(void)
{
    gpio_config_t LED_cfg=
    {
        .pin_bit_mask = (1 << GPIO_NUM_15),         //初始化GPIO15
        .mode = GPIO_MODE_OUTPUT,
        
    };

    gpio_config(&LED_cfg);                  //初始化GPIO
}

void LED_Breahe_Init(void)
{
    //初始化定时器
    ledc_timer_config_t ledc_timer =
    {
        .clk_cfg = LEDC_AUTO_CLK,           //自动选择时钟源
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,          //0号定时器
        .duty_resolution = LEDC_TIMER_12_BIT,//占空比分辨率，最大计数值2^12
        .freq_hz = 5000,                    //PWM频率 = 定时器频率
    };
    ledc_timer_config(&ledc_timer);

    // 初始化PWM通道
    ledc_channel_config_t ledc_channel = 
    {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = GPIO_NUM_15,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&ledc_channel);

    // 渐变效果
    ledc_fade_func_install(0);          //硬件渐变功能

}


void app_main(void)
{
    #if 0
    int led_level = 0;
    LED_Flash_Init();
    while (1)
    {
        gpio_set_level(GPIO_NUM_15, led_level);     //设置GPIO电平
        vTaskDelay(pdMS_TO_TICKS(500));
        led_level ^= 1;
    }
        #endif

    LED_Breahe_Init();
    while (1)
    {
       // ledc_set_fade_with_time: 设置 LEDC 通道的渐变效果
    // 参数1: speed_mode - 低速度模式 (LEDC_LOW_SPEED_MODE)
    // 参数2: channel - LEDC 通道 0 (LEDC_CHANNEL_0)
    // 参数3: target_duty - 目标占空比 (最大 4095，对应 12 位分辨率)
    // 参数4: fade_duration - 渐变持续时间，单位毫秒 (2000ms = 2秒)
    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,4095,2000);
    ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE);      //完成后才返回

    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_0,0,2000);
    ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE);
    }
    
            //启动渐变
    
}
