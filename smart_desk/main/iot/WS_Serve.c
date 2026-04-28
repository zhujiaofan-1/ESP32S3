#include "WS_Serve.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "http_parser.h"
#include <string.h>
#include "esp_log.h"


#define TAG     "WS_server"

//网页
static const char* http_html = NULL;

//http服务器句柄
static httpd_handle_t http_server_handle = NULL;

//接收到数据时，通知到其他应该功能模块来进行自定义的处理
static ws_receive_cb ws_receive_callback = NULL ;

static int Socket_ID = -1;

/**
 * @brief HTTP请求处理回调函数
 *
 * 处理HTTP GET请求，返回HTML页面
 *
 * @param r HTTP请求结构体指针
 * @return esp_err_t - ESP_OK表示成功，其他值表示失败
 */
//http请求回调函数
esp_err_t get_http_req(httpd_req_t *param)
{
    //检查HTML内容是否有效
    if(http_html == NULL)
    {
        ESP_LOGE(TAG, "HTML内容为空，无法响应请求");
        httpd_resp_send_404(param);
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "收到HTTP请求，发送HTML页面");
    return httpd_resp_send(param, http_html, HTTPD_RESP_USE_STRLEN);
}



/**
 * @brief WebSocket数据处理回调函数
 *
 * 当客户端通过 /ws 路径建立 WebSocket 连接时，
 * ESP-IDF 的 HTTP 服务器会自动调用此函数处理 WebSocket 相关操作
 *
 * @param param HTTP请求结构体指针，包含WebSocket连接信息
 * @return esp_err_t - ESP_OK表示成功，其他值表示失败
 */
esp_err_t ws_req(httpd_req_t *param)
{
    /**
     * 处理WebSocket握手请求
     * 第一次建立连接时会收到HTTP GET请求，需要过滤掉
     * 并提取Socket ID用于后续操作
     */
    if(param->method == HTTP_GET)
    {
        //提取Socket ID，用于标识WebSocket连接
        Socket_ID = httpd_req_to_sockfd(param);
        return ESP_OK;
    }

    //帧结构数据，用于存储WebSocket帧信息
    httpd_ws_frame_t packet;
    esp_err_t ret;
    //清空原始数据
    memset(&packet, 0, sizeof(packet));

    // 第三个参数0，表示获取长度
    ret = httpd_ws_recv_frame(param, &packet, 0);       

    //分配内存区域，用于存储实际的WebSocket数据
    if(ret != ESP_OK)
    {
        return ret;
    }
    uint8_t *buf = (uint8_t*)malloc(packet.len + 1);

    if(buf == NULL)
    {
        return ESP_FAIL;
    }
    
    // 获取真正的数据
    packet.payload = buf;       //设置数据存储缓冲区
    ret = httpd_ws_recv_frame(param, &packet, packet.len);

    if(ret == ESP_OK)
    {
        //判断数据包类型
        if(packet.type == HTTPD_WS_TYPE_TEXT)
        {
            //打印接收到的WebSocket文本信息
            ESP_LOGI(TAG, "获取到WebSocket信息:%s",packet.payload);
            
            //调用注册的WebSocket数据处理回调函数
            if(ws_receive_callback)
            {
                ws_receive_callback(packet.payload, packet.len);
            }
        }
    }

    //释放分配的内存
    free(buf);

    return ret;
}



/**
 * @brief 启动Web服务器和WebSocket服务
 *
 * 该函数初始化HTTP服务器，配置URI处理程序，并准备WebSocket功能
 *
 * @param cfg 配置结构体，包含HTML代码和WebSocket接收回调函数
 * @return esp_err_t - ESP_OK表示成功，ESP_FAIL表示失败
 *
 * @note 调用此函数前应确保配置结构体有效
 */
esp_err_t Web_WS_Start(ws_cfg_t* cfg)
{
    if(cfg == NULL)
    {
        return ESP_FAIL;
    }

    http_html = cfg->html_code;
    ws_receive_callback = cfg->receive_fn;

    //初始化http(默认配置)      ，传入服务器句柄 
    httpd_config_t http_config = HTTPD_DEFAULT_CONFIG();
    esp_err_t ret = httpd_start(&http_server_handle, &http_config);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "HTTP服务器启动失败: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "HTTP服务器启动成功");

    //配置uri
    httpd_uri_t uri_get=
    {
        .uri = "/",               //URI路径
        .method = HTTP_GET,       //方法：发送请求
        .handler = get_http_req,            //回调函数
    };
    // 注册：传入服务器句柄和uri配置
    ret = httpd_register_uri_handler(http_server_handle, &uri_get);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "注册URI处理程序失败: %s", esp_err_to_name(ret));
        httpd_stop(http_server_handle);
        return ret;
    }

    //配置WebSocket
    httpd_uri_t uri_WebSocket=
    {
        .uri = "/ws",               //URI路径
        .method = HTTP_GET,       //方法：发送请求
        .handler = ws_req,            //回调函数
        .is_websocket = true,       //表示这个uri专门处理WebSocket
    };
    ret = httpd_register_uri_handler(http_server_handle, &uri_WebSocket);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "注册WebSocket处理程序失败: %s", esp_err_to_name(ret));
        httpd_stop(http_server_handle);
        return ret;
    }
    
    ESP_LOGI(TAG, "WebSocket服务器配置完成");
    return ESP_OK;
}



/**
 * @brief 停止Web服务器和WebSocket服务
 *
 * 该函数停止HTTP服务器并清理相关资源
 *
 * @return esp_err_t - ESP_OK表示成功，其他值表示失败
 */
esp_err_t _Web_WS_Stop(void)
{
    //判断服务器句柄是否合法
    if(http_server_handle)
    {
        httpd_stop(http_server_handle);
        http_server_handle = NULL;
    }


    return ESP_OK;
}



/**
 * @brief 发送WebSocket数据
 *
 * 该函数通过WebSocket发送数据
 *
 * @param Data 要发送的数据缓冲区
 * @param len 数据长度
 * @return esp_err_t - ESP_OK表示成功，其他值表示失败
 */
esp_err_t _Web_WS_Send(uint8_t* Data, int len)
{
    // 帧数据
    httpd_ws_frame_t Packet;
    memset(&Packet, 0, sizeof(http_server_handle));
    Packet.payload = Data;              //填充数据
    Packet.len = len;                   
    Packet.type = HTTPD_WS_TYPE_TEXT;   //类型：文本

    httpd_ws_send_data(http_server_handle, Socket_ID, &Packet); //发送

    return ESP_OK;
}