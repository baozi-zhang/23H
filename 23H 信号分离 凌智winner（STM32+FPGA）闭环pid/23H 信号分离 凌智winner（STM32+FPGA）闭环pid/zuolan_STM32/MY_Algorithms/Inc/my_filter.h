#ifndef __MY_FILTER_H__
#define __MY_FILTER_H__
#include "arm_math.h"
#include "commond_init.h"
void arm_fir_f32_lp(const float *input_array, int length, float *output_array);
#endif // __MY_FILTER_H__
