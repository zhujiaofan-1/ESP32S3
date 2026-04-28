/**
 * @file lv_port.h
 * @brief LVGL LCD 显示驱动接口
 */

#ifndef _LV_PORT_H
#define _LV_PORT_H
#include "esp_err.h"

/**
 * @brief 初始化 LCD 硬件（Intel 8080 总线和 ST7789 控制器）
 */
void lv_display_hard_Init(void);

/**
 * @brief 初始化 LVGL 端口并注册显示设备
 * @return ESP_OK 成功，其他失败
 */
esp_err_t lv_port_Init(void);

#endif
