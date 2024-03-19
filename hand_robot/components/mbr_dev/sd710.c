#include "sd710.h"
#include "drv_modbus.h"
#include "drv_uart.h"
#include "init.h"
#include "fifo.h"
#include "cmsis_os.h"
#include "u8tool.h"

#define LOG_TAG "sd710_dev"
#define LOG_OUTPUT_LEVEL  LOG_INFO
#include "log.h"


static void sd710_u8TOs16(uint16_t offest, uint8_t *savebuffer, int16_t *data);
static void sd710_u8TOu32(uint16_t offest,uint8_t* savebuffer,uint32_t* data);
static void sd710_s32TOu8(uint16_t offest,uint8_t* savebuffer,int32_t data);
static void sd710_u8TOs32(uint16_t offest, uint8_t *savebuffer, int32_t *data);


static void sd710_delayms(uint16_t ms)
{
	osDelay(ms);
}


void sd710_03h_unpack(void* this_p, uint8_t* buff)
{
	modbus_rtu_p mbr;
	if(this_p != NULL)
		mbr = this_p;
	else
		log_e("null pointer!");

	int32_t pos_temp;

	
	switch(mbr->addr)
	{
		case SD710_ADDRESS_1:
			
			break;
		case SD710_ADDRESS_2:
			
			break;
		
		default:
			break;
	}
	
	switch (mbr->reg)
	{		
		case SD710_REG_P00_00:		
			mbr->ack = 1;
			break;
		
		case SD710_REG_P00_01:		
			mbr->ack = 1;
			break;
		
		case SD710_REG_P03_00:		
			mbr->ack = 1;
			break;
		
		case SD710_REG_P03_04:		
			mbr->ack = 1;
			break;
		
		case SD710_REG_U00_00:	// 速度反馈，单位rpm
			mbr->ack = 1;
			sd710_u8TOs16(3, buff, &mbr->dev_speed_s);
			break;

		case SD710_REG_U00_89:	// 温度反馈，单位1℃
			mbr->ack = 1;
			sd710_u8TOs16(3, buff, &mbr->dev_temperature_s);
			break;

		case SD710_REG_U01_42:	// 平均负载率 0~4000 单位0.1%
			sd710_u8TOs16(3,buff,&mbr->dev_load_rate_s);
			mbr->ack = 1;
			break;

		case SD710_REG_U08_00:	// 故障码，0~0xffff
			sd710_u8TOs16(3,buff,&mbr->dev_error_code_s);
			mbr->ack = 1;
			break;
		
		case SD710_REG_U06_07:	//机械绝对位置【低 32 位】, 编码器单位
			sd710_u8TOs32(3,buff,&pos_temp);
			mbr->dev_pos_s = pos_temp;
			mbr->ack = 1;
			break;
		
		default:
			break;

	}
}

uint8_t sd710_read_test(modbus_rtu_p mbr)
{
		MBR_Read_03H_Block(mbr,mbr->dev_addr,SD710_REG_P00_00,1);
		return 1;
}

uint8_t sd710_read_pos(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr,mbr->dev_addr,SD710_REG_U06_07,2);

	if(pass == 0)
			fail_cnt++;
	else
			fail_cnt = 0;
	
	if(fail_cnt > 20)
			return 0;
	else
			return 1;
}


uint8_t sd710_speed_mode_config(modbus_rtu_p mbr)
{
	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P00_01,0x0000);			//关闭伺服使能
	sd710_delayms(1000);

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P00_85,0x0000);			//不写入eeprom

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P00_00,0x0001);			//设置为速度模式

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P03_00,0x0000);			//速度指令设置为内部给定

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P03_04,0);					//速度设置为0

	return 1;
}

uint8_t sd710_pos_mode_config(modbus_rtu_p mbr)													//注意：SD710_REG_P00_00和SD710_REG_P02_00为重新上电有效，故不能由程序在线切换控制模式
{
	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P00_01,0x0000);			//关闭伺服使能
	sd710_delayms(1000);

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P00_00,0x0000);			//设置为位置模式

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P00_85,0x0000);			//不写入eeprom

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P02_00,0x0003);			//位置指令设置为内部给定

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P08_00,0x0000);			//内部位置指令设定

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P08_02,0x0110);			//内部多段位置（速度）运行模式

	uint8_t data_buf[4];
	sd710_s32TOu8(0,data_buf,0);
	MBR_Write_10H_Block(mbr,mbr->dev_addr,SD710_REG_P02_04,2,data_buf);		//电子齿轮分子(N)，该功能码设定为 0 时，驱动器内部自动以编码器的分辨率设定电子齿轮分子。

	sd710_s32TOu8(0,data_buf,5000);
	MBR_Write_10H_Block(mbr,mbr->dev_addr,SD710_REG_P02_06,2,data_buf);		//电子齿轮分母（M）

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P02_11,200);			//位置指令低通滤波时间常数 原始值200

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P08_90,30);			//加减速时间（编号#0）原始值30

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P06_04,0x1020);			//将虚拟 X4 端子分配为内部位置触发端子 CTRG

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P08_10,0x0020);			//PR 路径 1 控制字 L
	
	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P08_A0,60000);			//内部目标速度设定(编号#0)

	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P08_06,1);			//Pr 指令通讯参数（单段运行）

	return 1;
	
}

