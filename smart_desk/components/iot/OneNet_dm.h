/**
 * @file OneNet_dm.h
 * @brief OneNet设备物模型接口
 */

#ifndef _ONENET_DM_H
#define _ONENET_DM_H

#include "cJSON.h"
#include "mqtt_client.h"


/**
 * @brief 处理OneNet下发的属性设置数据
 *
 * 解析JSON数据，提取亮度、开关、RGB颜色等属性并控制硬件
 *
 * @param property cJSON格式的JSON数据对象
 */
void OneNet_property_handle(cJSON* property);

/**
 * @brief 拼接设备属性上报数据
 *
 * 将当前设备状态组装成JSON格式用于上报
 *
 * @return cJSON* 组装好的JSON对象指针
 */
cJSON* OneNet_property_upload(void);

/**
 * @brief 向OneNet平台回复属性设置结果
 *
 * @param mqtt_handle MQTT客户端句柄
 * @param id 指令ID
 * @param error_code 错误码（200表示成功）
 * @param msg 回复消息内容
 */
void OneNet_property_ack(esp_mqtt_client_handle_t mqtt_handle, const char* id, int error_code, const char* msg);

/**
 * @brief 向OneNet平台回复OTA通知结果
 *
 * @param mqtt_handle MQTT客户端句柄
 * @param id 指令ID
 * @param error_code 错误码（200表示成功）
 * @param msg 回复消息内容
 */
void OneNet_ota_ack(esp_mqtt_client_handle_t mqtt_handle, const char* id, int error_code, const char* msg);

/**
 * @brief 订阅OneNet物模型相关主题
 *
 * @param mqtt_handle MQTT客户端句柄
 */
void OneNet_subscribe(esp_mqtt_client_handle_t mqtt_handle);

/**
 * @brief 向OneNet平台上报设备属性数据
 *
 * @param mqtt_handle MQTT客户端句柄
 * @param data JSON格式的属性数据字符串
 * @return esp_err_t ESP_OK成功，其他失败
 */
esp_err_t OneNet_post_property_data(esp_mqtt_client_handle_t mqtt_handle, const char* data);

#endif 
