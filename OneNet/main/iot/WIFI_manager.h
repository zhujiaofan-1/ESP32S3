#ifndef _WIFI_MANAGER_H
#define _WIFI_MANAGER_H

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_wifi_types_generic.h"

typedef enum{
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTED,
}WIFI_STATE;

//声明回调函数
//WIFI状态执行逻辑回调
typedef void(*p_wifi_state_cb)(WIFI_STATE);
//WIFI扫描回调
typedef void(*p_wifi_scan_cb)(int num, wifi_ap_record_t *ap_record);

void WIFI_manager_Init(p_wifi_state_cb f);

void WIFI_manager_connect(const char* ssid,const char* password);

esp_err_t WIFI_manager_AP(void);        //进入AP模式

esp_err_t WIFI_manager_scan(p_wifi_scan_cb f);      //扫描WIFI，需要传入扫描的回调函数

#endif
