#ifndef _ONENET_MQTT_H
#define _ONENET_MQTT_H

#include "esp_err.h"

//产品ID
#define ONENET_PRODUCT_ID           "Dv57YAJzDX"
//产品密钥
#define ONENET_PRODUCT_ACCESS_KEY   "RzU/A0E1CQM1axUdbdEPM3uPV7G/ZQfLPaOJkXfRRwk="
//设备名称
#define ONE_DEVICE_NAME             "ESP32LED01"

esp_err_t OneNet_Start(void);




#endif
