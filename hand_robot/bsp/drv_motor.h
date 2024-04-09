#ifndef __DRV_MOTOR_H__
#define __DRV_MOTOR_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "proj_config.h"
#include "stm32f4xx_hal.h"
#include "my_pid.h"

#include "struct_typedef.h"

    typedef enum
    {
        CAN_FINGER_ALL_ID = 0x200,
        FINGER0_MOTOR_ID = 0x201,
        FINGER1_MOTOR_ID = 0x202,
        FINGER2_MOTOR_ID = 0x203,
        FINGER3_MOTOR_ID = 0x204,
        FINGER4_MOTOR_ID = 0x205,

    } can_msg_id_e;
#define FINGER_MOTOR_ID 0x0200
#define FINGER_CAN hcan1
    typedef struct
    {
        uint16_t ecd;
        int16_t speed_rpm;
        int16_t given_current;
        uint8_t temperate;
        int16_t last_ecd;
        int16_t rolls;
        int32_t actual_angle;
        int32_t totall_ecd;

        int32_t set_angle;
        int32_t set_totall_ecd;
        int32_t set_speed;
        int32_t set_current;
    } motor_measure_t;
    typedef struct 
    {
        motor_measure_t motor_measure;
        pid_parameter_t speed_pid;
        pid_parameter_t position_pid;
    }motor_controller_t;

    void CAN_cmd_chassis(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
    void motor_pid_init(void);

#ifdef __cplusplus
}
#endif

#endif // __DRV_MOTOR_H__
