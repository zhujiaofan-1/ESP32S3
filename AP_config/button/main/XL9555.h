#ifndef _XL9555_H
#define _XL9555_H

#include "esp_err.h"
#include "driver/gpio.h"


#define IO0_0       (1<<0)
#define IO0_1       (1<<1)
#define IO0_2       (1<<2)
#define IO0_3       (1<<3)
#define IO0_4       (1<<4)
#define IO0_5       (1<<5)
#define IO0_6       (1<<6)
#define IO0_7       (1<<7)

#define IO1_0       (1<<8)
#define IO1_1       (1<<9)
#define IO1_2       (1<<10)
#define IO1_3       (1<<11)
#define IO1_4       (1<<12)
#define IO1_5       (1<<13)
#define IO1_6       (1<<14)
#define IO1_7       (1<<15)


/**
 * @brief XL9555输入引脚变化回调函数类型
 *
 * @param Pin      变化的引脚（IO0_0~IO1_7）
 * @param Level    当前电平值（1高电平，0低电平）
 */
typedef void(*XL9555_Input_callback_t)(uint16_t Pin, int Level);



/**
 * @brief 初始化XL9555 I2C扩展芯片
 *
 * @param SDA      I2C数据引脚
 * @param SCL      I2C时钟引脚
 * @param INT      中断引脚（不使用中断则传GPIO_NUM_NC）
 * @param f        输入引脚变化回调函数
 * @return         esp_err_t ESP_OK成功，其他失败
 */
esp_err_t XL9555_Init(gpio_num_t SDA,gpio_num_t SCL, gpio_num_t INT, XL9555_Input_callback_t f);



/**
 * @brief 向XL9555写入Word数据（16位）
 *
 * @param Reg_ADDR  寄存器地址
 * @param Data      要写入的16位数据
 * @return          esp_err_t ESP_OK成功，其他失败
 */
esp_err_t XL9555_Write_Word(uint8_t Reg_ADDR, uint16_t Data);



/**
 * @brief 从XL9555读取Word数据（16位）
 *
 * @param Reg_ADDR  寄存器地址
 * @param Data      读取到的16位数据指针
 * @return          esp_err_t ESP_OK成功，其他失败
 */
esp_err_t XL9555_Read_Word(uint8_t Reg_ADDR, uint16_t *Data);



/**
 * @brief 配置XL9555 IO方向
 *
 * @param config    配置数据，1为输入，0为输出
 * @return          esp_err_t ESP_OK成功，其他失败
 */
esp_err_t XL9555_IO_Cofig(uint16_t config);



/**
 * @brief 读取XL9555引脚电平
 *
 * @param PIN       引脚宏（如IO0_0）
 * @return          int 1为高电平，0为低电平
 */
esp_err_t XL9555_Pin_Read(uint16_t PIN);



/**
 * @brief 写入XL9555引脚电平
 *
 * @param Pin       引脚宏（如IO0_0）
 * @param Level     电平值，1高电平，0低电平
 * @return          esp_err_t ESP_OK成功，其他失败
 */
esp_err_t XL9555_Pin_Write(uint16_t Pin, int Level);

#endif
