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

#include <string.h>


#define TAG             "ft6336u_driver"

// 全局句柄
static i2c_master_bus_handle_t ft6336u_bus_handle = NULL;
static i2c_master_dev_handle_t ft6336u_dev_handle = NULL;

// 触摸中断标志
static  bool ft6336u_int_flag = false;

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



/**
 * @brief 读取触摸坐标
 * @param x 输出 X 坐标指针
 * @param y 输出 Y 坐标指针
 * @param state 输出触摸状态（1=有触摸，0=无触摸）
 */
void ft6336u_read(int16_t* x, int16_t* y, int* state)
{
    uint8_t read_buf[5];
    uint8_t write_buf[1];

    // 静态变量保存上次坐标
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    // 判断中断标志位，只有产生中断的时候才能读取坐标
    if (!ft6336u_int_flag)
    {
        // 返回上次的值
        *x = last_x;
        *y = last_y;
        *state = 0;
        return;
    }

    // 如果是中断就执行下面的读取操作
    // 读取五个字节坐标（从寄存器0x02开始）
    write_buf[0] = 0x02; 
    i2c_master_transmit_receive(ft6336u_dev_handle, &write_buf[0], 1, &read_buf[0], 5, 500);

    // 判断按下的触点个数（低四位，第一个字节），只有1个触点时才读取
    if ((read_buf[0] & 0x0f) != 1)
    {
        // 返回上次的值
        *x = last_x;
        *y = last_y;
        *state = 0;
        return;
    }
    
    // 解析触摸坐标：FT6336U 使用12位坐标表示
    // X坐标 = (read_buf[1]的低4位 << 8) | read_buf[2]
    int16_t current_x = ((read_buf[1] & 0x0F) << 8) | read_buf[2];
    
    // Y坐标 = (read_buf[3]的低4位 << 8) | read_buf[4]
    int16_t current_y = ((read_buf[3] & 0x0F) << 8) | read_buf[4];
    
    // 保存当前坐标到静态变量，用于无触摸时返回上次值
    last_x = current_x;
    last_y = current_y;
    
    // 输出坐标和状态（1表示有触摸）
    *x = current_x;
    *y = current_y;
    *state = 1;
}

/**
 * @brief 设置触摸中断标志
 * @param flag 中断标志位（true=有触摸中断）
 */
void ft6336u_int_info(bool flag)
{
    ft6336u_int_flag = flag;
}

   
