/**
 * @file main.c
 * @brief 智能桌面主程序入口
 */

/*============================ 系统头文件 ============================*/
#include <stdio.h>
#include <string.h>

/*============================ FreeRTOS 头文件 ============================*/
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "portmacro.h"

/*============================ ESP-IDF 头文件 ============================*/
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "nvs_flash.h"
#include "soc/gpio_num.h"

/*============================ LVGL 头文件 ============================*/
#include "lvgl.h"
#include "lv_demos.h"
#include "esp_lvgl_port.h"
#include "gui_guider.h"
#include "custom.h"

/*============================ 项目模块头文件 ============================*/
#include "button.h"
#include "XL9555.h"
#include "Audio.h"
#include "lvgl/ft6336u_driver.h"
#include "lvgl/lv_port.h"
#include "iot/AP_WIFI.h"
#include "iot/OneNet_MQTT.h"
#include "OneNet_dm.h"
#include "my_sntp.h"
#include "esp_sntp.h"
#include <time.h>



#define TAG     "main"

#define WIFI_CONNECT_BIT        (BIT0)
#define RECORD_TRIGGER_BIT      (BIT1)

//用于通知WIFI已经连接
static EventGroupHandle_t main_ev = NULL;

//通过在xl9555中改变这个变量的值，就能体现出按键的高低电平
static volatile uint16_t XL9555_Button_Level = 0xffff;

lv_ui guider_ui;


/**
 * @brief XL9555 电平变化回调函数
 * 
 * 当 XL9555 扩展 IO 引脚电平发生变化时调用，
 * 更新全局按键电平状态变量。
 * 
 * @param Pin 发生变化的引脚编号
 * @param Level 新的电平状态（1为高电平，0为低电平）
 */
// XL9555的电平变化回调函数
void XL9555_Input_callback(uint16_t Pin, int Level)
{
    if (Level == 1)
    {
        XL9555_Button_Level |= Pin;
    }else{
        XL9555_Button_Level &= ~Pin;
    }

    //检测ft6336中断
    if(Pin == IO1_1)
    {
        if(!Level)      //低电平则产生中断
        {
            ft6336u_int_info(true);
            ESP_LOGI(TAG, "触摸中断");
        }else{
            ft6336u_int_info(false);
        }
    }
}

/**
 * @brief 按键长按回调函数
 * 
 * 当检测到按键长按事件时调用，进入 AP 配网模式。
 * 
 * @param gpio 触发长按事件的 GPIO 引脚编号
 */
//长按回调函数
void btn_long_press_callback(int gpio)
{
    if(gpio == IO0_1)
    {
        ESP_LOGI(TAG," 进入AP配网模式");
        AP_WIFI_apcfg();
    }
}

/**
 * @brief 按键短按回调函数
 * 
 * 当检测到按键短按事件时调用，触发录音和播放操作。
 * 
 * @param gpio 触发短按事件的 GPIO 引脚编号
 */
void btn_short_press_callback(int gpio)
{
    if(gpio == IO0_2)
    {
        ESP_LOGI(TAG, "IO0_2 短按，触发录音播放");
        xEventGroupSetBits(main_ev, RECORD_TRIGGER_BIT);
    }
}

/**
 * @brief 获取 GPIO 电平回调函数
 * 
 * 用于按键库读取指定 GPIO 引脚的当前电平状态。
 * 
 * @param gpio 要读取电平的 GPIO 引脚编号
 * @return 1 表示高电平，0 表示低电平
 */
//获取gpio电平回调函数
int get_gpio_level_callback(int gpio)
{
    // 与特定的引脚相与不为0，则return  1
    return XL9555_Button_Level & gpio ? 1 : 0;
}

/**
 * @brief 按键初始化
 * 
 * 初始化 XL9555 IO 扩展芯片并注册按键配置，
 * 设置输入模式和长按回调函数。
 */
//button初始化
void button_Init(void)
{
    //注册按键
    button_config_t btn_cfg =
    {
        .active_level = 0,              // 低电平有效
        .long_press_time = 1000,        // 长按时间 1000ms
        .gpio_num = IO0_1,              // 按键对应的 GPIO
        .long_press_cb = btn_long_press_callback,   // 长按回调
        .getlevel_cb = get_gpio_level_callback,     // 获取电平回调
    };
    Button_Add(&btn_cfg);
    btn_cfg.gpio_num = IO0_2;
    btn_cfg.short_press_cb = btn_short_press_callback;
    Button_Add(&btn_cfg);
}

