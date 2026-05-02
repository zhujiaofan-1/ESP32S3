/**
 * @file button.h
 * @brief 按键驱动管理接口
 */

#ifndef _BUTTON_H
#define _BUTTON_H

#include <stdint.h>
#include "esp_err.h"

/**
 * @brief 按键短按/长按回调函数类型
 *
 * @param gpio 触发事件的GPIO引脚编号
 */
typedef void(*button_press_cb_t)(int gpio);

/**
 * @brief 获取GPIO电平回调函数类型
 *
 * @param gpio 要读取的GPIO引脚编号
 * @return int 1高电平，0低电平
 */
typedef int(*button_getlevel_cb_t)(int gpio);

/**
 * @brief 按键配置结构体
 */
typedef struct 
{
    int gpio_num;                       /**< 引脚编号 */
    int active_level;                   /**< 按下的电平状态 */
    int long_press_time;                /**< 长按时间（ms） */
    button_press_cb_t short_press_cb;   /**< 短按回调函数 */
    button_press_cb_t long_press_cb;    /**< 长按回调函数 */
    button_getlevel_cb_t getlevel_cb;   /**< 获取电平回调函数 */
}button_config_t;

/**
 * @brief 按键状态枚举
 */
typedef enum{
    BUTTON_RELEASE,         /**< 按键松开 */
    BTTON_PRESS,            /**< 消抖状态（按键按下） */
    BUTTON_HOLD,            /**< 按键长按状态 */
    BUTTON_LONG_PRESS_HOLD, /**< 等待松手（超过长按时间） */
}BUTTON_STATE;

/**
 * @brief 按键信息结构体（链表节点）
 */
typedef struct Button_info{
    button_config_t btn_cfg;        /**< 按键配置 */
    BUTTON_STATE    state;          /**< 当前按键状态 */
    int press_cnt;                  /**< 计时长按时间 */
    struct Button_info  *next;      /**< 下一个按键的结构体指针 */
}button_info_t;


/**
 * @brief 初始化按键模块
 *
 * 初始化GPIO并创建按键扫描定时器
 *
 * @return esp_err_t ESP_OK成功，其他失败
 */
esp_err_t button_Init(void);

/**
 * @brief 添加按键
 *
 * 将按键配置加入按键链表
 *
 * @param config 按键配置结构体指针
 * @return esp_err_t ESP_OK成功，ESP_FAIL失败
 */
esp_err_t Button_Add(button_config_t* config);

/**
 * @brief 按键扫描处理
 *
 * 遍历按键链表，检测按键状态变化并触发回调
 * 由定时器周期性调用
 */
void button_scan(void);



#endif
