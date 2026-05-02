/**
 * @file OneNet_dm.c
 * @brief OneNet设备物模型实现文件
 *
 * 实现OneNet物模型的数据解析、属性上报、指令回复和主题订阅功能
 */

#include "OneNet_dm.h"

/*============================ ESP-IDF 头文件 ============================*/
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "mqtt_client.h"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"
#include "hal/ledc_types.h"

/*============================ 项目头文件 ============================*/
#include "OneNet_MQTT.h"
#include "led_ws2812.h"
#include "cJSON.h"
#include "led_control.h"

#include <string.h>



static bool LED_Status = false;

extern int RGB_red;
extern int RGB_green;
extern int RGB_blue;
extern int LED_brightness;
extern int RGB_brightness;

/**
 * @brief 构建OneNet物模型主题
 *
 * 拼接格式：$sys/{产品ID}/{设备名}/{后缀}
 *
 * @param buf 输出主题缓冲区
 * @param len 缓冲区大小
 * @param suffix 主题后缀
 */
static void build_topic(char* buf, size_t len, const char* suffix)
{
    snprintf(buf, len, "$sys/%s/%s/%s", ONENET_PRODUCT_ID, ONENET_DEVICE_NAME, suffix);
}

/**
 * @brief 发送指令回复消息
 *
 * 构建回复JSON并通过MQTT发布到指定主题
 *
 * @param mqtt_handle MQTT客户端句柄
 * @param topic_suffix 主题后缀
 * @param id 指令ID
 * @param error_code 错误码
 * @param msg 回复消息
 */
static void send_ack(esp_mqtt_client_handle_t mqtt_handle, const char* topic_suffix, const char* id, int error_code, const char* msg)
{
    char topic[128];
    build_topic(topic, sizeof(topic), topic_suffix);

    cJSON* replay_js = cJSON_CreateObject();
    cJSON_AddStringToObject(replay_js, "id", id);
    cJSON_AddNumberToObject(replay_js, "code", error_code);
    cJSON_AddStringToObject(replay_js, "msg", msg);

    char* data = cJSON_PrintUnformatted(replay_js);
    esp_mqtt_client_publish(mqtt_handle, topic, data, strlen(data), 1, 0);

    cJSON_free(data);
    cJSON_Delete(replay_js);
}




/**
 * @brief 处理OneNet下发的属性设置数据
 *
 * 解析MQTT下行JSON数据，提取Brightness、LightSwitch、RGBColor属性
 * 并控制对应的LED硬件
 *
 * @param property cJSON格式的JSON数据对象
 * @return 无
 */
void OneNet_property_handle(cJSON* property)
{
    //解析JSON数据
    /*
    {
        "id": "123",
        "version": "1.0",
        "params": {
            "Brightness":"30"
            "LightSwitch":"true"
            "RGBColor":{
                "Red":100,
                "Green":100,
                "Blue":100,
            }
        }
    }
    */

    
    cJSON* param_js = cJSON_GetObjectItem(property, "params");

    //遍历提取数据
    if(param_js)
    {
        cJSON* name_js = param_js->child;       //更深一层
        while (name_js)
        {
            /* code */
            if(strcmp(name_js->string, "Brightness") == 0)
            {
                LED_brightness = (int)cJSON_GetNumberValue(name_js);
                ESP_LOGI("OneNet_dm", "收到亮度设置: %d", LED_brightness);
                //控制亮度
                light_control(LED_brightness);
                //更新UI上的slider
                update_slider_values();
                
            }else if(strcmp(name_js->string, "LightSwitch") == 0)
            {
                if(cJSON_IsTrue(name_js))
                {
                    LED_Status = true;
                    light_control(LED_brightness);
                    ESP_LOGI("OneNet_dm", "灯已打开，亮度: %d", LED_brightness);
                }else{                  //关灯
                    LED_Status = false;
                    LED_brightness = 0;
                    light_control(LED_brightness);
                    ESP_LOGI("OneNet_dm", "灯已关闭");
                }
                //更新UI上的slider
                update_slider_values();
            }else if(strcmp(name_js->string, "RGBColor") == 0)
            {
                //再次提取数组数据
                RGB_red = cJSON_GetNumberValue(cJSON_GetObjectItem(name_js, "Red")) ;
                RGB_green = cJSON_GetNumberValue(cJSON_GetObjectItem(name_js, "Green")) ;
                RGB_blue = cJSON_GetNumberValue(cJSON_GetObjectItem(name_js, "Blue")) ;

                //设置
                RGB_control(RGB_brightness, RGB_red, RGB_green, RGB_blue);
                //更新UI上的slider
                update_slider_values();
            }

            //下一个对象
            name_js = name_js->next;
        }
        
    }
   
}



