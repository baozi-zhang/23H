/**
 * @file test_dual_peak.c
 * @brief 双峰检测功能测试程序
 * @author Project Team
 * @date 2024
 */

#include "my_fft.h"
#include "my_usart.h"
#include <math.h>

// 测试数据生成
static float test_signal[FFT_LENGTH];

/**
 * @brief 生成测试信号：两个正弦波相加
 * @param freq1 第一个正弦波频率 (Hz)
 * @param amp1 第一个正弦波幅度
 * @param freq2 第二个正弦波频率 (Hz)
 * @param amp2 第二个正弦波幅度
 * @param sampling_freq 采样频率 (Hz)
 * @param noise_level 噪声水平 (0-1)
 */
void generate_dual_sine_signal(float freq1, float amp1, float freq2, float amp2, 
                               float sampling_freq, float noise_level)
{
    float dt = 1.0f / sampling_freq;
    
    for(uint16_t i = 0; i < FFT_LENGTH; i++)
    {
        float t = i * dt;
        
        // 生成两个正弦波
        float signal1 = amp1 * sinf(2.0f * 3.14159265f * freq1 * t);
        float signal2 = amp2 * sinf(2.0f * 3.14159265f * freq2 * t);
        
        // 添加噪声（简单的随机噪声）
        float noise = noise_level * ((float)rand() / RAND_MAX - 0.5f);
        
        test_signal[i] = signal1 + signal2 + noise;
    }
}

/**
 * @brief 测试双峰检测功能
 */
void test_dual_peak_detection(void)
{
    my_printf(&huart1, "\r\n=== 双峰检测功能测试 ===\r\n");
    
    // 设置测试参数
    float sampling_freq = 10000.0f;  // 10kHz采样频率
    set_current_ad_frequency(sampling_freq);
    
    // 测试用例1：1kHz + 3kHz
    my_printf(&huart1, "\r\n--- 测试用例1：1kHz + 3kHz ---\r\n");
    generate_dual_sine_signal(1000.0f, 1.0f, 3000.0f, 0.7f, sampling_freq, 0.05f);
    perform_dual_peak_analysis(test_signal, FFT_LENGTH);
    
    // 测试用例2：500Hz + 2.5kHz
    my_printf(&huart1, "\r\n--- 测试用例2：500Hz + 2.5kHz ---\r\n");
    generate_dual_sine_signal(500.0f, 0.8f, 2500.0f, 1.2f, sampling_freq, 0.1f);
    perform_dual_peak_analysis(test_signal, FFT_LENGTH);
    
    // 测试用例3：幅度相近的两个频率
    my_printf(&huart1, "\r\n--- 测试用例3：800Hz + 1.2kHz (幅度相近) ---\r\n");
    generate_dual_sine_signal(800.0f, 1.0f, 1200.0f, 1.1f, sampling_freq, 0.02f);
    perform_dual_peak_analysis(test_signal, FFT_LENGTH);
    
    // 测试用例4：频率很接近的情况
    my_printf(&huart1, "\r\n--- 测试用例4：1kHz + 1.1kHz (频率接近) ---\r\n");
    generate_dual_sine_signal(1000.0f, 1.0f, 1100.0f, 0.9f, sampling_freq, 0.05f);
    perform_dual_peak_analysis(test_signal, FFT_LENGTH);
    
    my_printf(&huart1, "\r\n=== 测试完成 ===\r\n");
}

/**
 * @brief 测试单一峰值检测
 */
void test_single_peak_detection(void)
{
    my_printf(&huart1, "\r\n=== 单峰检测测试 ===\r\n");
    
    float sampling_freq = 10000.0f;
    set_current_ad_frequency(sampling_freq);
    
    // 生成单一正弦波
    for(uint16_t i = 0; i < FFT_LENGTH; i++)
    {
        float t = i / sampling_freq;
        test_signal[i] = 1.5f * sinf(2.0f * 3.14159265f * 1500.0f * t);
    }
    
    perform_dual_peak_analysis(test_signal, FFT_LENGTH);
    
    my_printf(&huart1, "=== 单峰测试完成 ===\r\n");
}

/**
 * @brief 验证频率精度
 */
void test_frequency_accuracy(void)
{
    my_printf(&huart1, "\r\n=== 频率精度测试 ===\r\n");
    
    float sampling_freq = 20000.0f;  // 更高的采样频率
    set_current_ad_frequency(sampling_freq);
    
    // 测试精确的频率值
    float test_freq1 = 1234.5f;
    float test_freq2 = 4567.8f;
    
    my_printf(&huart1, "目标频率: %.1f Hz + %.1f Hz\r\n", test_freq1, test_freq2);
    
    generate_dual_sine_signal(test_freq1, 1.0f, test_freq2, 0.8f, sampling_freq, 0.01f);
    perform_dual_peak_analysis(test_signal, FFT_LENGTH);
    
    my_printf(&huart1, "=== 精度测试完成 ===\r\n");
}

/**
 * @brief 运行所有测试
 */
void run_all_dual_peak_tests(void)
{
    my_printf(&huart1, "\r\n##### 双峰检测完整测试套件 #####\r\n");
    
    // 初始化FFT模块
    fft_init();
    
    // 运行各项测试
    test_dual_peak_detection();
    test_single_peak_detection();
    test_frequency_accuracy();
    
    my_printf(&huart1, "\r\n##### 所有测试完成 #####\r\n");
}