/**
 * @file WIFI_manager.c
 * @brief WIFI管理模块实现文件
 *
 * 该模块负责ESP32的WIFI STA模式连接管理，包括：
 * - WIFI初始化配置
 * - 连接WIFI热点
 * - 自动重连机制
 * - 连接状态回调通知
 */

#include "WIFI_manager.h"
#include "esp_err.h"
#include "esp_netif_types.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi_types_generic.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TAG         "WIFI_Manager"

//最大重连次数
#define MAX_CONNECT_RETRY        10

static uint8_t Connect_Cnt = 0;

static p_wifi_state_cb WIFI_callback = NULL;

//当前STA连接状态
static bool STA_Connect_State = false;

/**
 * @brief WIFI和IP事件处理函数
 *
 * 该函数作为事件循环的回调函数被调用，处理WIFI事件和IP事件。
 *
 * @param arg 注册事件处理时传入的用户参数（未使用）
 * @param event_base 事件基类，标识事件来源（WIFI_EVENT或IP_EVENT）
 * @param event_id 事件ID，具体的事件类型
 * @param event_data 事件携带的数据指针
 *
 * @note WIFI事件包括：启动、断开连接、连接成功等
 * @note IP事件包括：获取到IP地址
 */
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    //判断事件类型，再做相应操作
    if(event_base == WIFI_EVENT)
    {
        //根据WIFI事件ID进行分支处理
        switch (event_id)
        {
            /**
             * @brief WIFI启动事件
             *
             * 当WIFI STA模式启动成功后触发，此时应立即调用连接函数
             */
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();     /**< 启动成功之后直接启动连接 */
                ESP_LOGI(TAG, "正在连接...");
                break;

            /**
             * @brief WIFI断开连接事件
             *
             * 当WIFI连接断开时触发，此时会：
             * 1. 更新连接状态为断开
             * 2. 通知回调函数
             * 3. 尝试自动重连
             */
            case WIFI_EVENT_STA_DISCONNECTED:
                if(STA_Connect_State == true)
                {
                    //更新连接状态为断开
                    STA_Connect_State = false;

                    //通知外部模块WIFI已断开
                    if(WIFI_callback != NULL)
                    {
                        WIFI_callback(WIFI_STATE_DISCONNECTED);
                    }
                }
                

                //检查是否在最大重连次数限制内
                if(Connect_Cnt <= MAX_CONNECT_RETRY)
                {
                    esp_wifi_connect();  /**< 尝试重新连接 */
                    Connect_Cnt += 1;   /**< 重连计数加1 */
                    ESP_LOGI(TAG, "正在重连...");
                }
                else
                {
                    //超过最大重连次数，记录失败
                    ESP_LOGI(TAG, "重连失败...");
                }
                break;

            /**
             * @brief WIFI连接成功事件
             *
             * 当WIFI成功关联到AP但还未获取IP时触发
             */
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGI(TAG, "WIFI已连接到AP");
                break;

            //未处理的事件，不做任何操作
            default:
                break;
        }
    }
    //IP事件处理
    else if (event_base == IP_EVENT)
    {
        /**
         * @brief 获取到IP地址事件
         *
         * 当DHCP分配IP地址成功后触发，表示WIFI连接完全成功
         */
        if(event_id == IP_EVENT_STA_GOT_IP)     /**< 得到分配的IP */
        {
            //检查回调函数是否注册
            if(WIFI_callback != NULL)
            {
                STA_Connect_State = true;       /**< 标志设为已连接 */
                //获取到IP说明连接成功，直接给回调函数传入连接成功的事件
                WIFI_callback(WIFI_STATE_CONNECTED);
            }

            //打印IP获取成功日志
            ESP_LOGI(TAG, "获取到IP");

            //从事件数据中提取IP地址信息
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

            //打印获取到的IP地址（点分十进制格式）
            ESP_LOGI(TAG, "IP地址: " IPSTR, IP2STR(&event->ip_info.ip));
        }
    }
}

