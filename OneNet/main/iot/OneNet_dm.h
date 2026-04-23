#ifndef _ONENET_DM_H
#define _ONENET_DM_H

#include "cJSON.h"
#include "mqtt_client.h"

//初始化物模型
void OneNet_dm_Init(void);

//处理OneNet下行数据
void OneNet_property_handle(cJSON* property);

//上报数据
cJSON* OneNet_property_upload(void);

//下行回应数据
void OneNet_property_ack(esp_mqtt_client_handle_t mqtt_handle, const char* id, int error_code, const char* msg);
//OTA升级回应
void OneNet_ota_ack(esp_mqtt_client_handle_t mqtt_handle, const char* id, int error_code, const char* msg);

//主题订阅函数
void OneNet_subscribe(esp_mqtt_client_handle_t mqtt_handle);

esp_err_t OneNet_post_property_data(esp_mqtt_client_handle_t mqtt_handle, const char* data);

#endif 
