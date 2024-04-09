/*
 * @Author: Vincent_Jiang jwslove40@163.com
 * @Date: 2024-03-19 10:16:12
 * @LastEditors: Vincent_Jiang jwslove40@163.com
 * @LastEditTime: 2024-04-08 18:12:40
 * @FilePath: \hand_robot\application\fsm_task.c
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include "comm_task.h"

#include "cmsis_os.h"
#include "init.h"

#define LOG_TAG "comm_task"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

void Comm_Task(void const *argument)
{
	uint32_t period = osKernelSysTick();
	while (1)
	{
		
		osDelayUntil(&period, 1);
	}
}



