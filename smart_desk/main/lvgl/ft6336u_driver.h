/**
 * @file ft6336u_driver.h
 * @brief FT6336U 触摸屏驱动接口
 */

#ifndef _FT6336U_DRIVER_H
#define _FT6336U_DRIVER_H

#include "esp_err.h"
#include "driver/gpio.h"
#include "soc/gpio_num.h"

/**
 * @brief 初始化 FT6336U 触摸屏控制器
 * @param SDA I2C 数据线 GPIO
 * @param SCL I2C 时钟线 GPIO
 * @return ESP_OK 成功，其他失败
 */
esp_err_t ft6336_driver(gpio_num_t SDA, gpio_num_t SCL);

#endif
