#include "weather.h"

/*============================ ESP-IDF 头文件 ============================*/
#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

/*============================ 项目头文件 ============================*/
#include "WIFI_manager.h"
#include "iot/WIFI_manager.h"
#include "custom.h"
#include <stdio.h>
#include <string.h>


#define TAG     "weather"

//HTTP接收缓冲区大小
#define WEATHER_BUFF_LEN        1024

//心知天气API私钥
#define WEATHER_PRIVATE_KEY         "SuOsxAsIfNYvONOEZ"

extern lv_ui guider_ui;

//http接收到的数据
static uint8_t weather_data_buf[WEATHER_BUFF_LEN];
//数据长度
static int weather_data_size = 0;

//当前城市拼音，用于天气查询
static char weather_city[48];
static char chinese_city[48];   //中文城市名

/**
 * @brief HTTP事件回调函数
 *
 * 处理HTTP客户端各种事件，主要功能是接收服务器返回的数据
 * 并将数据拷贝到weather_data_buf缓冲区中
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
 
/**
 * @brief 天气数据解析函数
 *
 * 解析心知天气API返回的JSON数据，提取未来3天的最高温度、最低温度和天气代码
 * 并保存到weather_data_pkt_t结构体数组中
 *
 * JSON数据格式示例：
 * {
 *   "results": [{
 *     "daily": [
 *       {"high":"30","low":"22","code_day":"1"},
 *       {"high":"31","low":"23","code_day":"0"},
 *       {"high":"29","low":"21","code_day":"9"}
 *     ]
 *   }]
 * }
 *
 * @param weather_data 心知天气API返回的JSON字符串
 * @return esp_err_t ESP_OK表示解析成功，ESP_FAIL表示解析失败
 */
 //数据解析函数
static esp_err_t pasre_weather(char* weather_data)
{
     cJSON* wt_js = cJSON_Parse(weather_data);
     if(!wt_js)
     {
        ESP_LOGI(TAG, "天气数据CJSON错误");
        return ESP_FAIL;
     }

     cJSON* reslut_js = cJSON_GetObjectItem(wt_js, "results");
     if(!reslut_js)
     {
        ESP_LOGI(TAG, "天气result错误");
        return ESP_FAIL;
     }

     //获取数据第一项
     cJSON* result_child_js = reslut_js->child;

     cJSON* daily_js = cJSON_GetObjectItem(result_child_js, "daily");

     weather_data_pkt_t data[3];
    int index = 0;
     if(daily_js)
     {
        //获取天气（第一为第一个数组）
        cJSON* daily_child_js = daily_js->child;

        while (daily_child_js)
        {
            cJSON* high_js = cJSON_GetObjectItem(daily_child_js, "high");
            cJSON* low_js = cJSON_GetObjectItem(daily_child_js, "low");
            cJSON* code_day_js = cJSON_GetObjectItem(daily_child_js, "code_day");

            if(index < 3)
            {
                //转换  字符串到整形
                sscanf(cJSON_GetStringValue(high_js), "%d", &data[index].high_temp);
                sscanf(cJSON_GetStringValue(low_js), "%d", &data[index].low_temp);
                snprintf(data[index].weather_code, sizeof(data[index].weather_code),"%s", cJSON_GetStringValue(code_day_js));
                ESP_LOGI(TAG, "day[%d]->high=%d,low=%d,day_code=%s",index,data[index].high_temp,data[index].low_temp,data[index].weather_code);
            }
            index ++;
            daily_child_js = daily_child_js->next;

        }
        //设置到屏幕    
        
        //生成天气图片路径
        char img_path[32];
        snprintf(img_path,sizeof(img_path), "/img/%s@1x.png", data[0].weather_code);
        set_today_weather(&guider_ui, img_path, data[0].low_temp, data[0].high_temp);

        snprintf(img_path,sizeof(img_path), "/img/%s@1x.png", data[1].weather_code);
        set_tomorrow_weather(&guider_ui, img_path, data[1].low_temp, data[1].high_temp);

        snprintf(img_path,sizeof(img_path), "/img/%s@1x.png", data[2].weather_code);
        set_after_weather(&guider_ui, img_path, data[2].low_temp, data[2].high_temp);


        //设置城市
        set_city(&guider_ui, chinese_city);
     }


     cJSON_Delete(wt_js);
     return ESP_OK;
}
 
 /**
  * @brief 发起天气预报HTTP请求
  *
  * 构造心知天气API的请求URL，发起HTTP GET请求获取天气预报数据
  * 请求完成后清空接收缓冲区并解析天气数据
  *
  * 请求URL格式：
  * http://api.seniverse.com/v3/weather/daily.json?key=私钥&location=城市编码&language=zh-Hans&unit=c&start=0&days=3
  *
  * @return esp_err_t ESP_OK表示请求成功，其他值表示请求失败
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


/**
 * @brief 地址数据解析函数
 *
 * 解析IP定位API返回的JSON数据，提取城市编码并保存到全局变量weather_city中
 * 用于后续天气查询
 *
 * JSON数据格式示例：
 * {
 *   "data": {
 *     "city_code": "jiangmen",
 *     "city": "江门"
 *   }
 * }
 *
 * @param location_data IP定位API返回的JSON字符串
 * @return esp_err_t ESP_OK表示解析成功，ESP_FAIL表示解析失败
 */
