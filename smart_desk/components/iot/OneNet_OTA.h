/**
 * @file OneNet_OTA.h
 * @brief OneNet OTA升级接口
 */

#ifndef _ONENET_OTA_H
#define _ONENET_OTA_H

#include "esp_err.h"

//获取当前运行程序的APP版本号
const char* get_app_version(void);

//标记程序是否合法
void set_app_valid(int valid);

//上报版本号
esp_err_t OneNet_ota_upload_version(void);

void OneNet_ota_start(void);

#endif
