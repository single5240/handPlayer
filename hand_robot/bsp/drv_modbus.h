/*
 * @Author: lhq
 * @Date: 2023-06-27 11:39:47
 * @LastEditors: lhq
 * @LastEditTime: 2023-08-08 20:32:12
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\bsp\drv_modbus.h
 * @Description:
 *
 * Copyright (c) 2023 by lhq, All Rights Reserved.
 */
#ifndef __DRV_MODBUS_H__
#define __DRV_MODBUS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "proj_config.h"
#include "drv_led.h"
#include "fsm.h"
#include "fifo.h"


	typedef void (*unpack_success_callback)(void *, u8 *, uint16_t);

	typedef enum
	{
		MBR_DEV_SD710 = 1,
		MBR_DEV_HS100,
		MBR_DEV_G100,
	} mbr_dev_type_e;

	typedef enum
	{
		MBR_DEV_SPEED_MODE = 1,
		MBR_DEV_POS_MODE,
		MBR_DEV_TORQUE_MODE,
	} mbr_dev_mode_e;

	typedef void (*usart_dma_send_start_fn)(int16_t length);
	typedef struct _modbus_rtu_t modbus_rtu_t, *modbus_rtu_p;

#pragma pack(push, 1) // 将编译器自己的对齐方式先压入栈保存起来，并告诉编译器下面的就按照1字节对齐
	typedef struct _modbus_rtu_t
	{

		/* modbus 驱动层相关 */
		void (*usart_send_f)(uint8_t *, uint16_t);
		unpack_success_callback unpack_success_callback_f;
		void (*unpack_03h_callback_f)(void *this_p, uint8_t *);
		uint8_t *obj_name;
		uint8_t obj_name_len;
		u8 addr;
		u16 reg;
		volatile u8 ack;
		u8 cmd;
		u8 length;	   // 发送的数据长度
		u8 rec_length; // 接收到的数据长度
		uint8_t fail_cnt;

		/* RJ45 LED */
		led_s_obj_t led_L;
		led_s_obj_t led_R;
		uint16_t led_process_cnt;

		/* 状态机相关 */
		FSM_T fsm;
		fifo_s_t event_fifo;		  /* 事件触发队列 */
		uint8_t event_fifo_buffer[8]; /* 事件触发缓存 */
		void (*fsm_init_f)(void *this_p, FsmTable_T *, uint16_t, uint8_t);
		void (*fsm_eventHandle_f)(void *this_p);
		void (*fsm_eventUpdate_f)(void *this_p, uint8_t);

		/* 设备相关 */
		uint8_t (*dev_03h_unpack_f)(modbus_rtu_p, uint8_t *);		/* 特定设备 modbus rtu 03h 协议解析函数 */
		uint8_t (*dev_pos_cfg_f)(modbus_rtu_p);						/* 伺服驱动器位置模式配置函数 */
		uint8_t (*dev_speed_cfg_f)(modbus_rtu_p);					/* 伺服驱动器速度模式配置函数 */
		uint8_t (*dev_torque_cfg_f)(modbus_rtu_p);					/* 伺服驱动器力矩模式配置函数 */
		uint8_t (*dev_output_enable_f)(modbus_rtu_p);				/* 驱动器使能函数 */
		uint8_t (*dev_output_disable_f)(modbus_rtu_p);				/* 驱动器失能函数 */
		uint8_t (*dev_speed_set_f)(modbus_rtu_p, int16_t);			/* 驱动器速度设定函数 */
		uint8_t (*dev_speed_read_f)(modbus_rtu_p);					/* 驱动器速度读取函数 */
		uint8_t (*dev_read_load_f)(modbus_rtu_p);					/* 驱动器负载读取函数 */
		uint8_t (*dev_read_power_f)(modbus_rtu_p);					/* 驱动器功率读取函数 */
		uint8_t (*dev_read_voltage_f)(modbus_rtu_p);				/* 驱动器电压读取函数 */
		uint8_t (*dev_pos_set_f)(modbus_rtu_p, int32_t);			/* 驱动器位置设定函数 */
		uint8_t (*dev_pos_mode_speed_set_f)(modbus_rtu_p, int16_t); /* 驱动器位置模式下的速度设定函数 */
		uint8_t (*dev_torque_set_f)(modbus_rtu_p, int16_t);			/* 驱动器力矩设定函数 */
		uint8_t (*dev_read_pos_f)(modbus_rtu_p);					/* 驱动器位置读取函数 */
		uint8_t (*dev_read_test_f)(modbus_rtu_p);					/* 驱动器读取测试函数 */
		uint8_t (*dev_ctr_cmd_f)(modbus_rtu_p, uint8_t);			/* 变频器控制函数 */
		uint8_t (*dev_read_speed_f)(modbus_rtu_p);					/* 读取变频器转速 */
		uint8_t (*dev_read_temp_f)(modbus_rtu_p);					/* 读取温度 */
		uint8_t (*dev_read_error_f)(modbus_rtu_p);					/* 读取故障码 */

		uint8_t dev_type;
		uint8_t dev_mode;
		uint8_t dev_addr;
		int64_t dev_pos_s;		   // 位置
		int16_t dev_speed_s;	   // 速度
		int16_t dev_load_rate_s;   // 负载率
		int16_t dev_power_s;	   // 功率
		int16_t dev_temperature_s; // 温度
		int16_t dev_error_code_s;  // 故障码
		int16_t dev_voltage_s;	   // 电压
		uint8_t dev_mbr_delay;

		// struct
		// {
		// 	int16_t d40_00; // 位置指令-9999~9999
		// 	int16_t d40_01; // 位置反馈-9999~9999
		// 	int16_t d40_02;	 // 速度指令，-9000~9000，单位rpm
		// 	int16_t d40_03;	 // 速度反馈，-9000~9000，单位rpm
		// 	int16_t d40_04;	 // 转矩指令，-4000~4000，单位0.1%
		// 	int16_t d40_05;	 // 转矩反馈，-4000~4000，单位0.1%
		// 	uint16_t d40_08; // 电气角度，0~36000，单位0.01度
		// 	uint16_t d40_09; // 机械角度，0~36000，单位0.01度
		// 	int32_t d40_0C;	 // 位置偏差计数器，-2147483648~2147483647，单位p
		// 	int32_t d40_0E;	 // 输入脉冲计数器，-2147483648~2147483647，用户单位
		// 	int32_t d40_10;	 // 绝对位置指令，-2147483648~2147483647,用户单位
		// 	int32_t d40_12;	 // 绝对位置反馈，码盘单位
		// 	int32_t d40_14;	 // 绝对位置指令,-2147483648~2147483647,单位p
		// 	int32_t d40_16;	 // 绝对位置反馈，-2147483648~2147483647,单位p
		// 	int32_t d40_18;	 // 编码器单圈位置,-2147483648~2147483647,单位p
		// 	int32_t d40_1a;	 // 编码器位置低32位,-2147483648~2147483647,单位p
		// 	int32_t d40_1c;	 // 编码器位置高32位,-2147483648~2147483647,单位p
		// 	int32_t d40_1e;	 // 编码器多圈圈数,0~65535,单位圈
		// 	int32_t d40_20;	 // 母线电压,0~9000,单位0.1V
		// 	int32_t d40_21;	 // U相电流有效值,-9000~9000,单位0.1A
		// 	int32_t d40_22;	 // V相电流有效值,-9000~9000,单位0.1A
		// 	int32_t d40_23;	 // 平均负载率 0~4000 单位0.1%
		// 	int32_t d40_24;	 // IGBT温度 0~9000 单位0.1度
		// 	int32_t d40_30;	 // 离线惯量辨识值 0~12000 单位0.01
		// } HS100_D40;

		/*just for hs100*/
		uint8_t hs100_di13_state;

	} modbus_rtu_t, *modbus_rtu_p;
