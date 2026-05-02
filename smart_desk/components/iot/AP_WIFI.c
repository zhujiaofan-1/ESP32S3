/**
 * @file AP_WIFI.c
 * @brief AP配网模块实现文件
 *
 * 该模块负责ESP32的AP配网功能，包括：
 * - 启动AP热点模式供用户连接
 * - 启动Web服务器和WebSocket服务
 * - 接收用户通过网页提交的WiFi配置信息
 * - 切换回STA模式连接指定WiFi
 */

#include "AP_WIFI.h"

/*============================ ESP-IDF 头文件 ============================*/
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"

/*============================ 项目头文件 ============================*/
#include "WIFI_manager.h"
#include "WS_Serve.h"
#include "cJSON.h"
#include "OneNet_MQTT.h"

#include <string.h>
#include <sys/stat.h>

#define TAG             "AP_WIFI"

#define SPIFF_MOUNT     "/html"
#define HTML_PATH       "/html/apcfg.html"

#define APCFG_BIT       (BIT0)

//用于保存客户端发来的信息
static char Current_ssid[32];
static char Current_password[64];

static TaskHandle_t AP_WIFI_Handle = NULL;
static EventGroupHandle_t APcfg_ev;

#define APCFG_START_BIT     (BIT1)

static char * html_code = NULL;

static void ws_receive_handle(uint8_t* payload, int len);

/**
 * @brief 从SPIFFS读取AP配网页面HTML文件
 *
 * 挂载SPIFFS分区，读取apcfg.html文件内容到内存缓冲区
 *
 * @return char* HTML文件内容字符串指针，失败返回NULL
 */
static char *Init_web_page_buffer(void)
{
    esp_vfs_spiffs_conf_t spiffs_conf =
    {
        .base_path = SPIFF_MOUNT,
        .format_if_mount_failed = true,
        .max_files = 3,
        .partition_label = "html",
    };

    esp_err_t ret = esp_vfs_spiffs_register(&spiffs_conf);
    if(ret != ESP_OK && ret != ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE(TAG, "SPIFFS挂载失败: %s", esp_err_to_name(ret));
        return NULL;
    }
    ESP_LOGI(TAG, "SPIFFS挂载成功");

    struct stat st;
    if(stat(HTML_PATH, &st))
    {
        ESP_LOGE(TAG, "文件不存在: %s", HTML_PATH);
        return NULL;
    }
    ESP_LOGI(TAG, "文件大小: %ld bytes", st.st_size);

    char* buf = (char*)malloc(st.st_size + 1);
    if(buf == NULL)
    {
        ESP_LOGE(TAG, "内存分配失败，需要 %ld bytes", st.st_size + 1);
        return NULL;
    }
    memset(buf, 0, st.st_size + 1);

    //打开html
    FILE *fp = fopen(HTML_PATH, "r");
    if(fp == NULL)
    {
        ESP_LOGE(TAG, "打开文件失败: %s", HTML_PATH);
        free(buf);
        return NULL;
    }

    size_t read_count = fread(buf, st.st_size, 1, fp);
    if(read_count != 1)
    {
        ESP_LOGE(TAG, "读取文件失败，读取块数: %d", read_count);
        free(buf);
        buf = NULL;
    }
    else
    {
        ESP_LOGI(TAG, "HTML文件读取成功");
    }

    fclose(fp);
    return buf;
}

/**
 * @brief AP配网任务
 *
 * 等待AP配网启动事件，进入AP模式并启动Web服务器；
 * 等待配网完成事件，关闭Web服务器并连接用户指定的WiFi
 *
 * @param param 任务参数（未使用）
 */
static void AP_WIFI_Task(void* param)
{
    EventBits_t ev;
    while (1)
    {
        ev = xEventGroupWaitBits(APcfg_ev, APCFG_BIT | APCFG_START_BIT, pdTRUE, pdFALSE, pdMS_TO_TICKS(10*1000));

        if(ev & APCFG_START_BIT)
        {
            ESP_LOGI(TAG, "进入AP配网模式...");
            
            OneNet_Stop();
            
            esp_err_t ret = WIFI_manager_AP();
            if(ret != ESP_OK)
            {
                ESP_LOGE(TAG, "进入AP模式失败: %s", esp_err_to_name(ret));
            }
            else
            {
                ESP_LOGI(TAG, "AP模式启动成功，IP地址: 192.168.100.1");
                
                ws_cfg_t ws_cfg =
                {
                    .html_code = html_code,
                    .receive_fn = ws_receive_handle,
                };
                ret = Web_WS_Start(&ws_cfg);
                if(ret != ESP_OK)
                {
                    ESP_LOGE(TAG, "启动Web服务器失败: %s", esp_err_to_name(ret));
                }
                else
                {
                    ESP_LOGI(TAG, "Web服务器启动成功，请访问 http://192.168.100.1");
                }
            }
        }

        if(ev & APCFG_BIT)
        {
            _Web_WS_Stop();

            if(html_code)
            {
                free(html_code);
                html_code = NULL;
            }

            WIFI_manager_connect((char*)Current_ssid,(char*)Current_password);
        }
    }
}


/**
 * @brief 初始化AP配网模块
 *
 * 初始化WiFi管理器、加载HTML页面文件、创建事件组和AP配网任务
 *
 * @param f WiFi状态回调函数
 */
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

/**
 * @brief 使用配网获取的SSID和密码连接WiFi
 */
void AP_WIFI_Connect()
{

}

/**
 * @brief WiFi扫描结果处理回调函数
 *
 * 将扫描到的AP列表组装成JSON格式，通过WebSocket发送给网页客户端
 *
 * @param num 扫描到的AP数量
 * @param ap_record AP信息记录数组指针
 */
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


/**
 * @brief WebSocket数据接收处理回调函数
 *
 * 解析网页客户端发来的JSON指令：
 * - scan指令：启动WiFi扫描
 * - ssid+password：保存用户选择的WiFi凭据并触发连接
 *
 * @param payload 接收到的数据缓冲区
 * @param len 数据长度
 */
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

/**
 * @brief 请求进入AP配网模式
 *
 * 通过事件组通知AP配网任务启动AP模式
 */
void AP_WIFI_apcfg()
{
    ESP_LOGI(TAG, "请求进入AP配网模式...");
    xEventGroupSetBits(APcfg_ev, APCFG_START_BIT);
}