/**
 * @file onenet_token.h
 * @brief OneNet设备Token生成接口
 *
 * 提供Base64编解码和OneNet设备认证Token生成功能
 */

#ifndef _ONENET_TOKEN_H_
#define _ONENET_TOKEN_H_
#include <stdint.h>

/**
 * @brief Base64编码转义模式枚举
 */
enum Escaped {
        WC_STD_ENC = 0,       /**< 标准换行编码 */
        WC_ESC_NL_ENC,        /**< 使用转义序列编码 */
        WC_NO_NL_ENC          /**< 不编码 */
};


#ifndef byte
typedef unsigned char  byte;
#endif
typedef unsigned short word16;
typedef unsigned int   word32;
typedef byte           word24[3]; 	
	
/**
 * @brief Base64解码
 *
 * @param in 输入Base64编码数据
 * @param inLen 输入数据长度
 * @param out 输出解码数据缓冲区
 * @param outLen 输出数据长度（输入输出参数）
 * @return int 0成功，其他失败
 */
int Base64_Decode(const byte* in, word32 inLen, byte* out,word32* outLen);

/**
 * @brief Base64标准编码
 *
 * @param in 输入原始数据
 * @param inLen 输入数据长度
 * @param out 输出Base64编码缓冲区
 * @param outLen 输出数据长度（输入输出参数）
 * @return int 0成功，其他失败
 */
int Base64_Encode(const byte* in, word32 inLen, byte* out,word32* outLen);

/**
 * @brief Base64转义编码
 *
 * @param in 输入原始数据
 * @param inLen 输入数据长度
 * @param out 输出Base64编码缓冲区
 * @param outLen 输出数据长度（输入输出参数）
 * @return int 0成功，其他失败
 */
int Base64_EncodeEsc(const byte* in, word32 inLen, byte* out,word32* outLen);

/**
 * @brief Base64无换行编码
 *
 * @param in 输入原始数据
 * @param inLen 输入数据长度
 * @param out 输出Base64编码缓冲区
 * @param outLen 输出数据长度（输入输出参数）
 * @return int 0成功，其他失败
 */
int Base64_Encode_NoNl(const byte* in, word32 inLen, byte* out,word32* outLen);

/**
 * @brief 签名算法枚举
 */
enum sig_method_e
{
    SIG_METHOD_MD5,       /**< MD5签名 */
    SIG_METHOD_SHA1,      /**< SHA1签名 */
    SIG_METHOD_SHA256     /**< SHA256签名 */
};

/**
 * @brief 生成OneNet设备认证Token
 *
 * 根据产品ID、设备名和访问密钥生成用于MQTT认证的Token
 *
 * @param token 输出Token缓冲区
 * @param method 签名算法
 * @param exp_time Token过期时间戳
 * @param product_id 产品ID
 * @param dev_name 设备名称
 * @param access_key 产品访问密钥
 * @return int32_t 0成功，其他失败
 */
int32_t
dev_token_generate(char* token, enum sig_method_e method, uint32_t exp_time, const char* product_id, const char* dev_name, const char* access_key);


#endif
