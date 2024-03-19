/*
 * @Author: lhq
 * @Date: 2023-07-03 16:55:09
 * @LastEditors: lhq
 * @LastEditTime: 2023-08-09 16:42:55
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\components\mbr_dev\hs100.c
 * @Description:
 *
 * Copyright (c) 2023 by lhq, All Rights Reserved.
 */
#include "hs100.h"
#include "drv_modbus.h"
#include "drv_uart.h"
#include "init.h"
#include "fifo.h"
#include "cmsis_os.h"
#include "u8tool.h"

#define LOG_TAG "hs100_dev"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

static void hs100_u8TOs32(uint16_t offest, uint8_t *savebuffer, int32_t *data);
static void hs100_s32TOu8(uint16_t offest, uint8_t *savebuffer, int32_t data);
static void hs100_u8TOs16(uint16_t offest, uint8_t *savebuffer, int16_t *data);

uint8_t hs100_di13_trigger(modbus_rtu_p mbr);

static void hs100_delayms(uint16_t ms)
{
	osDelay(ms);
}

void hs100_03h_unpack(void *this_p, uint8_t *buff)
{
	modbus_rtu_p mbr;
	if (this_p != NULL)
		mbr = this_p;
	else
		log_e("null pointer!");

	int32_t pos_temp;
	switch (mbr->addr)
	{
	case HS100_ADDRESS_1:

		break;
	case HS100_ADDRESS_2:

		break;

	default:
		break;
	}

	switch (mbr->reg)
	{
	case HS100_REG_F31_41:

		mbr->ack = 1;
		break;

	case HS100_REG_P03_00: // 第一组规划位移，32位寄存器

		mbr->ack = 1;
		break;

	case HS100_REG_D40_12: // 绝对位置反馈，码盘单位
		hs100_u8TOs32(3, buff, &pos_temp);
		mbr->dev_pos_s = pos_temp;
		mbr->ack = 1;
		break;

	case HS100_REG_D40_16: // 绝对位置反馈，用户单位
		//						u8TOs32(3,buff,&hs100->Absolute_position_feedback);
		mbr->ack = 1;
		break;

	case HS100_REG_P03_02: // 第一组规划速度
		//						u8TOu16(3,buff,&hs100->posmode_speed);
		mbr->ack = 1;
		break;

	case HS100_REG_P00_02: // 读取伺服运行模式
		//						u8TOu16(3,buff,(uint16_t*)&hs100->mode);
		mbr->ack = 1;
		break;

	case HS100_REG_P01_20: // 速度指令选择

		mbr->ack = 1;
		break;

	case HS100_REG_D40_03:	// 速度反馈，单位rpm
		hs100_u8TOs16(3, buff, &mbr->dev_speed_s);
		mbr->ack = 1;
		break;

	case HS100_REG_D40_23:	// 平均负载率 0~4000 单位0.1%
		hs100_u8TOs16(3,buff,&mbr->dev_load_rate_s);
		mbr->ack = 1;
		break;

	case HS100_REG_P1E_10:	// 故障码，0~0xffff
		hs100_u8TOs16(3,buff,&mbr->dev_error_code_s);
		mbr->ack = 1;
		break;

	case HS100_REG_D40_24:	// IGBT温度 0~9000 单位0.1度
		hs100_u8TOs16(3,buff,&mbr->dev_temperature_s);
		mbr->ack = 1;
		break;


	// case HS100_REG_D40_00:	// 位置指令-9999~9999
	// 	log_i("length:%d",mbr->rec_length);
	// 	hs100_u8TOs16(3,buff,&mbr->HS100_D40.d40_00);
	// 	hs100_u8TOs16(5,buff,&mbr->HS100_D40.d40_01);
	// 	hs100_u8TOs16(7,buff,&mbr->HS100_D40.d40_02);
	// 	hs100_u8TOs16(9,buff,&mbr->HS100_D40.d40_03);
	// 	hs100_u8TOs16(11,buff,&mbr->HS100_D40.d40_04);
	// 	hs100_u8TOs16(13,buff,&mbr->HS100_D40.d40_05);
		
	// 	mbr->ack = 1;
	// 	break;


	default:
		break;
	}
}