/**
 * @brief WIFI 状态回调函数
 * 
 * 当 WIFI 连接状态发生变化时调用，
 * 通过事件组通知主任务 WIFI 已连接。
 * 
 * @param state 当前 WIFI 连接状态
 */
//WIFI状态回调函数
void wifi_stat_callback(WIFI_STATE state)
{
    if(state == WIFI_STATE_CONNECTED)
    {
        // 写入时间组
        xEventGroupSetBits(main_ev, WIFI_CONNECT_BIT);
        ESP_LOGI(TAG, "WIFI已连接！");
    }
    if(state == WIFI_STATE_DISCONNECTED)
    {
        ESP_LOGI(TAG, "WIFI已断开！");
    }
}

//对时回调函数
void sntp_finish_callback (struct timeval *tv)
{
    //分解时间
    struct tm t;
    localtime_r(&tv->tv_sec, &t);
    //设置初始时间
    set_home_time(&guider_ui, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_wday, t.tm_hour, t.tm_min, t.tm_sec);
}


/**
 * @brief 应用程序主函数
 * 
 * 系统启动入口，负责初始化以下模块：
 * 1. NVS 非易失性存储
 * 2. 数据模型模块
 * 3. 按键模块
 * 4. 扬声器和麦克风音频模块
 * 5. WIFI 网络模块
 * 
 * 主循环等待 WIFI 连接事件，连接成功后启动 OneNet MQTT 服务。
 */
void app_main(void)
{
    // 初始化非易失性存储
    nvs_flash_init();

    // 打印初始内存信息
    ESP_LOGI(TAG, "Initial free heap: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Free 8-bit accessible heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT));
    ESP_LOGI(TAG, "Free DMA capable heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_DMA));
    ESP_LOGI(TAG, "Free SPIRAM heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    Audio_spiffs_Init();

    // 创建事件组
    main_ev = xEventGroupCreate();

    // 初始化数据模型模块
    OneNet_dm_Init();

    // 初始化 XL9555 芯片，配置中断回调引脚
    XL9555_Init(GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_17, XL9555_Input_callback);

    XL9555_IO_Cofig(0xffff & (~(IO0_0 | IO1_3 | IO1_2)));    //设置为输入模式,IO_0需要为输出

    // 初始化按键
    button_Init();

    // 初始化扬声器（BCLK=GPIO46, WS=GPIO9, SD=GPIO8, 采样率48000Hz）
    Speaker_Init(GPIO_NUM_46, GPIO_NUM_9, GPIO_NUM_8, 48000);
    
    // 初始化麦克风 PDM 接口（Data=GPIO42, CLK=GPIO3, 采样率48000Hz）
    Microphone_pdm_Init(GPIO_NUM_42, GPIO_NUM_3, 48000);

    //初始化WIFI
    AP_WIFI_Init(wifi_stat_callback);

    //初始化屏幕
    lv_port_Init();
    lvgl_port_lock(0);              //代码不在 LVGL 自己的刷新任务里,在别的任务操作则需要加锁
    // lv_demo_widgets();
    setup_ui(&guider_ui);       //加载预设ui
    custom_init(&guider_ui);    //自定义需求初始化
    lvgl_port_unlock();

    EventBits_t wifi_ev_bit;
    while(1)
    {
        time_t now = time(NULL);
        ESP_LOGI("SNTP", "Time_count:%lld, sync_status:%d", (long long)now, esp_sntp_get_sync_status());
        vTaskDelay(pdMS_TO_TICKS(1000));

        //等待WIFI连接事件或录音触发事件
        wifi_ev_bit = xEventGroupWaitBits(main_ev, WIFI_CONNECT_BIT | RECORD_TRIGGER_BIT, pdFALSE, pdFALSE, 0);

        //录音触发
        if(wifi_ev_bit & RECORD_TRIGGER_BIT)
        {
            xEventGroupClearBits(main_ev, RECORD_TRIGGER_BIT);

            XL9555_Pin_Write(IO0_0, 0);
            Start_record(5);

            XL9555_Pin_Write(IO0_0, 1);
            vTaskDelay(pdMS_TO_TICKS(100));
            Play_Audio();
            XL9555_Pin_Write(IO0_0, 0);
        }

        //发起OneNet平台连接
        if(wifi_ev_bit & WIFI_CONNECT_BIT)
        {
            xEventGroupClearBits(main_ev, WIFI_CONNECT_BIT);

            // WiFi已连接，初始化SNTP对时
            my_sntp_Init(sntp_finish_callback);

            OneNet_Start();
        }
    }
}
