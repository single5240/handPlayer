#include "app_task.h"
#include "cmsis_os.h"
#include "fsm.h"

#define LOG_TAG "app_task"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

app_obj_t app_main_obj;

/** @enum radio_fsm_state_e
 *  @brief 状态机运行状态
 *
 */
enum {
	STATE_AUTOPARA = 0x00, //

	STATE_READYWAIT,	//
	STATE_ACTIVECTL,  //
	STATE_PASSIVECTL, //

	STATE_EMSTOP, //
	STATE_ONLINESET, //

};

/** @enum radio_trig_event_e
 *  @brief 状态机触发事件
 *
 */
enum {
	EVENT_AUTOPARA_SUCCESS,  //

	EVENT_ACTIVE_START, //
	EVENT_ACTIVE_FINISH,  //

	EVENT_PASSIVE_START, //
	EVENT_PASSIVE_FINISH, //

    EVENT_PARARESET_START, //
	EVENT_PARARESET_FINISH,
	EVENT_EMSTOP, //

	EVENT_QUIT_EMSTOP,

	EVENT_ACTIVE_RUNNING,
	EVENT_PASSIVE_RUNNING,
} ;


/* 动作函数 */
static void fsm_auto_para_callback(void *parm);

static void fsm_active_start_callback(void *parm);

static void fsm_passive_start_callback(void *parm);

static void fsm_active_finish_callback(void *parm);

static void fsm_passive_finish_callback(void *parm);

static void fsm_emstop_callback(void *parm);

static void fsm_para_set_callback(void *parm);

static void fsm_active_run_callback(void *parm);

static void fsm_passive_run_callback(void *parm);

/* 状态函数 */
// static void state_autoparam_proccess(app_obj_p aop);

// static void state_readywait_proccess(app_obj_p aop);

// static void state_activectl_proccess(app_obj_p aop);

// static void state_passivectl_proccess(app_obj_p aop);

// static void state_emstop_proccess(app_obj_p aop);

// static void state_onlineset_proccess(app_obj_p aop);

/* 状态迁移表 */
static FsmTable_T fsm_table[] =
	{
		/* 触发事件         				初态            		动作函数             				次态  	*/
		{EVENT_AUTOPARA_SUCCESS, 		STATE_AUTOPARA, 		fsm_auto_para_callback, 		STATE_READYWAIT},

		{EVENT_ACTIVE_START, 			STATE_READYWAIT, 		fsm_active_start_callback, 		STATE_ACTIVECTL},
		{EVENT_ACTIVE_START, 			STATE_PASSIVECTL, 		fsm_active_start_callback, 		STATE_ACTIVECTL},
		{EVENT_ACTIVE_FINISH, 			STATE_ACTIVECTL, 		fsm_active_finish_callback, 	STATE_READYWAIT},

		{EVENT_PASSIVE_START, 			STATE_READYWAIT, 		fsm_passive_start_callback, 	STATE_PASSIVECTL},
		{EVENT_PASSIVE_START, 			STATE_ACTIVECTL, 		fsm_passive_start_callback, 	STATE_PASSIVECTL},
		{EVENT_PASSIVE_FINISH, 			STATE_PASSIVECTL, 		fsm_passive_finish_callback, 	STATE_READYWAIT},

		{EVENT_PARARESET_START, 		STATE_READYWAIT, 		fsm_para_set_callback, 			STATE_ONLINESET},
		{EVENT_PARARESET_FINISH,		STATE_ONLINESET, 		fsm_para_set_callback, 			STATE_READYWAIT},

		{EVENT_EMSTOP, 					STATE_AUTOPARA, 		fsm_emstop_callback, 			STATE_EMSTOP},
		{EVENT_EMSTOP, 					STATE_READYWAIT, 		fsm_emstop_callback, 			STATE_EMSTOP},
		{EVENT_EMSTOP, 					STATE_ACTIVECTL, 		fsm_emstop_callback, 			STATE_EMSTOP},
		{EVENT_EMSTOP, 					STATE_PASSIVECTL, 		fsm_emstop_callback, 			STATE_EMSTOP},
		{EVENT_EMSTOP, 					STATE_ONLINESET, 		fsm_emstop_callback, 			STATE_EMSTOP},

		{EVENT_QUIT_EMSTOP, 			STATE_EMSTOP, 			fsm_emstop_callback, 			STATE_READYWAIT},

		/*循环函数*/
		{EVENT_ACTIVE_RUNNING, 			STATE_ACTIVECTL, 		fsm_active_run_callback, 		STATE_ACTIVECTL},
		{EVENT_PASSIVE_RUNNING, 		STATE_PASSIVECTL, 		fsm_passive_run_callback, 		STATE_PASSIVECTL},
};
static uint16_t fsm_table_lenth = sizeof(fsm_table) / sizeof(FsmTable_T);

