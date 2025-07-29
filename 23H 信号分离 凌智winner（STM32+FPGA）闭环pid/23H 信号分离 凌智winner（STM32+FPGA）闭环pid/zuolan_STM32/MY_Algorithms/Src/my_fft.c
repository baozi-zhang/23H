#include "my_fft.h"
#include "da_output.h"
#include <math.h>
#include <stdlib.h>

// FFT相关变量
arm_cfft_radix4_instance_f32 fft_instance;
float fft_input_buffer[FFT_LENGTH * 2]; // 复数输入缓冲区，实部和虚部交错存储
float fft_magnitude[FFT_LENGTH];
float window_buffer[FFT_LENGTH]; // 窗函数缓冲区
dual_peak_result_t dual_peaks;   // 双峰检测结果

// 全局变量储存双峰检测结果
float peak1_frequency = 0.0f;   // 第一个峰值的频率 (Hz)
float peak1_magnitude = 0.0f;   // 第一个峰值的幅度 (V)
float peak2_frequency = 0.0f;   // 第二个峰值的频率 (Hz)
float peak2_magnitude = 0.0f;   // 第二个峰值的幅度 (V)

/**
 * @brief FFT模块初始化
 */
/**
 * @brief 生成Hanning窗函数
 */
void generate_hanning_window(void)
{
    uint16_t i;
    for(i = 0; i < FFT_LENGTH; i++)
    {
        window_buffer[i] = 0.5f * (1.0f - arm_cos_f32(2.0f * 3.14159265f * i / (FFT_LENGTH - 1)));
    }
}

void fft_init(void)
{
    // 初始化radix4 FFT实例
    arm_cfft_radix4_init_f32(&fft_instance, FFT_LENGTH, 0, 1);
    // 参数说明：
    // fftLen: FFT长度 (1024)
    // ifftFlag: 0=FFT, 1=IFFT
    // bitReverseFlag: 1=输出按正常顺序
    
    // 生成Hanning窗函数
    generate_hanning_window();
}

/**
 * @brief 计算1024点FFT频谱
 * @param input_data 输入浮点数据数组指针
 * @param data_length 输入数据长度（最大1024）
 */
void calculate_fft_spectrum(float* input_data, uint16_t data_length)
{
    uint16_t i;
    uint16_t actual_length = (data_length > FFT_LENGTH) ? FFT_LENGTH : data_length;
    
    // 清空缓冲区
    memset(fft_input_buffer, 0, sizeof(fft_input_buffer));
    memset(fft_magnitude, 0, sizeof(fft_magnitude));
    
    // 将实数数据转换为复数格式，同时应用Hanning窗
    for(i = 0; i < actual_length; i++)
    {
        fft_input_buffer[2*i] = input_data[i]*window_buffer[i] ;     // 实部乘以窗函数
        fft_input_buffer[2*i + 1] = 0.0f;                           // 虚部设为0
    }
    
    // 如果数据不足1024点，剩余部分已经清零，相当于零填充
    
    // 执行复数FFT（前向变换）
    arm_cfft_radix4_f32(&fft_instance, fft_input_buffer);
    
    // 计算幅度谱 - 注意这里要计算完整的FFT_LENGTH点
    arm_cmplx_mag_f32(fft_input_buffer, fft_magnitude, FFT_LENGTH);
    
    // 由于对称性，只保留前半部分（0到Nyquist频率）
    // 归一化处理，需要补偿Hanning窗的功率损失
    float window_power_correction = 1.5f; // Hanning窗的功率补偿因子
    
    for(i = 0; i < FFT_LENGTH / 2; i++)
    {
        if(i == 0) {
            // 直流分量除以N，并补偿窗函数损失
            fft_magnitude[i] = fft_magnitude[i] / FFT_LENGTH * window_power_correction;
        } else {
            // 其他分量除以N/2（考虑双边频谱的对称性），并补偿窗函数损失
            fft_magnitude[i] = fft_magnitude[i] * 2.0f / FFT_LENGTH * window_power_correction;
        }
    }
}

/**
 * @brief 输出FFT频谱数据到串口
 */
