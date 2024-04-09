/*
 * @Author: Vincent_Jiang jwslove40@163.com
 * @Date: 2024-03-20 09:32:23
 * @LastEditors: Vincent_Jiang jwslove40@163.com
 * @LastEditTime: 2024-04-09 11:45:39
 * @FilePath: \handPlayer\hand_robot\application\app_task.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef _OFFLINE_TASK_H
#define _OFFLINE_TASK_H


#ifdef __cplusplus
extern "C" {
#endif

#include "proj_config.h"
#include "fsm.h"
#include "fifo.h"
#include "drv_motor.h"

typedef struct app_obj{
	/* 状态机相关 */
	FSM_T fsm;
	fifo_s_t event_fifo;		  /* 事件触发队列 */
	uint8_t event_fifo_buffer[8]; /* 事件触发缓存 */
	void (*fsm_init_f)(void *this_p, FsmTable_T *, uint16_t, uint8_t);
	void (*fsm_eventHandle_f)(void *this_p);
	void (*fsm_eventUpdate_f)(void *this_p, uint8_t);
    
	motor_controller_t (*finger_controllers)[5];
}app_obj_t, *app_obj_p;

void *get_obj_p(void);
#ifdef __cplusplus
}
#endif


#endif




