#ifndef _OFFLINE_TASK_H
#define _OFFLINE_TASK_H


#ifdef __cplusplus
extern "C" {
#endif

#include "proj_config.h"
#include "fsm.h"
#include "fifo.h"

typedef struct app_obj{
	/* 状态机相关 */
	FSM_T fsm;
	fifo_s_t event_fifo;		  /* 事件触发队列 */
	uint8_t event_fifo_buffer[8]; /* 事件触发缓存 */
	void (*fsm_init_f)(void *this_p, FsmTable_T *, uint16_t, uint8_t);
	void (*fsm_eventHandle_f)(void *this_p);
	void (*fsm_eventUpdate_f)(void *this_p, uint8_t);
}app_obj_t, *app_obj_p;

void *get_obj_p(void);
#ifdef __cplusplus
}
#endif


#endif




