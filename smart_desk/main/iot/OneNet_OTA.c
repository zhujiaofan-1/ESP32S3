#include "OneNet_OTA.h"
#include "OneNet_MQTT.h"
#include "cJSON.h"
#include "esp_app_desc.h"
#include "esp_err.h"
#include "esp_flash_partitions.h"
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include <stdio.h>
#include <string.h>
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "onenet_token.h"
#include "esp_https_ota.h"

#define TAG             "OneNet_OTA"
#define TOKEN_VALID_TIMERSTAMP      1790855684

#define ONENET_OTA_URL      "http://iot-api.heclouds.com/fuse-ota"

//任务运行标志
static bool ota_is_running = false;

//目标版本号和任务id
static char target_version[32] = {0};
static int task_id = 0;


/**
 * @brief 获取当前应用版本号
 *
 * 从当前运行的OTA分区中提取固件版本信息
 *
 * @return const char* 返回版本号字符串指针
 */
//获取版本号
const char* get_app_version(void)
{
    static char app_version[32] = {0};

    if(app_version[0] == 0)
    {
        //返回当前分区运行信息
        const esp_partition_t* running =  esp_ota_get_running_partition();

        // 提取信息
        esp_app_desc_t app_description;
        esp_ota_get_partition_description(running, &app_description);

        snprintf(app_version, sizeof(app_version), "%s",app_description.version);
    }

    return app_version;
}



/**
 * @brief 设置当前应用分区合法性
 *
 * 当应用首次启动验证成功后，标记为合法以取消回滚；
 * 若验证失败则标记为非法并触发回滚到旧版本
 *
 * @param valid 1表示标记为合法，0表示标记为非法并回滚
 * @return 无
 */
//传入0，把当前分区标记为不合法
void set_app_valid(int valid)
{
    esp_ota_img_states_t state;
    esp_err_t ret;

    //返回当前分区运行信息
    const esp_partition_t* running =  esp_ota_get_running_partition();

    //取出分区运行信息
    ret = esp_ota_get_state_partition(running, &state);

    //判断
    if(ret == ESP_OK)
    {
        //是否首次启动状态,此时才能修改其是否合法
        if(state == ESP_OTA_IMG_PENDING_VERIFY)
        {
            if(valid == 1)
            {
                //取消回滚
                esp_ota_mark_app_valid_cancel_rollback();
            }else{
                //回滚
                esp_ota_mark_app_invalid_rollback_and_reboot();
            }
        }
    }

}



// OTA数据接收缓冲区及已接收数据长度
#define OTA_DATA_LEN        1024
static uint8_t ota_data_buf[OTA_DATA_LEN];
static int ota_data_size = 0;



/**
 * @brief HTTP事件回调函数
 *
 * 处理HTTP客户端各种事件，主要功能是接收服务器返回的数据
 * 并将数据拷贝到ota_data_buf缓冲区中
 *
 * @param evt HTTP事件结构体指针
 * @return esp_err_t ESP_OK表示处理成功
 */
