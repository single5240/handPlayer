/*
 * @Author: lhq
 * @Date: 2023-07-20 17:41:37
 * @LastEditors: lhq
 * @LastEditTime: 2023-07-20 22:11:05
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\application\offline_task.h
 * @Description: 
 * 
 * Copyright (c) 2023 by lhq, All Rights Reserved. 
 */
#ifndef _OFFLINE_TASK_H
#define _OFFLINE_TASK_H


#ifdef __cplusplus
extern "C" {
#endif

#include "proj_config.h"

typedef enum
{
    MOUDLE_OFFLINE = 0, /*!< system is normal */
    MOUDLE_ONLINE,

} offline_state_e;

typedef struct _offline_module offline_module_t, *offline_module_p;


#pragma pack(push, 1) // 将编译器自己的对齐方式先压入栈保存起来，并告诉编译器下面的就按照1字节对齐
typedef struct _offline_module
{
    void (*offline_event_updata_f)(offline_module_p); 
    uint8_t (*offline_service_f)(offline_module_p, float cycle_time); 
	uint8_t (*offline_get_state_f)(offline_module_p);
	float 	(*offline_get_freq_f)(offline_module_p);
	
	/* offline 条件 */
	float check_gap_time;
	int16_t	min_cnt;
	
	uint16_t ol_cnt;
	uint16_t freq_cnt;
	float	 freq_hz;
	uint8_t state;	
	float freq_time;
	float c_check_time;
	
}offline_module_t,*offline_module_p;

typedef struct _offline_manage_obj
{
	offline_module_t radio;
	offline_module_t mbr1;
	offline_module_t mbr2;
	offline_module_t mbr3;
	offline_module_t io;
}offline_manage_obj_t,*offline_manage_obj_p;


#pragma pack(pop) // 将原来的对齐方式在释放出来。



void Offline_Task(void const * argument);
void offlin_module_init(offline_module_p olm, float check_gap_time, int16_t min_cnt);

void *get_offline_manage(void);

void offline_detector_event_updata(offline_module_p olm);
uint8_t offline_detector_get_state(offline_module_p olm);
float offline_detector_get_freq(offline_module_p olm);



#ifdef __cplusplus
}
#endif


#endif




