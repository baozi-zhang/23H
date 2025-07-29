#include "stm_sig.h"
#include "arm_math.h"
#include "stdio.h"
#include "arm_const_structs.h"
#include "ad_measure.h"
#include "my_fft.h"
#include "da_output.h"
#define FFT_LEN 1024
#define pi 3.1415926

uint16_t adc1_buf[1024];
uint16_t adc2_buf[1024];
uint32_t adc_buffer[1024];
float32_t windows[FFT_LEN];
float32_t window_compensation_factor;
float fft_cfft_input1[2048];
float fft_cfft_input2[2048];
float fft_cfft_output1[1024];
float fft_cfft_output2[1024];
uint8_t fft_ok[2]={0};
uint8_t main_bin1;
uint8_t main_bin2;
uint8_t phase_calculate_ok;
float diff;
uint8_t adc_flag=1;
// ADC设为同步主从触发模式，DMA设置为Normal模式
void stm32_adc_start(){
	HAL_ADC_Start(&hadc2);
	HAL_ADCEx_MultiModeStart_DMA(&hadc1,adc_buffer,1024);
	HAL_TIM_Base_Start(&htim2);
}	
float phase1;
float phase2;

void stm32_adc_restart(){
if(phase_calculate_ok) {
    phase_calculate_ok = 0;

    // 停止多模式DMA传输
    HAL_ADCEx_MultiModeStop_DMA(&hadc1);
    
    // 停止定时器触发源
    HAL_TIM_Base_Stop(&htim2);

    // ? 使用HAL库安全停止DMA
    HAL_DMA_Abort(hadc1.DMA_Handle);
    
    // ? 清除ADC状态标志（主+从）
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC | ADC_FLAG_OVR | ADC_FLAG_JEOC | ADC_FLAG_AWD);
    __HAL_ADC_CLEAR_FLAG(&hadc2, ADC_FLAG_EOC | ADC_FLAG_OVR | ADC_FLAG_JEOC | ADC_FLAG_AWD);

    // ? 重置HAL状态机（关键步骤）
    hadc1.State = HAL_ADC_STATE_READY;
    hadc2.State = HAL_ADC_STATE_READY;
    hadc1.DMA_Handle->State = HAL_DMA_STATE_READY;

    // ? 重新启动采集序列
    HAL_ADC_Start(&hadc2);
	HAL_ADCEx_MultiModeStart_DMA(&hadc1,adc_buffer,1024);
	HAL_TIM_Base_Start(&htim2);  
}

}

// 窗函数生成函数并且自动计算补偿系数
void window(uint8_t enable) {
    uint16_t i;
    float window_sum = 0.0f;
    
    for(i = 0; i < FFT_LEN; i++) {
        if(enable) {
            windows[i] = 0.539f - 0.46f * arm_cos_f32(2 * pi * i / (FFT_LEN - 1));
            window_sum += windows[i]; // 计算窗函数的总和
        } else {
            windows[i] = 1.0f;
            window_sum += 1.0f;
        }
    }
    
    // 计算窗函数补偿系数（能量归一化因子）
    if(enable) {
        // 补偿系数 = N / sum(window^2)
        float window_energy = 0.0f;
        for(i = 0; i < FFT_LEN; i++) {
            window_energy += windows[i] * windows[i];
        }
        window_compensation_factor = (float)FFT_LEN / window_energy;
        
        // 对于幅度谱，更简单的补偿：FFT点数 / 窗函数和
        // window_compensation_factor = (float)FFT_LEN / window_sum;
    } else {
        window_compensation_factor = 1.0f; // 矩形窗无需补偿
    }
}


//处理收集到的adc数据并且进行fft计算
void stm32_adc_fft(){
	//生成窗函数
	window(1);
	//对于adc1的数据进行处理
  uint16_t i=0;
	for(i=0;i<1024;i++){
		adc1_buf[i]=(int16_t)(adc_buffer[i] & 0xFFFF); 
	  fft_cfft_input1[i*2]=(float)(adc1_buf[i])*windows[i]*3.3f/65536.0f;
		fft_cfft_input1[i*2+1]=0;
	}
  arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_cfft_input1, 0, 1); // 直接使用
	arm_cmplx_mag_f32(fft_cfft_input1,fft_cfft_output1,1024);
	 for(i = 0; i < FFT_LEN; i++) {
        fft_cfft_output1[i] *= window_compensation_factor;
    }
	fft_cfft_output1[0]/=1024.0f;

	for(i=1;i<1024;i++){
	  fft_cfft_output1[i]/=512.0f;
	}
	fft_ok[0]=1;
