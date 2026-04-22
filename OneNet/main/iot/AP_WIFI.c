#include "AP_WIFI.h"
#include "WIFI_manager.h"
#include "WS_Serve.h"
#include "cJSON.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <string.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

#define TAG             "AP_WIFI"

#define SPIFF_MOUNT     "/spiffs"
#define HTML_PATH       "/spiffs/apcfg.html"

#define APCFG_BIT       (BIT0)

//用于保存客户端发来的信息
static char Current_ssid[32];
static char Current_password[64];

static TaskHandle_t AP_WIFI_Handle = NULL;
static EventGroupHandle_t APcfg_ev;

//存储html网页
static char * html_code = NULL;

static char *Init_web_page_buffer(void)
{
    esp_vfs_spiffs_conf_t spiffs_conf =
    {
        .base_path = SPIFF_MOUNT,
        .format_if_mount_failed = false,
        .max_files = 3,
        .partition_label = NULL,
    };
    esp_vfs_spiffs_register(&spiffs_conf);

    struct stat st;
    if(stat(HTML_PATH, &st))
    {
        return NULL;
    }

    char* buf = (char*)malloc(st.st_size + 1);
    memset(buf, 0, st.st_size + 1);

    //打开html
    FILE *fp = fopen(HTML_PATH, "r");
    if(fp)
    {
        if(0 == fread(buf, st.st_size, 1, fp))
        {
            free(buf);
            buf =NULL;
        }
        fclose(fp);
    }
    else
    {
        free(buf);
        buf =NULL;
    }
    return buf;
}

static void AP_WIFI_Task(void* param)
{
    EventBits_t ev;
    while (1)
    {
        //等待事件通知
        ev = xEventGroupWaitBits(APcfg_ev, APCFG_BIT, pdTRUE, pdFALSE, pdMS_TO_TICKS(10*1000));
        if(ev & APCFG_BIT)
        {
            //停止服务器
            _Web_WS_Stop();
            //发起连接
            WIFI_manager_connect((char*)Current_ssid,(char*)Current_password);
        }
    }
    
}


void AP_WIFI_Init(p_wifi_state_cb f)
{
    //初始化WIFI
    WIFI_manager_Init(f);

    //读取html文件
    html_code = Init_web_page_buffer();
    
    if(html_code == NULL)
    {
        ESP_LOGE(TAG, "HTML文件加载失败！请检查SPIFFS中是否存在 /spiffs/apcfg.html");
    }
    else
    {
        ESP_LOGI(TAG, "HTML文件加载成功，大小: %d bytes", (int)strlen(html_code));
    }

    APcfg_ev = xEventGroupCreate();
    //新建任务用于断开wifi，然后连接
    xTaskCreatePinnedToCore(AP_WIFI_Task, "AP_WIFI_Task", 4096, NULL, 3, &AP_WIFI_Handle, 1);
    
}

void AP_WIFI_Connect()
{

}

// 扫描得到数据处理的回调函数
void WIFI_Scan_handle(int num, wifi_ap_record_t *ap_record)
{
    cJSON* root = cJSON_CreateObject();

    //向CJSON中填充内容,用于发送到网页
    cJSON* wifi_list_cj = cJSON_AddArrayToObject(root, "wifi_list");

    //遍历提取内容
    for(int i = 0; i < num; i++)
    {
        cJSON* wifi_cj = cJSON_CreateObject();
        //填充ssid
        cJSON_AddStringToObject((wifi_cj), "ssid", (char*)ap_record[i].ssid);
        //填充rssi(信号强度)
        cJSON_AddNumberToObject((wifi_cj), "rssi", ap_record[i].rssi);
        //是否加密
        if(ap_record[i].authmode == WIFI_AUTH_OPEN)
        {
            //如果是开放wifi，就写0
            cJSON_AddBoolToObject(wifi_cj, "encrypted", 0);
        }else{
            cJSON_AddBoolToObject(wifi_cj, "encrypted", 1);
        }

        //把wifi_cj加入到wifi_list_cj
        cJSON_AddItemToArray(wifi_list_cj, wifi_cj);
    }

    char* Data = cJSON_Print(root);

    ESP_LOGI(TAG, "WS Send Data:%s",Data);

    //把扫描结果发送到客户端
    _Web_WS_Send((uint8_t*)Data, strlen(Data));

    cJSON_free(Data);
    cJSON_Delete(root);
}


// WS数据解析处理回调
void ws_receive_handle(uint8_t* payload, int len)
{

    cJSON* root = cJSON_Parse((char*)payload);

    // 提取键值对
    if(root)
    {
        //提取键值对的键值：“scan”：“start”
        cJSON* scan_js = cJSON_GetObjectItem(root, "scan");

        cJSON* ssid_js = cJSON_GetObjectItem(root, "ssid");

        cJSON* password_js = cJSON_GetObjectItem(root, "password");
    
        //判断指令
        if(scan_js)
        {
            //提取键值对的字符串：“scan”：“start”
            char* scan_value = cJSON_GetStringValue(scan_js);
            if(strcmp(scan_value, "start") == 0)
            {
                //启动扫描
                WIFI_manager_scan(WIFI_Scan_handle);
            }
        }
        if(ssid_js && password_js)
        {
            char* ssid_value = cJSON_GetStringValue(ssid_js);
            char* password_value = cJSON_GetStringValue(password_js);
            
            //保存密码
            snprintf(Current_ssid, sizeof(Current_ssid), "%s", ssid_value);
            snprintf(Current_password, sizeof(Current_password), "%s", password_value);

            //此回调函数里面由websocket底层调用，不宜直接调用关闭服务器操作
            // WIFI_manager_connect(ssid_value, password_value);

            //发出事件通知任务
            xEventGroupSetBits(APcfg_ev, APCFG_BIT);
        }
    }
}

//进入配网模式
void AP_WIFI_apcfg()
{
    ESP_LOGI(TAG, "进入AP配网模式...");
    
    esp_err_t ret = WIFI_manager_AP();
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "进入AP模式失败: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "AP模式启动成功，IP地址: 192.168.100.1");
    
    //启动http服务器
    ws_cfg_t ws_cfg =
    {
        .html_code = html_code,
        .receive_fn = ws_receive_handle,
    };
    ret = Web_WS_Start(&ws_cfg);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "启动Web服务器失败: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "Web服务器启动成功，请访问 http://192.168.100.1");
}