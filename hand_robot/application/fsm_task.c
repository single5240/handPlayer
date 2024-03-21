/*
 * @Author: Vincent_Jiang jwslove40@163.com
 * @Date: 2024-03-19 10:16:12
 * @LastEditors: Vincent_Jiang jwslove40@163.com
 * @LastEditTime: 2024-03-20 14:35:08
 * @FilePath: \handPlayer\hand_robot\application\fsm_task.c
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include "fsm_task.h"

#include "fsm.h"
#include "cmsis_os.h"
#include "drv_uart.h"
#include "init.h"
#include "fifo.h"
#include "event_mgr.h"
#include "app_task.h"

#define LOG_TAG "fsm_task"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

void Fsm_Task(void const *argument)
{
	uint32_t period = osKernelSysTick();
	app_obj_p app_main_temp_p = (app_obj_p) get_obj_p();
	log_i("Fsm_Task_launch!");
	while (1)
	{
		if(app_main_temp_p->fsm_eventHandle_f != NULL){
			app_main_temp_p->fsm_eventHandle_f(app_main_temp_p);
		} else {
			log_e("null pointer!");
		}
		osDelayUntil(&period, 1);
	}
}



