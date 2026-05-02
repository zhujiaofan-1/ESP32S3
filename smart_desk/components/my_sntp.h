/**
 * @file my_sntp.h
 * @brief SNTP网络对时接口
 *
 * 提供NTP网络时间同步功能
 */

#ifndef _MY_SNTP_H
#define _MY_SNTP_H

#include "esp_sntp.h"

/**
 * @brief 初始化SNTP时间同步
 *
 * 配置NTP服务器（阿里云、Apple、NTP池）并注册对时回调函数，
 * 设置时区为CST-8（中国标准时间）
 *
 * @param f 时间同步完成回调函数
 */
void my_sntp_Init(sntp_sync_time_cb_t f);

#endif