esp_err_t _http_event_handler(esp_http_client_event_t *evt)
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
        case HTTP_EVENT_ON_DATA:                //http
        {
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            printf("HTTP_EVENT_ON_DATA data=%.*s\r\n",evt->data_len,(char*)evt->data);

            int copy_len = 0;
            //如果大于剩余空间的话
            if(evt->data_len > OTA_DATA_LEN - ota_data_size)
            {
                //剩余的空间
                copy_len = OTA_DATA_LEN - ota_data_size;

            }else{
                copy_len = evt->data_len;
            }

            //拷贝数据
            memcpy(&ota_data_buf[ota_data_size], evt->data, copy_len);
            ota_data_size += copy_len;
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
 * @brief 发起OneNet OTA HTTP请求
 *
 * 构造HTTP请求并发送到OneNet平台，包括生成认证Token、设置请求头
 * 请求完成后清空接收缓冲区
 *
 * @param url 请求URL地址
 * @param method HTTP请求方法（GET/POST等）
 * @param payload POST请求的数据体，为NULL时表示GET请求
 * @return esp_err_t ESP_OK表示请求成功，其他值表示失败
 */
static esp_err_t OneNet_ota_http_connect(const char* url,esp_http_client_method_t method, const char* payload)
{

    
    esp_http_client_config_t config = {
       .url = url,
       .event_handler = _http_event_handler,
    };
     esp_http_client_handle_t client = esp_http_client_init(&config);

    char* token = (char*)malloc(256);
    memset(token, 0, 256);

    dev_token_generate(token, SIG_METHOD_SHA256, TOKEN_VALID_TIMERSTAMP, ONENET_PRODUCT_ID, NULL, ONENET_PRODUCT_ACCESS_KEY);
    ESP_LOGI(TAG, "OTA_token:%s", token);
     // POST
     //设置方法
    esp_http_client_set_method(client, method);             
    //设置请求头
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "host", "iot-api.heclouds.com");
     esp_http_client_set_header(client, "Authorization", token);
    if(payload)
    {
        ESP_LOGI(TAG, "post data:%s", payload);
        esp_http_client_set_post_field(client, payload, strlen(payload));
    }
    // 这是为了 清空上一次HTTP请求的残留数据 ，确保本次请求接收到的数据是干净的。
    memset(ota_data_buf, 0, sizeof(ota_data_buf));
    ota_data_size = 0;

    //直接发起http请求，阻塞
    esp_err_t ret = esp_http_client_perform(client);

    //释放内存
    free(token);
    esp_http_client_cleanup(client);

    return ret;
}



/**
 * @brief 向OneNet平台上报当前固件版本号
 *
 * 将设备当前运行的软件版本和固件版本上报到OneNet平台，
 * 用于平台判断是否有可用的升级任务
 *
 * @return esp_err_t ESP_OK表示上报成功，其他值表示失败
 */
//上报版本号
esp_err_t OneNet_ota_upload_version(void)
{
    char url[128];     // http://iot-api.heclouds.com/fuse-ota/{pro_id}/{dev_name}/version
    snprintf(url, sizeof(url), ONENET_OTA_URL"/%s/%s/version", ONENET_PRODUCT_ID, ONENET_DEVICE_NAME);

    //生成字符串            {"s_version":"V1.3", "f_version": "V2.0"}
    char version[128];
    //获取版本号
    const char* app_version = get_app_version();
    snprintf(version, sizeof(version), "{\"s_version\":\"%s\", \"f_version\": \"%s\"}", app_version, app_version); 
    
    //发送请求
    esp_err_t ret = OneNet_ota_http_connect(url, HTTP_METHOD_POST, version);
    

    /*  OneNet响应内容
    {
        "code": 0,
        "msg": "succ",
        "request_id": "**********"
    }
    */

    if(ret == ESP_OK)
    {
        cJSON* root = cJSON_Parse((const char*)ota_data_buf);
        if(root)
        {
            //取出code,为0表示发送成功
            cJSON* code_js = cJSON_GetObjectItem(root, "code");
            if(code_js && cJSON_GetNumberValue(code_js) == 0)
            {
                ret = ESP_OK;
            }
        }
    }
    if(ret != ESP_OK)
    {
        ESP_LOGI(TAG, "上报版本号失败");
    }
  return ret;
}


/*
发送一下请求给OneNet，获取升级任务

GET http://iot-api.heclouds.com/fuse-ota/{pro_id}/{dev_name}/check?type=1&version=1.2
Content-Type: application/json


*/

    /*平台返回
    {
	"code": 0,
	"msg": "succ",
	"request_id": "**********",
	"data": {
		"target": "1.2", // 升级任务的目标版本
		"tid": 12, //任务ID
		"size": 123, //文件大小
		"md5": "dfkdajkfd", //升级文件的md5
		"status": 1 | 2 | 3, //1 ：待升级， 2 ：下载中， 3 ：升级中
		"type": 1 | 2 // 1:完整包，2：差分包  
	}
}
*/

/**
 * @brief 检测OneNet平台是否有可用的OTA升级任务
 *
 * 向OneNet平台发送检查请求，获取目标版本号、任务ID等升级信息
 * 并将结果保存到全局变量target_version和task_id中
 *
 * @return esp_err_t ESP_OK表示检测到升级任务，ESP_FAIL表示无升级任务或检测失败
 */
