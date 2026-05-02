/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file led_ws2812.h
 * @brief WS2812 RGB LED驱动接口
 *
 * 提供WS2812可寻址RGB LED灯带的初始化、颜色设置和亮度控制功能
 */

#pragma once

#include <stdint.h>
#include "driver/rmt_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief WS2812灯带句柄类型
 */
typedef struct ws2812_strip_t *ws2812_strip_handle_t;

/**
 * @brief 初始化WS2812外设
 *
 * @param gpio 控制WS2812的管脚
 * @param maxled 控制WS2812的个数
 * @param led_handle 返回的控制句柄
 * @return ESP_OK or ESP_FAIL
*/
esp_err_t ws2812_init(gpio_num_t gpio,int maxled,ws2812_strip_handle_t* led_handle);

/**
 * @brief 反初始化WS2812外设
 *
 * @param handle 初始化的句柄
 * @return ESP_OK or ESP_FAIL
*/
esp_err_t ws2812_deinit(ws2812_strip_handle_t handle);

/**
 * @brief 向某个WS2812写入RGB数据
 *
 * @param handle 句柄
 * @param index 第几个WS2812（0开始）
 * @param r 红色分量（0-255）
 * @param g 绿色分量（0-255）
 * @param b 蓝色分量（0-255）
 * @return ESP_OK or ESP_FAIL
*/
esp_err_t ws2812_write(ws2812_strip_handle_t handle,uint32_t index,uint32_t r,uint32_t g,uint32_t b);

/**
 * @brief 设置某个WS2812亮度
 *
 * @param handle 句柄
 * @param index 第几个WS2812（0开始）
 * @param brightness 亮度 (0-100)
 * @return ESP_OK or ESP_FAIL
*/
esp_err_t ws2812_set_brightness(ws2812_strip_handle_t handle,uint32_t index,uint32_t brightness);

/**
 * @brief 获取某个WS2812亮度
 *
 * @param handle 句柄
 * @param index 第几个WS2812（0开始）
 * @return 亮度值
*/
uint32_t ws2812_get_brightness(ws2812_strip_handle_t handle,uint32_t index);

#ifdef __cplusplus
}
#endif
