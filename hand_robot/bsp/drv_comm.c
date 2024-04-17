/*
 * @Author: Vincent_Jiang jwslove40@163.com
 * @Date: 2024-04-10 20:59:52
 * @LastEditors: Vincent_Jiang jwslove40@163.com
 * @LastEditTime: 2024-04-12 10:35:23
 * @FilePath: \handPlayer\hand_robot\bsp\drv_comm.c
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include "drv_comm.h"
#include "string.h"
#define LOG_TAG "drv_comm"
#define LOG_OUTPUT_LEVEL  LOG_INFO
#include "log.h"
motor_status_t finger_status;
void rxDataHandler(uint8_t *buff, uint16_t len){
	// log_i("rxdata:%d %d %d",buff[0],buff[1],buff[2]);
	switch(buff[0]){
		case CONTROL:
		{
			switch(buff[1]){
				case TOTAlFINGER:
				{
					finger_status.motor_control_range = TOTAL;
					finger_status.totaol_status = buff[2];
					memset(finger_status.single_status, 0, 5);
				}
				break;
				case THUMB:
				case FOREFINGER:
				case MIDDLE:
				case RING:
				case LITTLE:
				{
					finger_status.motor_control_range = SINGLE;
					finger_status.single_status[buff[1]] = buff[2]; 
				}
				break;
			}
			if(buff[3] != buff[0]){
			log_e("rx data end error!!!");
		}
		}
		break;
		case PARAMSET:
		{
			
		}
		break;
		case MODESET:
		{
			finger_status.motor_control_type = buff[1];
			if(buff[2] != buff[0]){
				log_e("rx data end error!!!");
			}
		}
		break;
		default:
		{
			log_e("rx data start error!!!");
		}
		break;
	}
}

motor_status_p get_finger_status_p(){
	return &finger_status;
}
