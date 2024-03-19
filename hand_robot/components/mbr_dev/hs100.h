/*
 * @Author: lhq
 * @Date: 2023-07-03 16:55:17
 * @LastEditors: lhq
 * @LastEditTime: 2023-08-09 16:28:56
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\components\mbr_dev\hs100.h
 * @Description:
 *
 * Copyright (c) 2023 by lhq, All Rights Reserved.
 */
#ifndef __HS100_H__
#define __HS100_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "proj_config.h"
#include "drv_modbus.h"

#define HS100_ADDRESS_1 0x01
#define HS100_ADDRESS_2 0x02

#define HS100_REG_P00_02 0x0002 // 伺服运行模式选择， 0：位置模式		1：速度模式		2：转矩模式		3：位置模式->速度模式		4：位置模式->转矩模式	 5：速度模式->转矩模式
#define HS100_REG_P01_20 0x0120 // 速度指令选择，0：数字给定		1：AI1给定		2：AI2给定		3：速度规划
#define HS100_REG_P01_19 0x0119 // 位置规划指令类型 0:执行位置指令规划后的最终位置为当前驱动器的绝对位置   1:执行位置指令规划后的最终位置为执行前的相对增量位置
#define HS100_REG_P01_21 0x0121 // 速度给定 -6000~6000
#define HS100_REG_P01_36 0x0136 // 速度规划模式选择  0：单次运行 从初始规划组号到结束规划组号的位置指令运行一遍后停止。1：连续运行 从初始规划组号到结束规划组号的位置指令运行一遍后再按此顺序不断重复运行。

#define HS100_REG_P01_40 0x0140 // 转矩指令选择，0：数字给定		1：AI1给定		2：AI2给定
#define HS100_REG_P01_41 0x0141 // 转矩给定		-4000~4000

#define HS100_REG_P04_00 0x0400 // 第一组规划速度
#define HS100_REG_P04_02 0x0402 // 第一组规划运行时间
#define HS100_REG_P04_04 0x0404 // 第一组规划加速时间
#define HS100_REG_P04_06 0x0406 // 第一组规划减速时间

#define HS100_REG_P05_20 0x0520 // 位置指令滑动平均滤波时间常数A
#define HS100_REG_P05_22 0x0522 // 位置指令低通滤波时间常数A

#define HS100_REG_F31_41 0x3141
#define HS100_REG_P01_00 0x0100
#define HS100_REG_P03_00 0x0300 // 第一组规划位移，32位寄存器
#define HS100_REG_P03_02 0x0302 // 第一组规划速度，16位寄存器
#define HS100_REG_P03_04 0x0304 // 第一组规划加速时间
#define HS100_REG_P03_06 0x0306 // 第一组规划减速时间

#define HS100_REG_P01_1A 0x011A // 位置规划指令更新方式		0：缓存更新  1:立即更新
#define HS100_REG_P01_18 0x0118 // 位置规划模式选择		0：单次运行		1：连续运行		2：DI选择运行		3：轮廓轨迹模式运行（DI功能13触发启动,可实时修改第一段位置指令并根据PO1-1A选择的更新方式连续运行）
#define HS100_REG_P0A_05 0x0A05 // Modbus通讯存储选择		0：不存入eeprom		1：存入eeprom

#define HS100_REG_P1E_10 0x1E10 // 故障码，0~0xffff