#pragma pack(pop) // 将原来的对齐方式在释放出来。

	uint8_t MBR_ReadParam_03H(modbus_rtu_t *mbr, uint8_t _addr, uint16_t _reg, uint16_t _num);
	uint8_t MBR_WriteParam_06H(modbus_rtu_t *mbr, uint8_t _addr, uint16_t _reg, uint16_t _value);
	uint8_t MBR_WriteParam_10H(modbus_rtu_t *mbr, uint8_t _addr, uint16_t _reg, uint8_t _num, uint8_t *_buf);

	void MBR_AnalyzeApp(modbus_rtu_t *mbr, u8 *buff, uint16_t rec_length);

	void MBR_Read_03H_Block(modbus_rtu_t *mbr, uint8_t _addr, uint16_t _reg, uint16_t _num);
	void MBR_Write_06H_Block(modbus_rtu_t *mbr, uint8_t _addr, uint16_t _reg, uint16_t _value);
	void MBR_Write_10H_Block(modbus_rtu_t *mbr, uint8_t _addr, uint16_t _reg, uint8_t _num, uint8_t *_buf);

	void mbr_fsm_config(modbus_rtu_p mbr);
	void mbr_up_ok_callback_register(modbus_rtu_t *m_obj, unpack_success_callback fun);

	void mbr_led_process(modbus_rtu_p mbr);
	void mbr_dev_mode_set(modbus_rtu_p mbr, uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif // __DRV_MODBUS_H__
