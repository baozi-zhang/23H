#include "app_pid.h"

PID_TypeDef PID;             // PID控制器实例

void PID_Init(void)
{
    // 初始化PID参数
    PID.SetPoint = 3.0f;    // 目标电压 30V
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
 * @brief       pid闭环控制
 * @param       *PID：PID结构体变量地址
 * @param       Feedback_value：当前实际值
 * @retval      期望输出值
 */
//增量式 PID 输出的是调节量，所以计算期望输出值 ActualValue 的时候是自增运算，这一点和位置式 PID 是不一样的
int32_t increment_pid_ctrl(PID_TypeDef *PID, float Feedback_value)
{
    PID->Error = (float)(PID->SetPoint - Feedback_value); /* 计算偏差 */

#if INCR_LOCT_SELECT /* 增量式PID */

    PID->ActualValue += (PID->Proportion * (PID->Error - PID->LastError)) /* 比例环节 */
                        + (PID->Integral * PID->Error) /* 积分环节 */
                        + (PID->Derivative * (PID->Error - 2 * PID->LastError + PID->PrevError)); /* 微分环节 */

    PID->PrevError = PID->LastError; /* 存储偏差，用于下次计算 */
    PID->LastError = PID->Error;

#else /* 位置式PID */

    PID->SumError += PID->Error;
    PID->ActualValue = (PID->Proportion * PID->Error) /* 比例环节 */
                       + (PID->Integral * PID->SumError) /* 积分环节 */
                       + (PID->Derivative * (PID->Error - PID->LastError)); /* 微分环节 */
    PID->LastError = PID->Error;

#endif
    return ((int32_t)(PID->ActualValue)); /* 返回计算后输出的数值 */
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




