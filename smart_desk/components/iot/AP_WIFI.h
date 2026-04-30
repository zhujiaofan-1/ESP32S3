/**
 * @file AP_WIFI.h
 * @brief AP配网模块接口
 */

#ifndef _AP_WIFI_H
#define _AP_WIFI_H

#include "WIFI_manager.h"

void AP_WIFI_Init(p_wifi_state_cb f);


//启动AP配网
void AP_WIFI_apcfg();

#endif
