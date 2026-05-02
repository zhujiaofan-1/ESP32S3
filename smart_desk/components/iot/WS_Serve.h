/**
 * @file WS_Serve.h
 * @brief WebSocket服务器接口
 */

#ifndef _WS_SERVE_H
#define _WS_SERVE_H

#include "esp_err.h"


/**
 * @brief WebSocket接收数据回调函数类型
 *
 * @param payload 接收到的数据缓冲区
 * @param len 数据长度
 */
typedef void(*ws_receive_cb)(uint8_t* payload, int len);

/**
 * @brief WebSocket配置结构体
 */
typedef struct 
{
    const char* html_code;          /**< 网页HTML内容 */
    ws_receive_cb receive_fn;       /**< 数据接收回调函数 */
}ws_cfg_t;


/**
 * @brief 启动Web服务器和WebSocket服务
 *
 * @param cfg 配置结构体，包含HTML代码和回调函数
 * @return esp_err_t ESP_OK成功，ESP_FAIL失败
 */
esp_err_t Web_WS_Start(ws_cfg_t* cfg);

/**
 * @brief 停止Web服务器和WebSocket服务
 *
 * @return esp_err_t ESP_OK成功，其他失败
 */
esp_err_t _Web_WS_Stop(void);

/**
 * @brief 通过WebSocket发送数据
 *
 * @param Data 要发送的数据缓冲区
 * @param len 数据长度
 * @return esp_err_t ESP_OK成功，其他失败
 */
esp_err_t _Web_WS_Send(uint8_t* Data, int len);

#endif
