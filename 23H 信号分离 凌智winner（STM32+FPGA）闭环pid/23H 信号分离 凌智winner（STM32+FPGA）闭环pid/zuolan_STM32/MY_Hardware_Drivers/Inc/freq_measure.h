#ifndef __FREQ_MEASURE_H__
#define __FREQ_MEASURE_H__
#include "commond_init.h"
#include "cmd_to_fun.h"
#include "bsp_system.h"
extern float freq1, freq2;
void fre_measure_ad1(void);
void fre_measure_ad2(void);
void freq_proc(void);
#endif // __FREQ_MEASURE_H__