//for(i=0;i<FFT_LEN;i++){
//  printf(("%.4f \n  "),fft_cfft_output1[i]);
//}
	
	//对于adc2的数据进行处理
	
	for(i=0;i<1024;i++){
		adc2_buf[i]=(int16_t)(adc_buffer[i]>>16); 
	  fft_cfft_input2[i*2]=(float)(adc2_buf[i])*windows[i]*3.3f/65536.0f;
//		printf(("2:%.2f \n"),(float)(adc2_buf[i]*3.3f/65536.0f));
		fft_cfft_input2[i*2+1]=0;
	}
  arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_cfft_input2, 0, 1); // 直接使用
	arm_cmplx_mag_f32(fft_cfft_input2,fft_cfft_output2,1024);
	 for(i = 0; i < FFT_LEN; i++) {
        fft_cfft_output2[i] *= window_compensation_factor;
    }
	fft_cfft_output2[0]/=1024.0f;
	for(i=1;i<1024;i++){
	  fft_cfft_output2[i]/=512.0f;
	}
	fft_ok[1]=1;
}


uint32_t FindFundamentalFrequencyBin(float* magnitude_spectrum, uint32_t fft_size) {
    uint32_t max_bin = 3; // 跳过直流分量
	  
    float max_mag = 0.0f;
    // 只搜索前半部分频谱（奈奎斯特频率以下）
    for (uint32_t bin = 3; bin < fft_size/2; bin++) {
        if (magnitude_spectrum[bin] > max_mag) {
            max_mag = magnitude_spectrum[bin];
            max_bin = bin;
        }
    }
    return max_bin;
}


float stm32_fft_phasedifferance_calculate(){
   if(fft_ok[0]&fft_ok[1])
			{
		main_bin1 = FindFundamentalFrequencyBin(fft_cfft_output1, FFT_LEN);
    main_bin2 = FindFundamentalFrequencyBin(fft_cfft_output2, FFT_LEN);
    // 提取基波频率分量（bin 1）
    float real1 = fft_cfft_input1[2*main_bin1];     // bin1实部 (索引2)
    float imag1 = fft_cfft_input1[2*main_bin1+1];     // bin1虚部 (索引3)
    float real2 = fft_cfft_input2[2*main_bin2];    // bin1实部
    float imag2 = fft_cfft_input2[2*main_bin2+1];    // bin1虚部

    // 计算各信号的相位（使用四象限反正切）
    phase1 = atan2f(imag1, real1);
    phase2 = atan2f(imag2, real2);
    
    // 计算相位差并归一化到[-π, π]范围
    float delta_phase = phase1 - phase2;
    
    // 相位差归一化处理
    while (delta_phase > PI) delta_phase -= 2 * PI;
    while (delta_phase < -PI) delta_phase += 2 * PI;
    phase_calculate_ok=1;
    return delta_phase;
  }
			else 
				return 0;
}
float calculate_median(float*num){
  uint16_t i=0;
	float max=0.0;
	float min=3.3;
	for(i=0;i<FFT_LEN;i++){
	if(num[i]>=max) max=num[i];
	if(num[i]<=min) min=num[i];
	}
	return (max+min)/2.0;
}

// PID控制器结构体（全局变量）
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float prev_error;
    float max_integral;
} TrackingPID;

TrackingPID track_pid = {
    .Kp = 0.05f,     // 比例系数
    .Ki = 0,    // 积分系数
    .Kd = 0.6f,     // 微分系数
    .integral = 0,
    .prev_error = 0,
    .max_integral = 0.2f
};

// 更新PID控制器
float track_pid_update(TrackingPID* pid, float error) {
    // 积分项更新（带限幅）
    pid->integral += pid->Ki * error;
    pid->integral = fmaxf(fminf(pid->integral, pid->max_integral), -pid->max_integral);
    
    // 微分项
    float derivative = pid->Kd * (error - pid->prev_error);
    pid->prev_error = error;
    
    return pid->Kp * error + pid->integral + derivative;
}

void stm32_adc_proc() {
    if(adc_flag) {
        stm32_adc_fft();
        
        // 1. 获取当前DA频率和相位差
        float current_freq = da_channels[0].frequency;
        float diff = stm32_fft_phasedifferance_calculate();
        
        // 2. 相位差方向定义（关键！）
        // diff = phase_DA - phase_Source
        // 当DA信号超前源信号时：diff > 0 → 需要降低DA频率
        // 当DA信号滞后源信号时：diff < 0 → 需要提高DA频率
        float error = -diff;  // 反转符号
        
        // 3. 使用PID控制器计算频率调整量
        float freq_adjust = track_pid_update(&track_pid, error);
        
        // 4. 应用调整后的频率（带限幅）
        float new_freq = current_freq + freq_adjust;
        new_freq = fmaxf(fminf(new_freq, current_freq + 0.5f), current_freq - 0.5f);
        
        // 5. 更新DA设置
        da_channels[0].frequency = new_freq;
        DA_Apply_Settings();
 
        
        adc_flag = 0;
        stm32_adc_restart();
    }
}