/**
 * @brief 初始化WIFI管理器
 *
 * 该函数完成WIFI STA模式的初始化配置，包括：
 * - 初始化网络接口层（esp_netif）
 * - 创建默认事件循环
 * - 创建默认WIFI STA网络接口
 * - 初始化WIFI配置
 * - 注册事件处理函数
 * - 设置WIFI模式为STA并启动
 *
 * @param f WIFI状态回调函数，当连接状态改变时被调用
 *        - 参数为WIFI_STATE_CONNECTED表示连接成功
 *        - 参数为WIFI_STATE_DISCONNECTED表示连接断开
 *
 * @note 此函数只能调用一次，在使用WIFI_manager_connect之前必须先调用此函数
 * @note 回调函数参数可以为NULL，表示不需要状态通知
 *
 * @par 示例
 * @code
 * void wifi_state_cb(WIFI_STATE state)
 * {
 *     if(state == WIFI_STATE_CONNECTED)
 *         printf("WIFI已连接\n");
 *     else
 *         printf("WIFI已断开\n");
 * }
 *
 * // 初始化并注册回调
 * WIFI_manager_Init(wifi_state_cb);
 *
 * // 连接WIFI
 * WIFI_manager_connect("SSID", "password");
 * @endcode
 */
void WIFI_manager_Init(p_wifi_state_cb f)
{
    WIFI_callback = f;

    //初始化网络接口层
    ESP_ERROR_CHECK(esp_netif_init());

    //创建默认事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //创建默认WIFI STA网络接口
    esp_netif_create_default_wifi_sta();

    //使用默认配置初始化WIFI
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /**
     * @brief 注册WIFI事件处理函数
     *
     * 监听所有WIFI事件（ESP_EVENT_ANY_ID）
     */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    /**
     * @brief 注册IP事件处理函数
     *
     * 专门监听IP获取事件（IP_EVENT_STA_GOT_IP）
     */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        NULL));

    //设置WIFI模式为STA（客户端）
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );

    //启动WIFI
    ESP_ERROR_CHECK(esp_wifi_start() );
}

/**
 * @brief 连接到WIFI热点
 *
 * 该函数配置并连接到指定的WIFI热点（SSID和密码）
 *
 * @param ssid WIFI热点的名称（SSID），最大32个字符
 * @param password WIFI热点的密码，最大64个字符
 *
 * @note 调用此函数前必须先调用WIFI_manager_Init进行初始化
 * @note 如果当前已是STA模式，会直接使用现有连接
 * @note 连接过程是异步的，连接结果会通过回调函数通知
 *
 * @par 安全说明
 * - 支持WPA2-PSK加密方式
 * - 密码为空时使用OPEN模式（不推荐）
 *
 * @par 示例
 * @code
 * // 初始化WIFI管理器
 * WIFI_manager_Init(wifi_state_cb);
 *
 * // 连接到WIFI热点
 * WIFI_manager_connect("MyHomeWiFi", "12345678");
 * @endcode
 *
 * @see WIFI_manager_Init
 */
void WIFI_manager_connect(const char* ssid,const char* password)
{
    wifi_mode_t mode;

    /**
     * @brief WIFI配置结构体
     *
     * 配置STA模式连接到AP的参数
     */
    wifi_config_t wifi_config = {
        .sta = {
            //设置加密模式为WPA2-PSK
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    //将SSID复制到配置结构体
    snprintf((char*)wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), "%s", ssid);

    //将密码复制到配置结构体
    snprintf((char*)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", password);

    //获取当前WIFI模式
    esp_wifi_get_mode(&mode);

    //检查当前模式是否为STA模式，如果不是则重新配置
    if(mode != WIFI_MODE_STA)
    {
        //停止当前WIFI，在切换模式前必须先停止
        esp_wifi_stop();

        //设置为STA模式
        esp_wifi_set_mode(WIFI_MODE_STA);
    }

    //重置重连计数，从0开始
    Connect_Cnt = 0;

    //设置WIFI配置参数，将SSID和密码应用到WIFI接口
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    //启动WIFI并开始连接
    esp_wifi_start();
}