uint8_t sd710_output_enable(modbus_rtu_p mbr)
{
	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P00_01,0x0001);			//开启伺服使能
	return 1;
}

uint8_t sd710_output_disable(modbus_rtu_p mbr)
{
	MBR_Write_06H_Block(mbr,mbr->dev_addr,SD710_REG_P00_01,0x0000);			//关闭伺服使能
	return 1;
}


uint8_t sd710_speed_set(modbus_rtu_p mbr,int16_t speed_cmd)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_WriteParam_06H(mbr,mbr->dev_addr,SD710_REG_P03_04,speed_cmd);

	if(pass == 0)
			fail_cnt++;
	else
			fail_cnt = 0;
	
	if(fail_cnt > 20)
			return 0;
	else
			return 1;
}

uint8_t sd710_pos_set(modbus_rtu_p mbr,int32_t pos_cmd)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;
	u8 data_buf[4];

	sd710_s32TOu8(0,data_buf,pos_cmd);

	pass = MBR_WriteParam_10H(mbr,mbr->dev_addr,SD710_REG_P08_12,2,data_buf);

	sd710_delayms(mbr->dev_mbr_delay);

	pass = MBR_WriteParam_06H(mbr,mbr->dev_addr,SD710_REG_P08_06,0x0001);			//Pr 指令通讯参数（单段运行）

	// sd710_delayms(mbr->dev_mbr_delay);

	// pass = MBR_WriteParam_06H(mbr,mbr->dev_addr,SD710_REG_P06_30,0x0008);			//内部软件给定输入端子（X）状态

	// sd710_delayms(mbr->dev_mbr_delay);

	// pass = MBR_WriteParam_06H(mbr,mbr->dev_addr,SD710_REG_P06_30,0x0000);			//内部软件给定输入端子（X）状态

	if(pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;
	
	if(fail_cnt > 20)
		return 0;
	else
		return 1;
}

uint8_t sd710_read_speed(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, SD710_REG_U00_00, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

uint8_t sd710_read_temperature(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, SD710_REG_U00_89, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

uint8_t sd710_read_average_load(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, SD710_REG_U01_42, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

uint8_t sd710_read_err(modbus_rtu_p mbr)
{
	uint8_t pass;
	static uint8_t fail_cnt = 0;

	pass = MBR_ReadParam_03H(mbr, mbr->dev_addr, SD710_REG_U08_00, 1);

	if (pass == 0)
		fail_cnt++;
	else
		fail_cnt = 0;

	if (fail_cnt > 20)
		return 0;
	else
		return 1;
}

static void sd710_s32TOu8(uint16_t offest,uint8_t* savebuffer,int32_t data)
{
	savebuffer += offest;
	*savebuffer = (uint8_t)(data >> 8);
	savebuffer +=1;
	*savebuffer = (uint8_t)(data);
	savebuffer +=1;
	*savebuffer = (uint8_t)(data >> 24);
	savebuffer +=1;
	*savebuffer = (uint8_t)(data >> 16);
}



static void sd710_u8TOu32(uint16_t offest,uint8_t* savebuffer,uint32_t* data)
{
	*data = (uint32_t)(*(savebuffer + offest + 1) | *(savebuffer + offest)<<8 | *(savebuffer + offest + 3)<<16 | *(savebuffer + offest + 2)<<24);
}

static void sd710_u8TOs16(uint16_t offest, uint8_t *savebuffer, int16_t *data)
{
	*data = (int16_t)(*(savebuffer + offest + 1) | *(savebuffer + offest) << 8);
}

static void sd710_u8TOs32(uint16_t offest, uint8_t *savebuffer, int32_t *data)
{
	*data = (int32_t)(*(savebuffer + offest + 1) | *(savebuffer + offest) << 8 | *(savebuffer + offest + 3) << 16 | *(savebuffer + offest + 2) << 24);
}


