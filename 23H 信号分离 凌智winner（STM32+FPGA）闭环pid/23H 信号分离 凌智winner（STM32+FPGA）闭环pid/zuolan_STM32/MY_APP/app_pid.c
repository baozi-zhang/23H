#include "app_pid.h"

PID_TypeDef PID;             // PID������ʵ��

void PID_Init(void)
{
    // ��ʼ��PID����
    PID.SetPoint = 3.0f;    // Ŀ���ѹ 30V
    PID.ActualValue = 0.0f;
    PID.SumError = 0.0f;
    PID.Proportion = KP;
    PID.Integral = KI;
    PID.Derivative = KD;
    PID.Error = 0.0f;
    PID.LastError = 0.0f;
    PID.PrevError = 0.0f;

}

/**
 * @brief       pid�ջ�����
 * @param       *PID��PID�ṹ�������ַ
 * @param       Feedback_value����ǰʵ��ֵ
 * @retval      �������ֵ
 */
//����ʽ PID ������ǵ����������Լ����������ֵ ActualValue ��ʱ�����������㣬��һ���λ��ʽ PID �ǲ�һ����
int32_t increment_pid_ctrl(PID_TypeDef *PID, float Feedback_value)
{
    PID->Error = (float)(PID->SetPoint - Feedback_value); /* ����ƫ�� */

#if INCR_LOCT_SELECT /* ����ʽPID */

    PID->ActualValue += (PID->Proportion * (PID->Error - PID->LastError)) /* �������� */
                        + (PID->Integral * PID->Error) /* ���ֻ��� */
                        + (PID->Derivative * (PID->Error - 2 * PID->LastError + PID->PrevError)); /* ΢�ֻ��� */

    PID->PrevError = PID->LastError; /* �洢ƫ������´μ��� */
    PID->LastError = PID->Error;

#else /* λ��ʽPID */

    PID->SumError += PID->Error;
    PID->ActualValue = (PID->Proportion * PID->Error) /* �������� */
                       + (PID->Integral * PID->SumError) /* ���ֻ��� */
                       + (PID->Derivative * (PID->Error - PID->LastError)); /* ΢�ֻ��� */
    PID->LastError = PID->Error;

#endif
    return ((int32_t)(PID->ActualValue)); /* ���ؼ�����������ֵ */
}

int32_t output=38;
float vin;
void Pid_Proc(void)
{


    vin=vol_amp2/2*10;
    output = increment_pid_ctrl(&PID,vin);
    if(output>1023)
			output=1023;
		else if(output<38)
			output=38;
    pid_vin=output;
}




