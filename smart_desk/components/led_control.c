/**
 * @file led_control.c
 * @brief LED灯光控制实现文件
 *
 * 实现白光LED亮度控制（LEDC PWM）和WS2812 RGB灯控制
 */

#include "led_control.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "led_ws2812.h"
#include "custom.h"
#include "gui_guider.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "portmacro.h"
#include <stddef.h>
#include "esp_lvgl_port.h"

extern lv_ui guider_ui;

//ws2812操作句柄
static ws2812_strip_handle_t    ws2812_handle = NULL;

// 存储原始的RGB值（0-255，不乘以亮度）
int RGB_red = 0;
int RGB_green = 0;
int RGB_blue = 0;
int LED_brightness = 0;
int RGB_brightness = 0;
static int last_valid_brightness = 50;  // 保存上次有效的亮度值

/**
 * @brief 更新RGB LED硬件输出
 *
 * 根据RGB颜色值和亮度计算实际输出并写入WS2812
 */
static void update_rgb_hardware(void)
{
    int actual_red = RGB_red * RGB_brightness / 100;
    int actual_green = RGB_green * RGB_brightness / 100;
    int actual_blue = RGB_blue * RGB_brightness / 100;

    for(int i = 0; i < 3; i++)
    {
        ws2812_write(ws2812_handle, i, actual_red, actual_green, actual_blue);
    }
}

/**
 * @brief 设置白光LED亮度
 *
 * @param brightness 亮度值（0-100），0为关闭
 */
void light_control(int brightness)
{
    static bool led_state = false;
    
    if(brightness != 0)
    {
        LED_brightness = brightness;
        last_valid_brightness = brightness;  // 保存上次有效的亮度值
        led_state = true;
    }
    else
    {
        LED_brightness = 0;  // 更新亮度值为 0
        led_state = false;
    }
    
    // 更新 LED 硬件
    int duty = LED_brightness * 4095 / 100;
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty, 0);
    
    // 更新 UI（加锁保护）
    lvgl_port_lock(0);
    if(led_state)
    {
        set_light_state(&guider_ui, true);
    }
    else
    {
        set_light_state(&guider_ui, false);
    }
    lvgl_port_unlock();
}

/**
 * @brief 恢复上次有效的亮度值
 */
void light_restore_brightness(void)
{
    light_control(last_valid_brightness);
}

/**
 * @brief 设置RGB灯颜色和亮度
 *
 * @param brightness 亮度（0-100）
 * @param red 红色分量（0-255）
 * @param green 绿色分量（0-255）
 * @param blue 蓝色分量（0-255）
 */
void RGB_control(int brightness, int red, int green, int blue)
{
    // 只保存原始值，不在这里相乘
    RGB_red = red;
    RGB_green = green;
    RGB_blue = blue;
    RGB_brightness = brightness;

    // 更新硬件
    update_rgb_hardware();
}

/**
 * @brief 初始化LED控制模块
 *
 * 初始化WS2812接口和LEDC PWM通道
 */
void led_control_Init(void)
{
    //初始化WS2812接口
    ws2812_init(GPIO_NUM_18, 3, &ws2812_handle);

    //初始化LEDC
    //初始化定时器
    ledc_timer_config_t led_timer =
    {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .clk_cfg =LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 5000,
        .timer_num = LEDC_TIMER_0,
    };
    ledc_timer_config(&led_timer);

    //初始化PWM
    ledc_channel_config_t led_channel =
    {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = GPIO_NUM_15,
        .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&led_channel);

    //停止之前的渐变
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    //启动渐变
    ledc_fade_func_install(0);
}

/**
 * @brief 获取LED开关状态
 *
 * @return true 灯亮，false 灯灭
 */
bool LED_state(void)
{
    if(LED_brightness == 0)
    {  
        return false;
    }else{
        return true;
    }
}

/**
 * @brief 更新UI上的slider值
 *
 * OneNet修改值后调用，同步更新控制页面的滑块位置
 */
void update_slider_values(void)
{
    lvgl_port_lock(0);
    // 检查slider对象是否存在
    if(guider_ui.screen_control_slider_RGB_red)
    {
        lv_slider_set_value(guider_ui.screen_control_slider_RGB_red, RGB_red, LV_ANIM_OFF);
    }
    if(guider_ui.screen_control_slider_RGB_green)
    {
        lv_slider_set_value(guider_ui.screen_control_slider_RGB_green, RGB_green, LV_ANIM_OFF);
    }
    if(guider_ui.screen_control_slider_RGB_blue)
    {
        lv_slider_set_value(guider_ui.screen_control_slider_RGB_blue, RGB_blue, LV_ANIM_OFF);
    }
    if(guider_ui.screen_control_slider_RGB_bri)
    {
        lv_slider_set_value(guider_ui.screen_control_slider_RGB_bri, RGB_brightness, LV_ANIM_OFF);
    }
    if(guider_ui.screen_control_slider_light_bri)
    {
        lv_slider_set_value(guider_ui.screen_control_slider_light_bri, LED_brightness, LV_ANIM_OFF);
    }
    lvgl_port_unlock();
}