uint8_t hs100_read_test(modbus_rtu_p mbr)
{
	//		return MBR_ReadParam_03H(mbr,mbr->dev_addr,HS100_REG_F31_41,1);
	MBR_Read_03H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 1);
	return 1;
}

// HS100伺服位置模式配置，内含阻塞操作，只能在初始化阶段调用
uint8_t hs100_pos_mode_config(modbus_rtu_p mbr)
{
	u8 data_buf[4];

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x0000); // 关闭伺服使能

	hs100_delayms(1000);

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P0A_05, 0x0000); // EEPROM存储设置，不存入eeprom

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P0A_03, 0x0001); // MODBUS通讯应答时间 1~1000

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P00_02, 0x0000); // 设置伺服运行模式：位置模式

	MBR_Read_03H_Block(mbr, mbr->dev_addr, HS100_REG_P00_02, 1); // 读取伺服运行模式

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P01_00, 0x0001); // 位置指令选择  1：位置规划   0：脉冲输入

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P01_18, 0x0003); // 位置规划模式选择		3：轮廓轨迹模式运行（DI功能13触发启动,可实时修改第一段位置指令并根据PO1-1A选择的更新方式连续运行）

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P01_1A, 0x0001); // 位置指令更新方式设置，1：立即更新

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P01_19, 0x0001); // 1: 执行位置指令规划后的最终位置为执行前的相对增量位置

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P03_02, 4000); // 设置第一组规划速度

	hs100_s32TOu8(0, data_buf, 200);
	MBR_Write_10H_Block(mbr, mbr->dev_addr, HS100_REG_P03_04, 2, data_buf); // 第一组规划加速时间	单位ms

	hs100_s32TOu8(0, data_buf, 200);
	MBR_Write_10H_Block(mbr, mbr->dev_addr, HS100_REG_P03_06, 2, data_buf); // 第一组规划减速时间	单位ms

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P05_22, 200); // 位置指令低通滤波时间常数A  单位0.1ms

	MBR_Read_03H_Block(mbr, mbr->dev_addr, HS100_REG_P03_02, 1); // 读取第一组规划速度

	hs100_delayms(50);

	return 1;
}

// HS100伺服速度模式配置，内含阻塞操作，只能在初始化阶段调用
uint8_t hs100_speed_mode_config(modbus_rtu_p mbr)
{
	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x0000); // 关闭伺服使能

	hs100_delayms(1000);

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P0A_05, 0x0000); // EEPROM存储设置，不存入eeprom

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P00_02, 0x0001); // 设置伺服运行模式：速度模式

	MBR_Read_03H_Block(mbr, mbr->dev_addr, HS100_REG_P00_02, 1); // 读取伺服运行模式

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P01_20, 0x0000); // 速度指令选择，0：数字给定

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P01_21, 0); // 速度给定值

	hs100_delayms(50);

	return 1;
}

// HS100伺服速度模式配置，内含阻塞操作，只能在初始化阶段调用（规划模式）
uint8_t hs100_speed_mode_config_plan(modbus_rtu_p mbr)
{
	u8 data_buf[4];

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x0000); // 关闭伺服使能

	hs100_delayms(1000);

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P0A_05, 0x0000); // EEPROM存储设置，不存入eeprom

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P00_02, 0x0001); // 设置伺服运行模式：速度模式

	MBR_Read_03H_Block(mbr, mbr->dev_addr, HS100_REG_P00_02, 1); // 读取伺服运行模式

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P01_20, 0x0003); // 速度指令选择，3:速度规划

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P01_36, 0x0000); // 速度规划模式选择 0：从初始规划组号到结束规划组号的位置指令运行一遍后停止。

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P04_00, 0); // 第一组规划速度

	hs100_s32TOu8(0, data_buf, 10);
	MBR_Write_10H_Block(mbr, mbr->dev_addr, HS100_REG_P04_02, 2, data_buf); // 第一组规划运行时间

	hs100_s32TOu8(0, data_buf, 10);
	MBR_Write_10H_Block(mbr, mbr->dev_addr, HS100_REG_P04_04, 2, data_buf); // 第一组规划加速时间

	hs100_s32TOu8(0, data_buf, 10);
	MBR_Write_10H_Block(mbr, mbr->dev_addr, HS100_REG_P04_06, 2, data_buf); // 第一组规划减速时间

	hs100_delayms(50);

	return 1;
}

