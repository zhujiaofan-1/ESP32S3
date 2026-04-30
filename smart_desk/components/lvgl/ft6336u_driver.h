/**
 * @file ft6336u_driver.h
 * @brief FT6336U 触摸屏驱动接口
 */

#ifndef _FT6336U_DRIVER_H
#define _FT6336U_DRIVER_H

#include "esp_err.h"
#include "driver/gpio.h"
#include "soc/gpio_num.h"
#include <stdint.h>

/**
 * @brief 初始化 FT6336U 触摸屏控制器
 * @param SDA I2C 数据线 GPIO
 * @param SCL I2C 时钟线 GPIO
 * @return ESP_OK 成功，其他失败
 */
esp_err_t ft6336_driver(gpio_num_t SDA, gpio_num_t SCL);

/**
 * @brief 读取触摸坐标
 * @param x 输出 X 坐标指针
 * @param y 输出 Y 坐标指针
 * @param state 输出触摸状态（1=有触摸，0=无触摸）
 */
void ft6336u_read(int16_t* x, int16_t* y, int* state);

/**
 * @brief 设置触摸中断标志
 * @param flag 中断标志位（true=有触摸中断）
 */
void ft6336u_int_info(bool flag);

#endif