// static void fsm_stateHandle(app_obj_p aop)
// {
// 	switch(aop->fsm.curState){
// 		case STATE_AUTOPARA:
//             state_autoparam_proccess(aop);
//             break;
//         case STATE_READYWAIT:
//             state_readywait_proccess(aop);
//             break;
//         case STATE_ACTIVECTL:
//             state_activectl_proccess(aop);
//             break;
//         case STATE_PASSIVECTL:
//             state_passivectl_proccess(aop);
//             break;
//         case STATE_EMSTOP:
//             state_emstop_proccess(aop);
//             break;
//         case STATE_ONLINESET:
//             state_onlineset_proccess(aop);
//             break;
//         default:
//             break;
// 	}
// }

/****************************** 事件触发函数 ******************************/

/******************************** 动作函数 ********************************/
static void fsm_auto_para_callback(void *parm){
	app_main_obj.fsm_eventUpdate_f(&app_main_obj, EVENT_ACTIVE_START);
	log_i("auto_para_callback");
}
static void fsm_active_start_callback(void *parm){
	app_main_obj.finger_controllers[0]->all_motor_circul_flag = 0;
	app_main_obj.finger_controllers[0]->passive_control_flag = 0;
	memset(app_main_obj.finger_controllers[0]->single_motor_circul_flag, 0, 5);
	app_main_obj.fsm_eventUpdate_f(&app_main_obj, EVENT_ACTIVE_RUNNING);
	log_i("active_start_callback");
}
static void fsm_active_finish_callback(void *parm){
	log_i("active_finish_callback");
}
static void fsm_passive_start_callback(void *parm){
	app_main_obj.finger_controllers[0]->all_motor_circul_flag = 0;
	app_main_obj.finger_controllers[0]->passive_control_flag = 0;
	memset(app_main_obj.finger_controllers[0]->single_motor_circul_flag, 0, 5);
	app_main_obj.fsm_eventUpdate_f(&app_main_obj, EVENT_PASSIVE_RUNNING);
    log_i("passive_start_callback");
}
static void fsm_passive_finish_callback(void *parm){
    log_i("passive_finish_callback");
}
static void fsm_emstop_callback(void *parm){
    log_i("emstop_callback");
}
static void fsm_para_set_callback(void *parm){
    log_i("para_set_callback");
}
static void fsm_active_run_callback(void *parm){
	if(app_main_obj.finger_status->motor_control_type == ACTIVE){
		app_main_obj.fsm_eventUpdate_f(&app_main_obj, EVENT_ACTIVE_RUNNING);
	} else if(app_main_obj.finger_status->motor_control_type == PASSIVE){
		app_main_obj.fsm_eventUpdate_f(&app_main_obj, EVENT_PASSIVE_START);
		return;
	}

	if(app_main_obj.finger_status->motor_control_range == TOTAL){
		all_motor_controller(app_main_obj.finger_status->totaol_status);
	} else if(app_main_obj.finger_status->motor_control_range == SINGLE){
		single_motor_controller(app_main_obj.finger_status->single_status);
	}
	// log_i("active_run_callback");
}

static void fsm_passive_run_callback(void *parm){
	if(app_main_obj.finger_status->motor_control_type == ACTIVE){
		app_main_obj.fsm_eventUpdate_f(&app_main_obj, EVENT_ACTIVE_START);
		return;
	} else if(app_main_obj.finger_status->motor_control_type == PASSIVE){
		app_main_obj.fsm_eventUpdate_f(&app_main_obj, EVENT_PASSIVE_RUNNING);
	}
	passive_controller();
	// log_i("passive_run_callback");
}

// /******************************** 状态函数 ********************************/