// HS100伺服转矩模式配置，内含阻塞操作，只能在初始化阶段调用
uint8_t hs100_torque_mode_config(modbus_rtu_p mbr)
{
	MBR_Read_03H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 1);

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x0000); // 关闭伺服使能

	hs100_delayms(1000);

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P0A_05, 0x0000); // EEPROM存储设置，不存入eeprom

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P00_02, 0x0002); // 设置伺服运行模式：转矩模式

	MBR_Read_03H_Block(mbr, mbr->dev_addr, HS100_REG_P00_02, 1); // 读取伺服运行模式

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P01_40, 0x0000); // 转矩指令选择，0：数字给定

	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P01_41, 0); // 转矩给定值

	hs100_delayms(50);

	return 1;
}

uint8_t hs100_output_enable(modbus_rtu_p mbr)
{
	if (mbr->dev_mode == MBR_DEV_POS_MODE)
	{
		MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x0001); // 开启伺服使能
		hs100_delayms(500);

		MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x1001); // DI功能13触发位置规划指令更新
		hs100_delayms(50);
	}
	else
	{
		MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x0001); // 开启伺服使能
		hs100_delayms(500);
	}
	return 1;
}

// HS伺服在速度模式下设置速度给定		-6000~6000
uint8_t hs100_speed_set(modbus_rtu_p mbr, int16_t speed_cmd)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_WriteParam_06H(mbr, mbr->dev_addr, HS100_REG_P01_21, speed_cmd);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

// HS伺服在速度模式下设置速度给定（规划模式）		-6000~6000
uint8_t hs100_speed_set_plan(modbus_rtu_p mbr, int16_t speed_cmd)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;
	// u8 data_buf[4];

	pass = MBR_WriteParam_06H(mbr, mbr->dev_addr, HS100_REG_P04_00, speed_cmd);

	hs100_delayms(mbr->dev_mbr_delay);

	MBR_WriteParam_06H(mbr, mbr->dev_addr, HS100_REG_P07_21, 0); // DI功能13触发位置规划指令更新

	hs100_delayms(mbr->dev_mbr_delay);

	MBR_WriteParam_06H(mbr, mbr->dev_addr, HS100_REG_P07_21, 1); // DI功能13触发位置规划指令更新

	// hs100_s32TOu8(0, data_buf, 100);
	// MBR_WriteParam_10H(mbr, mbr->dev_addr, HS100_REG_P04_02, 2, data_buf); // 第一组规划运行时间

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

// HS伺服在位置模式下设置目标位置（必须在位置模式下）
uint8_t hs100_pos_set(modbus_rtu_p mbr, int32_t pos_cmd)
{
	uint8_t pass;
	u8 data_buf[4];

	hs100_s32TOu8(0, data_buf, pos_cmd);

	pass = MBR_WriteParam_10H(mbr, mbr->dev_addr, HS100_REG_P03_00, 2, data_buf);

	hs100_delayms(mbr->dev_mbr_delay);
	
	hs100_di13_trigger(mbr);

	if (pass == 0)
		mbr->fail_cnt++;
	else
		mbr->fail_cnt = 0;

	if (mbr->fail_cnt > 20)
		return 0;
	else
		return 1;
}

