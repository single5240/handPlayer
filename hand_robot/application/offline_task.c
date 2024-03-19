#include "offline_task.h"

#include "cmsis_os.h"
#include "init.h"
#include "fifo.h"
#include "event_mgr.h"
#include "drv_led.h"

#define LOG_TAG "offline_task"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

offline_manage_obj_t offline_manage = {0};

static void offline_event_updata(offline_module_p olm)
{
	olm->ol_cnt++;
	olm->freq_cnt++;
}
	
static uint8_t offline_service(offline_module_p olm, float cycle_time)
{
	olm->freq_time += cycle_time;
	olm->c_check_time += cycle_time;
	
	if(olm->check_gap_time != 0 && olm->c_check_time > olm->check_gap_time)
	{
		olm->c_check_time = 0;
		if(olm->ol_cnt > olm->min_cnt)
			olm->state = MOUDLE_ONLINE;
		else
			olm->state = MOUDLE_OFFLINE;
		olm->ol_cnt = 0;
	}
	
	if(olm->freq_time > 1.0f)		//统计频率
	{
		olm->freq_time = 0;
		olm->freq_hz = olm->freq_cnt;
		olm->freq_cnt = 0;
	}
	
	return olm->state;
}

static uint8_t offline_get_state(offline_module_p olm)
{
	return olm->state;
}

static float offline_get_freq(offline_module_p olm)
{
	return olm->freq_hz;
}

void offlin_module_init(offline_module_p olm, float check_gap_time, int16_t min_cnt)
{
	olm->offline_event_updata_f = offline_event_updata;
	olm->offline_get_freq_f = offline_get_freq;
	olm->offline_get_state_f = offline_get_state;
	olm->offline_service_f = offline_service;
	
	olm->min_cnt = min_cnt;
	olm->check_gap_time = check_gap_time;
	
	olm->ol_cnt = 0;
	olm->freq_cnt = 0;
	olm->freq_hz = 0;
	olm->state = MOUDLE_OFFLINE;
	olm->freq_time = 0;
	olm->c_check_time = 0;	
}



void Offline_Task(void const *argument)
{
	offlin_module_init(&offline_manage.radio,0.5f,5);
	offlin_module_init(&offline_manage.mbr1,0.5f,10);
	offlin_module_init(&offline_manage.mbr2,0.5f,10);
	offlin_module_init(&offline_manage.mbr3,0.5f,10);
	offlin_module_init(&offline_manage.io,0.5f,10);
	
	osDelay(500);

	log_i("offline_task_launch!");

	uint32_t period = osKernelSysTick();

	while (1)
	{
//		offline_manage.io.offline_service_f(&offline_manage.io,0.002);
//		offline_manage.mbr1.offline_service_f(&offline_manage.mbr1,0.002);
//		offline_manage.mbr2.offline_service_f(&offline_manage.mbr2,0.002);
//		offline_manage.mbr3.offline_service_f(&offline_manage.mbr3,0.002);
//		offline_manage.radio.offline_service_f(&offline_manage.radio,0.002);
//		
//		radio_match_timeout_check();
		osDelayUntil(&period, 2);
	}
}


void *get_offline_manage(void)
{
	return &offline_manage;
}



void offline_detector_event_updata(offline_module_p olm)
{
	if(olm->offline_event_updata_f != NULL)
		olm->offline_event_updata_f(olm);
}

uint8_t offline_detector_get_state(offline_module_p olm)
{
	if(olm->offline_get_state_f != NULL)
		return olm->offline_get_state_f(olm);
	return MOUDLE_OFFLINE;
}

float offline_detector_get_freq(offline_module_p olm)
{
	if(olm->offline_get_freq_f != NULL)
		return olm->offline_get_freq_f(olm);
	return 0;
}


