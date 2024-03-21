/*
 * @Author: lhq
 * @Date: 2023-07-13 14:08:53
 * @LastEditors: lhq
 * @LastEditTime: 2023-08-09 17:29:45
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\application\init.h
 * @Description:
 *
 * Copyright (c) 2023 by lhq, All Rights Reserved.
 */
#ifndef __INIT_H__
#define __INIT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "proj_config.h"

#include "SEGGER_SYSVIEW.h"
#include "SEGGER_RTT.h"

void board_config(void);

uint32_t get_time_us(void);
uint32_t get_time_ms(void);
float get_time_ms_us(void);

	uint32_t usart6_rx_callback(uint8_t *buff, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // __INIT_H__
