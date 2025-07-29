#ifndef APP_APP_PID_H_
#define APP_APP_PID_H_

#include "bsp_system.h"

/* PID��ز��� */
#define INCR_LOCT_SELECT 1          /* 0��λ��ʽ ��1������ʽ */
#if INCR_LOCT_SELECT
/* ����ʽPID������غ� */
#define KP 0.8f                /* P����*/
#define KI 0.7f                /* I����*/
#define KD 0.20f                /* D����*/
#define SMAPLSE_PID_SPEED 1    /* �������� ��λms*/
#else
/* λ��ʽPID������غ� */
#define KP 10.0f                /* P����*/
#define KI 0.5f                 /* I����*/
#define KD 6.00f                /* D����*/
#define SMAPLSE_PID_SPEED 10    /* �������� ��λms*/
#endif
/* PID�����ṹ�� */
typedef struct
{
    __IO float SetPoint;            /* �趨Ŀ�� */
    __IO float ActualValue;         /* �������ֵ */
    __IO float SumError;            /* ����ۼ� */
    __IO float Proportion;          /* �������� P */
    __IO float Integral;            /* ���ֳ��� I */
    __IO float Derivative;          /* ΢�ֳ��� D */
    __IO float Error;               /* Error[1] */
    __IO float LastError;           /* Error[-1] */
    __IO float PrevError;           /* Error[-2] */
} PID_TypeDef;


void Pid_Proc(void);
void PID_Init(void);

#endif /* APP_APP_PID_H_ */