void output_fft_spectrum(void)
{
    uint16_t i;
    float freq_resolution;
    float current_freq;
    
    // 获取当前采样频率
    float sampling_freq = get_current_ad_frequency();
    freq_resolution = sampling_freq / FFT_LENGTH;
    
    my_printf(&huart1, "=== FFT Spectrum Analysis ===\r\n");
    my_printf(&huart1, "Sampling Freq: %.0f Hz\r\n", sampling_freq);
    my_printf(&huart1, "Freq Resolution: %.2f Hz\r\n", freq_resolution);
   //my_printf(&huart1, "Points: %d\r\n", FFT_LENGTH);
    my_printf(&huart1, "--- Spectrum Data ---\r\n");
    
    // 输出频谱数据，只输出前512个点（对应0到Nyquist频率）
    for(i = 10; i < FFT_LENGTH / 2; i++) 
    {
        current_freq = i * freq_resolution;
        my_printf(&huart1, "%.1f Hz: %.6f\r\n", current_freq, fft_magnitude[i]);
    }
    
    // 额外输出一些关键信息
    my_printf(&huart1, "--- Peak Analysis ---\r\n");
    
    // 寻找最大幅度点（排除直流分量）
    float max_magnitude = 0.0f;
    uint16_t max_index = 1;
    for(i = 1; i < FFT_LENGTH / 2; i++)
    {
        if(fft_magnitude[i] > max_magnitude)
        {
            max_magnitude = fft_magnitude[i];
            max_index = i;
        }
    }
    
    float peak_freq_raw = get_precise_peak_frequency(sampling_freq);
    float peak_freq = round_to_nearest_k(peak_freq_raw);
    float thd = calculate_thd(peak_freq, sampling_freq);
    
    my_printf(&huart1, "Peak Freq: %.0f Hz (Raw: %.2f Hz), Magnitude: %.6fv\r\n", peak_freq, peak_freq_raw, max_magnitude);
    my_printf(&huart1, "THD: %.2f%%\r\n", thd);
    my_printf(&huart1, "DC Component: %.6fv\r\n", fft_magnitude[0]);
    
    // 执行双峰检测
    dual_peaks = find_dual_peaks(sampling_freq, 0.2f);  // 阈值为最大值的20%
    
    my_printf(&huart1, "\r\n");
    output_dual_peaks_info();
    
    my_printf(&huart1, "=== End of Spectrum ===\r\n");
}

/**
 * @brief 使用抛物线插值法精确计算峰值频率
 * @param sampling_freq 采样频率
 * @return 精确的峰值频率
 */
float get_precise_peak_frequency(float sampling_freq)
{
    uint16_t i;
    float freq_resolution = sampling_freq / FFT_LENGTH;
    
    // 寻找最大幅度点（排除直流分量）
    float max_magnitude = 0.0f;
    uint16_t max_index = 1;
    for(i = 1; i < FFT_LENGTH / 2; i++)
    {
        if(fft_magnitude[i] > max_magnitude)
        {
            max_magnitude = fft_magnitude[i];
            max_index = i;
        }
    }
    
    // 如果峰值在边界，直接返回
    if(max_index <= 1 || max_index >= (FFT_LENGTH / 2 - 1))
    {
        return max_index * freq_resolution;
    }
    
    // 抛物线插值法提高频率精度
    float y1 = fft_magnitude[max_index - 1];
    float y2 = fft_magnitude[max_index];
    float y3 = fft_magnitude[max_index + 1];
    
    // 计算抛物线顶点位置的偏移量
    float delta = 0.5f * (y1 - y3) / (y1 - 2.0f * y2 + y3);
    
    // 防止分母为零
    if(fabsf(y1 - 2.0f * y2 + y3) < 1e-10f)
    {
        delta = 0.0f;
    }
    
    // 限制偏移量在合理范围内
    if(delta > 0.5f) delta = 0.5f;
    if(delta < -0.5f) delta = -0.5f;
    
    // 计算精确频率
    float precise_freq = (max_index + delta) * freq_resolution;
    
    return precise_freq;
}

/**
 * @brief 将频率四舍五入到最近的1000Hz整数倍
 * @param frequency 原始频率
 * @return 四舍五入后的频率
 */
float round_to_nearest_k(float frequency)
{
    // 四舍五入到最近的1000Hz
    return roundf(frequency / 1000.0f) * 1000.0f;
}

