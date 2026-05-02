/**
 * @file AP_WIFI.h
 * @brief AP配网模块接口
 */

#ifndef _AP_WIFI_H
#define _AP_WIFI_H

#include "WIFI_manager.h"

/**
 * @brief 初始化AP配网模块
 *
 * 创建AP配网任务和事件组，注册WiFi状态回调
 *
 * @param f WiFi状态变化回调函数
 */
void AP_WIFI_Init(p_wifi_state_cb f);

/**
 * @brief 请求进入AP配网模式
 *
 * 通过事件组通知AP配网任务启动AP模式，非阻塞调用
 */
void AP_WIFI_apcfg(void);

#endif
