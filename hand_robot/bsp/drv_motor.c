#include "drv_motor.h"
#include "my_pid.h"
#include "main.h"
#include "drv_comm.h"

#define LOG_TAG "drv_motor"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
// motor data read
#define get_motor_measure(ptr, data)                                   \
	{                                                                  \
		(ptr)->last_ecd = (ptr)->ecd;                                  \
		(ptr)->ecd = (uint16_t)((data)[0] << 8 | (data)[1]);           \
		(ptr)->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);     \
		(ptr)->given_current = (uint16_t)((data)[4] << 8 | (data)[5]); \
		(ptr)->temperate = (data)[6];                                  \
	}

motor_controller_t motor_controllers[5];

static CAN_TxHeaderTypeDef chassis_tx_message;
static uint8_t chassis_can_send_data[8];

void ws_cla_motor(motor_measure_t *motor_m_t)
{
	if (motor_m_t->ecd > 7500 && motor_m_t->last_ecd < 1000 && motor_m_t->speed_rpm < 0)
	{
		motor_m_t->rolls--;
	}
	else if (motor_m_t->ecd < 1000 && motor_m_t->last_ecd > 7500 && motor_m_t->speed_rpm > 0)
	{
		motor_m_t->rolls++;
	}
	motor_m_t->last_angle = motor_m_t->actual_angle;
	motor_m_t->actual_angle = (float)(motor_m_t->rolls * 360 / 36) + ((float)(motor_m_t->ecd))/8192.0f;
	// motor_m_t->actual_angle = (motor_m_t->rolls * 360 / 36);
	motor_m_t->totall_ecd = motor_m_t->rolls * 8192 + motor_m_t->ecd;
}

/**
 * @brief          hal CAN fifo call back, receive motor data
 * @param[in]      hcan, the point to CAN handle
 * @retval         none
 */
