#include "phase_measure.h"

float phase_diff; // 保存相位差（单位：角度）

/**
 * @brief 过零点检测法计算相位差
 * @param fifo_data1_f AD1 的采样数据数组
 * @param fifo_data2_f AD2 的采样数据数组
 */
void calculate_phase_diff(float *fifo_data1_f, float *fifo_data2_f,int lengh)
{
    // 初始化变量
    int zero_cross1 = -1, zero_cross2 = -1; // 两路信号的过零点索引
    int i;

    // 遍历寻找过零点
    for (i = 1; i < lengh; i++)
    {
        // AD1 过零点检测
        if (zero_cross1 == -1 && fifo_data1_f[i] > 0 && fifo_data1_f[i - 1] <= 0)
        {
            zero_cross1 = i;
        }

        // AD2 过零点检测
        if (zero_cross2 == -1 && fifo_data2_f[i] > 0 && fifo_data2_f[i - 1] <= 0)
        {
            zero_cross2 = i;
        }

        // 如果都找到过零点，提前退出
        if (zero_cross1 != -1 && zero_cross2 != -1)
        {
            break;
        }
    }

    // 判断是否找到过零点，避免计算错误
    if (zero_cross1 == -1 || zero_cross2 == -1)
    {
        phase_diff = 0.0f; // 如果没有找到过零点，相位差置为 0
        return;
    }

    // 计算时间差
    int delta_t = zero_cross2 - zero_cross1;

    // 将时间差转换为相位差（单位：角度）
    phase_diff = ((float)delta_t / (float)lengh) * 360.0f;

    // 修正相位差范围 [-180, 180]
    if (phase_diff > 180.0f)
    {
        phase_diff -= 360.0f;
    }
    else if (phase_diff < -180.0f)
    {
        phase_diff += 360.0f;
    }
}
