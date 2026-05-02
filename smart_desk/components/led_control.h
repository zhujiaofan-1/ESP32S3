/**
 * @file led_control.h
 * @brief LED灯光控制接口
 *
 * 提供白光LED亮度控制和WS2812 RGB灯控制功能
 */

#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include <stdbool.h>

extern int RGB_red;          /**< RGB红色分量（0-255） */
extern int RGB_green;        /**< RGB绿色分量（0-255） */
extern int RGB_blue;         /**< RGB蓝色分量（0-255） */
extern int LED_brightness;   /**< 白光LED亮度（0-100） */
extern int RGB_brightness;   /**< RGB灯亮度（0-100） */

/**
 * @brief 设置白光LED亮度
 *
 * @param brightness 亮度值（0-100），0为关闭
 */
void light_control(int brightness);

/**
 * @brief 恢复上次有效的亮度值
 */
void light_restore_brightness(void);

/**
 * @brief 设置RGB灯颜色和亮度
 *
 * @param brightness 亮度（0-100）
 * @param red 红色分量（0-255）
 * @param green 绿色分量（0-255）
 * @param blue 蓝色分量（0-255）
 */
void RGB_control(int brightness, int red, int green, int blue);

/**
 * @brief 初始化LED控制模块
 *
 * 初始化WS2812接口和LEDC PWM通道
 */
void led_control_Init(void);

/**
 * @brief 获取LED开关状态
 *
 * @return true 灯亮，false 灯灭
 */
bool LED_state(void);

/**
 * @brief 更新UI上的slider值
 *
 * OneNet修改值后调用，同步更新控制页面的滑块位置
 */
void update_slider_values(void);

#endif