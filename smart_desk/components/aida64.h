/**
 * @file aida64.h
 * @brief AIDA64硬件监控数据获取接口
 *
 * 通过SSE连接AIDA64远程传感器服务，获取CPU、内存等硬件参数
 */

#ifndef _AIDA64_H_
#define _AIDA64_H_

#include <stdbool.h>

/**
 * @brief 硬件参数结构体
 */
typedef struct 
{
    int cpu_used;       /**< CPU使用率(%) */
    int cpu_temp;       /**< CPU温度(℃) */
    int mem_used;       /**< 内存使用率(%) */
    int mem_valid;      /**< 可用内存大小(MB) */
}aida64_data_t;

/**
 * @brief 启动AIDA64监控
 * 
 * @param ip 服务器IP地址
 */
void aida64_monitor_start(const char* ip);

/**
 * @brief 停止AIDA64监控
 */
void aida64_monitor_stop(void);

/**
 * @brief 获取AIDA64连接状态
 * 
 * @return true 已连接
 * @return false 未连接
 */
bool aida64_monitor_isconnect(void);

#endif
