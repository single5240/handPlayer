/*
 * @Author: Vincent_Jiang jwslove40@163.com
 * @Date: 2024-03-19 10:16:12
 * @LastEditors: Vincent_Jiang jwslove40@163.com
 * @LastEditTime: 2024-04-21 19:54:33
 * @FilePath: \handPlayer\hand_robot\application\comm_task.c
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include "comm_task.h"

#include "cmsis_os.h"
#include "init.h"
#include "drv_motor.h"
#include "drv_comm.h"
#define LOG_TAG "comm_task"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"
extern motor_controller_t motor_controllers[5];
char buf[128];
uint8_t init[] = "DIR(3);CLR(0);\r\n";
// uint8_t text2[] = "CLR(0);DC32(50,10,'Hand Robot',4);\r\n";
motor_status_p finger_status_comm;
uint8_t last_status[2] = {3};
void Comm_Task(void const *argument)
{
	uint32_t period = osKernelSysTick();
	finger_status_comm = get_finger_status_p();
	vTaskDelay(200);
	sendData(init);
	vTaskDelay(500);
	while (1)
	{
		if(last_status[0] != finger_status_comm->motor_control_type || last_status[1] != finger_status_comm->motor_control_range)
		{
			if(finger_status_comm->motor_control_type == ACTIVE){
				sendData("CLR(0);DC32(50,40,'Hand Robot',4);DC32(50,70,'Mode:Active',4);");
			} else if(finger_status_comm->motor_control_type == PASSIVE){
				sendData("CLR(0);DC32(50,40,'Hand Robot',4);DC32(50,70,'Mode:Passive',4);");
			}
			if(finger_status_comm->motor_control_range == TOTAL){
				sendData("DC32(50,100,'Range:Total',4);\r\n");
			} else if(finger_status_comm->motor_control_range == SINGLE){
				sendData("DC32(50,100,'Range:Single',4);\r\n");
			}
		}
		last_status[0] = finger_status_comm->motor_control_type;
		last_status[1] = finger_status_comm->motor_control_range;
		osDelayUntil(&period, 50);
	}
}

void sendData(uint8_t *data){
	uint8_t *data_temp = data;
	uint8_t count = 0;
	while((*data_temp)!=0){
		data_temp++;
		count++;
	}
	usart3_transmit(data, count);
}


