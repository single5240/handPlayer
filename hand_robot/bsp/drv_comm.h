/*
 * @Author: Vincent_Jiang jwslove40@163.com
 * @Date: 2024-04-10 17:20:05
 * @LastEditors: Vincent_Jiang jwslove40@163.com
 * @LastEditTime: 2024-04-21 20:25:45
 * @FilePath: \handPlayer\hand_robot\bsp\drv_comm.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __DRV_COMM_H__
#define __DRV_COMM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "proj_config.h"
#include "stm32f4xx_hal.h"

enum CMD_TYPE
{
    CONTROL = 0x04, // 控制命令
    PARAMSET = 0x40, // 参数设置
	MODESET = 0x80, // 参数发送
};

typedef enum {
	ACTIVE,
	PASSIVE,
}CONTROL_TYPE;

typedef enum {
	TOTAL,
	SINGLE,
} CONTROL_RANGE;

enum FINGER_INDEX
{
    THUMB = 0, //拇指
    FOREFINGER = 1, // 食指
    MIDDLE = 2, // 中指
    RING = 3, // 无名指
    LITTLE = 4, // 小指
    TOTAlFINGER =5, // 全部手指
    SINGLEFINGER =6, // 单个手指
};

typedef enum 
{
    STOP = 0, //停止
    CIRCUL = 1, //循环
    STRETCH = 2, //伸展
    SHRINK = 3, //收缩
}FINGLE_STATUS;

typedef struct {
	//电机状态
	CONTROL_TYPE motor_control_type; // 0-主动 1-被动
	CONTROL_RANGE motor_control_range; //0-全部 1-单个
	FINGLE_STATUS totaol_status; // 0-停止 1-循环 2-伸展 3-收缩
	FINGLE_STATUS single_status[5]; // 0-停止 1-循环 2-伸展 3-收缩

    uint8_t key_status[4];
    uint8_t led_status[4];
} motor_status_t, *motor_status_p;

void rxDataHandler(uint8_t *buff, uint16_t len);
motor_status_p get_finger_status_p(void);
void led_turn(uint8_t index, uint8_t status);
GPIO_PinState key_read(int index);
void key_led_controller(void);
#ifdef __cplusplus
}
#endif

#endif // __DRV_COMM_H__


