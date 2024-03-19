/*
 * @Author: lhq
 * @Date: 2023-07-01 13:00:05
 * @LastEditors: lhq
 * @LastEditTime: 2023-08-03 17:49:37
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\application\fsm_task.c
 * @Description: 
 * 
 * Copyright (c) 2023 by lhq, All Rights Reserved. 
 */
#include "fsm_task.h"

#include "cmsis_os.h"
#include "drv_uart.h"
#include "init.h"
#include "fifo.h"
#include "drv_modbus.h"
#include "drv_e28.h"
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
		
//		if (mbr1->fsm_eventHandle_f != NULL)
//			mbr1->fsm_eventHandle_f(mbr1);



		osDelayUntil(&period, 1);
	}
}
