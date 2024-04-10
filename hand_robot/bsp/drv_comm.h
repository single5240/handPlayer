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
} motor_status_t, *motor_status_p;

void rxDataHandler(uint8_t *buff, uint16_t len);
motor_status_p get_finger_status_p(void);

#ifdef __cplusplus
}
#endif


#endif // __DRV_COMM_H__