//地址数据解析
static esp_err_t pasre_location(char* location_data)
{
    // cJSON* location_js = cJSON_Parse(location_data);

    // //直接解析
    // if(!location_js)
    // {
    //     ESP_LOGI(TAG, "地址CJSON获取失败");
    //     return ESP_FAIL;
    // }
    // cJSON* city_js = cJSON_GetObjectItem(location_js, "city");
    // if(!city_js)
    // {
    //     ESP_LOGI(TAG, "城市获取失败");
    //     return ESP_FAIL;
    // }
    // snprintf(weather_city, sizeof(weather_city), "%s", cJSON_GetStringValue(city_js));

    // ESP_LOGI(TAG, "地区：%s",weather_city);

    // cJSON_Delete(location_js);


    cJSON* location_js = cJSON_Parse(location_data);

    if(!location_js)
    {
        ESP_LOGI(TAG, "地址CJSON获取失败");
        return ESP_FAIL;
    }

    //获取data对象
    cJSON* data_js = cJSON_GetObjectItem(location_js, "data");

    //从data中提取城市编码和中文城市名
    cJSON* city_js = cJSON_GetObjectItem(data_js, "city");
    cJSON* city_code_js = cJSON_GetObjectItem(data_js, "city_code");

    snprintf(weather_city, sizeof(weather_city), "%s", cJSON_GetStringValue(city_code_js));
    snprintf(chinese_city, sizeof(chinese_city), "%s", cJSON_GetStringValue(city_js));

    ESP_LOGI(TAG, "地区：%s,%s", weather_city, chinese_city);

    cJSON_Delete(location_js);
    return ESP_OK;
}


 /**
  * @brief 发起IP定位HTTP请求
  *
  * 向IP定位服务发起HTTP GET请求，获取当前IP所在城市信息
  * 请求完成后清空接收缓冲区并解析地址数据
  *
  * 请求URL：http://ip9.com.cn/get
  *
  * @return esp_err_t ESP_OK表示请求成功，其他值表示请求失败
  */
 static esp_err_t location_http_connect(void)
 {
    static char url[256];
    // snprintf(url, sizeof(url), "http://ip-api.com/json/?lang=en");
     snprintf(url, sizeof(url), "http://ip9.com.cn/get");
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

/**
 * @brief 天气任务函数
 *
 * 循环检测WiFi连接状态，联网后依次执行IP定位和天气查询
 * 每30分钟更新一次天气数据
 *
 * @param param 任务参数（未使用）
 * @return 无
 */
static void weather_task(void* param)
{
    //判断是否联网
    while(1)
    {
        if(!WIFI_manager_is_connect())
        {
            //未联网时每2秒检测一次
            vTaskDelay(pdMS_TO_TICKS(2000));
        }else{
            //先获取IP定位，再查询天气
            location_http_connect();
            weather_http_connect();
            //每30分钟更新一次天气数据
            vTaskDelay(pdMS_TO_TICKS(1000*60*30));
        }
    }
}


/**
 * @brief 启动天气任务
 *
 * 创建天气任务，在核心1上运行，任务栈大小4096字节，优先级2
 *
 * @return 无
 */
void weather_start(void)
{
    xTaskCreatePinnedToCore(weather_task, "weater_task", 4096, NULL, 2, NULL, 1);

}