// HS伺服在位置模式下的速度给定		0~6000
uint8_t hs100_pos_mode_speed_set(modbus_rtu_p mbr, int16_t speed)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_WriteParam_06H(mbr, mbr->dev_addr, HS100_REG_P03_02, speed); // 设置第一组规划速度

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

// HS伺服在转矩模式下设置转矩给定		-4000~4000
uint8_t hs100_torque_set(modbus_rtu_p mbr, int16_t torque_cmd)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_WriteParam_06H(mbr, mbr->dev_addr, HS100_REG_P01_41, torque_cmd);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

uint8_t hs100_read_pos(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, HS100_REG_D40_12, 2);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

uint8_t hs100_read_speed(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, HS100_REG_D40_03, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

static void hs100_u8TOs16(uint16_t offest, uint8_t *savebuffer, int16_t *data)
{
	*data = (int16_t)(*(savebuffer + offest + 1) | *(savebuffer + offest) << 8);
}

static void hs100_u8TOs32(uint16_t offest, uint8_t *savebuffer, int32_t *data)
{
	*data = (int32_t)(*(savebuffer + offest + 1) | *(savebuffer + offest) << 8 | *(savebuffer + offest + 3) << 16 | *(savebuffer + offest + 2) << 24);
}

static void hs100_s32TOu8(uint16_t offest, uint8_t *savebuffer, int32_t data)
{
	savebuffer += offest;
	*savebuffer = (uint8_t)(data >> 8);
	savebuffer += 1;
	*savebuffer = (uint8_t)(data);
	savebuffer += 1;
	*savebuffer = (uint8_t)(data >> 24);
	savebuffer += 1;
	*savebuffer = (uint8_t)(data >> 16);
}

uint8_t hs100_di13_trigger(modbus_rtu_p mbr)
{
	if (mbr->hs100_di13_state == 0)
	{
		mbr->hs100_di13_state = 1;
		MBR_WriteParam_06H(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x1001); // DI功能13触发位置规划指令更新
		return 1;
	}
	else
	{
		mbr->hs100_di13_state = 0;
		MBR_WriteParam_06H(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x0001); // DI功能13触发位置规划指令更新
		return 1;
	}
}

uint8_t hs100_di13_trigger_block(modbus_rtu_p mbr)
{
	if (mbr->hs100_di13_state == 0)
	{
		mbr->hs100_di13_state = 1;
		MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x1001); // DI功能13触发位置规划指令更新
		return 1;
	}
	else
	{
		mbr->hs100_di13_state = 0;
		MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_F31_41, 0x0001); // DI功能13触发位置规划指令更新
		return 1;
	}
}


uint8_t hs100_pos_mode_set_speed_block(modbus_rtu_p mbr,uint16_t speed)
{
	MBR_Write_06H_Block(mbr, mbr->dev_addr, HS100_REG_P03_02, speed); // 设置第一组规划速度
	return 1;
}

uint8_t hs100_pos_set_block(modbus_rtu_p mbr,int32_t pos_cmd)
{
	u8 data_buf[4];

	hs100_s32TOu8(0, data_buf, pos_cmd);

	MBR_Write_10H_Block(mbr, mbr->dev_addr, HS100_REG_P03_00, 2, data_buf);

	hs100_di13_trigger_block(mbr);
	
	return 1;
	
}

uint8_t hs100_read_average_load(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, HS100_REG_D40_23, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}


uint8_t hs100_read_temperature(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, HS100_REG_D40_24, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

uint8_t hs100_read_err(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, HS100_REG_P1E_10, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

// uint8_t hs100_read_D40(modbus_rtu_p mbr)
// {
// 	uint8_t pass;
// 	static uint8_t fail_cnt = 0;

// 	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, HS100_REG_D40_00, 8);

// 	if (pass == 0)
// 		fail_cnt++;
// 	else
// 		fail_cnt = 0;

// 	if (fail_cnt > 20)
// 		return 0;
// 	else
// 		return 1;

// }

