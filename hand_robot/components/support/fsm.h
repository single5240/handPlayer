/*
 * @Author: lhq
 * @Date: 2023-06-21 19:45:21
 * @LastEditors: lhq
 * @LastEditTime: 2023-08-07 19:56:08
 * @FilePath: \MDK-ARMe:\cube\SL_Receiver\components\support\fsm.h
 * @Description: 
 * 
 * Copyright (c) 2023 by lhq, All Rights Reserved. 
 */
#ifndef _FSM_H_
#define _FSM_H_
 
#include <stdint.h>
#include <stddef.h>

 #pragma pack(push, 1) // 将编译器自己的对齐方式先压入栈保存起来，并告诉编译器下面的就按照1字节对齐
typedef struct FsmTable_s
{
    uint8_t event;                /* 触发事件 */
    uint8_t CurState;             /* 当前状态 */
    void (*eventActFun)(void *);  /* 动作函数 */
    uint8_t NextState;            /* 跳转状态 */
}FsmTable_T;
 
typedef struct FSM_s
{
    FsmTable_T *FsmTable;         /* 状态迁移表 */
    uint8_t curState;             /* 状态机当前状态 */
    uint16_t stuMaxNum;            /* 状态机状态迁移数量 */
}FSM_T;
#pragma pack(pop) // 将原来的对齐方式在释放出来。




 
/*********************************************************************************
使用方法：1.创建FSM_T对象；
          2.创建FsmTable_T表；
          3.调用FSM_Init()初始化；
          4.程序轮询FSM_EventHandle()运行状态机。
*********************************************************************************/
void FSM_Init(FSM_T *pFsm, FsmTable_T *pTable, uint16_t stuMaxNum, uint8_t curState);
void FSM_EventHandle(FSM_T *pFsm, uint8_t event, void *parm);
 
#endif

