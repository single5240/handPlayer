/*
 * @Author: Vincent_Jiang jwslove40@163.com
 * @Date: 2024-03-19 10:16:12
 * @LastEditors: Vincent_Jiang jwslove40@163.com
 * @LastEditTime: 2024-04-16 20:34:42
 * @FilePath: \handPlayer\hand_robot\components\log\log_config.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __LOG_CONFIG_H__
#define __LOG_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"
#include "drv_uart.h"
#include "SEGGER_RTT.h"
#include "proj_config.h"
#include "usbd_cdc_if.h"

/* config */
#define LOG_OUTPUT_MAX_LEN  512

#define LOG_TIMESTAMP_EN     1
#define LOG_FUNCTION_EN      1
#define LOG_FILE_LINE_EN     1

#define LOG_ASSERT_EN        1
#define LOG_ERROR_EN         1
#define LOG_WARINING_EN      1
#define LOG_INFO_EN          1
#define LOG_DEBUG_EN         1


//#define __log_output(log_str, len) do{SEGGER_SYSVIEW_PrintfTargetEx((const char*)log_str,SEGGER_SYSVIEW_LOG);}while(0)

#if DEBUG_LOG_SHELL_METHOD
#define __log_output(log_str, len) do{SEGGER_RTT_Write(0,(const char*)log_str,len);}while(0)
#else
// #define __log_output(log_str, len) do{uart5_transmit((uint8_t*)log_str, len);}while(0)
#define __log_output(log_str, len) do{CDC_Transmit_FS((uint8_t*)log_str, len);}while(0)

#endif

#ifdef __cplusplus
}
#endif



#endif // __LOG_CONFIG_H__
