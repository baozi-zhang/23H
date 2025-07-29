#include "freq_measure.h"

float freq1, freq2;
u32 freq_ad1, freq_base1;
u32 freq_ad2, freq_base2;

/**
 * @brief 测量频率的通用函数
 *
 * 此函数用于测量指定通道的频率。它清除采集数据，启动采集，采集一定时间后停止采集，
 * 读取频率值和基准频率值，计算实际频率。
 *
 * @param channel 通道号 (1: AD1, 2: AD2)
 * @param freq_ad 指向存储频率的变量的指针
 * @param freq_base 指向存储基准频率的变量的指针
 * @param freq_result 指向存储计算结果的变量的指针
 */
void fre_measure(int channel, u32 *freq_ad, u32 *freq_base, float *freq_result)
{
    // 清除采集数据
    AD_FREQ_CLR_ENABLE(channel);
    HAL_Delay(1);
    AD_FREQ_CLR_DISABLE(channel);
    HAL_Delay(1);

    // 启动采集
    AD_FREQ_START(channel);
    HAL_Delay(100); // 采集0.1秒
    AD_FREQ_STOP(channel);

    // 读取频率高低字
    u32 freq_h = (channel == 1) ? AD1_FREQ_H : AD2_FREQ_H;
    HAL_Delay(1);
    u32 freq_l = (channel == 1) ? AD1_FREQ_L : AD2_FREQ_L;
    HAL_Delay(1);
    *freq_ad = (freq_h << 16) + freq_l;

    // 读取基准频率高低字
    u32 base_h = (channel == 1) ? BASE1_FREQ_H : BASE2_FREQ_H;
    HAL_Delay(1);
    u32 base_l = (channel == 1) ? BASE1_FREQ_L : BASE2_FREQ_L;
    HAL_Delay(1);
    *freq_base = (base_h << 16) + base_l;

    // 如果采集频率或基准频率无效，直接返回
    if (*freq_ad == 0 || *freq_base <= 14000000.0)
    {
        *freq_result = 0;
        return;
    }

    // 计算实际频率
    *freq_result = 150000000.0 / *freq_base * *freq_ad;
}

/**
 * @brief 测量 AD1 的频率
 *
 * 调用通用函数 `fre_measure` 测量 AD1 的频率。
 */
void fre_measure_ad1(void)
{
    fre_measure(1, &freq_ad1, &freq_base1, &freq1);
}

/**
 * @brief 测量 AD2 的频率
 *
 * 调用通用函数 `fre_measure` 测量 AD2 的频率。
 */
void fre_measure_ad2(void)
{
    fre_measure(2, &freq_ad2, &freq_base2, &freq2);
}


void freq_proc(void)
{
	fre_measure_ad1();              // 测量AD1的频率
  fre_measure_ad2();              // 测量AD2的频率
}