#define HS100_REG_D40_00 0x4000 // 位置指令-9999~9999
#define HS100_REG_D40_01 0x4001 // 位置反馈-9999~9999
#define HS100_REG_D40_02 0x4002 // 速度指令，-9000~9000，单位rpm
#define HS100_REG_D40_03 0x4003 // 速度反馈，-9000~9000，单位rpm
#define HS100_REG_D40_04 0x4004 // 转矩指令，-4000~4000，单位0.1%
#define HS100_REG_D40_05 0x4005 // 转矩反馈，-4000~4000，单位0.1%
#define HS100_REG_D40_08 0x4008 // 电气角度，0~36000，单位0.01度
#define HS100_REG_D40_09 0x4009 // 机械角度，0~36000，单位0.01度
#define HS100_REG_D40_0C 0x400C // 位置偏差计数器，-2147483648~2147483647，单位p
#define HS100_REG_D40_0E 0x400E // 输入脉冲计数器，-2147483648~2147483647，用户单位
#define HS100_REG_D40_10 0x4010 // 绝对位置指令，-2147483648~2147483647,用户单位
#define HS100_REG_D40_12 0x4012 // 绝对位置反馈，码盘单位
#define HS100_REG_D40_14 0x4014 // 绝对位置指令,-2147483648~2147483647,单位p
#define HS100_REG_D40_16 0x4016 // 绝对位置反馈，-2147483648~2147483647,单位p
#define HS100_REG_D40_18 0x4018 // 编码器单圈位置,-2147483648~2147483647,单位p
#define HS100_REG_D40_1A 0x401A // 编码器位置低32位,-2147483648~2147483647,单位p
#define HS100_REG_D40_1C 0x401C // 编码器位置高32位,-2147483648~2147483647,单位p
#define HS100_REG_D40_1E 0x401E // 编码器多圈圈数,0~65535,单位圈
#define HS100_REG_D40_20 0x4020 // 母线电压,0~9000,单位0.1V
#define HS100_REG_D40_21 0x4021 // U相电流有效值,-9000~9000,单位0.1A
#define HS100_REG_D40_22 0x4022 // V相电流有效值,-9000~9000,单位0.1A
#define HS100_REG_D40_23 0x4023 // 平均负载率 0~4000 单位0.1%
#define HS100_REG_D40_24 0x4024 // IGBT温度 0~9000 单位0.1度
#define HS100_REG_D40_30 0x4030 // 离线惯量辨识值 0~12000 单位0.01

#define HS100_REG_D41_0A 0x410A // 参数异常的组号 0~0xff
#define HS100_REG_D41_0B 0x410B // 参数异常的偏置 0~0xff

#define HS100_REG_P07_10 0x0710 // DI1功能选择
#define HS100_REG_P07_21 0x0721 // DI5逻辑选择

#define HS100_REG_P0A_03 0x0A03 // MODBUS通讯应答时间 1~1000

    void hs100_03h_unpack(void *this_p, uint8_t *buff);

    uint8_t hs100_pos_mode_config(modbus_rtu_p mbr);
    uint8_t hs100_speed_mode_config(modbus_rtu_p mbr);
    uint8_t hs100_torque_mode_config(modbus_rtu_p mbr);

    uint8_t hs100_speed_set(modbus_rtu_p mbr, int16_t speed_cmd);
    uint8_t hs100_pos_set(modbus_rtu_p mbr, int32_t pos_cmd);
    // HS伺服在位置模式下的速度给定		0~6000
    uint8_t hs100_pos_mode_speed_set(modbus_rtu_p mbr, int16_t speed);
    uint8_t hs100_torque_set(modbus_rtu_p mbr, int16_t torque_cmd);

    uint8_t hs100_read_pos(modbus_rtu_p mbr);
    uint8_t hs100_output_enable(modbus_rtu_p mbr);

    uint8_t hs100_read_test(modbus_rtu_p mbr);

    // HS100伺服速度模式配置，内含阻塞操作，只能在初始化阶段调用（规划模式）
    uint8_t hs100_speed_mode_config_plan(modbus_rtu_p mbr);
    // HS伺服在速度模式下设置速度给定（规划模式）		-6000~6000
    uint8_t hs100_speed_set_plan(modbus_rtu_p mbr, int16_t speed_cmd);
    uint8_t hs100_di13_trigger(modbus_rtu_p mbr);

    uint8_t hs100_pos_mode_set_speed_block(modbus_rtu_p mbr, uint16_t speed);

    uint8_t hs100_pos_set_block(modbus_rtu_p mbr, int32_t pos_cmd);
    uint8_t hs100_read_speed(modbus_rtu_p mbr);
    uint8_t hs100_read_average_load(modbus_rtu_p mbr);
    uint8_t hs100_read_temperature(modbus_rtu_p mbr);
    uint8_t hs100_read_err(modbus_rtu_p mbr);


#ifdef __cplusplus
}
#endif

#endif // __HS100_H__
