#ifndef __MY_FFT_H
#define __MY_FFT_H

#include "bsp_system.h"
#include "arm_math.h"

#define FFT_LENGTH 1024
#define MAX_PEAKS 10  // 最大峰值数量

// 峰值结构体
typedef struct {
    uint16_t bin_index;      // FFT bin索引
    float frequency;         // 频率值
    float magnitude;         // 幅度值
    float precise_frequency; // 精确频率（插值后）
} peak_info_t;

// 双峰检测结果结构体
typedef struct {
    peak_info_t peak1;       // 第一个峰值
    peak_info_t peak2;       // 第二个峰值
    uint8_t peaks_found;     // 找到的峰值数量
    float freq_separation;   // 两个峰值的频率间隔
} dual_peak_result_t;

extern float fft_input_buffer[FFT_LENGTH * 2]; // 复数输入缓冲区
extern float fft_magnitude[FFT_LENGTH];
extern float window_buffer[FFT_LENGTH]; // 窗函数缓冲区
extern dual_peak_result_t dual_peaks;   // 双峰检测结果

// 全局变量储存双峰检测结果
extern float peak1_frequency;   // 第一个峰值的频率 (Hz)
extern float peak1_magnitude;   // 第一个峰值的幅度 (V)
extern float peak2_frequency;   // 第二个峰值的频率 (Hz)
extern float peak2_magnitude;   // 第二个峰值的幅度 (V)

void fft_init(void);
void calculate_fft_spectrum(float* input_data, uint16_t data_length);
void output_fft_spectrum(void);
void generate_hanning_window(void);
float get_precise_peak_frequency(float sampling_freq);
float round_to_nearest_k(float frequency);
float calculate_thd(float fundamental_freq, float sampling_freq);

// 新增寻峰功能函数
uint8_t find_spectrum_peaks(peak_info_t* peaks, uint8_t max_peaks, float sampling_freq, float min_threshold);
dual_peak_result_t find_dual_peaks(float sampling_freq, float min_threshold);
float calculate_precise_frequency(uint16_t bin_index, float sampling_freq);
void output_dual_peaks_info(void);
void perform_dual_peak_analysis(float* input_data, uint16_t data_length);

// 全局变量访问函数
float get_peak1_frequency(void);
float get_peak1_magnitude(void);
float get_peak2_frequency(void);
float get_peak2_magnitude(void);

// DA输出配置函数
void configure_da_output_from_peaks(void);

#endif /*__FFT_H*/
