#include "Audio.h"
#include "driver/i2s_common.h"
#include "driver/i2s_std.h"
#include "driver/i2s_types.h"
#include "esp_log.h"
#include "esp_log_level.h"
#include "hal/i2s_types.h"
#include "driver/i2s_pdm.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "esp_spiffs.h"
#include "esp_log.h"
#include <sys/stat.h>


#define TAG         "Audio"

#define SAMPLE_RATE 48000

static int samples_g;


/**
 * @file Audio.c
 * @brief ESP32-S3 音频驱动实现
 * 
 * 提供 I2S 扬声器输出和 PDM 麦克风输入功能。
 */

//i2s发送和接收句柄
/** I2S 发送通道句柄（扬声器） */
static i2s_chan_handle_t TX_handle = NULL;

/** I2S 接收通道句柄（麦克风） */
static i2s_chan_handle_t RX_handle = NULL;

/**
 * @brief 初始化扬声器 I2S 接口
 * 
 * 配置 I2S 外设使用标准 Philips 模式进行扬声器输出。
 * 设置为 16位单声道音频，使用右声道掩码。
 * 
 * @param BCLK 位时钟 GPIO 引脚编号
 * @param WS 字选择（LRCK）GPIO 引脚编号
 * @param SD 串行数据输出 GPIO 引脚编号
 * @param Sample_rate 音频采样率（Hz）
 */
void Speaker_Init(gpio_num_t BCLK, gpio_num_t WS, gpio_num_t SD, uint32_t Sample_rate)
{
    //创建 I2S TX 通道句柄
    i2s_chan_config_t cha_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_1, I2S_ROLE_MASTER);
    i2s_new_channel(&cha_cfg, &TX_handle, NULL);

    //设定各种参数：时钟、时隙和 GPIO
    i2s_std_config_t i2s_tx_cfg =
    {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(Sample_rate),
        .slot_cfg = I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),

        .gpio_cfg =
        {
            .bclk = BCLK,
            .dout = SD,
            .ws = WS,
            .mclk = -1,
            .din = -1,
        },
    };

    //使用右声道输出单声道音频
    i2s_tx_cfg.slot_cfg.slot_mask = I2S_STD_SLOT_RIGHT;

    //初始化并启用 I2S TX 通道
    i2s_channel_init_std_mode(TX_handle, &i2s_tx_cfg);
    i2s_channel_enable(TX_handle);
}




/**
 * @brief 初始化麦克风 PDM 接口
 * 
 * 配置 I2S 外设使用 PDM 模式进行麦克风输入。
 * 设置为 16位单声道音频，使用右声道掩码。
 * 
 * @param Data PDM 数据输入 GPIO 引脚编号
 * @param CLK PDM 时钟输出 GPIO 引脚编号
 * @param Sample_rate 音频采样率（Hz）
 */
void Microphone_pdm_Init(gpio_num_t Data, gpio_num_t CLK, uint32_t Sample_rate)
{
    //创建 I2S RX 通道句柄
    i2s_chan_config_t cha_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_new_channel(&cha_cfg, NULL, &RX_handle);
    cha_cfg.dma_desc_num = 8; // 增加 DMA 描述符数量
    cha_cfg.dma_frame_num = 1024; // 增加 DMA 帧大小
    //配置 PDM 参数：时钟、时隙和 GPIO
    i2s_pdm_rx_config_t pdm_cfg =
    {   
        .clk_cfg = I2S_PDM_RX_CLK_DEFAULT_CONFIG(Sample_rate),
        .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),

        .gpio_cfg =
        {
            .clk = CLK,
            .din = Data,
        },
    };

    //使用右声道输入单声道音频
    pdm_cfg.slot_cfg.slot_mask = I2S_PDM_SLOT_RIGHT;

    //初始化并启用 I2S RX 通道
    i2s_channel_init_pdm_rx_mode(RX_handle, &pdm_cfg);

    i2s_channel_enable(RX_handle);
}




/**
 * @brief 从麦克风读取音频数据
 * 
 * 从 PDM 麦克风接口读取音频采样数据到提供的缓冲区。
 * 
 * @param Data 存储音频采样数据的缓冲区指针
 * @param samples 要读取的采样数量
 * @return 实际读取的采样数量，出错时返回负值
 */
int Audio_read(int16_t* Data, int samples)
{
    size_t bytes_read = 0;
    esp_err_t ret = i2s_channel_read(RX_handle, Data, samples*2, &bytes_read, 1000);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2S读取失败: %s", esp_err_to_name(ret));
        return -1;
    }

    int samples_read = bytes_read / 2;

    return samples_read;
}




/**
 * @brief 向扬声器写入音频数据
 * 
 * 将音频采样数据写入 I2S 扬声器接口进行播放。
 * 
 * @param Data 包含要播放音频采样数据的缓冲区指针
 * @param samples 要写入的采样数量
 * @return 实际写入的采样数量，出错时返回负值
 */
