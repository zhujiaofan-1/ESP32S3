/*
* Copyright 2024 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

/**
 * @file custom.c
 * @brief 自定义UI更新实现文件
 *
 * 实现屏幕UI元素更新函数，包括时间、天气、WiFi状态、硬件监控等
 */


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"
#include "esp_lvgl_port.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */


 //手动更新时钟值 
extern int screen_home_clock_now_hour_value;
extern int screen_home_clock_now_min_value;
extern int screen_home_clock_now_sec_value;

int screen_home_clock_now_year_value;
int screen_home_clock_now_month_value;
int screen_home_clock_now_day_value;
int screen_home_clock_now_w_day_value;

/**
 * @brief 设置首页时间和日期
 *
 * 更新首页的年月日和星期显示
 *
 * @param ui LVGL UI对象指针
 * @param year 年
 * @param month 月
 * @param day 日
 * @param w_day 星期（0=星期日）
 * @param hour 时
 * @param min 分
 * @param sec 秒
 */
void set_home_time(lv_ui* ui, int year, int month, int day, int w_day, int hour, int min, int sec)
{
    screen_home_clock_now_hour_value = hour;
    screen_home_clock_now_min_value = min;
    screen_home_clock_now_sec_value = sec;
    screen_home_clock_now_year_value = year;
    screen_home_clock_now_month_value =month;
    screen_home_clock_now_day_value = day;
    screen_home_clock_now_w_day_value = w_day;

    static const char* week_day_text[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};

     lvgl_port_lock(0);
    //手动更新值
    if(ui->screen_home_label_day != NULL)
    {
        lv_label_set_text_fmt(ui->screen_home_label_day,"%d年%d月%d日",year,month,day);
    }
    if(ui->screen_home_label_week != NULL)
    {
        lv_label_set_text_fmt(ui->screen_home_label_week, "%s", week_day_text[w_day]);
    }

    lvgl_port_unlock();
}

/**
 * @brief 设置今天天气信息
 *
 * @param ui LVGL UI对象指针
 * @param img_path 天气图标路径
 * @param low 最低温度
 * @param high 最高温度
 */
void set_today_weather(lv_ui* ui, char* img_path, int low, int high)
{
    lvgl_port_lock(0);

    //设置图片
    if(ui->screen_home_img_today != NULL)
    {
        lv_img_set_src(ui->screen_home_img_today, img_path);
    }
    if(ui->screen_home_label_temp1 != NULL)
    {
        lv_label_set_text_fmt(ui->screen_home_label_temp1, "%d-%d℃", low, high);
    }

    lvgl_port_unlock();
}
/**
 * @brief 设置明天天气信息
 *
 * @param ui LVGL UI对象指针
 * @param img_path 天气图标路径
 * @param low 最低温度
 * @param high 最高温度
 */
void set_tomorrow_weather(lv_ui* ui, char* img_path, int low, int high)
{
    lvgl_port_lock(0);

    if(ui->screen_home_img_tomorrow != NULL)
    {
        lv_img_set_src(ui->screen_home_img_tomorrow, img_path);
    }
    if(ui->screen_home_label_temp2 != NULL)
    {
        lv_label_set_text_fmt(ui->screen_home_label_temp2, "%d-%d℃", low, high);
    }

    lvgl_port_unlock();
}
/**
 * @brief 设置后天天气信息
 *
 * @param ui LVGL UI对象指针
 * @param img_path 天气图标路径
 * @param low 最低温度
 * @param high 最高温度
 */
void set_after_weather(lv_ui* ui, char* img_path, int low, int high)
{
    lvgl_port_lock(0);

    if(ui->screen_home_img_afterq != NULL)
    {
        lv_img_set_src(ui->screen_home_img_afterq, img_path);
    }
    if(ui->screen_home_label_temp3 != NULL)
    {
        lv_label_set_text_fmt(ui->screen_home_label_temp3, "%d-%d℃", low, high);
    }

    lvgl_port_unlock();
}

/**
 * @brief 设置WiFi图标
 *
 * @param ui LVGL UI对象指针
 * @param img_path 图标图片路径
 */
void set_wifi_img(lv_ui* ui, char*img_path)
{
    lvgl_port_lock(0);
    
    // 更新首页的wifi图标（这个对象总是存在的）
    if(ui->screen_home_img_wifi != NULL)
    {
        lv_img_set_src(ui->screen_home_img_wifi, img_path);
    }
    lvgl_port_unlock();
}

/**
 * @brief 设置WiFi连接状态文本
 *
 * @param ui LVGL UI对象指针
 * @param wifi_state true=已连接，false=未连接
 */
