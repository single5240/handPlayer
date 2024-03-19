/*
 * @Author: lhq
 * @Date: 2023-07-01 13:00:05
 * @LastEditors: Vincent_Jiang jwslove40@163.com
 * @LastEditTime: 2024-03-19 10:45:52
 * @FilePath: \handPlayer\hand_robot\application\fsm_task.c
 * @Description: 
 * 
 * Copyright (c) 2023 by lhq, All Rights Reserved. 
 */
#include "fsm_task.h"

#include "cmsis_os.h"
#include "drv_uart.h"
#include "init.h"
#include "fifo.h"
#include "event_mgr.h"

#define LOG_TAG "fsm_task"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

float sin_test;
void Fsm_Task(void const *argument)
{

	log_i("Fsm_Task_launch!");

	uint32_t period = osKernelSysTick();

	while (1)
	{
		
		osDelayUntil(&period, 1);
	}
}
