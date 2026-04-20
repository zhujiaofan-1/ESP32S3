#ifndef _WIFI_MANAGER_H
#define _WIFI_MANAGER_H

typedef enum{
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTED,
}WIFI_STATE;


typedef void(*p_wifi_state_cb)(WIFI_STATE);

void WIFI_manager_Init(p_wifi_state_cb f);

void WIFI_manager_connect(const char* ssid,const char* password);

#endif
