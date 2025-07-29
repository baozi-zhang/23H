/**
 * @file test_dual_peak.h
 * @brief 双峰检测功能测试程序头文件
 * @author Project Team
 * @date 2024
 */

#ifndef __TEST_DUAL_PEAK_H
#define __TEST_DUAL_PEAK_H

#include "bsp_system.h"

// 测试函数声明
void generate_dual_sine_signal(float freq1, float amp1, float freq2, float amp2, 
                               float sampling_freq, float noise_level);
void test_dual_peak_detection(void);
void test_single_peak_detection(void);
void test_frequency_accuracy(void);
void run_all_dual_peak_tests(void);

#endif /* __TEST_DUAL_PEAK_H */