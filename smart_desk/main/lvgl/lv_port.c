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
#include "esp_log_config.h"
#include "freertos/projdefs.h"
#include "ft6336u_driver.h"
#include "indev/lv_indev.h"
#include "soc/gpio_num.h"
#include <string.h>
#include "esp_lvgl_port.h"
#include "XL9555.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ft6336u_driver.h"


#define TAG         "LV_port"

#define LCD_WIDTH       320
#define LCD_HEIGHT      240

#define LCD_RST_IO      IO1_3
#define LCD_BL_IO       IO1_2

// 全局句柄
static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t lcd_panel = NULL;
static lv_display_t* lvgl_disp = NULL;
static lv_indev_t* touch_indev = NULL;  // 触摸输入设备句柄

/**
 * @brief 初始化 LCD 硬件（Intel 8080 总线和 ST7789 控制器）
 */
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

/**
 * @brief LVGL 触摸输入设备读取回调函数
 * @param indev LVGL 输入设备句柄
 * @param data 输入数据结构体指针
 */
void indev_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    int16_t x = 0, y = 0;
    int state = 0;
    ft6336u_read(&x, &y, &state);
    // 需要交换坐标系，因为从竖屏变成横屏了
    data->point.x = (LCD_WIDTH - y) - 1;
    data->point.y = x;
    data->state = state;
}

/**
 * @brief 初始化 LVGL 端口并注册显示和触摸设备
 * @return ESP_OK 成功，其他失败
 */
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
    //初始化触摸屏
    ft6336_driver(GPIO_NUM_13, GPIO_NUM_12);

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
        .buffer_size = LCD_WIDTH * 80,  // 内存区域，lvgl在这个内存绘制（320*80=25600像素=51200字节）
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


    //触摸
    /**
     * 为什么需要加锁？
     * 
     * 1. LVGL 多任务安全机制：
     *    - LVGL 内部使用独立的 FreeRTOS 任务处理渲染和事件
     *    - 多个任务可能同时访问 LVGL 对象树（widget 链表、显示缓冲区等）
     *    - 不加锁会导致竞态条件：画面撕裂、内存崩溃、数据损坏
     * 
     * 2. 临界区保护：
     *    - `lv_indev_create()` 修改全局 LVGL 对象链表
     *    - `lv_indev_set_type()` 修改设备状态
     *    - 这些操作不是原子的，必须在一次事务中完成
     * 
     * 3. lvgl_port_lock(0) 参数含义：
     *    - 参数=0：阻塞模式，一直等待直到获取锁
     *    - 参数>0：超时时间（毫秒）
     *    - 使用互斥量（Mutex）实现，不是禁用中断
     * 
     * 4. 不加锁的后果示例：
     *    - 渲染任务正在遍历 widget 树时，这里同时创建新对象 → 野指针崩溃
     *    - 输入设备正在刷新时，触摸中断同时修改 → 触摸坐标错乱
     */
    lvgl_port_lock(0);
    //创建输入设备 
    touch_indev = lv_indev_create();

    lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);

    //设置设备获取数据的接口
    lv_indev_set_read_cb(touch_indev, indev_read);
    lvgl_port_unlock();
    return ESP_OK;
}