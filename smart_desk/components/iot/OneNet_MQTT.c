#include "OneNet_MQTT.h"
#include "OneNet_OTA.h"
#include "cJSON.h"
#include "esp_event_base.h"
#include "onenet_token.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "OneNet_dm.h"
#include "OneNet_OTA.h"


#define TAG     "OneNet"

static esp_mqtt_client_handle_t mqtt_handle = NULL;
static onenet_connected_cb_t connected_callback = NULL;

void OneNet_RegisterConnectedCallback(onenet_connected_cb_t cb)
{
    connected_callback = cb;
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        OneNet_subscribe(mqtt_handle);

        cJSON* property_js = OneNet_property_upload();
        char* data = cJSON_PrintUnformatted(property_js);

        OneNet_post_property_data(mqtt_handle, data);

        cJSON_Delete(property_js);
        cJSON_free(data);

        OneNet_ota_upload_version();
        set_app_valid(true);

        if(connected_callback != NULL)
        {
            //连接成功后通过回调函数通知主函数
            connected_callback();
        }

        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:           //下行数据事件
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);

        //判断下行数据主题,是否包含设备属性设置   property/set  
        if(strstr(event->topic, "property/set") != 0)
        {
            //生成CJSO对象
            cJSON* property_js = cJSON_Parse(event->data);
            
            OneNet_property_handle(property_js);
            //取出下发id，用于回应数据
            cJSON* id_js = cJSON_GetObjectItem(property_js, "id");
            OneNet_property_ack(mqtt_handle, cJSON_GetStringValue(id_js), 200, "success");

            cJSON_Delete(property_js);
        }
        //判断下行数据主题,是否包含OTA通知   ota/inform
        if(strstr(event->topic, "ota/inform") != NULL)
        {
            ESP_LOGI(TAG, "收到OTA通知");
            //生成CJSO对象
            cJSON* ota_js = cJSON_Parse(event->data);
            
            //取出下发id，用于回应数据
            cJSON* id_js = cJSON_GetObjectItem(ota_js, "id");
            OneNet_ota_ack(mqtt_handle, cJSON_GetStringValue(id_js), 200, "success");

            cJSON_Delete(ota_js);

            //开始OTA升级流程
            OneNet_ota_start();
        }

        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
        if(event->error_handle)
        {
            if(event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
            {
                ESP_LOGE(TAG, "TCP传输错误, esp_tls_err=0x%x, esp_err=0x%x",
                         event->error_handle->esp_tls_last_esp_err,
                         event->error_handle->esp_transport_sock_errno);
            }
            else if(event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED)
            {
                ESP_LOGE(TAG, "连接被拒绝, return_code=0x%x",
                         event->error_handle->connect_return_code);
            }
            else
            {
                ESP_LOGE(TAG, "未知错误类型: 0x%x", event->error_handle->error_type);
            }
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}





esp_err_t OneNet_Start(void)
{
    if(mqtt_handle != NULL)
    {
        ESP_LOGW(TAG, "MQTT客户端已存在，先销毁旧连接");
        esp_mqtt_client_stop(mqtt_handle);
        esp_mqtt_client_destroy(mqtt_handle);
        mqtt_handle = NULL;
    }

    ESP_LOGI(TAG, "当前可用堆内存: %d bytes", esp_get_free_heap_size());

    esp_mqtt_client_config_t mqtt_config ;
    memset(&mqtt_config, 0, sizeof(esp_mqtt_client_config_t));
    mqtt_config.broker.address.uri = "mqtt://mqtts.heclouds.com:1883";
    mqtt_config.broker.address.port = 1883;

    mqtt_config.credentials.client_id = ONENET_DEVICE_NAME;
    mqtt_config.credentials.username = ONENET_PRODUCT_ID;

    static char token[512];
    dev_token_generate(token, SIG_METHOD_SHA1, 1790855684, ONENET_PRODUCT_ID, ONENET_DEVICE_NAME, ONENET_PRODUCT_ACCESS_KEY);
    mqtt_config.credentials.authentication.password = token;

    ESP_LOGI(TAG, "Generated Token: %s", token);

    mqtt_config.session.keepalive = 120;

    mqtt_handle = esp_mqtt_client_init(&mqtt_config);
    if(mqtt_handle == NULL)
    {
        ESP_LOGE(TAG, "MQTT客户端初始化失败！内存不足？可用堆: %d bytes", esp_get_free_heap_size());
        return ESP_FAIL;
    }

    esp_mqtt_client_register_event(mqtt_handle, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    return esp_mqtt_client_start(mqtt_handle);
}