/**
 * @brief 计算总谐波失真THD
 * @param fundamental_freq 基波频率
 * @param sampling_freq 采样频率
 * @return THD值（百分比）
 */
float calculate_thd(float fundamental_freq, float sampling_freq)
{
    float freq_resolution = sampling_freq / FFT_LENGTH;
    float fundamental_power = 0.0f;
    float harmonic_power = 0.0f;
    
    // 找到基波频率对应的FFT bin
    uint16_t fundamental_bin = (uint16_t)(fundamental_freq / freq_resolution + 0.5f);
    
    // 确保基波bin在有效范围内
    if(fundamental_bin < 1 || fundamental_bin >= FFT_LENGTH / 2)
    {
        return 0.0f;
    }
    
    // 计算基波功率（包含邻近bin以考虑频谱泄漏）
    fundamental_power = fft_magnitude[fundamental_bin] * fft_magnitude[fundamental_bin];
    if(fundamental_bin > 1)
    {
        fundamental_power += 0.5f * fft_magnitude[fundamental_bin - 1] * fft_magnitude[fundamental_bin - 1];
    }
    if(fundamental_bin < FFT_LENGTH / 2 - 1)
    {
        fundamental_power += 0.5f * fft_magnitude[fundamental_bin + 1] * fft_magnitude[fundamental_bin + 1];
    }
    
    // 计算谐波功率（2次到10次谐波）
    for(uint8_t harmonic = 2; harmonic <= 10; harmonic++)
    {
        uint16_t harmonic_bin = harmonic * fundamental_bin;
        
        // 确保谐波bin在Nyquist频率以下
        if(harmonic_bin >= FFT_LENGTH / 2)
        {
            break;
        }
        
        // 累加谐波功率（包含邻近bin）
        float harmonic_mag_squared = fft_magnitude[harmonic_bin] * fft_magnitude[harmonic_bin];
        
        // 加上邻近bin的贡献
        if(harmonic_bin > 1)
        {
            harmonic_mag_squared += 0.25f * fft_magnitude[harmonic_bin - 1] * fft_magnitude[harmonic_bin - 1];
        }
        if(harmonic_bin < FFT_LENGTH / 2 - 1)
        {
            harmonic_mag_squared += 0.25f * fft_magnitude[harmonic_bin + 1] * fft_magnitude[harmonic_bin + 1];
        }
        
        harmonic_power += harmonic_mag_squared;
    }
    
    // 计算THD = sqrt(谐波功率总和) / 基波幅度 * 100%
    if(fundamental_power > 0.0f)
    {
        float thd = sqrtf(harmonic_power) / sqrtf(fundamental_power) * 100.0f;
        return thd;
    }
    
    return 0.0f;
}

/**
 * @brief 使用抛物线插值法计算精确频率
 * @param bin_index FFT bin索引
 * @param sampling_freq 采样频率
 * @return 精确频率值
 */
float calculate_precise_frequency(uint16_t bin_index, float sampling_freq)
{
    float freq_resolution = sampling_freq / FFT_LENGTH;
    
    // 边界检查
    if(bin_index <= 1 || bin_index >= (FFT_LENGTH / 2 - 1))
    {
        return bin_index * freq_resolution;
    }
    
    // 抛物线插值法
    float y1 = fft_magnitude[bin_index - 1];
    float y2 = fft_magnitude[bin_index];
    float y3 = fft_magnitude[bin_index + 1];
    
    float delta = 0.5f * (y1 - y3) / (y1 - 2.0f * y2 + y3);
    
    // 防止分母为零
    if(fabsf(y1 - 2.0f * y2 + y3) < 1e-10f)
    {
        delta = 0.0f;
    }
    
    // 限制偏移量
    if(delta > 0.5f) delta = 0.5f;
    if(delta < -0.5f) delta = -0.5f;
    
    return (bin_index + delta) * freq_resolution;
}

/**
 * @brief 在FFT频谱中寻找峰值
 * @param peaks 峰值信息数组
 * @param max_peaks 最大峰值数量
 * @param sampling_freq 采样频率
 * @param min_threshold 最小阈值（相对于最大值的比例）
 * @return 找到的峰值数量
 */
