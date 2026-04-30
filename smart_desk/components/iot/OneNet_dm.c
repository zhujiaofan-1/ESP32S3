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

#include <string.h>


//ws2812操作句柄
static ws2812_strip_handle_t    ws2812_handle = NULL;


static int LED_brightness = 0;
static bool LED_Status = false;
static int ws2812_red = 0;
static int ws2812_green = 0;
static int ws2812_blue = 0;

static void build_topic(char* buf, size_t len, const char* suffix)
{
    snprintf(buf, len, "$sys/%s/%s/%s", ONENET_PRODUCT_ID, ONENET_DEVICE_NAME, suffix);
}

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
 * @brief 初始化物模型相关外设
 *
 * 初始化WS2812 RGB LED和LEDC PWM调光外设
 *
 * @param 无
 * @return 无
 */
void OneNet_dm_Init(void)
{
    //初始化WS2812接口
    ws2812_init(GPIO_NUM_18, 3, &ws2812_handle);

    //初始化LEDC
    //初始化定时器
    ledc_timer_config_t led_timer =
    {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .clk_cfg =LEDC_AUTO_CLK,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 5000,
        .timer_num = LEDC_TIMER_0,
    };
    ledc_timer_config(&led_timer);

    //初始化PWM
    ledc_channel_config_t led_channel =
    {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = GPIO_NUM_15,
        .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&led_channel);

    //停止之前的渐变
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    //启动渐变
    ledc_fade_func_install(0);
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
                //转换为占空比来控制亮度
                int duty = LED_brightness * 4095 / 100;
                ESP_LOGI("OneNet_dm", "设置占空比: %d", duty);
                ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty, 0);
                
            }else if(strcmp(name_js->string, "LightSwitch") == 0)
            {
                if(cJSON_IsTrue(name_js))
                {
                    LED_Status = true;
                    int duty = LED_brightness * 4095/100;
                    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty, 0);
                    ESP_LOGI("OneNet_dm", "灯已打开，亮度: %d", LED_brightness);
                }else{                  //关灯
                    LED_Status = false;
                    LED_brightness = 0;
                    int duty = 0;
                    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty, 0);
                    ESP_LOGI("OneNet_dm", "灯已关闭");
                }
            }else if(strcmp(name_js->string, "RGBColor") == 0)
            {
                //再次提取数组数据
                ws2812_red = cJSON_GetNumberValue(cJSON_GetObjectItem(name_js, "Red")) ;
                ws2812_green = cJSON_GetNumberValue(cJSON_GetObjectItem(name_js, "Green")) ;
                ws2812_blue = cJSON_GetNumberValue(cJSON_GetObjectItem(name_js, "Blue")) ;

                //设置
                for(int i = 0; i < 3; i++)
                {
                    ws2812_write(ws2812_handle, i, ws2812_red, ws2812_green, ws2812_blue);
                }
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

    //创建根节点
    cJSON* root = cJSON_CreateObject();

    cJSON* params_js = cJSON_CreateObject();

    //填充内容
    cJSON_AddStringToObject(root, "id", "123");
    cJSON_AddStringToObject(root, "version", "1.0");
    //添加对象
    params_js = cJSON_AddObjectToObject(root, "params");
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
    cJSON_AddNumberToObject(color_value_js, "Red", ws2812_red);
    cJSON_AddNumberToObject(color_value_js, "Green", ws2812_green);
    cJSON_AddNumberToObject(color_value_js, "Blue", ws2812_blue);

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