void set_wifi_state(lv_ui* ui, bool wifi_state)
{
    lvgl_port_lock(0);
    if(ui->screen_wifi_label_wifi_state != NULL)
    {
        if(wifi_state)
        {
            lv_label_set_text_fmt(ui->screen_wifi_label_wifi_state,"已连接");
        }else{
            lv_label_set_text_fmt(ui->screen_wifi_label_wifi_state,"未连接");
        }
    }
    lvgl_port_unlock();
}

/**
 * @brief 设置WiFi名称文本
 *
 * @param ui LVGL UI对象指针
 * @param ssid WiFi SSID字符串
 */
void set_wifi_ssid(lv_ui* ui, char* ssid)
{
    lvgl_port_lock(0);
    if(ui->screen_wifi_label_current_ssid != NULL)
    {
        lv_label_set_text_fmt(ui->screen_wifi_label_current_ssid, "%s", ssid); 
    }
   
   lvgl_port_unlock();
}



void set_AP_ssid(lv_ui* ui, char* ssid)
{
    lvgl_port_lock(0);
    if(ui->screen_wifi_label_wifi_AP != NULL)
    {
        lv_label_set_text_fmt(ui->screen_wifi_label_wifi_AP, "%s", ssid); 
    }
   
   lvgl_port_unlock();
}

/**
 * @brief 设置城市名称
 *
 * @param ui LVGL UI对象指针
 * @param city 城市名称字符串
 */
void set_city(lv_ui* ui,  char* city)
{
    lvgl_port_lock(0);
    if(ui->screen_home_label_city != NULL)
    {
        lv_label_set_text_fmt(ui->screen_home_label_city, "%s", city);
    }
    lvgl_port_unlock();
}




/**
 * @brief 设置硬件监控参数
 *
 * @param ui LVGL UI对象指针
 * @param cpu_used CPU使用率(%)
 * @param cpu_temp CPU温度(℃)
 * @param mem_used 内存使用率(%)
 * @param mem_valid 可用内存大小(MB)
 */
void set_monitor_param(lv_ui* ui, int cpu_used, int cpu_temp, int mem_used, int mem_valid)
{
    lvgl_port_lock(0);
    if(ui->screen_monitor_label_cpu_value1 != NULL)
    {
        lv_label_set_text_fmt(ui->screen_monitor_label_cpu_value1, "%d%%", cpu_used);
    }
    if(ui->screen_monitor_label_cpu_value2 != NULL)
    {
        lv_label_set_text_fmt(ui->screen_monitor_label_cpu_value2, "%d℃", cpu_temp);
    }
    if(ui->screen_monitor_label_mem_value1 != NULL)
    {
        lv_label_set_text_fmt(ui->screen_monitor_label_mem_value1, "%d%%", mem_used);
    }
    if(ui->screen_monitor_label_mem_value2 != NULL)
    {
        lv_label_set_text_fmt(ui->screen_monitor_label_mem_value2, "%dMB", mem_valid);
    }
    lvgl_port_unlock();
}

/**
 * @brief 设置硬件监控连接状态
 *
 * @param ui LVGL UI对象指针
 * @param flag true=已连接，false=未连接
 */
void set_monitor_connect(lv_ui* ui, bool flag)
{
    lvgl_port_lock(0);
    if(flag)
    {
        if(ui->screen_monitor_label_state != NULL)
        {
            lv_label_set_text_fmt(ui->screen_monitor_label_state, "已连接");
        }
        if(ui->screen_monitor_btn_connect_label != NULL)
        {
            lv_label_set_text_fmt(ui->screen_monitor_btn_connect_label, "断开");
        }
    }else{
        if(ui->screen_monitor_label_state != NULL)
        {
            lv_label_set_text_fmt(ui->screen_monitor_label_state, "未连接");
        }
        if(ui->screen_monitor_btn_connect_label != NULL)
        {
            lv_label_set_text_fmt(ui->screen_monitor_btn_connect_label, "连接");
        }
    }
    lvgl_port_unlock();
}

/**
 * @brief 设置灯光开关按钮文本
 *
 * @param ui LVGL UI对象指针
 * @param flag true=灯亮（显示"关"），false=灯灭（显示"开"）
 */
void set_light_state(lv_ui* ui, bool flag)
{
    lvgl_port_lock(0);
    if(ui->screen_control_btn_light_switch_label != NULL)
    {
        if(flag)
        {
            lv_label_set_text_fmt(ui->screen_control_btn_light_switch_label, "关");
        }else{
            lv_label_set_text_fmt(ui->screen_control_btn_light_switch_label, "开");
        }
    }
    lvgl_port_unlock();
}




/**
 * @brief 自定义UI初始化
 *
 * @param ui LVGL UI对象指针
 */
void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

