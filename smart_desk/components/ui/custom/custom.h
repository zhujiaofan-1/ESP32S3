/*
* Copyright 2024 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

/**
 * @file custom.h
 * @brief 自定义UI更新接口
 *
 * 提供屏幕UI元素更新函数，包括时间、天气、WiFi状态、硬件监控等
 */

#ifndef __CUSTOM_H_
#define __CUSTOM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

/**
 * @brief 设置首页时间和日期
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
void set_home_time(lv_ui* ui, int year, int month, int day, int w_day, int hour, int min, int sec);

/**
 * @brief 设置今天天气信息
 *
 * @param ui LVGL UI对象指针
 * @param img_path 天气图标路径
 * @param low 最低温度
 * @param high 最高温度
 */
void set_today_weather(lv_ui* ui, char* img_path, int low, int high);

/**
 * @brief 设置明天天气信息
 *
 * @param ui LVGL UI对象指针
 * @param img_path 天气图标路径
 * @param low 最低温度
 * @param high 最高温度
 */
void set_tomorrow_weather(lv_ui* ui, char* img_path, int low, int high);

/**
 * @brief 设置后天天气信息
 *
 * @param ui LVGL UI对象指针
 * @param img_path 天气图标路径
 * @param low 最低温度
 * @param high 最高温度
 */
void set_after_weather(lv_ui* ui, char* img_path, int low, int high);

/**
 * @brief 设置城市名称
 *
 * @param ui LVGL UI对象指针
 * @param city 城市名称字符串
 */
void set_city(lv_ui* ui, char* city);

/**
 * @brief 设置WiFi图标
 *
 * @param ui LVGL UI对象指针
 * @param img_path 图标图片路径
 */
void set_wifi_img(lv_ui* ui, char*img_path);

/**
 * @brief 设置WiFi连接状态文本
 *
 * @param ui LVGL UI对象指针
 * @param wifi_state true=已连接，false=未连接
 */
void set_wifi_state(lv_ui* ui, bool wifi_state);

/**
 * @brief 设置WiFi名称文本
 *
 * @param ui LVGL UI对象指针
 * @param ssid WiFi SSID字符串
 */
void set_wifi_ssid(lv_ui* ui, char* ssid);


void set_AP_ssid(lv_ui* ui, char* ssid);
/**
 * @brief 设置硬件监控参数
 *
 * @param ui LVGL UI对象指针
 * @param cpu_used CPU使用率(%)
 * @param cpu_temp CPU温度(℃)
 * @param mem_used 内存使用率(%)
 * @param mem_valid 可用内存大小(MB)
 */
void set_monitor_param(lv_ui* ui, int cpu_used, int cpu_temp, int mem_used, int mem_valid);

/**
 * @brief 设置硬件监控连接状态
 *
 * @param ui LVGL UI对象指针
 * @param flag true=已连接，false=未连接
 */
void set_monitor_connect(lv_ui* ui, bool flag);

/**
 * @brief 设置灯光开关按钮文本
 *
 * @param ui LVGL UI对象指针
 * @param flag true=灯亮（显示"关"），false=灯灭（显示"开"）
 */
void set_light_state(lv_ui* ui, bool flag);

/**
 * @brief 自定义UI初始化
 *
 * @param ui LVGL UI对象指针
 */
void custom_init(lv_ui *ui);


#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
