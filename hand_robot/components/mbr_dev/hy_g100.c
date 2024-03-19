/*
 * @Author: lhq
 * @Date: 2023-07-10 23:05:08
 * @LastEditors: lhq
 * @LastEditTime: 2023-08-08 21:59:04
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\components\mbr_dev\hy_g100.c
 * @Description:
 *
 * Copyright (c) 2023 by lhq, All Rights Reserved.
 */
#include "hy_g100.h"
#include "drv_modbus.h"
#include "drv_uart.h"
#include "init.h"
#include "fifo.h"
#include "cmsis_os.h"
#include "u8tool.h"

#define LOG_TAG "hy_g100_dev"
#define LOG_OUTPUT_LEVEL LOG_INFO
#include "log.h"

g100_converter_t g100_fb;

static void hy_g100_delayms(uint16_t ms)
{
	osDelay(ms);
}

/**
 * @description: g100变频器协议解包
 * @param {void} *this_p
 * @param {uint8_t} *buff
 * @return {*}
 */
void hy_g100_03h_unpack(void *this_p, uint8_t *buff)
{
	modbus_rtu_p mbr;
	if (this_p != NULL)
		mbr = this_p;
	else
		log_e("null pointer!");

	// int32_t pos_temp;
	// switch (mbr->addr)
	// {
	// case HS100_ADDRESS_1:

	// 	break;
	// case HS100_ADDRESS_2:

	// 	break;

	// default:
	// 	break;
	// }

	switch (mbr->reg)
	{
	case G100_REG_P00_01: // 命令源选择
		u8TOu16(3, buff, (uint16_t *)&g100_fb.cmd_cfg);
		mbr->ack = 1;
		break;

	case G100_REG_P00_08: // 预设频率
		u8TOu16(3, buff, &g100_fb.set_frequency);
		mbr->ack = 1;
		break;

	case G100_REG_PD1_00: // 通信设定值
		mbr->ack = 1;
		break;

	case G100_REG_PD1_10: // 电机转速
		u8TOs16(3, buff, &mbr->dev_speed_s);
		mbr->ack = 1;
		break;

	case G100_REG_PD1_02: // 母线电压
		u8TOs16(3, buff, &mbr->dev_voltage_s);
		mbr->ack = 1;
		break;
		
	case G100_REG_PD1_05: // 输出功率
		u8TOs16(3, buff, &mbr->dev_power_s);
		mbr->ack = 1;
		break;
	
	case G100_REG_PD1_1E: // 输出转矩（相对于变频器额定转矩）
		u8TOs16(3, buff, &mbr->dev_load_rate_s);
		mbr->ack = 1;
		break;

	case G100_REG_PD4_00: // 变频器故障描述
		u8TOs16(3, buff, &mbr->dev_error_code_s);
		mbr->ack = 1;
		break;

	case G100_REG_P07_08: // 散热器温度
		u8TOs16(3, buff, &mbr->dev_temperature_s);
		mbr->ack = 1;
		break;

	default:
		break;
	}
}

/**
 * @description: g100变频器读取测试
 * @param {modbus_rtu_p} mbr
 * @return {*}
 */
uint8_t hy_g100_read_test(modbus_rtu_p mbr)
{
	MBR_Read_03H_Block(mbr, mbr->dev_addr, G100_REG_P00_01, 1); // 读取命令源选择：通讯
	return 1;
}

/**
 * @description: G100变频器配置，内含阻塞操作，只能在初始化阶段调用
 * @param {modbus_rtu_p} mbr
 * @return {*}
 */
