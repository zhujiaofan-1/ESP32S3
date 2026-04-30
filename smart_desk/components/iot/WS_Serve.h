/**
 * @file WS_Serve.h
 * @brief WebSocket服务器接口
 */

#ifndef _WS_SERVE_H
#define _WS_SERVE_H

#include "esp_err.h"


//接收到数据就通过回调函数通知别的模块
typedef void(*ws_receive_cb)(uint8_t* payload, int len);

typedef struct 
{
    const char* html_code;          //网页
    ws_receive_cb receive_fn;       //回调函数
}ws_cfg_t;


esp_err_t Web_WS_Start(ws_cfg_t* cfg);      //需要传入网页和回调函数

esp_err_t _Web_WS_Stop(void);

esp_err_t _Web_WS_Send(uint8_t* Data, int len);

#endif
