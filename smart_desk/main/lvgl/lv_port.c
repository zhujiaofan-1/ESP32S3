/**
 * @file lv_port.c
 * @brief LVGL 显示驱动移植实现
 */

#include "lv_port.h"
#include "esp_lcd_types.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_check.h"
#include "freertos/projdefs.h"
#include "soc/gpio_num.h"
#include <string.h>
#include "esp_lvgl_port.h"
#include "XL9555.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define TAG         "LV_port"

#define LCD_WIDTH       320
#define LCD_HEIGHT      240

#define LCD_RST_IO      IO1_3
#define LCD_BL_IO       IO1_2

// 全局句柄
static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;
static lv_display_t* lvgl_disp = NULL;

void lv_display_hard_Init(void)
{
    ESP_LOGI(TAG, "Initialize Intel 8080 bus");
    esp_lcd_i80_bus_handle_t i80_bus = NULL;        // 总线句柄
    esp_lcd_i80_bus_config_t bus_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .dc_gpio_num = GPIO_NUM_1,
        .wr_gpio_num = GPIO_NUM_41,
        .data_gpio_nums = {
            GPIO_NUM_40,
            GPIO_NUM_38,
            GPIO_NUM_39,
            GPIO_NUM_48,
            GPIO_NUM_45,
            GPIO_NUM_21,
            GPIO_NUM_47,
            GPIO_NUM_14,

        },
        .bus_width = 8,
        .max_transfer_bytes = LCD_WIDTH * LCD_HEIGHT,       // 一次性写整个屏幕的大小
        .dma_burst_size = 64,           // 突发传输字节 DMA
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    // 配置 LCD 面板 IO
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = GPIO_NUM_2,
        .pclk_hz = 10*1000*1000,          // 像素时钟，像素传输的频率
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,              // LCD 命令长度
        .lcd_param_bits = 8,            // 数据长度
        .flags = {
            .swap_color_bytes = 1,      
        }
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    ESP_LOGI(TAG, "Install LCD driver of st7789");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = GPIO_NUM_NC,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &lcd_panel));

    // 写入初始化命令并配置屏幕
    esp_lcd_panel_reset(lcd_panel);
    esp_lcd_panel_init(lcd_panel);
    esp_lcd_panel_swap_xy(lcd_panel, true);
    esp_lcd_panel_mirror(lcd_panel, false, true);
    esp_lcd_panel_disp_on_off(lcd_panel, true);
}

esp_err_t lv_port_Init(void)
{   
    // 打开背光
    XL9555_Pin_Write(LCD_BL_IO, 1);
    // RST 硬件复位
    XL9555_Pin_Write(LCD_RST_IO, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    XL9555_Pin_Write(LCD_RST_IO, 1);

    // 初始化 LCD 硬件
    lv_display_hard_Init();

    // 配置并初始化 LVGL 端口
     const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        .task_stack = 8192,
        .task_affinity = 1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5
    };
    ESP_RETURN_ON_ERROR(lvgl_port_init(&lvgl_cfg), TAG, "LVGL port initialization failed");

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = lcd_panel,
        .buffer_size = LCD_WIDTH * 20,  // 内存区域，lvgl在这个内存绘制（320*20=6400像素=12800字节）
        .double_buffer = 0,      // 双缓存
        .hres = LCD_WIDTH,
        .vres = LCD_HEIGHT,

        .color_format = LV_COLOR_FORMAT_RGB565,         // RGB565模式
        // 设置翻转
        .rotation = {
            .swap_xy = true,
            .mirror_x = false,
            .mirror_y = true,
        },
        .flags = {
            .buff_dma = true,

        }
    };
    // 添加显示设备
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);

    return ESP_OK;
}