uint8_t find_spectrum_peaks(peak_info_t* peaks, uint8_t max_peaks, float sampling_freq, float min_threshold)
{
    uint8_t peak_count = 0;
    float freq_resolution = sampling_freq / FFT_LENGTH;
    
    // 找到最大幅度作为阈值参考
    float max_magnitude = 0.0f;
    for(uint16_t i = 5; i < FFT_LENGTH / 2; i++)  // 从索引5开始，避免直流和极低频分量
    {
        if(fft_magnitude[i] > max_magnitude)
        {
            max_magnitude = fft_magnitude[i];
        }
    }
    
    float threshold = max_magnitude * min_threshold;
    
    // 寻找局部最大值
    for(uint16_t i = 5; i < FFT_LENGTH / 2 - 1 && peak_count < max_peaks; i++)
    {
        // 检查是否为局部最大值
        if(fft_magnitude[i] > fft_magnitude[i-1] && 
           fft_magnitude[i] > fft_magnitude[i+1] && 
           fft_magnitude[i] > threshold)
        {
            // 确保与已找到的峰值有足够的间隔（至少10个bin）
            uint8_t is_isolated = 1;
            for(uint8_t j = 0; j < peak_count; j++)
            {
                if(abs((int)i - (int)peaks[j].bin_index) < 10)
                {
                    is_isolated = 0;
                    break;
                }
            }
            
            if(is_isolated)
            {
                peaks[peak_count].bin_index = i;
                peaks[peak_count].frequency = i * freq_resolution;
                peaks[peak_count].magnitude = fft_magnitude[i];
                peaks[peak_count].precise_frequency = calculate_precise_frequency(i, sampling_freq);
                peak_count++;
            }
        }
    }
    
    // 按幅度降序排序
    for(uint8_t i = 0; i < peak_count - 1; i++)
    {
        for(uint8_t j = i + 1; j < peak_count; j++)
        {
            if(peaks[j].magnitude > peaks[i].magnitude)
            {
                peak_info_t temp = peaks[i];
                peaks[i] = peaks[j];
                peaks[j] = temp;
            }
        }
    }
    
    return peak_count;
}

/**
 * @brief 寻找两个主要的基波频率峰值
 * @param sampling_freq 采样频率
 * @param min_threshold 最小阈值（相对于最大值的比例，建议0.1-0.3）
 * @return 双峰检测结果
 */
dual_peak_result_t find_dual_peaks(float sampling_freq, float min_threshold)
{
    dual_peak_result_t result = {0};
    peak_info_t temp_peaks[MAX_PEAKS];
    
    // 寻找所有峰值
    uint8_t peak_count = find_spectrum_peaks(temp_peaks, MAX_PEAKS, sampling_freq, min_threshold);
    
    result.peaks_found = (peak_count > 2) ? 2 : peak_count;
    
    if(peak_count >= 1)
    {
        result.peak1 = temp_peaks[0];  // 最大峰值
        
        if(peak_count >= 2)
        {
            result.peak2 = temp_peaks[1];  // 第二大峰值
            
            // 计算频率间隔
            result.freq_separation = fabsf(result.peak1.precise_frequency - result.peak2.precise_frequency);
            
            // 确保peak1的频率小于peak2（按频率升序）
            if(result.peak1.precise_frequency > result.peak2.precise_frequency)
            {
                peak_info_t temp = result.peak1;
                result.peak1 = result.peak2;
                result.peak2 = temp;
            }
        }
    }
    
    return result;
}

/**
 * @brief 输出双峰检测信息到串口并更新全局变量
 */
