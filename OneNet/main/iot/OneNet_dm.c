#include "OneNet_dm.h"
#include "OneNet_MQTT.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/ledc_types.h"
#include "led_ws2812.h"
#include "driver/gpio.h"
#include "mqtt_client.h"
#include "soc/clk_tree_defs.h"
#include "soc/gpio_num.h"
#include <string.h>
#include "driver/ledc.h"


//ws2812操作句柄
static ws2812_strip_handle_t    ws2812_handle = NULL;


static int LED_brightness = 0;          //LED亮度
static bool LED_Status = false;              //LED开关状态
static int ws2812_red = 0;
static int ws2812_green = 0;
static int ws2812_blue = 0;


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


//下行数据提取
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


//上行数据拼接
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

//OneNet接受数据时需要回应的信息
void OneNet_property_ack(esp_mqtt_client_handle_t mqtt_handle, const char* id, int error_code, const char* msg)
{
    char topic[128];
    snprintf(topic, 128, "$sys/%s/%s/thing/property/set_reply", ONENET_PRODUCT_ID, ONENET_DEVICE_NAME);

    cJSON* replay_js = cJSON_CreateObject();
    cJSON_AddStringToObject(replay_js, "id", id);
    cJSON_AddNumberToObject(replay_js, "code", error_code);
    cJSON_AddStringToObject(replay_js, "msg", msg);

    //设置格式
    char* data = cJSON_PrintUnformatted(replay_js);
    //推送消息
    esp_mqtt_client_publish(mqtt_handle, topic, data, strlen(data), 1, 0);

    cJSON_free(replay_js);
}


//主题订阅函数
void OneNet_subscribe(esp_mqtt_client_handle_t mqtt_handle)
{
    char topic[128];

    //订阅上报属性回复主题
    snprintf(topic, 128, "$sys/%s/%s/thing/property/post/reply", ONENET_PRODUCT_ID, ONENET_DEVICE_NAME);
    esp_mqtt_client_subscribe_single(mqtt_handle, topic, 1);

    //订阅设置主题
    snprintf(topic, 128, "$sys/%s/%s/thing/property/set", ONENET_PRODUCT_ID, ONENET_DEVICE_NAME);
    esp_mqtt_client_subscribe_single(mqtt_handle, topic, 1);
}


//数据上报函数
esp_err_t OneNet_post_property_data(esp_mqtt_client_handle_t mqtt_handle, const char* data)
{
    char topic[128];
    snprintf(topic, 128, "$sys/%s/%s/thing/property/post", ONENET_PRODUCT_ID, ONENET_DEVICE_NAME);
    esp_mqtt_client_subscribe_single(mqtt_handle, topic, 1);

    esp_err_t ret = esp_mqtt_client_publish(mqtt_handle, topic, data, strlen(data), 1, 0);

    if(ret == ESP_OK)
    {
         ESP_LOGI("OneNet_post", "上传主题：%s，数据：%s",topic, data);
    }

    return ret;

}