uint8_t G100_Converter_Config(modbus_rtu_p mbr)
{
	hy_g100_delayms(50);

	log_i("准备初始化变频器\r\n");

	MBR_Read_03H_Block(mbr, mbr->addr, G100_REG_P00_01, 1); // 读取命令源选择：通讯

	if (g100_fb.cmd_cfg == 2)
	{
		log_i("该变频器命令源选择已经设置为通讯,接下来参数写入RAM		\r\n");

		hy_g100_delayms(10);

		MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_P00_01 + G100_REG_RAM, 0x0002); // 命令源选择：通讯

		MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_P00_02 + G100_REG_RAM, 0x0007); // 主频率源X选择	7：通讯

		MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_P00_07 + G100_REG_RAM, 0x0000); // 主频率源叠加选择	0：主频率源X

		MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_P00_08 + G100_REG_RAM, 0x0000); // 预设频率
	}
	else
	{
		log_i("该变频器命令源选择未设置为通讯,接下来参数写入EEPROM		\r\n");

		hy_g100_delayms(10);

		MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_P00_01, 0x0002); // 命令源选择：通讯

		MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_P00_02, 0x0007); // 主频率源X选择	7：通讯

		MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_P00_07, 0x0000); // 主频率源叠加选择	0：主频率源X

		MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_P00_08, 0x0000); // 预设频率
	}

	MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_P00_17 + G100_REG_RAM, 0x0064); //	加速时间1	0.0s～6500.0s	单位0.1s 初始值20

	MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_P00_18 + G100_REG_RAM, 0x0032); //	减速时间1	0.0s～6500.0s	单位0.1s 初始值20
	
	MBR_Write_06H_Block(mbr, mbr->addr, G100_REG_PD1_00, 0x0000); // 修改通信设定值

	MBR_Read_03H_Block(mbr, mbr->addr, G100_REG_PD1_00, 1); // 回读通信设定值

	hy_g100_delayms(50);

	return 1;
}


/**
 * @description: 通讯命令控制
 * @param {modbus_rtu_p} mbr
 * @param {uint8_t} cmd 0001：正转运行		0002：反转运行		0003：正转点动		0004：反转点动		0005：自由停机		0006：减速停机		0007：故障复位		0008：调谐启动
 * @return {*}
 */
uint8_t hy_g100_ctr_cmd(modbus_rtu_p mbr,uint8_t cmd)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_WriteParam_06H(mbr, mbr->dev_addr, G100_REG_PD2_00, cmd); // 变频器控制命令 

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}


/**
 * @brief G100变频器速度设置
 * @param {modbus_rtu_p} mbr
 * @param {int16_t} speed_cmd
 * @return {*}
 */
uint8_t hy_g100_speed_set(modbus_rtu_p mbr, int16_t speed_cmd)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_WriteParam_06H(mbr, mbr->dev_addr, G100_REG_PD1_00, speed_cmd);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}


/// @brief 读取主轴转速
/// @param mbr 
/// @return 
uint8_t hy_g100_read_speed(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, G100_REG_PD1_10, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}


/// @brief 读取主轴母线电压
/// @param mbr 
/// @return 
uint8_t hy_g100_read_voltage(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, G100_REG_PD1_02, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

/// @brief 读取主轴输出功率
/// @param mbr 
/// @return 
uint8_t hy_g100_read_power(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, G100_REG_PD1_05, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

/// @brief 读取输出转矩（相对于变频器额定转矩）
/// @param mbr 
/// @return 
uint8_t hy_g100_read_load(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, G100_REG_PD1_1E, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

/// @brief 读取变频器故障描述
/// @param mbr 
/// @return 
uint8_t hy_g100_read_err(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, G100_REG_PD4_00, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

/// @brief 读取散热器温度
/// @param mbr 
/// @return 
uint8_t hy_g100_read_temp(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, G100_REG_P07_08, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}



//static void hs100_u8TOs32(uint16_t offest, uint8_t *savebuffer, int32_t *data)
//{
//	*data = (int32_t)(*(savebuffer + offest + 1) | *(savebuffer + offest) << 8 | *(savebuffer + offest + 3) << 16 | *(savebuffer + offest + 2) << 24);
//}

//static void hs100_s32TOu8(uint16_t offest, uint8_t *savebuffer, int32_t data)
//{
//	savebuffer += offest;
//	*savebuffer = (uint8_t)(data >> 8);
//	savebuffer += 1;
//	*savebuffer = (uint8_t)(data);
//	savebuffer += 1;
//	*savebuffer = (uint8_t)(data >> 24);
//	savebuffer += 1;
//	*savebuffer = (uint8_t)(data >> 16);
//}
