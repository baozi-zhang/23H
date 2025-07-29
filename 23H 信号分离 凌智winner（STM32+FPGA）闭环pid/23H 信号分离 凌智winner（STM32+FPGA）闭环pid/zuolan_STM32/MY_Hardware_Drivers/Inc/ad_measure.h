#ifndef __AD_H__
#define __AD_H__
#include "commond_init.h"
#include "cmd_to_fun.h"
#include "bsp_system.h"
extern float fifo_data1_f[FIFO_SIZE], fifo_data2_f[FIFO_SIZE]; // 采样结果转换为浮点数
extern float vol_amp1, vol_amp2;
void vpp_adc_parallel(float ad1_freq, float ad2_freq);
void ad_proc(void);
#endif //__AD_H__