/**
 * @brief          hal库CAN回调函数,接收电机数据
 * @param[in]      hcan:CAN句柄指针
 * @retval         none
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef rx_header;
	uint8_t rx_data[8];

	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);

	switch (rx_header.StdId)
	{
	case FINGER0_MOTOR_ID:
	case FINGER1_MOTOR_ID:
	case FINGER2_MOTOR_ID:
	case FINGER3_MOTOR_ID:
	case FINGER4_MOTOR_ID:
	{
		get_motor_measure(&(motor_controllers[rx_header.StdId - CAN_FINGER_ALL_ID - 1].motor_measure), rx_data);
		ws_cla_motor(&(motor_controllers[rx_header.StdId - CAN_FINGER_ALL_ID - 1].motor_measure));
		break;
	}
	default:
	{
		log_i("Can rx Error!!!\r\n");
		break;
	}
	}
}

void CAN_cmd_chassis(uint32_t StdId, int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
	uint32_t send_mail_box;
	chassis_tx_message.StdId = StdId;
	chassis_tx_message.IDE = CAN_ID_STD;
	chassis_tx_message.RTR = CAN_RTR_DATA;
	chassis_tx_message.DLC = 0x08;
	chassis_can_send_data[0] = motor1 >> 8;
	chassis_can_send_data[1] = motor1;
	chassis_can_send_data[2] = motor2 >> 8;
	chassis_can_send_data[3] = motor2;
	chassis_can_send_data[4] = motor3 >> 8;
	chassis_can_send_data[5] = motor3;
	chassis_can_send_data[6] = motor4 >> 8;
	chassis_can_send_data[7] = motor4;

	HAL_CAN_AddTxMessage(&FINGER_CAN, &chassis_tx_message, chassis_can_send_data, &send_mail_box);
}

uint8_t motor_reset_positions(void)
{
	int icount[5] = {20, 20, 20, 20, 20};
	int last_roll[5] = {10, 10, 10, 10, 10};
	uint8_t done_flag[5] = {0};
	int done = 5;
	motor_controllers[0].motor_measure.set_speed = -1500;
	motor_controllers[1].motor_measure.set_speed = 1100;
	motor_controllers[2].motor_measure.set_speed = 1100;
	motor_controllers[3].motor_measure.set_speed = -1100;
	motor_controllers[4].motor_measure.set_speed = -1100;
	while (done != 0)
	{
		for (int i = 0; i < 5; i++)
		{
			if (icount[i] > 0)
			{
				motor_controllers[i].motor_measure.set_current = PidCalculate(&motor_controllers[i].speed_pid, motor_controllers[i].motor_measure.set_speed, motor_controllers[i].motor_measure.speed_rpm);
				if (motor_controllers[i].motor_measure.rolls == last_roll[i])
				{
					icount[i]--;
					if (icount[i] == 0 && done_flag[i] == 0)
					{
						done_flag[i] = 1;
						done--;
						if(done == 0){
							motor_controllers[i].motor_measure.set_current = 0;
							motor_controllers[i].motor_measure.rolls = 0;
						}
					}
				}
				last_roll[i] = motor_controllers[i].motor_measure.rolls;
			}
			else
			{
				motor_controllers[i].motor_measure.set_current = 0;
				motor_controllers[i].motor_measure.rolls = 0;
			}
		}
		CAN_cmd_chassis(FINGER0_3_MOTOR_ID, motor_controllers[0].motor_measure.set_current, motor_controllers[1].motor_measure.set_current,
						motor_controllers[2].motor_measure.set_current, motor_controllers[3].motor_measure.set_current);
		HAL_Delay(10);
		CAN_cmd_chassis(FINGER4_4_MOTOR_ID, motor_controllers[4].motor_measure.set_current, 0, 0, 0);
	}
	CAN_cmd_chassis(FINGER0_3_MOTOR_ID, 0, 0, 0, 0);
	CAN_cmd_chassis(FINGER4_4_MOTOR_ID, 0, 0, 0, 0);

	return 0;
}

void motor_pid_init(void)
{
	PidInit(&(motor_controllers[0].speed_pid), 		7, 0.004, 0.01,   3500, 300, 200);
	PidInit(&(motor_controllers[0].position_pid), 0.1, 		0,  0.00,  600,   0,  50);

	PidInit(&(motor_controllers[1].speed_pid),      7,  0.004,  0.01, 3000, 300, 200);
	PidInit(&(motor_controllers[1].position_pid), 0.1,      0,  0.00,  500,   0,  50);

	PidInit(&(motor_controllers[2].speed_pid),      7,  0.004,  0.01, 3000, 300, 200);
	PidInit(&(motor_controllers[2].position_pid), 0.1,      0,  0.00,  500,   0,  50);

	PidInit(&(motor_controllers[3].speed_pid),      7,  0.004,  0.01, 3000, 300, 200);
	PidInit(&(motor_controllers[3].position_pid), 0.1,      0,  0.00,  500,   0,  50);

	PidInit(&(motor_controllers[4].speed_pid),      7,  0.004,  0.01, 3500, 300, 200);
	PidInit(&(motor_controllers[4].position_pid), 0.1,      0,  0.00,  600,   0,  50);
}

void motor_init(void){
	motor_pid_init();
	motor_controllers[0].circul_end = 170;  
	motor_controllers[1].circul_end = -40;
	motor_controllers[2].circul_end = -75;
	motor_controllers[3].circul_end = 90;
	motor_controllers[4].circul_end = 100;
}

motor_controller_t (*get_finger_motor_controller_p(void))[5]
{
	return &motor_controllers;
}

#include "main.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

void can_filter_init(void)
{

	CAN_FilterTypeDef can_filter_st;
	can_filter_st.FilterActivation = ENABLE;
	can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
	can_filter_st.FilterIdHigh = 0x0000;
	can_filter_st.FilterIdLow = 0x0000;
	can_filter_st.FilterMaskIdHigh = 0x0000;
	can_filter_st.FilterMaskIdLow = 0x0000;
	can_filter_st.FilterBank = 0;
	can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
	HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

	can_filter_st.SlaveStartFilterBank = 14;
	can_filter_st.FilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);
	HAL_CAN_Start(&hcan2);
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void all_motor_control_circul(void)
{
	switch (motor_controllers->all_motor_circul_flag)
	{
	case 0:
	{
		motor_controllers[0].motor_measure.set_totall_ecd = (int32_t)motor_controllers[0].circul_end * 819;
		motor_controllers[1].motor_measure.set_totall_ecd = (int32_t)motor_controllers[1].circul_end * 819;
		motor_controllers[2].motor_measure.set_totall_ecd = (int32_t)motor_controllers[2].circul_end * 819;
		motor_controllers[3].motor_measure.set_totall_ecd = (int32_t)motor_controllers[3].circul_end * 819;
		motor_controllers[4].motor_measure.set_totall_ecd = (int32_t)motor_controllers[4].circul_end * 819;
		motor_controllers->all_motor_circul_flag = 1;
		log_i("control_flag:1\r\n");
		break;
	}
	case 1:
	{
		// if (motor_controllers[0].motor_measure.actual_angle <= -90 
		// 			&& motor_controllers[1].motor_measure.actual_angle <= -90 
		// 			&& motor_controllers[2].motor_measure.actual_angle <= -90 
		// 			&& motor_controllers[3].motor_measure.actual_angle >= 90 
		// 			&& motor_controllers[4].motor_measure.actual_angle <= -90)
		// {
		// 	motor_controllers->all_motor_circul_flag = 2;
		// 	log_i("control_flag:2\r\n");
		// }
		if(motor_controllers[0].motor_measure.actual_angle >= motor_controllers[3].circul_end - 10
					&& motor_controllers[1].motor_measure.actual_angle <= motor_controllers[1].circul_end + 3
					&& motor_controllers[2].motor_measure.actual_angle <= motor_controllers[2].circul_end + 3
					&& motor_controllers[3].motor_measure.actual_angle >= motor_controllers[3].circul_end -10
					&& motor_controllers[4].motor_measure.actual_angle >= motor_controllers[4].circul_end -12){
			motor_controllers->all_motor_circul_flag = 2;
			log_i("control_flag:2\r\n");
		}
		break;
	}
	case 2:
	{
		motor_controllers[0].motor_measure.set_totall_ecd = 4*8192;
		motor_controllers[1].motor_measure.set_totall_ecd = 8192;
		motor_controllers[2].motor_measure.set_totall_ecd = 8192;
		motor_controllers[3].motor_measure.set_totall_ecd = -8192;
		motor_controllers[4].motor_measure.set_totall_ecd = 0;
		motor_controllers->all_motor_circul_flag = 3;
		log_i("control_flag:3\r\n");
		break;
	}
	case 3:
	{
		// if (motor_controllers[0].motor_measure.actual_angle >= 0 
		// 			&& motor_controllers[1].motor_measure.actual_angle >= 0 
		// 			&& motor_controllers[2].motor_measure.actual_angle >= 0 
		// 			&& motor_controllers[3].motor_measure.actual_angle <= 0 
		// 			&& motor_controllers[4].motor_measure.actual_angle >= 0)
		// {
		// 	motor_controllers->all_motor_circul_flag = 0;
		// 	log_i("control_flag:0\r\n");
		// }
		if(motor_controllers[0].motor_measure.actual_angle <= 55
					&& motor_controllers[1].motor_measure.actual_angle >= -5
					&& motor_controllers[2].motor_measure.actual_angle >= -5
					&& motor_controllers[3].motor_measure.actual_angle <= 5
					&& motor_controllers[4].motor_measure.actual_angle <= 15){
			motor_controllers->all_motor_circul_flag = 0;
			log_i("control_flag:0\r\n");
		}
		break;
	}
	default:
		log_i("error: unknown\r\n");
		break;
	}
	all_motor_cal_send();
}

void all_motor_cal_send(void)
{
	for (int i = 0; i < 5; i++)
	{
		motor_controllers[i].motor_measure.set_speed = PidCalculate(&motor_controllers[i].position_pid, motor_controllers[i].motor_measure.set_totall_ecd, motor_controllers[i].motor_measure.totall_ecd);
		motor_controllers[i].motor_measure.set_current = PidCalculate(&motor_controllers[i].speed_pid, motor_controllers[i].motor_measure.set_speed, motor_controllers[i].motor_measure.speed_rpm);
	}
	CAN_cmd_chassis(FINGER0_3_MOTOR_ID, motor_controllers[0].motor_measure.set_current, motor_controllers[1].motor_measure.set_current,
					motor_controllers[2].motor_measure.set_current, motor_controllers[3].motor_measure.set_current);
	HAL_Delay(1);
	CAN_cmd_chassis(FINGER4_4_MOTOR_ID, motor_controllers[4].motor_measure.set_current, 0, 0, 0);
}

void all_motor_stretch(void)
{
	if(motor_controllers[0].motor_measure.actual_angle <= 55
					&& motor_controllers[1].motor_measure.actual_angle >= -5
					&& motor_controllers[2].motor_measure.actual_angle >= -5
					&& motor_controllers[3].motor_measure.actual_angle <= 5
					&& motor_controllers[4].motor_measure.actual_angle <= 15){
		all_motor_stop();
	} else {
		motor_controllers[0].motor_measure.set_totall_ecd = 4*8192;
		motor_controllers[1].motor_measure.set_totall_ecd = 8192;
		motor_controllers[2].motor_measure.set_totall_ecd = 8192;
		motor_controllers[3].motor_measure.set_totall_ecd = -8192;
		motor_controllers[4].motor_measure.set_totall_ecd = 0;
		all_motor_cal_send();
	}
	
}

void all_motor_shrink(void)
{
	if(motor_controllers[0].motor_measure.actual_angle >= motor_controllers[3].circul_end - 5
					&& motor_controllers[1].motor_measure.actual_angle <= motor_controllers[1].circul_end + 3
					&& motor_controllers[2].motor_measure.actual_angle <= motor_controllers[2].circul_end + 3
					&& motor_controllers[3].motor_measure.actual_angle >= motor_controllers[3].circul_end -10
					&& motor_controllers[4].motor_measure.actual_angle >= motor_controllers[4].circul_end -10){
		all_motor_stop();
	} else {
		motor_controllers[0].motor_measure.set_totall_ecd =  motor_controllers[0].circul_end * 819;
		motor_controllers[1].motor_measure.set_totall_ecd =  motor_controllers[1].circul_end * 819;
		motor_controllers[2].motor_measure.set_totall_ecd =  motor_controllers[2].circul_end * 819;
		motor_controllers[3].motor_measure.set_totall_ecd =  motor_controllers[3].circul_end * 819;
		motor_controllers[4].motor_measure.set_totall_ecd =  motor_controllers[4].circul_end * 819;
		all_motor_cal_send();
	}
}

void all_motor_stop(void)
{
	CAN_cmd_chassis(FINGER0_3_MOTOR_ID, 0, 0, 0, 0);
	CAN_cmd_chassis(FINGER0_3_MOTOR_ID, 0, 0, 0, 0);
}

void all_motor_controller(uint8_t cmd)
{
	switch (cmd)
	{
	case STOP:
		all_motor_stop();
		break;
	case CIRCUL:
		all_motor_control_circul();
		break;
	case STRETCH:
		all_motor_stretch();
		break;
	case SHRINK:
		all_motor_shrink();
		break;
	default:
		break;
	}
}


void single_motor_control_circul(uint8_t motor_number){
	switch (motor_controllers->single_motor_circul_flag[motor_number]){
		case 0:
        {
            motor_controllers[motor_number].motor_measure.set_totall_ecd =  motor_controllers[motor_number].circul_end/10 * 8192;
            motor_controllers->single_motor_circul_flag[motor_number] = 1;
            log_i("%dcontrol_flag_single:1\r\n", motor_number);
            break;
        }
        case 1:
		{
			if(motor_number>=3){
				if (motor_controllers[motor_number].motor_measure.actual_angle >= motor_controllers[motor_number].circul_end){
					motor_controllers->single_motor_circul_flag[motor_number] = 2;
					log_i("%dcontrol_flag_single:2\r\n", motor_number);
				}
			} else {
				if (motor_controllers[motor_number].motor_measure.actual_angle <= motor_controllers[motor_number].circul_end){
					motor_controllers->single_motor_circul_flag[motor_number] = 2;
					log_i("%dcontrol_flag_single:2\r\n", motor_number);
				}
			}
			
			break;
		}
		case 2:
		{
			motor_controllers[motor_number].motor_measure.set_totall_ecd = 8192;
            motor_controllers->single_motor_circul_flag[motor_number] = 3;
            log_i("%dcontrol_flag_single:3\r\n", motor_number);
            break;
        }
        case 3:{
			if(motor_number >= 3){
				if (motor_controllers[motor_number].motor_measure.actual_angle <= 0){
					motor_controllers->single_motor_circul_flag[motor_number] = 0;
					log_i("%dcontrol_flag_single:0\r\n", motor_number);
            	}
			} else {
				if (motor_controllers[motor_number].motor_measure.actual_angle >= 0){
					motor_controllers->single_motor_circul_flag[motor_number] = 0;
					log_i("%dcontrol_flag_single:0\r\n", motor_number);
            	}
			}
            
            break;
		}
	}
}

void single_motor_controller(FINGLE_STATUS status[])
{
	for (int i = 0; i < 5; i++)
	{
		switch (status[i])
		{
		case STOP:
			motor_controllers[i].motor_measure.set_current = 0;
			break;
		case CIRCUL:
			single_motor_control_circul(i);
			break;
		case STRETCH:
			motor_controllers[i].motor_measure.set_totall_ecd = 0;
			motor_controllers[i].motor_measure.set_speed = PidCalculate(&motor_controllers[i].position_pid, motor_controllers[i].motor_measure.set_totall_ecd, motor_controllers[i].motor_measure.totall_ecd);
			motor_controllers[i].motor_measure.set_current = PidCalculate(&motor_controllers[i].speed_pid, motor_controllers[i].motor_measure.set_speed, motor_controllers[i].motor_measure.speed_rpm);
			break;
		case SHRINK:
			motor_controllers[i].motor_measure.set_totall_ecd = motor_controllers[i].circul_end * 819;
			motor_controllers[i].motor_measure.set_speed = PidCalculate(&motor_controllers[i].position_pid, motor_controllers[i].motor_measure.set_totall_ecd, motor_controllers[i].motor_measure.totall_ecd);
			motor_controllers[i].motor_measure.set_current = PidCalculate(&motor_controllers[i].speed_pid, motor_controllers[i].motor_measure.set_speed, motor_controllers[i].motor_measure.speed_rpm);
			break;
		default:
			break;
		}
	}
	CAN_cmd_chassis(FINGER0_3_MOTOR_ID, motor_controllers[0].motor_measure.set_current, motor_controllers[1].motor_measure.set_current,
                    motor_controllers[2].motor_measure.set_current, motor_controllers[3].motor_measure.set_current);
	HAL_Delay(1);
	CAN_cmd_chassis(FINGER4_4_MOTOR_ID, motor_controllers[4].motor_measure.set_current, 0, 0, 0);
}

void passive_controller(void){
	switch(motor_controllers->passive_control_flag)
	{
		case 0:
	    {
			if(motor_controllers[0].motor_measure.last_angle != motor_controllers[0].motor_measure.actual_angle
						|| motor_controllers[1].motor_measure.last_angle != motor_controllers[1].motor_measure.actual_angle
						|| motor_controllers[2].motor_measure.last_angle != motor_controllers[2].motor_measure.actual_angle
						|| motor_controllers[3].motor_measure.last_angle != motor_controllers[3].motor_measure.actual_angle
						|| motor_controllers[4].motor_measure.last_angle != motor_controllers[4].motor_measure.actual_angle){
				 motor_controllers->passive_control_flag = 1;
			}
            break;
        }
		case 1:
		{
			all_motor_control_circul();
            break;
		}
		case 2:
		{
            break;
		}
		default:
			log_i("Unknown flag: %d\r\n", motor_controllers->passive_control_flag);
		    break;
	}
}

uint8_t data_send_motor[] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
void sendMotorData(void){
	for(int i=0;i<5;i++){
		if(motor_controllers[i].motor_measure.actual_angle<0){
			data_send_motor[i+1] = (uint8_t)(-motor_controllers[i].motor_measure.actual_angle);
		} else {
			data_send_motor[i+1] = (uint8_t)(motor_controllers[i].motor_measure.actual_angle);
		}
		if(i == 0 || i == 4){
			data_send_motor[i+1] = (uint8_t)(data_send_motor[3]);
		}
		// data_send_motor[i+1] += i;
		// if(data_send_motor[i+1]>=90) 
		// 	data_send_motor[i+1] = 0; 
	}
	for(int j=0;j<5;j++){
		if(motor_controllers[j].motor_measure.given_current<0){
			data_send_motor[j+6] = (uint8_t)(-motor_controllers[j].motor_measure.given_current/1000);
		} else {
			data_send_motor[j+6] = (uint8_t)(motor_controllers[j].motor_measure.given_current/1000);
		}
		
		// data_send_motor[j+6] = j*10;
	}
	usart1_transmit(data_send_motor, 11);
}

