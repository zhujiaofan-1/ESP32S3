/**
 * @file button.c
 * @brief 按键驱动管理实现文件
 *
 * 该文件实现了按键的添加、状态管理和事件检测功能。采用链表结构管理多个按键，
 * 使用 ESP32-S3 的 esp_timer 定时器进行按键扫描，支持短按和长按检测。
 */

/*================================== 头文件引用 =========================================*/
#include "button.h"

#include "esp_err.h"
#include "esp_log.h"

#include <stdlib.h>
#include <string.h>
#include "esp_timer.h"

/*================================== 宏定义 =========================================*/
#define TAG         "Button"

/*================================== 全局变量 =========================================*/
static button_info_t       *button_head = NULL;              // 按键链表头指针

static esp_timer_handle_t  button_timer_handle;              // 按键扫描定时器句柄

static bool timer_running = false;                           // 定时器创建标志位，防止重复创建

/*================================== 函数声明 =========================================*/
static void button_handle(void* arg);//定时器回调函数

/*================================== 按键管理函数 =======================================*/
/**
 * @brief 添加一个新的按键到系统中
 * @param cfg 按键配置结构体指针，包含按键的GPIO引脚、激活电平、长按时间和回调函数等信息
 * @return ESP_OK: 成功；ESP_FAIL: 失败
 * @note 该函数会将新按键添加到按键管理链表中，为后续的按键状态检测和事件处理做准备
 */
esp_err_t Button_Add(button_config_t* cfg)
{
    //========================== 参数检查 ==========================//
    // 检查输入参数是否为空
    if(!cfg)
    {
        return ESP_FAIL;
    }
    
    //========================== 内存分配 ==========================//
    // 为新按键分配内存空间
    button_info_t* btn = (button_info_t*)malloc(sizeof(button_info_t));
    /*
     * malloc 分解说明：
     * ├── sizeof(button_info_t)  → 申请一个按键结构体那么大的内存
     * ├── malloc(...)            → 向系统"借"一块内存
     * ├── (button_info_t*)        → 强制类型转换，告诉编译器这块内存用来存按键结构体
     * └── button_info_t* btn      → btn 指针指向刚申请出来的按键内存
     */

    // 检查内存分配是否成功
    if(!btn)
    {
        return ESP_FAIL; // 内存分配失败，返回错误
    }
    
    //========================== 内存初始化 ==========================//
    // 初始化内存，将所有成员变量设为默认值（0/NULL）
    memset(btn, 0, sizeof(button_info_t));
    
    // 复制按键配置信息到新分配的内存中
    memcpy(&btn->btn_cfg, cfg, sizeof(button_config_t));

    //========================== 链表操作 ==========================//
    // 检查按键链表是否为空
    if(!button_head)
    {
        // 链表为空，将新按键设为链表头节点
        button_head = btn;
    }else{
        // 链表不为空，遍历到链表末尾
        button_info_t* info = button_head;
        while(info->next)
        {
            info = info->next;
        }
        // 将新按键添加到链表末尾
        info->next = btn;
    }
    
    //========================== 定时器创建 ==========================//
    // 防止重复创建定时器，只在第一次添加按键时创建
    if(timer_running == false)
    {
        static int button_interval = 5;         // 按键扫描间隔：5ms

        // 定时器配置参数
        esp_timer_create_args_t button_timer =
        {
            .callback = button_handle,           // 定时器到期执行的回调函数
            .name = "button",                     // 定时器名称（用于调试）
            .dispatch_method = ESP_TIMER_TASK,   // 在 ESP_TIMER_TASK 任务中执行回调
            .arg = (void*)button_interval,       // 传递给回调函数的参数
        };

        // 创建硬件定时器
        esp_timer_create(&button_timer, &button_timer_handle);
        // 启动周期性定时器，每 5ms (5000μs) 触发一次
        esp_timer_start_periodic(button_timer_handle, 5000);
        // 设置标志位，防止重复创建定时器
        timer_running = true;
    }
    
    return ESP_OK; // 添加按键成功
}

//按键定时器回调函数
static void button_handle(void* arg)
{
    button_info_t * btn_info = button_head;

    int interval = (int)arg;  //传入扫描间隔5ms  

    //遍历链表
    for(;btn_info; btn_info = btn_info->next)
    {
        int gpio_num = btn_info->btn_cfg.gpio_num;      //引脚

        switch (btn_info->state)
        {
            case BUTTON_RELEASE://按键松开状态
                 // 在松开的时候判断是否会等于“按下的状态”
                if(btn_info->btn_cfg.getlevel_cb(gpio_num) == btn_info->btn_cfg.active_level)
                {
                    //进入消抖状态
                    btn_info->state = BTTON_PRESS;
                    btn_info->press_cnt += interval;    //累计按下时间，每一次扫描为5ms
                }
                break;

            case BTTON_PRESS://消抖状态（按键按下）
                // 判断是否会等于“按下的状态”
                if(btn_info->btn_cfg.getlevel_cb(gpio_num) == (btn_info->btn_cfg.active_level))
                {
                    
                    btn_info->press_cnt += interval;    //累计按下时间，每一次扫描为5ms
                    //判断是否过了消抖时间
                    if(btn_info->press_cnt >= 10)
                    {        
                        //转入长按状态
                        btn_info->state = BUTTON_HOLD;
                    }

                }else{
                    btn_info->state = BUTTON_RELEASE;       //如果是噪声就返回松开状态
                    btn_info->press_cnt = 0;
                }
                break;


            case BUTTON_HOLD://按键按住状态
                // 判断是否会等于“按下的状态”
                if(btn_info->btn_cfg.getlevel_cb(gpio_num) == (btn_info->btn_cfg.active_level))
                {
                    
                    btn_info->press_cnt += interval;    //累计按下时间，每一次扫描为5ms
                    //判断是否过了设定的长按时间
                    if((btn_info->press_cnt) >= (btn_info->btn_cfg.long_press_time))
                    {
                        // 执行长按回调函数
                        if(btn_info->btn_cfg.long_press_cb != 0)
                        {
                            btn_info->btn_cfg.long_press_cb(gpio_num);
                        }
                        //转入等待松手状态
                        btn_info->state = BUTTON_LONG_PRESS_HOLD;
                    }

                }else{
                    // 执行短按回调函数,如果在长按计时到之前松手
                    if(btn_info->btn_cfg.short_press_cb != 0)
                    {
                        btn_info->btn_cfg.short_press_cb(gpio_num);
                    }

                    btn_info->state = BUTTON_RELEASE;       //如果不是长按返回松开状态
                    btn_info->press_cnt = 0;
                }

                break;

            case BUTTON_LONG_PRESS_HOLD://等待松手（超过长按时间）
                // 判断是否会等于“按下的状态”
                if(btn_info->btn_cfg.getlevel_cb(gpio_num) != (btn_info->btn_cfg.active_level))
                {
                    //如果不是按下的状态了，就返回松手的状态
                    btn_info->state = BUTTON_RELEASE;
                    btn_info->press_cnt = 0;
                }
                break;
            
            default:
                break;
        }
    }
}