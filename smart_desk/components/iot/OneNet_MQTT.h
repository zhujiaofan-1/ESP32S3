/**
 * @file OneNet_MQTT.h
 * @brief OneNet MQTT客户端接口
 *
 * 提供与OneNet物联网平台的MQTT连接管理功能
 */

#ifndef _ONENET_MQTT_H
#define _ONENET_MQTT_H

#include "esp_err.h"

#define ONENET_PRODUCT_ID           "Dv57YAJzDX"
#define ONENET_PRODUCT_ACCESS_KEY   "RzU/A0E1CQM1axUdbdEPM3uPV7G/ZQfLPaOJkXfRRwk="
#define ONENET_DEVICE_NAME             "ESP32LED01"

/**
 * @brief OneNet MQTT连接成功回调函数类型
 */
typedef void (*onenet_connected_cb_t)(void);

/**
 * @brief 启动OneNet MQTT客户端
 *
 * 初始化MQTT客户端配置，生成认证Token并启动连接
 *
 * @return esp_err_t ESP_OK成功，其他失败
 */
esp_err_t OneNet_Start(void);

/**
 * @brief 停止OneNet MQTT客户端
 *
 * 销毁MQTT客户端并释放资源
 */
void OneNet_Stop(void);

/**
 * @brief 注册MQTT连接成功回调函数
 *
 * @param cb 连接成功时的回调函数
 */
void OneNet_RegisterConnectedCallback(onenet_connected_cb_t cb);

#endif