// static void state_autoparam_proccess(app_obj_p aop)
// {
// 	aop->fsm_eventUpdate_f(aop, EVENT_AUTOPARA_SUCCESS);
// 	// log_i("state_autoparam_proccess");
// }

// static void state_readywait_proccess(app_obj_p aop)
// {
// 	aop->fsm_eventUpdate_f(aop, EVENT_ACTIVE_START);
// 	//	aop->fsm_eventUpdate_f(aop, EVENT_PASSIVE_START);
// 	// log_i("state_readywait_proccess");
// }

// static void state_activectl_proccess(app_obj_p aop)
// {
// //	aop->fsm_eventUpdate_f(aop, EVENT_ACTIVE_FINISH);
// 	// log_i("state_activectl_proccess");
// }

// static void state_passivectl_proccess(app_obj_p aop)
// {
// //	aop->fsm_eventUpdate_f(aop, EVENT_PASSIVE_FINISH);
// 	// log_i("state_passivectl_proccess");
// }

// static void state_emstop_proccess(app_obj_p aop)
// {
// 	// log_i("state_emstop_proccess");
// }

// static void state_onlineset_proccess(app_obj_p aop)
// {
// 	// log_i("state_onlineset_proccess");
// }

static void fsmUpdateEvent(void *this_p, uint8_t event)
{
	app_obj_p aop;
	if (this_p != NULL)
		aop = this_p;
	else
		log_e("null pointer!");

	/* 触发事件入队 */
	if (fifo_s_isfull(&aop->event_fifo) == 1)
		log_e("fifo is fulled!\r\n");
	else
		fifo_s_put(&aop->event_fifo, event);
}
static void fsmEventHandle(void *this_p)
{
	app_obj_p aop;
	if (this_p != NULL)
		aop = this_p;
	else
		log_e("null pointer!");

	uint8_t event;

	/* 取出触发事件队列中的事件 */
	if (fifo_s_isempty(&aop->event_fifo) != 1)
	{
		event = fifo_s_get(&aop->event_fifo);
		//				log_i("取出触发事件队列中的事件!");

		/* 在其它模块中改变触发事件，即可完成相应动作的执行 */
		FSM_EventHandle(&aop->fsm, event, NULL);
	}
	else
	{
		//				log_e("事件队列为空！");
	}
}

static void fsm_init(void *this_p, FsmTable_T *pTable, uint16_t stuMaxNum, uint8_t curState)
{
	app_obj_p aop;
	if (this_p != NULL)
		aop = this_p;
	else
		log_e("null pointer!");

	/* 创建事件触发队列 */
	fifo_s_init(&aop->event_fifo,
				aop->event_fifo_buffer,
				sizeof(aop->event_fifo_buffer));

	/* 初始化状态机 */
	FSM_Init(&aop->fsm, pTable, stuMaxNum, curState);
}

void aop_fsm_config(app_obj_p aop)
{
	aop->fsm_init_f = fsm_init;
	aop->fsm_eventHandle_f = fsmEventHandle;
	aop->fsm_eventUpdate_f = fsmUpdateEvent;
}


void *get_obj_p(void){
	return &app_main_obj;
}

void jws_test(void){
	app_main_obj.finger_status->motor_control_type = ACTIVE;
	app_main_obj.finger_status->motor_control_range == TOTAL;
	app_main_obj.finger_status->totaol_status = CIRCUL;
}

void AppTask(void const * argument)
{
	osDelay(5);
	led_turn(0,0);
	led_turn(1,0);
	led_turn(2,0);
	led_turn(3,0);
	aop_fsm_config(&app_main_obj);
	app_main_obj.fsm_init_f(&app_main_obj, fsm_table, fsm_table_lenth, STATE_AUTOPARA);
	app_main_obj.fsm_eventUpdate_f(&app_main_obj, EVENT_AUTOPARA_SUCCESS);

	app_main_obj.finger_status = get_finger_status_p();
	app_main_obj.finger_controllers = get_finger_motor_controller_p();
	motor_init();
	motor_reset_positions();
	osDelay(50);
	log_i("App_Task_launch!");
	for(;;) 
	{
		if(app_main_obj.fsm_eventHandle_f != NULL){
			app_main_obj.fsm_eventHandle_f(&app_main_obj);
		} else {
			log_e("null pointer!");
		}
		osDelay(5);
	}
}


