/**
 * @file weather_img.c
 * @brief 天气图片SPIFFS挂载实现文件
 */

#include "weather_img.h"
#include "esp_spiffs.h"
#include "esp_log.h"

#define TAG         "img"

#define SPIFF_MOUNT     "/img"

/**
 * @brief 初始化天气图片SPIFFS分区
 *
 * 挂载img分区，用于存放天气图标等图片资源
 */
void img_spiffs_Init(void)
{
    esp_vfs_spiffs_conf_t spiffs_conf =
    {
        .base_path = SPIFF_MOUNT,
        .format_if_mount_failed = true,
        .max_files = 3,
        .partition_label = "img",
    };

    esp_err_t ret = esp_vfs_spiffs_register(&spiffs_conf);
    if(ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE(TAG, "Image SPIFFS挂载失败: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "Image SPIFFS挂载成功");

}
