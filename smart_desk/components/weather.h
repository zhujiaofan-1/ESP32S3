/**
 * @file weather.h
 * @brief 天气数据获取接口
 *
 * 提供IP定位和天气预报数据获取功能
 */

#ifndef __WEATHER_H__
#define __WEATHER_H__

/**
 * @brief 天气数据包结构体
 */
typedef struct {
    int high_temp;           /**< 最高温度(℃) */
    int low_temp;            /**< 最低温度(℃) */
    char weather_code[4];    /**< 天气代码 */
}weather_data_pkt_t;

/**
 * @brief 天气图片路径结构体
 */
typedef struct{
    char today_img_path[32];      /**< 今天天气图标路径 */
    char tomorrow_img_path[32];   /**< 明天天气图标路径 */
    char after_img_path[32];      /**< 后天天气图标路径 */
}weather_img_path_t;

/**
 * @brief 启动天气任务
 *
 * 创建天气任务，循环执行IP定位和天气查询，每30分钟更新一次
 */
void weather_start(void);

#endif