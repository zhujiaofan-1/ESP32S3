/**
 * @file ft6336u_driver.c
 * @brief FT6336U 触摸屏驱动实现
 */

#include "ft6336u_driver.h"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "hal/i2c_types.h"
#include "soc/clk_tree_defs.h"
#include <cstddef>
#include <cstdint>
#include <string.h>


#define TAG             "ft6336u_driver"

static i2c_master_bus_handle_t ft6336u_bus_handle = NULL;
static i2c_master_dev_handle_t ft6336u_dev_handle = NULL;

esp_err_t ft6336_driver(gpio_num_t SDA, gpio_num_t SCL)
{
    // 初始化 I2C 总线
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .scl_io_num = SCL,
        .sda_io_num = SDA,
        .glitch_ignore_cnt = 7,         // 滤波
        .i2c_port = -1,
    };
    i2c_new_master_bus(&i2c_bus_config, &ft6336u_bus_handle);

    // I2C 添加设备
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x38,
        .scl_speed_hz = 400 * 1000,
        
    };
    i2c_master_bus_add_device(ft6336u_bus_handle, &dev_config, &ft6336u_dev_handle);

    // 读取设备 ID 验证
    esp_err_t ret = ESP_FAIL;
    uint8_t read_buf[1];
    uint8_t write_buf[2];
    write_buf[0] = 0xA3;    // ID 寄存器地址
    ret = i2c_master_transmit_receive(ft6336u_dev_handle, &write_buf[0], 1, &read_buf[0], 1, 1000);
    if (ret != ESP_OK)
    {
        return ret;
    }

    // 配置中断模式
    write_buf[0] = 0x43;    // 设置中断模式寄存器地址
    write_buf[1] = 0x00;    // 使用查询模式
    ret = i2c_master_transmit(ft6336u_dev_handle, &write_buf[0], 2, 1000);
    if (ret != ESP_OK)
    {
        return ret;
    }

    return ESP_OK;
}