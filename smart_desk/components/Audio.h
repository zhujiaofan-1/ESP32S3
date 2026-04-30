/**
 * @file Audio.h
 * @brief ESP32-S3 音频驱动接口
 * 
 * 提供 I2S 扬声器输出和 PDM 麦克风输入功能声明。
 */

#ifndef _AUDIO_H
#define _AUDIO_H

#include "driver/gpio.h"
#include "soc/gpio_num.h"
#include <stdint.h>

/**
 * @brief 初始化扬声器 I2S 接口
 * 
 * 配置并初始化 I2S 外设用于扬声器输出，
 * 使用标准 Philips 模式，16位单声道音频。
 * 
 * @param BCLK 位时钟 GPIO 引脚编号
 * @param WS 字选择（LRCK）GPIO 引脚编号
 * @param SD 串行数据输出 GPIO 引脚编号
 * @param Sample_rate 音频采样率（Hz）
 */
// 初始化喇叭
void Speaker_Init(gpio_num_t BCLK, gpio_num_t WS, gpio_num_t SD, uint32_t Sample_rate);

/**
 * @brief 初始化麦克风 PDM 接口
 * 
 * 配置并初始化 I2S 外设用于麦克风输入，
 * 使用 PDM（脉冲密度调制）模式，16位单声道音频。
 * 
 * @param Data PDM 数据输入 GPIO 引脚编号
 * @param CLK PDM 时钟输出 GPIO 引脚编号
 * @param Sample_rate 音频采样率（Hz）
 */
//初始化麦克风
void Microphone_pdm_Init(gpio_num_t Data, gpio_num_t CLK, uint32_t Sample_rate);

/**
 * @brief 从麦克风读取音频数据
 * 
 * 从 PDM 麦克风接口读取音频采样数据。
 * 
 * @param Data 存储音频采样数据的缓冲区指针
 * @param samples 要读取的采样数量
 * @return 实际读取的采样数量，出错时返回负值
 */
//读取音频数据
int Audio_read(int16_t* Data, int samples);

/**
 * @brief 向扬声器写入音频数据
 * 
 * 将音频采样数据写入 I2S 扬声器接口进行播放。
 * 
 * @param Data 包含要播放音频采样数据的缓冲区指针
 * @param samples 要写入的采样数量
 * @return 实际写入的采样数量，出错时返回负值
 */
//写入音频数据
int Audio_write(const int16_t* Data, int samples);

/**
 * @brief 初始化音频 SPIFFS 文件系统
 * 
 * 挂载 SPIFFS 文件系统用于存储录音文件。
 */
void Audio_spiffs_Init(void);

/**
 * @brief 开始录音
 * 
 * 从 PDM 麦克风读取音频数据，进行音量放大后保存到文件。
 * 
 * @param rec_time 录音时长（秒）
 */
void Start_record(uint32_t rec_time);

/**
 * @brief 播放录音文件
 * 
 * 从录音文件读取音频数据并播放。
 */
void Play_Audio(void);

#endif
