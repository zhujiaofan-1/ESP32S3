/**
 * @file aida64.c
 * @brief AIDA64硬件监控数据获取实现文件
 *
 * 通过SSE（Server-Sent Events）连接AIDA64远程传感器服务，
 * 实时获取CPU使用率、CPU温度、内存使用率等硬件参数
 */

#include "aida64.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "freertos/event_groups.h"
#include "portmacro.h"
#include "ui/generated/gui_guider.h"
#include "ui/custom/custom.h"

#define TAG             "AIDA64"

#define AIDA64_CONNECT_BIT          BIT0

static char host_aida64_url[128];

static bool connect_state = false;

extern lv_ui guider_ui;

static TaskHandle_t aida64_task_handle = NULL;

static esp_http_client_handle_t http_handle = NULL;

static EventGroupHandle_t aida64_event_handle = NULL;


static void aida64_monitor_task(void* param);

/**
 * @brief 启动AIDA64监控
 * 
 * @param ip 服务器IP地址
 */
void aida64_monitor_start(const char* ip)
{
    //创建事件组通知任务已经启动
    if(aida64_event_handle ==NULL)
    {
        aida64_event_handle = xEventGroupCreate();
    }


    if(aida64_task_handle == NULL)
    {
        xTaskCreatePinnedToCore(aida64_monitor_task, "monitor_task", 4096, NULL, 3, &aida64_task_handle, 1);
    }
    snprintf(host_aida64_url, sizeof(host_aida64_url),"http://%s:80/sse", ip);

    xEventGroupSetBits(aida64_event_handle, AIDA64_CONNECT_BIT);
    
}

/**
 * @brief 停止AIDA64监控
 */
void aida64_monitor_stop(void)
{
    esp_http_client_close(http_handle);
}

/**
 * @brief 获取AIDA64连接状态
 * 
 * @return true 已连接
 * @return false 未连接
 */
bool aida64_monitor_isconnect(void)
{
    return connect_state;
}

/**
 * @brief 解析AIDA64数据
 * 
 * @param data 接收到的数据字符串
 * @param aida64_data 解析后的数据结构体
 * @return true 解析成功
 * @return false 解析失败
 */
static bool aida64_monitor_parse(char* data, aida64_data_t* aida64_data)
{
    const char* search_str = NULL;
    if(!data)
    {
        return false;
    }

    //查找CPU使用率
    search_str = strstr(data, "CPU_used ");     
    if(search_str)
    {
        sscanf(search_str, "%*[^0-9]%d", &aida64_data->cpu_used);
    }else{
        return false;
    }

    //查找CPU温度
    search_str = strstr(data, "CPU_temp ");     
    if(search_str)
    {
        sscanf(search_str, "%*[^0-9]%d", &aida64_data->cpu_temp);
    }else{
        return false;
    }

    //查找内存使用率
    search_str = strstr(data, "mem_used ");     
    if(search_str)
    {
        sscanf(search_str, "%*[^0-9]%d", &aida64_data->mem_used);
    }else{
        return false;
    }

    //查找可用内存
    search_str = strstr(data, "mem_vaild ");     
    if(search_str)
    {
        sscanf(search_str, "%*[^0-9]%d", &aida64_data->mem_valid);
    }else{
        return false;
    }

    return true;
}



/**
 * @brief HTTP事件回调函数
 *
 * 处理HTTP客户端各种事件，主要功能是接收服务器返回的数据
 * 并实时解析和更新到屏幕
 *
 * @param evt HTTP事件结构体指针
 * @return esp_err_t ESP_OK表示处理成功
 */
 static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
 {
 
     switch(evt->event_id) {
         case HTTP_EVENT_ERROR:
             ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
             break;
         case HTTP_EVENT_ON_CONNECTED:
             ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");

             connect_state = true;
             set_monitor_connect(&guider_ui, true);

             break;
         case HTTP_EVENT_HEADER_SENT:
             ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
             break;
         case HTTP_EVENT_ON_HEADER:
             ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
             break;
         case HTTP_EVENT_ON_DATA:                //接收http数据
         {
             ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
             printf("HTTP_EVENT_ON_DATA data=%.*s\r\n",evt->data_len,(char*)evt->data);

             //解析数据
            aida64_data_t aida64_data;

            if(aida64_monitor_parse((char*)evt->data, &aida64_data))
            {
                //设置参数到屏幕
                set_monitor_param(&guider_ui, aida64_data.cpu_used, aida64_data.cpu_temp, aida64_data.mem_used, aida64_data.mem_valid);

            }
             break;
         }
             
             
         case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
         case HTTP_EVENT_DISCONNECTED:
             ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");

             break;
         case HTTP_EVENT_REDIRECT:
             ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
             break;
     }
     return ESP_OK;
 }
 

/**
 * @brief AIDA64监控任务
 * 
 * @param param 任务参数（未使用）
 */
static void aida64_monitor_task(void* param)
{
    esp_http_client_config_t config = {
       .url = "http://192.168.1.2:80/sse",
       .event_handler = _http_event_handler,
       .timeout_ms = 10000,
       .keep_alive_enable = false,
    };
      http_handle = esp_http_client_init(&config);
 
 
     while (1)
     {
        //直接发起http请求，阻塞
        // esp_err_t ret = esp_http_client_perform(client);
        //等待事件
        EventBits_t ev = xEventGroupWaitBits(aida64_event_handle, AIDA64_CONNECT_BIT, pdTRUE, pdFALSE, portMAX_DELAY);

        if(ev & AIDA64_CONNECT_BIT)
        {
            //发起http连接请求(sse连接)
            esp_http_client_set_url(http_handle, host_aida64_url);

            esp_http_client_set_method(http_handle, HTTP_METHOD_GET);

            //设置请求头
            esp_http_client_set_header(http_handle, "Accept", "text/event-stream");

            //启动连接
            ESP_LOGI(TAG, "启动硬件数据HTTP连接");

            esp_http_client_perform(http_handle);

            connect_state = false;
        }

     }
     
}