//检测升级任务函数
esp_err_t OneNet_ota_check_task(void)
{
    char url[128];    
    const char* app_version = get_app_version();

    esp_err_t ret = ESP_FAIL;

    snprintf(url, sizeof(url), ONENET_OTA_URL"/%s/%s/check?type=1&version=%s", ONENET_PRODUCT_ID, ONENET_DEVICE_NAME, app_version);

    //发送请求
    ret = OneNet_ota_http_connect(url, HTTP_METHOD_GET, NULL);
    



    if(ret == ESP_OK)
    {
        cJSON* root = cJSON_Parse((const char*)ota_data_buf);
        if(root)
        {
            //取出code,为0表示发送成功
            cJSON* code_js = cJSON_GetObjectItem(root, "code");
            //获取tid  任务id,任务版本号
            cJSON* data_js = cJSON_GetObjectItem(root, "data");
            cJSON* tid_js = cJSON_GetObjectItem(data_js, "tid");
            cJSON* target_js = cJSON_GetObjectItem(data_js, "target");

            if(code_js && cJSON_GetNumberValue(code_js) == 0)
            {
                //当发送成功之后，再提取任务id
                if(tid_js && target_js)
                {
                    //提取内容
                    task_id = cJSON_GetNumberValue(tid_js);
                    snprintf(target_version, sizeof(target_version), "%s",  cJSON_GetStringValue(target_js));
                    
                }
                
                ret = ESP_OK;
            }else{
                ESP_LOGI(TAG, "检测OTA升级任务失败");
                ret = ESP_FAIL;
            }
        }
    }
    
  return ret;
}


/*
POST http://iot-api.heclouds.com/fuse-ota/{pro_id}/{dev_name}/{tid}/status

Content-Type: application/json

Authorization:version=2022-05-01&res=userid%2F112&et=1662515432&method=sha1&sign=Pd14JLeTo77e0FOpKN8bR1INPLA%3D 

host:iot-api.heclouds.com

Content-Length:20

{"step":10} 
*/
//返回
/*
{
{
	"code": 0,
	"msg": "succ",
	"request_id": "**********"
}
*/

/**
 * @brief 上报OTA升级进度到OneNet平台
 *
 * 向OneNet平台发送升级进度信息，用于平台显示升级状态
 *
 * @param step 升级进度百分比（0-100）
 * @return esp_err_t ESP_OK表示上报成功，其他值表示失败
 */
//上报任务升级状态
esp_err_t OneNet_ota_upload_state(int step)
{
    char url[128];     
    char payload[16];

    snprintf(url, sizeof(url), ONENET_OTA_URL"/%s/%s/%d/status", ONENET_PRODUCT_ID, ONENET_DEVICE_NAME, task_id);

    snprintf(payload, sizeof(payload), "{\"step\":%d} ", step); 
    
    //发送请求
    esp_err_t ret = OneNet_ota_http_connect(url, HTTP_METHOD_POST, payload);
    


    if(ret == ESP_OK)
    {
        cJSON* root = cJSON_Parse((const char*)ota_data_buf);
        if(root)
        {
            //取出code,为0表示发送成功
            cJSON* code_js = cJSON_GetObjectItem(root, "code");
            if(code_js && cJSON_GetNumberValue(code_js) == 0)
            {
                ret = ESP_OK;
            }
        }
    }
    if(ret != ESP_OK)
    {
        ESP_LOGI(TAG, "上报版本号失败");
    }
  return ret;
}



/**
 * @brief 下载升级包时的HTTP请求头初始化回调函数
 *
 * 在esp_https_ota下载升级包前被调用，用于设置HTTP请求方法
 * 和添加必要的请求头（认证Token、Host等）
 *
 * @param client HTTP客户端句柄
 * @return esp_err_t ESP_OK表示设置成功，其他值表示失败
 */
