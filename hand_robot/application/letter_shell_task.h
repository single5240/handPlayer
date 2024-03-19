/*
 * @Author: lhq
 * @Date: 2023-07-16 12:48:24
 * @LastEditors: lhq
 * @LastEditTime: 2023-07-17 07:04:12
 * @FilePath: \MDK-ARMd:\CUBE\SL_RemoteContorller\application\letter_shell_task.h
 * @Description: 
 * 
 * Copyright (c) 2023 by lhq Author, All Rights Reserved. 
 */
#ifndef _LETTER_SHELL_TASK_H
#define _LETTER_SHELL_TASK_H


#ifdef __cplusplus
extern "C" {
#endif

#include "proj_config.h"

void Shell_Task(void const * argument);

void shell_interupt(uint8_t *buff, uint16_t len);



#ifdef __cplusplus
}
#endif


#endif




