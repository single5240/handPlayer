#include "drv_motor.h"
#include "my_pid.h"
#include "main.h"

#define LOG_TAG "drv_motor"
#define LOG_OUTPUT_LEVEL  LOG_INFO
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
	motor_m_t->actual_angle = motor_m_t->rolls * 360 / 36;
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
			get_motor_measure(&(motor_controllers[rx_header.StdId-CAN_FINGER_ALL_ID-1].motor_measure), rx_data);
			ws_cla_motor(&(motor_controllers[rx_header.StdId-CAN_FINGER_ALL_ID-1].motor_measure));
			log_i("%d\r\n", rx_header.StdId);
			break;
		}
		default:
		{
			log_i("Can rx Error!!!\r\n");
			break;
		}
	}
}

void CAN_cmd_chassis(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4)
{
	uint32_t send_mail_box;
	chassis_tx_message.StdId = CAN_FINGER_ALL_ID;
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

uint8_t motor_reset_positions(void){
	
}

void motor_pid_init(void){
    PidInit(&(motor_controllers[0].speed_pid),      7,  0.004,  0.01, 2000, 300, 200);
    PidInit(&(motor_controllers[0].position_pid), 0.1,      0,  0.00,  300,   0,  50);

	// PidInit(&(motor_controllers[1].speed_pid),      7,  0.004,  0.01, 2000, 300, 200);
    // PidInit(&(motor_controllers[1].position_pid), 0.1,      0,  0.00,  300,   0,  50);

	// PidInit(&(motor_controllers[2].speed_pid),      7,  0.004,  0.01, 2000, 300, 200);
	// PidInit(&(motor_controllers[2].position_pid), 0.1,      0,  0.00,  300,   0,  50);

	// PidInit(&(motor_controllers[3].speed_pid),      7,  0.004,  0.01, 2000, 300, 200);
	// PidInit(&(motor_controllers[3].position_pid), 0.1,      0,  0.00,  300,   0,  50);

	// PidInit(&(motor_controllers[4].speed_pid),      7,  0.004,  0.01, 2000, 300, 200);
	// PidInit(&(motor_controllers[4].position_pid), 0.1,      0,  0.00,  300,   0,  50);
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

