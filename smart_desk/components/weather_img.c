#include "weather_img.h"
#include "esp_spiffs.h"
#include "esp_log.h"

#define TAG         "img"

#define SPIFF_MOUNT     "/img"

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
