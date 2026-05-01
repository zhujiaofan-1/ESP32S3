#include "weather.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "CJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "WIFI_manager.h"
#include "iot/WIFI_manager.h"



#define TAG     "weather"

#define WEATHER_BUFF_LEN        1024
//http接收到的数据
static uint8_t weather_data_buf[WEATHER_BUFF_LEN];
//数据长度
static int weather_data_size = 0;


#define WEATHER_PRIVATE_KEY         "SuOsxAsIfNYvONOEZ"
static char weather_city[48];

/**
 * @brief HTTP事件回调函数
 *
 * 处理HTTP客户端各种事件，主要功能是接收服务器返回的数据
 * 并将数据拷贝到ota_data_buf缓冲区中
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
 
             int copy_len = 0;
             //如果大于剩余空间的话
             if(evt->data_len > WEATHER_BUFF_LEN - weather_data_size)
             {
                 //剩余的空间
                 copy_len = WEATHER_BUFF_LEN - weather_data_size;
 
             }else{
                 copy_len = evt->data_len;
             }
 
             //拷贝数据
             memcpy(&weather_data_buf[weather_data_size], evt->data, copy_len);
             weather_data_size += copy_len;
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
 
 //数据解析函数
static esp_err_t pasre_weather(char* weather_data)
{
     cJSON* wt_js = cJSON_Parse(weather_data);
     if(! wt_js)
     {
        ESP_LOGI(TAG, "天气数据CJSON错误");
        return ESP_FAIL;
     }

     cJSON* reslu_js = cJSON_GetObjectItem(wt_js, "results");
     if(! reslu_js)
     {
        return ESP_FAIL;
     }

     //获取数据第一项
     cJSON* result_child_js = reslu_js->child;

     cJSON* daily_js = cJSON_GetObjectItem(reslu_js, "daily");

     if(daily_js)
     {
        weather_data_pkt_t data[3];
        int index = 0;


        //获取天气（第一为第一个数组）
        cJSON* daily_child_js = daily_js->child;

        while (daily_child_js)
        {
            cJSON* high_js = cJSON_GetObjectItem(daily_js, "high");
            cJSON* low_js = cJSON_GetObjectItem(daily_js, "low");
            cJSON* code_day_js = cJSON_GetObjectItem(daily_js, "code_day");

            if(index < 3)
            {
                //转换  字符串到整形
                sscanf(cJSON_GetStringValue(high_js), "%d", &data[index].high_temp);
                sscanf(cJSON_GetStringValue(low_js), "%d", &data[index].low_temp);
                snprintf(data[index].weather_code, sizeof(data[index].weather_code),"%s", cJSON_GetStringValue(code_day_js));
                ESP_LOGI(TAG, "day[%d]->high=%d,low=%d,day_code=%s",data[index].high_temp,data[index].low_temp,data[index].weather_code);
            }
            index ++;
            daily_child_js = daily_child_js->next;

        }
        //用togo设置到屏幕        
        
     }


     cJSON_Delete(wt_js);
     return ESP_OK;
}
 
 /**
  * @brief 发起天气预报 HTTP请求
  *
  * 构造HTTP请求并发送到OneNet平台，包括生成认证Token、设置请求头
  * 请求完成后清空接收缓冲区
  *
  * @param url 请求URL地址
  * @param method HTTP请求方法（GET/POST等）
  * @param payload POST请求的数据体，为NULL时表示GET请求
  * @return esp_err_t ESP_OK表示请求成功，其他值表示失败
  */
 static esp_err_t   weather_http_connect(void)
 {
    static char url[256];
    snprintf(url, sizeof(url),"http://api.seniverse.com/v3/weather/daily.json?key=%s&location=%s&language=zh-Hans&unit=c&start=0&days=3",WEATHER_PRIVATE_KEY, weather_city);
     
     esp_http_client_config_t config = {
       .url = url,
       .event_handler = _http_event_handler,
       .timeout_ms = 10000,
       .keep_alive_enable = false,
    };
      esp_http_client_handle_t client = esp_http_client_init(&config);
 

     // 这是为了 清空上一次HTTP请求的残留数据 ，确保本次请求接收到的数据是干净的。
     memset(weather_data_buf, 0, sizeof(weather_data_buf));
     weather_data_size = 0;
 
     //直接发起http请求，阻塞
     esp_err_t ret = esp_http_client_perform(client);
    
     //解析数据
     pasre_weather((char*)weather_data_buf);
     
     //释放内存
     esp_http_client_cleanup(client);
 
     return ret;
 }


//地址数据解析
static esp_err_t pasre_location(char* location_data)
{
    cJSON* location_js = cJSON_Parse(location_data);

    //直接解析
    if(!location_js)
    {
        ESP_LOGI(TAG, "地址CJSON获取失败");
        return ESP_FAIL;
    }
    cJSON* city_js = cJSON_GetObjectItem(location_js, "city");
    if(!city_js)
    {
        ESP_LOGI(TAG, "城市获取失败");
        return ESP_FAIL;
    }
    snprintf(weather_city, sizeof(weather_city), "%s", cJSON_GetStringValue(city_js));

    ESP_LOGI(TAG, "地区：%s",weather_city);

    cJSON_Delete(location_js);

    return ESP_OK;
}


  /**
  * @brief 发起天气预报 HTTP请求
  *
  * 构造HTTP请求并发送到OneNet平台，包括生成认证Token、设置请求头
  * 请求完成后清空接收缓冲区
  *
  * @param url 请求URL地址
  * @param method HTTP请求方法（GET/POST等）
  * @param payload POST请求的数据体，为NULL时表示GET请求
  * @return esp_err_t ESP_OK表示请求成功，其他值表示失败
  */
 static esp_err_t location_http_connect(void)
 {
    static char url[256];
    snprintf(url, sizeof(url), "http://ip-api.com/json/?lang=en");
     
     esp_http_client_config_t config = {
       .url = url,
       .event_handler = _http_event_handler,
       .timeout_ms = 10000,
       .keep_alive_enable = false,
    };
      esp_http_client_handle_t client = esp_http_client_init(&config);
 

     // 这是为了 清空上一次HTTP请求的残留数据 ，确保本次请求接收到的数据是干净的。
     memset(weather_data_buf, 0, sizeof(weather_data_buf));
     weather_data_size = 0;
 
     //直接发起http请求，阻塞
     esp_err_t ret = esp_http_client_perform(client);
    
     //解析数据
     pasre_location((char*)weather_data_buf);
     
     //释放内存
     esp_http_client_cleanup(client);
 
     return ret;
 }

static void weather_task(void* param)
{
    //判断是否联网
    while(1)
    {
        if(!WIFI_manager_is_connect())
        {
            vTaskDelay(pdMS_TO_TICKS(20000));
        }else{
            location_http_connect();
            weather_http_connect();
            vTaskDelay(pdMS_TO_TICKS(1000*60*30));
        }
    }
}


void weather_start(void)
{
    xTaskCreatePinnedToCore(weather_task, "weater_task", 4096, NULL, 2, NULL, 1);

}
 