void output_dual_peaks_info(void)
{
    // 清空全局变量
    peak1_frequency = 0.0f;
    peak1_magnitude = 0.0f;
    peak2_frequency = 0.0f;
    peak2_magnitude = 0.0f;
    
    if(dual_peaks.peaks_found >= 1)
    {
        // 将频率四舍五入为千的整数倍
        float rounded_freq1 = round_to_nearest_k(dual_peaks.peak1.precise_frequency);
        
        // 更新全局变量
        peak1_frequency = rounded_freq1;
        peak1_magnitude = dual_peaks.peak1.magnitude;
        
        my_printf(&huart1, "Peak 1: %.0f Hz (Bin: %d), Magnitude: %.6f V\r\n", 
                  rounded_freq1, 
                  dual_peaks.peak1.bin_index, 
                  dual_peaks.peak1.magnitude);
        
        if(dual_peaks.peaks_found >= 2)
        {
            float rounded_freq2 = round_to_nearest_k(dual_peaks.peak2.precise_frequency);
            
            // 更新全局变量
            peak2_frequency = rounded_freq2;
            peak2_magnitude = dual_peaks.peak2.magnitude;
            
            my_printf(&huart1, "Peak 2: %.0f Hz (Bin: %d), Magnitude: %.6f V\r\n", 
                      rounded_freq2, 
                      dual_peaks.peak2.bin_index, 
                      dual_peaks.peak2.magnitude);
        }
    }
    else
    {
        my_printf(&huart1, "No significant peaks found!\r\n");
    }
}

/**
 * @brief 执行双峰分析的完整流程
 * @param input_data 输入数据
 * @param data_length 数据长度
 */
void perform_dual_peak_analysis(float* input_data, uint16_t data_length)
{
    // 获取采样频率
    float sampling_freq = get_current_ad_frequency();
    
    // 执行FFT
    calculate_fft_spectrum(input_data, data_length);
    
    // 执行双峰检测
    dual_peaks = find_dual_peaks(sampling_freq, 0.15f);  // 阈值为最大值的15%
    
    // 输出结果
    output_dual_peaks_info();
}

/**
 * @brief 获取第一个峰值的频率
 * @return 第一个峰值的频率 (Hz)
 */
float get_peak1_frequency(void)
{
    return peak1_frequency;
}

/**
 * @brief 获取第一个峰值的幅度
 * @return 第一个峰值的幅度 (V)
 */
float get_peak1_magnitude(void)
{
    return peak1_magnitude;
}

/**
 * @brief 获取第二个峰值的频率
 * @return 第二个峰值的频率 (Hz)
 */
float get_peak2_frequency(void)
{
    return peak2_frequency;
}

/**
 * @brief 获取第二个峰值的幅度
 * @return 第二个峰值的幅度 (V)
 */
float get_peak2_magnitude(void)
{
    return peak2_magnitude;
}

/**
 * @brief 根据双峰检测结果配置DA输出
 * @details 
 * 该函数根据检测到的两个峰值的频率和幅度，自动配置DA1和DA2的输出：
 * - DA1输出频率A对应的波形
 * - DA2输出频率B对应的波形
 * - 根据幅度判断波形类型：>0.3V为正弦波，<0.3V为三角波
 */
void configure_da_output_from_peaks(void)
{
    // 根据检测结果配置DA输出
    if(peak1_frequency > 0.0f) // 检测到第一个峰值
    {
        Waveform_t wave_a = (peak1_magnitude > 0.300f) ? WAVE_SINE : WAVE_TRIANGLE;
        const char* wave_name_a = (peak1_magnitude > 0.300f) ? "SINE" : "TRIANGLE";
        
        // 配置DA1输出频率A对应的波形
        DA_SetConfig(0, peak1_frequency, 1000, 0, wave_a);
        my_printf(&huart1,"A: %s, DA1: %.0f Hz\r\n", wave_name_a, peak1_frequency);
    }
    
    if(peak2_frequency > 0.0f) // 检测到第二个峰值
    {
        Waveform_t wave_b = (peak2_magnitude > 0.300f) ? WAVE_SINE : WAVE_TRIANGLE;
        const char* wave_name_b = (peak2_magnitude > 0.300f) ? "SINE" : "TRIANGLE";
        
        // 配置DA2输出频率B对应的波形
        DA_SetConfig(1, peak2_frequency, 1000, 0, wave_b);
        my_printf(&huart1,"B: %s, DA2: %.0f Hz\r\n", wave_name_b, peak2_frequency);
    }
    
    // 应用DA配置到硬件
    if(peak1_frequency > 0.0f || peak2_frequency > 0.0f)
    {
        DA_Apply_Settings();
       
    }
    else
    {
        my_printf(&huart1,"No valid peaks found, DA unchanged\r\n");
    }
}
