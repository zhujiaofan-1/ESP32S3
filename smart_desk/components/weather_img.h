/**
 * @file weather_img.h
 * @brief 天气图片SPIFFS挂载接口
 */

#ifndef _WEATHER_IMG_H
#define _WEATHER_IMG_H

/**
 * @brief 初始化天气图片SPIFFS分区
 *
 * 挂载img分区，用于存放天气图标等图片资源
 */
void img_spiffs_Init(void);

#endif
