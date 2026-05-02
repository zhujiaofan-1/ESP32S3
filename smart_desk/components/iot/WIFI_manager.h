/**
 * @file WIFI_manager.h
 * @brief WiFi管理模块接口
 *
 * 提供WiFi STA模式连接管理、AP模式切换和WiFi扫描功能
 */

#ifndef _WIFI_MANAGER_H
#define _WIFI_MANAGER_H

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"

/**
 * @brief WiFi连接状态枚举
 */
typedef enum{
    WIFI_STATE_CONNECTED,       /**< 已连接 */
    WIFI_STATE_DISCONNECTED,    /**< 已断开 */
}WIFI_STATE;

/**
 * @brief WiFi状态变化回调函数类型
 *
 * @param state 当前WiFi连接状态
 */
typedef void(*p_wifi_state_cb)(WIFI_STATE);

/**
 * @brief WiFi扫描结果回调函数类型
 *
 * @param num 扫描到的AP数量
 * @param ap_record AP信息记录数组指针
 */
typedef void(*p_wifi_scan_cb)(int num, wifi_ap_record_t *ap_record);

/**
 * @brief 初始化WiFi管理器
 *
 * 初始化网络接口、事件循环、WiFi STA模式并注册事件处理函数
 *
 * @param f WiFi状态回调函数，连接/断开时被调用
 */
void WIFI_manager_Init(p_wifi_state_cb f);

/**
 * @brief 连接到指定WiFi热点
 *
 * @param ssid WiFi名称
 * @param password WiFi密码
 */
void WIFI_manager_connect(const char* ssid,const char* password);

/**
 * @brief 切换到AP+STA模式
 *
 * @return esp_err_t ESP_OK成功，其他失败
 */
esp_err_t WIFI_manager_AP(void);

/**
 * @brief 启动WiFi扫描
 *
 * @param f 扫描结果回调函数
 * @return esp_err_t ESP_OK成功，ESP_ERR_INVALID_STATE正在扫描中
 */
esp_err_t WIFI_manager_scan(p_wifi_scan_cb f);

/**
 * @brief 查询WiFi是否已连接
 *
 * @return true 已连接，false 未连接
 */
bool WIFI_manager_is_connect(void);

/**
 * @brief 获取当前连接的WiFi名称
 *
 * @return char* WiFi SSID字符串指针
 */
char* get_wifi_ssid(void);

char* get_AP_wifi_ssid(void);

#endif
