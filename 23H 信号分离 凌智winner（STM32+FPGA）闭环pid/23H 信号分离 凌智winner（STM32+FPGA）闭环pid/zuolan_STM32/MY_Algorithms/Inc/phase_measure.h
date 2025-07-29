#ifndef __PHASE_MEASURE_H__
#define __PHASE_MEASURE_H__
#include "ad_measure.h"
#include "commond_init.h"
extern float phase_diff; // 相位差
void calculate_phase_diff(float *fifo_data1_f, float *fifo_data2_f, int lengh);
#endif // __PHASE_MEASURE_H__
