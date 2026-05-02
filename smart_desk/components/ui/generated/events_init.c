/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include <sys/stat.h>
#include "lvgl.h"
#include "aida64.h"
#include "WIFI_manager.h"
#include "AP_WIFI.h"
#include "led_control.h"
#include "esp_wifi.h"


#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

//灯的各种参数

extern int RGB_red;
extern int RGB_green;
extern int RGB_blue;
extern int LED_brightness;
extern int RGB_brightness;

static void screen_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        case LV_DIR_LEFT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_monitor, guider_ui.screen_monitor_del, &guider_ui.screen_home_del, setup_scr_screen_monitor, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 200, false, false);
            break;
        }
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_control, guider_ui.screen_control_del, &guider_ui.screen_home_del, setup_scr_screen_control, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, false, false);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_home (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_home, screen_home_event_handler, LV_EVENT_ALL, ui);
}

static void screen_monitor_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_monitor_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, false, false);
            break;
        }
        case LV_DIR_LEFT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi, guider_ui.screen_wifi_del, &guider_ui.screen_monitor_del, setup_scr_screen_wifi, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 200, false, false);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_monitor_btn_connect_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    //获取用户参数
    lv_ui* ui = (lv_ui*)lv_event_get_user_data(e);

    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(!aida64_monitor_isconnect())
        {
            //获取控件的字符(IP地址)
            const char* ip = lv_textarea_get_text(ui->screen_monitor_ta_ip);
            //利用这个字符串进行连接
            aida64_monitor_start(ip);
        }else{
            aida64_monitor_stop();
        }

        break;
    }
    default:
        break;
    }
}

void events_init_screen_monitor (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_monitor, screen_monitor_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_monitor_btn_connect, screen_monitor_btn_connect_event_handler, LV_EVENT_ALL, ui);
}

static void screen_control_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        case LV_DIR_LEFT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_control_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 200, false, false);
            break;
        }
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi, guider_ui.screen_wifi_del, &guider_ui.screen_control_del, setup_scr_screen_wifi, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, false, false);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_control_btn_light_switch_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if(LED_state())
        {
            light_control(0); 
        }else{
            light_restore_brightness(); 
        }
        break;
    }
    default:
        break;
    }
}

static void screen_control_slider_RGB_red_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    //获取slider对象
    lv_obj_t * slider = lv_event_get_target(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        RGB_red = lv_slider_get_value(slider);

        RGB_control(RGB_brightness, RGB_red, RGB_green, RGB_blue);

        break;
    }
    default:
        break;
    }
}

static void screen_control_slider_RGB_green_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    //获取slider对象
    lv_obj_t * slider = lv_event_get_target(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        RGB_green = lv_slider_get_value(slider);

        RGB_control(RGB_brightness, RGB_red, RGB_green, RGB_blue);
        break;
    }
    default:
        break;
    }
}

static void screen_control_slider_RGB_bri_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    //获取slider对象
    lv_obj_t * slider = lv_event_get_target(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        RGB_brightness = lv_slider_get_value(slider);

        RGB_control(RGB_brightness, RGB_red, RGB_green, RGB_blue);
        break;
    }
    default:
        break;
    }
}

static void screen_control_slider_RGB_blue_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    //获取slider对象
    lv_obj_t * slider = lv_event_get_target(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        RGB_blue = lv_slider_get_value(slider);

        RGB_control(RGB_brightness, RGB_red, RGB_green, RGB_blue);
        break;
    }
    default:
        break;
    }
}

static void screen_control_slider_light_bri_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    //获取slider对象
    lv_obj_t * slider = lv_event_get_target(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        LED_brightness = lv_slider_get_value(slider);

        light_control(LED_brightness);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_control (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_control, screen_control_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_control_btn_light_switch, screen_control_btn_light_switch_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_control_slider_RGB_red, screen_control_slider_RGB_red_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_control_slider_RGB_green, screen_control_slider_RGB_green_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_control_slider_RGB_bri, screen_control_slider_RGB_bri_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_control_slider_RGB_blue, screen_control_slider_RGB_blue_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_control_slider_light_bri, screen_control_slider_light_bri_event_handler, LV_EVENT_ALL, ui);

    // 初始化滑块值，使其显示当前状态
    lv_slider_set_value(ui->screen_control_slider_RGB_red, RGB_red, LV_ANIM_OFF);
    lv_slider_set_value(ui->screen_control_slider_RGB_green, RGB_green, LV_ANIM_OFF);
    lv_slider_set_value(ui->screen_control_slider_RGB_blue, RGB_blue, LV_ANIM_OFF);
    lv_slider_set_value(ui->screen_control_slider_RGB_bri, RGB_brightness, LV_ANIM_OFF);
    lv_slider_set_value(ui->screen_control_slider_light_bri, LED_brightness, LV_ANIM_OFF);
}

static void screen_wifi_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        case LV_DIR_LEFT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_control, guider_ui.screen_control_del, &guider_ui.screen_wifi_del, setup_scr_screen_control, LV_SCR_LOAD_ANIM_OVER_LEFT, 200, 200, false, false);
            break;
        }
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_monitor, guider_ui.screen_monitor_del, &guider_ui.screen_wifi_del, setup_scr_screen_monitor, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, false, false);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_wifi_btn_AP_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        AP_WIFI_apcfg();
        break;
    }
    default:
        break;
    }
}

static void screen_wifi_btn_connect_wifi_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        
        if(!WIFI_manager_is_connect())
        {
            // 尝试重新连接上次保存的wifi配置
            esp_wifi_connect();
        }

        break;
    }
    default:
        break;
    }
}

void events_init_screen_wifi (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_wifi, screen_wifi_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_btn_AP, screen_wifi_btn_AP_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_btn_connect_wifi, screen_wifi_btn_connect_wifi_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
