/*
* Copyright 2024 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
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
    lv_label_set_text_fmt(ui->screen_home_label_day,"%d年%d月%d日",year,month,day);

    lv_label_set_text_fmt(ui->screen_home_label_week, "%s", week_day_text[w_day]);

    lvgl_port_unlock();
}


void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

