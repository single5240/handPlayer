/*
 * @Author: lhq
 * @Date: 2023-07-10 23:05:16
 * @LastEditors: lhq
 * @LastEditTime: 2023-08-08 21:41:09
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\components\mbr_dev\hy_g100.h
 * @Description: 
 * 
 * Copyright (c) 2023 by lhq, All Rights Reserved. 
 */
#ifndef __HY_G100_H__
#define __HY_G100_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "proj_config.h"
#include "drv_modbus.h"


#define G100_ADDRESS 0x01

#define G100_REG_RAM 0x8000
#define G100_REG_P00_01 0x0001 // 命令源选择	0：键盘		1：端子		2：通讯
#define G100_REG_P00_02 0x0002 // 主频率源X选择	7：通讯
#define G100_REG_P00_07 0x0007 // 主频率源叠加选择	0：主频率源X
#define G100_REG_P00_08 0x0008 // 预设频率

#define G100_REG_P00_17 0x0011 // 加速时间1	0.0s～6500.0s	初始值20
#define G100_REG_P00_18 0x0012 // 减速时间1	0.0s～6500.0s	初始值20

#define G100_REG_P30_05 0x3005 // 输出功率（KW） 最小单位：0.1kw

#define G100_REG_P07_08 0x0708 // 散热器温度	最小单位：1℃

#define G100_REG_PD1_00 0xd100 // 通信设定值		10000对应100%
#define G100_REG_PD1_02 0xd102 // 母线电压（V）		最小单位：0.1V
#define G100_REG_PD1_05 0xd105 // 输出功率（KW） 最小单位：0.1kw

#define G100_REG_PD1_1D 0xd11d // 电机转速
#define G100_REG_PD1_07 0xd107 // 运行速度
#define G100_REG_PD1_10 0xd110 // 反馈速度

#define G100_REG_PD1_1E 0xd11e // 输出转矩（相对于变频器额定转矩）

#define G100_REG_PD2_00 0xd200 // 通讯命令控制		0001：正转运行		0002：反转运行		0003：正转点动		0004：反转点动		0005：自由停机		0006：减速停机		0007：故障复位		0008：调谐启动

#define G100_REG_PD4_00 0xd400 // 变频器故障描述


typedef struct _g100_converter_t {
	
		uint16_t set_frequency;	
		u8	cmd_cfg;
	
} g100_converter_t;


void hy_g100_03h_unpack(void *this_p, uint8_t *buff);
uint8_t G100_Converter_Config(modbus_rtu_p mbr);
uint8_t hy_g100_read_test(modbus_rtu_p mbr);
uint8_t hy_g100_speed_set(modbus_rtu_p mbr, int16_t speed_cmd);
uint8_t hy_g100_ctr_cmd(modbus_rtu_p mbr,uint8_t cmd);
uint8_t hy_g100_read_speed(modbus_rtu_p mbr);


uint8_t hy_g100_read_voltage(modbus_rtu_p mbr);
uint8_t hy_g100_read_power(modbus_rtu_p mbr);
uint8_t hy_g100_read_load(modbus_rtu_p mbr);
uint8_t hy_g100_read_err(modbus_rtu_p mbr);
uint8_t hy_g100_read_temp(modbus_rtu_p mbr);



#ifdef __cplusplus
}
#endif


#endif // __HY_G100_H__


