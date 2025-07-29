#include "ad_measure.h"
#include "stm32f4xx_hal.h"

#define FREQ_CONSTANT 4294967296.0f
#define CLOCK_FREQ 150000000.0f
#define ADC_SCALE 2048.0f
#define VOLTAGE_OFFSET 10.0f

u16 vol_maxnum1, vol_minnum1, vol_maxnum2, vol_minnum2;
u16 fifo_data1[FIFO_SIZE], fifo_data2[FIFO_SIZE];       // 采样结果
float fifo_data1_f[FIFO_SIZE], fifo_data2_f[FIFO_SIZE]; // 采样结果转换为浮点数
float vol_amp1, vol_amp2;

/**
 * @brief 查找数组中的最大和最小值
 *
 * @param numbers 数字数组指针
 * @param size 数组大小
 * @param max 输出最大值指针
 * @param min 输出最小值指针
 */
void findMinMax(u16 *numbers, int size, u16 *max, u16 *min)
{
    *max = numbers[0];
    *min = numbers[0];
    for (int i = 1; i < size; i++)
    {
        if (numbers[i] > *max)
        {
            *max = numbers[i];
        }
        if (numbers[i] < *min)
        {
            *min = numbers[i];
        }
    }
}

/**
 * @brief 设置 ADC 采样频率
 *
 * @param fre 采样频率（单位：Hz）
 * @param channel 通道号
 */
void setSamplingFrequency(float fre, int channel)
{
    if (fre == 0)
        return;

    unsigned int fs = 0;
    if (fre <= 60000)
        fs = fre * FIFO_SIZE_N;
    else // 欠采样
        fs = fre * FIFO_SIZE_N / (float)(FIFO_SIZE_N + 1);

    unsigned int M = FREQ_CONSTANT * fs / CLOCK_FREQ;

    // 设置频率控制字
    if (channel == 1)
    {
        AD_FREQ_SET(1);
        HAL_Delay(1);
        AD1_FS_H = M >> 16;
        HAL_Delay(1);
        AD1_FS_L = M & 0xFFFF;
    }
    else
    {
        AD_FREQ_SET(2);
        HAL_Delay(1);
        AD2_FS_H = M >> 16;
        HAL_Delay(1);
        AD2_FS_L = M & 0xFFFF;
    }
}

/**
 * @brief 读取 FIFO 数据
 *
 * @param channel 通道号
 * @param fifo_data FIFO 数据缓冲区
 * @param fifo_data_f 浮点数形式的 FIFO 数据缓冲区
 */
void readFIFOData(int channel, u16 *fifo_data, float *fifo_data_f)
{
    // 开启 FIFO 读取
    if (channel == 1)
        AD_FIFO_READ_ENABLE(1);
    else
        AD_FIFO_READ_ENABLE(2);

    HAL_Delay(1);
    for (int i = 0; i < FIFO_SIZE; i++)
    {
        if (channel == 1)
            fifo_data[i] = AD1_DATA_SHOW;
        else
            fifo_data[i] = AD2_DATA_SHOW;

        // 转换为浮点数并使用公式计算电压值
        fifo_data_f[i] = fifo_data[i] * VOLTAGE_OFFSET / ADC_SCALE - VOLTAGE_OFFSET;
				
    }

    // 关闭 FIFO 读取
    if (channel == 1)
        AD_FIFO_READ_DISABLE(1);
    else
        AD_FIFO_READ_DISABLE(2);
}

/**
 * @brief 同时设置 AD1 和 AD2 的采样频率并读取数据
 *
 * @param ad1_freq AD1 的采样频率（单位：Hz），为 0 时不采集
 * @param ad2_freq AD2 的采样频率（单位：Hz），为 0 时不采集
 */
void vpp_adc_parallel(float ad1_freq, float ad2_freq)
{
    // 设置 AD1 和 AD2 的采样频率控制字
    if (ad1_freq > 0)
    {
        setSamplingFrequency(ad1_freq, 1);
        AD_FIFO_WRITE_ENABLE(1); // 开启 AD1 写入
    }

    if (ad2_freq > 0)
    {
        setSamplingFrequency(ad2_freq, 2);
        AD_FIFO_WRITE_ENABLE(2); // 开启 AD2 写入
    }

    HAL_Delay(1); // 等待频率设置完成

    // 等待 FIFO 写满
    if (ad1_freq > 0)
    {
        while (AD1_FULL_FLAG != 0x0001)
            ;                     // 等待 AD1 FIFO 写满
        AD_FIFO_WRITE_DISABLE(1); // 关闭 AD1 写入
    }

    if (ad2_freq > 0)
    {
        while (AD2_FULL_FLAG != 0x0001)
            ;                     // 等待 AD2 FIFO 写满
        AD_FIFO_WRITE_DISABLE(2); // 关闭 AD2 写入
    }

    // 读取 AD1 和 AD2 的 FIFO 数据
    if (ad1_freq > 0)
    {
        readFIFOData(1, fifo_data1, fifo_data1_f);
        findMinMax(fifo_data1, FIFO_SIZE, &vol_maxnum1, &vol_minnum1);
        vol_amp1 = (vol_maxnum1 - vol_minnum1) * VOLTAGE_OFFSET / ADC_SCALE;
    }

    if (ad2_freq > 0)
    {
        readFIFOData(2, fifo_data2, fifo_data2_f);
        findMinMax(fifo_data2, FIFO_SIZE, &vol_maxnum2, &vol_minnum2);
        vol_amp2 = (vol_maxnum2 - vol_minnum2) * VOLTAGE_OFFSET / ADC_SCALE;
    }
}


void ad_proc(void)
{
	 
   vpp_adc_parallel(2000000, 2000000);   
	 
	 
	 
}


