/*
 * @Author: lhq
 * @Date: 2023-07-13 14:08:53
 * @LastEditors: lhq
 * @LastEditTime: 2023-08-09 17:29:45
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\application\init.h
 * @Description:
 *
 * Copyright (c) 2023 by lhq, All Rights Reserved.
 */
#ifndef __INIT_H__
#define __INIT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "proj_config.h"

#include "SEGGER_SYSVIEW.h"
#include "SEGGER_RTT.h"

#define RC_DATA_MSG 0
#define RC_DATA_MSG_LEN sizeof(rc_data_pack_t)

#define RADIO_RX_MSG 1
#define RADIO_RX_MSG_LEN 32

#define ENV_BOOT_TIMES "boot_times"
#define ENV_RADIO_ADDR "radio_addr"
#define ENV_RADIO_CHAN "radio_chan"
#define ENV_RADIO_MATCH_KEY "radio_match_key"
#define ENV_TURNBACK_RECORD "turnback_record"
#define ENV_TURNBACK_STRAIGHT_DIS "turnback_str_dis"
#define ENV_CHASSIS_ACCELERATION "chassis_acceleration"
#define ENV_CHASSIS_DECELERATION "chassis_deceleration"
#define ENV_INVERTER_ACCELERATION "inverter_acceleration"
#define ENV_INVERTER_DECELERATION "inverter_deceleration"

#pragma pack(push, 1) // 将编译器自己的对齐方式先压入栈保存起来，并告诉编译器下面的就按照1字节对齐
	typedef struct env_pack
	{
		struct
		{
			uint16_t addr;
			uint8_t chan;
			uint16_t match_key;
		} radio;

		struct
		{
			int32_t boot_times;
		} sys;

		struct
		{
			int32_t leftturn_360_leftwheel;	  // 左轮左转360度的编码器数
			int32_t leftturn_360_rightwheel;  // 右轮左转360度的编码器数
			int32_t rightturn_360_leftwheel;  // 左轮右转360度的编码器数
			int32_t rightturn_360_rightwheel; // 右轮右转360度的编码器数
			int32_t stright_1m;				  // 直行1m的编码器数
		} turnback_record;

		uint8_t turnback_str_dis;	// 调头直行距离
		uint8_t chassis_acceleration;	//底盘加速时间 1-255 单位20ms
		uint8_t chassis_deceleration;	//底盘减速时间 1-255 单位20ms

		uint8_t inverter_acceleration;	//变频器加速时间 3-255 单位s
		uint8_t inverter_deceleration;	//变频器减速时间 3-255 单位s

        uint8_t depended_turn; //独立旋转
        
		uint8_t crcH;
		uint8_t crcL;

	} env_pack_t, *env_pack_p;
#pragma pack(pop) //)将原来的对齐方式在释放出来。

	void board_config(void);

	uint32_t get_time_us(void);
	uint32_t get_time_ms(void);
	float get_time_ms_us(void);
	void *get_rc_env(void);
	void printf_all_env(void);

	uint32_t usart6_rx_callback(uint8_t *buff, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // __INIT_H__