//下载升级包的回调函数，用于发送请求头
esp_err_t OneNet_ota_init_cb(esp_http_client_handle_t client)
{
/*
GET 
http://iot-api.heclouds.com/fuse-ota/{pro_id}/{dev_name}/{tid}/download

Authorization:version=2022-05-01&res=userid%2F112&et=1662515432&method=sha1&sign=Pd14JLeTo77e0FOpKN8bR1INPLA%3D

host:iot-api.heclouds.com
*/

    static char token[256];
    esp_err_t ret = ESP_FAIL;
    memset(token, 0, 256);

    ret = dev_token_generate(token, SIG_METHOD_SHA256, TOKEN_VALID_TIMERSTAMP, ONENET_PRODUCT_ID, NULL, ONENET_PRODUCT_ACCESS_KEY);
     // POST
     //设置方法
    ret = esp_http_client_set_method(client, HTTP_METHOD_GET);             
    //设置请求头
    ret = esp_http_client_set_header(client, "Content-Type", "application/json");
    ret = esp_http_client_set_header(client, "host", "iot-api.heclouds.com");
    ret = esp_http_client_set_header(client, "Authorization", token);

    return ret;
}



/**
 * @brief 从OneNet平台下载并安装OTA升级包
 *
 * 使用esp_https_ota组件从OneNet平台下载固件升级包，
 * 下载完成后自动完成OTA升级流程
 *
 * @param tid 升级任务ID
 * @return esp_err_t ESP_OK表示升级成功，其他值表示升级失败
 */
//下载升级包
esp_err_t OneNet_ota_download(int tid)
{
    esp_err_t ota_ret = ESP_FAIL;
    char url[128] = {0};
    snprintf(url, sizeof(url), "http://iot-api.heclouds.com/fuse-ota/%s/%s/%d/download", ONENET_PRODUCT_ID, ONENET_DEVICE_NAME, tid);

    //http配置结构体变量
    esp_http_client_config_t http_config =
    {
        .url = url,
    };

    esp_https_ota_config_t ota_config =
    {
        .http_config = &http_config,
        .http_client_init_cb = OneNet_ota_init_cb,     //回调函数用于设置请求头
    };

    ota_ret = esp_https_ota(&ota_config);
    if(ota_ret == ESP_OK)
    {
        ESP_LOGI(TAG, "升级成功");
    }else
    {
        ESP_LOGI(TAG, "升级失败，错误码:%d",ota_ret);
    }

    return ota_ret;
}



/**
 * @brief OTA升级任务函数
 *
 * 执行完整的OTA升级流程：上报版本号 -> 检测升级任务 -> 上报进度 -> 下载升级包 -> 重启
 *
 * @param param 任务参数（未使用）
 * @return 无
 */
//执行操作的任务
static void OneNet_ota_Task(void* param)
{
    esp_err_t ret = ESP_FAIL;

    //上报版本号
    ret = OneNet_ota_upload_version();
    if(ret != ESP_OK)
    {
        ESP_LOGI(TAG, "上报版本号失败 !");
        goto delete_ota_task;
    }

    // 检测升级任务
    ret = OneNet_ota_check_task();
    if(ret != ESP_OK)
    {
        ESP_LOGI(TAG, "检测升级任务失败 !");
        goto delete_ota_task;
    }

    // 上报任务升级状态，10%
    ret = OneNet_ota_upload_state(10);
    if(ret != ESP_OK)
    {
        ESP_LOGI(TAG, "上报升级状态失败 !");
        goto delete_ota_task;
    }

    //进行http下载
    ret = OneNet_ota_download(task_id);
    if(ret != ESP_OK)
    {
        ESP_LOGI(TAG, "下载更新失败 !");
        goto delete_ota_task;
    }

    // 上报进度100%
    ret = OneNet_ota_upload_state(100); 
    if(ret != ESP_OK)
    {
        ESP_LOGI(TAG, "上报升级状态失败 !");
        goto delete_ota_task;
    }

    // 重启
    esp_restart();

delete_ota_task:
    ota_is_running = false;
    vTaskDelete(NULL);
}



/**
 * @brief 启动OTA升级流程
 *
 * 创建OTA升级任务，开始执行完整的OTA升级流程
 * 如果已有升级任务在运行，则直接返回
 *
 * @return 无
 */
TaskHandle_t OTA_Task_handle = NULL;
void OneNet_ota_start(void)
{
    if(ota_is_running == true)
    {
        return;
    }
    ESP_LOGI(TAG, "开始OTA升级");
    xTaskCreatePinnedToCore(OneNet_ota_Task, "OneNet_OTA_Task", 8192, NULL, 2, &OTA_Task_handle, 1);    
    
    if(OTA_Task_handle != NULL)
    {
        ESP_LOGI(TAG, "OTA任务创建成功！");
    }
    
}