/**
 * @brief 拼接设备属性上报数据
 *
 * 将当前设备状态（亮度、开关、RGB值）组装成JSON格式用于上报到OneNet平台
 *
 * @param 无
 * @return cJSON* 返回组装好的JSON对象指针
 */
cJSON* OneNet_property_upload(void)
{
    /*
    {
        "id": "123",
        "version": "1.0",
        "params": {
            "Brightness":{
                "value":"50"
            },
            "LightSwitch":{
                "value":"true"
            },
            "RGBColor":{
                "value":{
                    "Red":100,
                    "Green":100,
                    "Blue":100,
                }
            }
        }
    }
    */

    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "id", "123");
    cJSON_AddStringToObject(root, "version", "1.0");
    cJSON* params_js = cJSON_AddObjectToObject(root, "params");
    //添加属性
    //亮度
    cJSON* Brightness_js = cJSON_AddObjectToObject(params_js,"Brightness");
    cJSON_AddNumberToObject(Brightness_js, "value", LED_brightness);
    //开关
    cJSON* LightSwitch_js = cJSON_AddObjectToObject(params_js,"LightSwitch");
    cJSON_AddBoolToObject(LightSwitch_js, "value", LED_Status);
    //RGB值
    cJSON* RGBColor_js = cJSON_AddObjectToObject(params_js,"RGBColor");
    cJSON* color_value_js = cJSON_AddObjectToObject(RGBColor_js,"value");
    cJSON_AddNumberToObject(color_value_js, "Red", RGB_red);
    cJSON_AddNumberToObject(color_value_js, "Green", RGB_green);
    cJSON_AddNumberToObject(color_value_js, "Blue", RGB_blue);

    return root;
}



/**
 * @brief 向OneNet平台回复属性设置结果
 *
 * 当接收到设备属性设置指令后，向平台发送确认回复
 *
 * @param mqtt_handle MQTT客户端句柄
 * @param id 指令ID，用于匹配回复对应的设置请求
 * @param error_code 错误码，200表示成功
 * @param msg 回复消息内容
 * @return 无
 */
void OneNet_property_ack(esp_mqtt_client_handle_t mqtt_handle, const char* id, int error_code, const char* msg)
{
    send_ack(mqtt_handle, "thing/property/set_reply", id, error_code, msg);
}



/**
 * @brief 向OneNet平台回复OTA通知结果
 *
 * 当接收到设备OTA升级指令后，向平台发送确认回复
 *
 * @param mqtt_handle MQTT客户端句柄
 * @param id 指令ID，用于匹配回复对应的设置请求
 * @param error_code 错误码，200表示成功
 * @param msg 回复消息内容
 * @return 无
 */
void OneNet_ota_ack(esp_mqtt_client_handle_t mqtt_handle, const char* id, int error_code, const char* msg)
{
    send_ack(mqtt_handle, "ota/inform_reply", id, error_code, msg);
}



/**
 * @brief 订阅OneNet物模型相关主题
 *
 * 订阅属性上报回复主题和属性设置主题，用于接收平台指令
 *
 * @param mqtt_handle MQTT客户端句柄
 * @return 无
 */
void OneNet_subscribe(esp_mqtt_client_handle_t mqtt_handle)
{
    char topic[128];

    build_topic(topic, sizeof(topic), "thing/property/post/reply");
    esp_mqtt_client_subscribe_single(mqtt_handle, topic, 1);

    build_topic(topic, sizeof(topic), "thing/property/set");
    esp_mqtt_client_subscribe_single(mqtt_handle, topic, 1);

    build_topic(topic, sizeof(topic), "ota/inform");
    esp_mqtt_client_subscribe_single(mqtt_handle, topic, 1);
}



/**
 * @brief 向OneNet平台上报设备属性数据
 *
 * 将设备当前属性数据发布到OneNet平台的属性上报主题
 *
 * @param mqtt_handle MQTT客户端句柄
 * @param data JSON格式的属性数据字符串
 * @return esp_err_t ESP_OK表示成功，其他值表示失败
 */
esp_err_t OneNet_post_property_data(esp_mqtt_client_handle_t mqtt_handle, const char* data)
{
    char topic[128];
    build_topic(topic, sizeof(topic), "thing/property/post");
    esp_mqtt_client_subscribe_single(mqtt_handle, topic, 1);

    esp_err_t ret = esp_mqtt_client_publish(mqtt_handle, topic, data, strlen(data), 1, 0);

    if(ret == ESP_OK)
    {
         ESP_LOGI("OneNet_post", "上传主题：%s，数据：%s",topic, data);
    }

    return ret;
}
