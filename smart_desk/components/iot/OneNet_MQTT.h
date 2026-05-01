#ifndef _ONENET_MQTT_H
#define _ONENET_MQTT_H

#include "esp_err.h"

#define ONENET_PRODUCT_ID           "Dv57YAJzDX"
#define ONENET_PRODUCT_ACCESS_KEY   "RzU/A0E1CQM1axUdbdEPM3uPV7G/ZQfLPaOJkXfRRwk="
#define ONENET_DEVICE_NAME             "ESP32LED01"

typedef void (*onenet_connected_cb_t)(void);

esp_err_t OneNet_Start(void);
void OneNet_RegisterConnectedCallback(onenet_connected_cb_t cb);

#endif
