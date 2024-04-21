/*
 * @Author: Vincent_Jiang jwslove40@163.com
 * @Date: 2024-04-10 20:59:52
 * @LastEditors: Vincent_Jiang jwslove40@163.com
 * @LastEditTime: 2024-04-21 20:21:31
 * @FilePath: \handPlayer\hand_robot\bsp\drv_comm.c
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include "drv_comm.h"
#include "drv_led.h"
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
				case SINGLEFINGER:
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
	usart1_transmit(buff, len);
}

motor_status_p get_finger_status_p(){
	return &finger_status;
}

void short_push_key(uint8_t index){
	switch(index){
        case 0:
        break;
        case 1:
        break;
        case 2:
        break;
        case 3:
        break;
        default:
        break;
    }
}

void long_push_key(uint8_t index){
	switch(index){
        case 0:
        break;
        case 1:
        break;
        case 2:
        break;
        case 3:
        break;
        default:
        break;
    }
}

void key_led_controller(void){
	uint8_t key_temp_status = 0;
	for(int i=0; i<4; i++){
		key_temp_status = key_read(i);
		if(key_temp_status == GPIO_PIN_SET){
			finger_status.key_status[i]++;
		} else {
			if(finger_status.key_status[i]>10){
				short_push_key(i);
				led_turn(i, 0);
			} else if(finger_status.key_status[i] >0) {
				long_push_key(i);
				led_turn(i, 1);
			}
			finger_status.key_status[i] = 0;
		}
	}
}
//每100ms执行此函数，1s内为短按，1s以上为长按
void led_turn(uint8_t index, uint8_t status){
	switch(index){
		case 0:
			if(status == 0) LED0_OFF
			else LED0_ON
		break;
		case 1:
			if(status == 1) LED1_OFF
			else LED1_ON
		break;
		case 2:
			if(status == 2) LED2_OFF
			else LED2_ON
		break;
		case 3:
			if(status == 3) LED3_OFF
			else LED3_ON
		break;
		default:
		break;
	}
}

uint8_t key_read(int index){
	switch(index){
		case 0:
            return HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin);
        case 1:
            return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
        case 2:
            return HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
        case 3:
            return HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin);
        default:
        break;
	}
}





