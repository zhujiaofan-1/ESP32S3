/**
 * @file my_sntp.c
 * @brief SNTP网络对时实现文件
 *
 * 配置NTP服务器并启动时间同步，设置中国标准时区
 */

#include "my_sntp.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "esp_log.h"
#include "esp_sntp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief 初始化SNTP时间同步
 *
 * 配置NTP服务器（阿里云、Apple、NTP池）并注册对时回调函数，
 * 设置时区为CST-8（中国标准时间）
 *
 * @param f 时间同步完成回调函数
 */
void my_sntp_Init(sntp_sync_time_cb_t f)
{
    if(! esp_sntp_enabled())
    {
        //设置SNTP工作模式,单播模式
        esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);

        //设置对时服务器
        esp_sntp_setservername(0, "ntp.aliyun.com");
        esp_sntp_setservername(1, "time.asia.apple.com");
        esp_sntp_setservername(2, "pool.ntp.org");

        //设置对时回调函数
        esp_sntp_set_time_sync_notification_cb(f);

        //启动对时 
        esp_sntp_init();

        //设置时区
        setenv("TZ", "CST-8", 1);
        tzset();
    }
    
}