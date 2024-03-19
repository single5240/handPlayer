/*
 * @Author: lhq
 * @Date: 2023-06-19 23:36:23
 * @LastEditors: lhq
 * @LastEditTime: 2023-08-03 16:30:01
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\components\support\proj_config.h
 * @Description: 
 * 
 * Copyright (c) 2023 by lhq, All Rights Reserved. 
 */
#ifndef __PROJ_CONFIG_H__
#define __PROJ_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
	
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
	
#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )

#define DEBUG_RTT 1
#define DEBUG_UART 0

#define DEBUG_LOG_SHELL_METHOD DEBUG_RTT

#ifdef __cplusplus
}
#endif

#endif // __PROJ_CONFIG_H__
