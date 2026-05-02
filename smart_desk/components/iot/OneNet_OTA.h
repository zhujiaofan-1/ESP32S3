/**
 * @file OneNet_OTA.h
 * @brief OneNet OTA升级接口
 */

#ifndef _ONENET_OTA_H
#define _ONENET_OTA_H

#include "esp_err.h"

/**
 * @brief 获取当前运行程序的APP版本号
 *
 * @return const char* 版本号字符串指针
 */
const char* get_app_version(void);

/**
 * @brief 标记当前应用分区合法性
 *
 * @param valid 1表示合法（取消回滚），0表示非法（触发回滚）
 */
void set_app_valid(int valid);

/**
 * @brief 上报当前固件版本号到OneNet平台
 *
 * @return esp_err_t ESP_OK成功，其他失败
 */
esp_err_t OneNet_ota_upload_version(void);

/**
 * @brief 启动OTA升级流程
 */
void OneNet_ota_start(void);

#endif
