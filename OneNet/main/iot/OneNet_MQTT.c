#include "OneNet_MQTT.h"
#include "cJSON.h"
#include "esp_event_base.h"
#include "onenet_token.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "OneNet_dm.h"


#define TAG     "OneNet"

//存储mqtt初始化返回句柄，用于后续操作
static esp_mqtt_client_handle_t mqtt_handle = NULL;


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        //连接成功时订阅主题
        OneNet_subscribe(mqtt_handle);

        //订阅之后要上报所有数据,为了数据同步
        cJSON* property_js = OneNet_property_upload();
        char* data = cJSON_PrintUnformatted(property_js);

        OneNet_post_property_data(mqtt_handle, data);

        cJSON_free(property_js);
        cJSON_free(data);
      
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

        //判断下行数据主题,是否包含   property/set
        if(strstr(event->topic, "property/set") != 0)
        {
            //生成CJSO对象
            cJSON* property_js = cJSON_Parse(event->data);
            
            OneNet_property_handle(property_js);
            //取出下发id，用于回应数据
            cJSON* id_js = cJSON_GetObjectItem(property_js, "id");
            OneNet_property_ack(mqtt_handle, cJSON_GetStringValue(id_js), 200, "success");

            cJSON_free(property_js);
        }

        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}





esp_err_t OneNet_Start(void)
{
    //初始化mqtt
    esp_mqtt_client_config_t mqtt_config ;
    memset(&mqtt_config, 0, sizeof(esp_mqtt_client_config_t));
    //填充初始化内容
    mqtt_config.broker.address.uri = "mqtt://mqtts.heclouds.com:1883";
    mqtt_config.broker.address.port = 1883;

    //验证信息
    mqtt_config.credentials.client_id = ONENET_DEVICE_NAME;
    mqtt_config.credentials.username = ONENET_PRODUCT_ID;

    //调用函数计算token
    static char token[512];
    dev_token_generate(token, SIG_METHOD_SHA1, 1790855684, ONENET_PRODUCT_ID, ONENET_DEVICE_NAME, ONENET_PRODUCT_ACCESS_KEY);
    mqtt_config.credentials.authentication.password = token;

    ESP_LOGI(TAG, "Generated Token: %s", token);

    //添加 keepalive 防止服务器超时断开
    mqtt_config.session.keepalive = 120;

    mqtt_handle = esp_mqtt_client_init(&mqtt_config);

    //事件注册
    esp_mqtt_client_register_event(mqtt_handle, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    //启动mqtt连接
    return esp_mqtt_client_start(mqtt_handle);
}


