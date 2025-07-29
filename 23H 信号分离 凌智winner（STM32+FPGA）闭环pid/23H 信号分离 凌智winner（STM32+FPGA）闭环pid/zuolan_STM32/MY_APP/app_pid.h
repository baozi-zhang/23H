#ifndef APP_APP_PID_H_
#define APP_APP_PID_H_

#include "bsp_system.h"

/* PID相关参数 */
#define INCR_LOCT_SELECT 1          /* 0：位置式 ，1：增量式 */
#if INCR_LOCT_SELECT
/* 增量式PID参数相关宏 */
#define KP 0.8f                /* P参数*/
#define KI 0.7f                /* I参数*/
#define KD 0.20f                /* D参数*/
#define SMAPLSE_PID_SPEED 1    /* 采样周期 单位ms*/
#else
/* 位置式PID参数相关宏 */
#define KP 10.0f                /* P参数*/
#define KI 0.5f                 /* I参数*/
#define KD 6.00f                /* D参数*/
#define SMAPLSE_PID_SPEED 10    /* 采样周期 单位ms*/
#endif
/* PID参数结构体 */
typedef struct
{
    __IO float SetPoint;            /* 设定目标 */
    __IO float ActualValue;         /* 期望输出值 */
    __IO float SumError;            /* 误差累计 */
    __IO float Proportion;          /* 比例常数 P */
    __IO float Integral;            /* 积分常数 I */
    __IO float Derivative;          /* 微分常数 D */
    __IO float Error;               /* Error[1] */
    __IO float LastError;           /* Error[-1] */
    __IO float PrevError;           /* Error[-2] */
} PID_TypeDef;


void Pid_Proc(void);
void PID_Init(void);

#endif /* APP_APP_PID_H_ */