int Audio_write(const int16_t* Data, int samples)
{
    samples_g = samples;
    size_t bytes_write = 0;
    esp_err_t ret = i2s_channel_write(TX_handle, Data, samples*2, &bytes_write, 1000);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "I2S写入失败: %s", esp_err_to_name(ret));
        return -1;
    }

    int samples_write = bytes_write / 2;

    return samples_write;
}




/**
 * @brief 初始化音频 SPIFFS 文件系统
 * 
 * 挂载 SPIFFS 文件系统用于存储录音文件。
 */
void Audio_spiffs_Init(void)
{
    esp_vfs_spiffs_conf_t spiffs_conf=
    {
        .base_path = "/audio",
        .partition_label = "audio",
        .max_files = 5,
        .format_if_mount_failed = false,
    };
    esp_err_t ret = esp_vfs_spiffs_register(&spiffs_conf);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "SPIFFS挂载失败: %s，尝试格式化...", esp_err_to_name(ret));
        spiffs_conf.format_if_mount_failed = true;
        ret = esp_vfs_spiffs_register(&spiffs_conf);
        if(ret != ESP_OK)
        {
            ESP_LOGE(TAG, "SPIFFS格式化后仍挂载失败: %s", esp_err_to_name(ret));
            return;
        }
        ESP_LOGI(TAG, "SPIFFS格式化并挂载成功");
    }
    else
    {
        ESP_LOGI(TAG, "SPIFFS挂载成功");
    }
}




/**
 * @brief 开始录音
 * 
 * 从 PDM 麦克风读取音频数据，进行音量放大后保存到文件。
 * 
 * @param rec_time 录音时长（秒）
 */
void Start_record(uint32_t rec_time)
{
    ESP_LOGI(TAG, "开始录音，录音时长: %d 秒", rec_time);

    //每次读取的音频数据长度（字）
    const size_t read_size_word = 8192;

    //录音数据总长度(字) = 采样率 * 录音秒数
    const int flash_rec_size = SAMPLE_RATE * rec_time;
    int flash_wr_size = 0;

    FILE* f = fopen("/audio/record.pcm", "w");
    if(!f)
    {
        ESP_LOGE(TAG, "打开录音文件失败");
        return;
    }

    int16_t* i2s_read_buf = (int16_t*)malloc(read_size_word * 2);
    if(i2s_read_buf == NULL)
    {
        ESP_LOGE(TAG, "录音缓冲区分配失败");
        fclose(f);
        return;
    }

    while (flash_wr_size < flash_rec_size)
    {
        int read_word = Audio_read(i2s_read_buf, read_size_word);
        if(read_word > 0)
        {
            //声音放大
            for(int i = 0; i < read_word; i ++)
            {
                i2s_read_buf[i] = i2s_read_buf[i] << 1;
            }
            size_t written = fwrite(i2s_read_buf, read_word * 2, 1, f);
            if(written != 1)
            {
                ESP_LOGE(TAG, "写入文件失败");
                break;
            }
            flash_wr_size += read_word;
        }
        else if(read_word < 0)
        {
            ESP_LOGE(TAG, "麦克风读取错误");
            break;
        }
    }

    free(i2s_read_buf);
    fclose(f);
    ESP_LOGI(TAG, "录音结束，共录制 %d 采样", flash_wr_size);
}




/**
 * @brief 播放录音文件
 * 
 * 从录音文件读取音频数据并播放。
 */
void Play_Audio(void)
{
    const size_t write_size_word = 8192;

    struct stat st;

    if(stat("/audio/record.pcm", &st) != 0)
    {
        ESP_LOGE(TAG, "录音文件不存在");
        return;
    }

    ESP_LOGI(TAG, "录音文件大小：%ld bytes", st.st_size);

    FILE* f = fopen("/audio/record.pcm", "r");
    if(!f)
    {
        ESP_LOGE(TAG, "打开录音文件失败");
        return;
    }

    int16_t *i2s_write_buff = (int16_t *)malloc(write_size_word * 2);
    if(i2s_write_buff == NULL)
    {
        ESP_LOGE(TAG, "播放缓冲区分配失败");
        fclose(f);
        return;
    }

    ESP_LOGI(TAG, "开始播放");

    size_t read_byte = 0;
    while(read_byte < st.st_size)
    {
        size_t to_read = write_size_word * 2;
        if(st.st_size - read_byte < to_read)
        {
            to_read = st.st_size - read_byte;
        }

        size_t actual_read = fread(i2s_write_buff, 1, to_read, f);
        if(actual_read <= 0)
        { 
            ESP_LOGW(TAG, "文件读取结束，已读 %d / %ld bytes", read_byte, st.st_size);
            break;
        }

        int written = Audio_write(i2s_write_buff, actual_read / 2);
        if(written < 0)
        {
            ESP_LOGE(TAG, "扬声器写入失败");
            break;
        }
        read_byte += actual_read;
    }
    
    free(i2s_write_buff);
    fclose(f);
    ESP_LOGI(TAG, "播放完成");
}

