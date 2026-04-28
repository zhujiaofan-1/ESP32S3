#ifndef _BUTTON_H
#define _BUTTON_H

#include <stdint.h>
#include "esp_err.h"

//按键回调函数
typedef void(*button_press_cb_t)(int gpio);
//获取gpio电平回调函数
typedef int(*button_getlevel_cb_t)(int gpio);

//按键配置
typedef struct 
{
    int gpio_num;       //引脚编号
    int active_level;   //按下的状态
    int long_press_time;//长按时间（ms）
    button_press_cb_t short_press_cb;   //短按回调函数
    button_press_cb_t long_press_cb; //长按回调函数
    button_getlevel_cb_t getlevel_cb;   //获取电平回调函数
    
}button_config_t;
 
//按键状态枚举
typedef enum{
    BUTTON_RELEASE,         //按键松开
    BTTON_PRESS,            //消抖状态（按键按下）
    BUTTON_HOLD,            //按键长按状态
    BUTTON_LONG_PRESS_HOLD,  //等待松手（超过长按时间）
}BUTTON_STATE;

//获取按键状态
typedef struct Button_info{
    button_config_t btn_cfg;
    BUTTON_STATE    state;      //当前按键状态
    int press_cnt;              //计时长按时间
    struct Button_info  *next;  //【指向：下一个按键的结构体】
    
}button_info_t;


esp_err_t Button_Add(button_config_t* cfg);



#endif
