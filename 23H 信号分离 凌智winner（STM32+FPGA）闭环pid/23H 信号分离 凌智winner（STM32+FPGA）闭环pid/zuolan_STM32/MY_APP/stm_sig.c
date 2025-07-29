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
// ADC��Ϊͬ�����Ӵ���ģʽ��DMA����ΪNormalģʽ
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

    // ֹͣ��ģʽDMA����
    HAL_ADCEx_MultiModeStop_DMA(&hadc1);
    
    // ֹͣ��ʱ������Դ
    HAL_TIM_Base_Stop(&htim2);

    // ? ʹ��HAL�ⰲȫֹͣDMA
    HAL_DMA_Abort(hadc1.DMA_Handle);
    
    // ? ���ADC״̬��־����+�ӣ�
    __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC | ADC_FLAG_OVR | ADC_FLAG_JEOC | ADC_FLAG_AWD);
    __HAL_ADC_CLEAR_FLAG(&hadc2, ADC_FLAG_EOC | ADC_FLAG_OVR | ADC_FLAG_JEOC | ADC_FLAG_AWD);

    // ? ����HAL״̬�����ؼ����裩
    hadc1.State = HAL_ADC_STATE_READY;
    hadc2.State = HAL_ADC_STATE_READY;
    hadc1.DMA_Handle->State = HAL_DMA_STATE_READY;

    // ? ���������ɼ�����
    HAL_ADC_Start(&hadc2);
	HAL_ADCEx_MultiModeStart_DMA(&hadc1,adc_buffer,1024);
	HAL_TIM_Base_Start(&htim2);  
}

}

// ���������ɺ��������Զ����㲹��ϵ��
void window(uint8_t enable) {
    uint16_t i;
    float window_sum = 0.0f;
    
    for(i = 0; i < FFT_LEN; i++) {
        if(enable) {
            windows[i] = 0.539f - 0.46f * arm_cos_f32(2 * pi * i / (FFT_LEN - 1));
            window_sum += windows[i]; // ���㴰�������ܺ�
        } else {
            windows[i] = 1.0f;
            window_sum += 1.0f;
        }
    }
    
    // ���㴰��������ϵ����������һ�����ӣ�
    if(enable) {
        // ����ϵ�� = N / sum(window^2)
        float window_energy = 0.0f;
        for(i = 0; i < FFT_LEN; i++) {
            window_energy += windows[i] * windows[i];
        }
        window_compensation_factor = (float)FFT_LEN / window_energy;
        
        // ���ڷ����ף����򵥵Ĳ�����FFT���� / ��������
        // window_compensation_factor = (float)FFT_LEN / window_sum;
    } else {
        window_compensation_factor = 1.0f; // ���δ����貹��
    }
}


//�����ռ�����adc���ݲ��ҽ���fft����
void stm32_adc_fft(){
	//���ɴ�����
	window(1);
	//����adc1�����ݽ��д���
  uint16_t i=0;
	for(i=0;i<1024;i++){
		adc1_buf[i]=(int16_t)(adc_buffer[i] & 0xFFFF); 
	  fft_cfft_input1[i*2]=(float)(adc1_buf[i])*windows[i]*3.3f/65536.0f;
		fft_cfft_input1[i*2+1]=0;
	}
  arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_cfft_input1, 0, 1); // ֱ��ʹ��
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
	
	//����adc2�����ݽ��д���
	
	for(i=0;i<1024;i++){
		adc2_buf[i]=(int16_t)(adc_buffer[i]>>16); 
	  fft_cfft_input2[i*2]=(float)(adc2_buf[i])*windows[i]*3.3f/65536.0f;
//		printf(("2:%.2f \n"),(float)(adc2_buf[i]*3.3f/65536.0f));
		fft_cfft_input2[i*2+1]=0;
	}
  arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_cfft_input2, 0, 1); // ֱ��ʹ��
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
    uint32_t max_bin = 3; // ����ֱ������
	  
    float max_mag = 0.0f;
    // ֻ����ǰ�벿��Ƶ�ף��ο�˹��Ƶ�����£�
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
    // ��ȡ����Ƶ�ʷ�����bin 1��
    float real1 = fft_cfft_input1[2*main_bin1];     // bin1ʵ�� (����2)
    float imag1 = fft_cfft_input1[2*main_bin1+1];     // bin1�鲿 (����3)
    float real2 = fft_cfft_input2[2*main_bin2];    // bin1ʵ��
    float imag2 = fft_cfft_input2[2*main_bin2+1];    // bin1�鲿

    // ������źŵ���λ��ʹ�������޷����У�
    phase1 = atan2f(imag1, real1);
    phase2 = atan2f(imag2, real2);
    
    // ������λ���һ����[-��, ��]��Χ
    float delta_phase = phase1 - phase2;
    
    // ��λ���һ������
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

// PID�������ṹ�壨ȫ�ֱ�����
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float integral;
    float prev_error;
    float max_integral;
} TrackingPID;

TrackingPID track_pid = {
    .Kp = 0.05f,     // ����ϵ��
    .Ki = 0,    // ����ϵ��
    .Kd = 0.6f,     // ΢��ϵ��
    .integral = 0,
    .prev_error = 0,
    .max_integral = 0.2f
};

// ����PID������
float track_pid_update(TrackingPID* pid, float error) {
    // ��������£����޷���
    pid->integral += pid->Ki * error;
    pid->integral = fmaxf(fminf(pid->integral, pid->max_integral), -pid->max_integral);
    
    // ΢����
    float derivative = pid->Kd * (error - pid->prev_error);
    pid->prev_error = error;
    
    return pid->Kp * error + pid->integral + derivative;
}

void stm32_adc_proc() {
    if(adc_flag) {
        stm32_adc_fft();
        
        // 1. ��ȡ��ǰDAƵ�ʺ���λ��
        float current_freq = da_channels[0].frequency;
        float diff = stm32_fft_phasedifferance_calculate();
        
        // 2. ��λ����壨�ؼ�����
        // diff = phase_DA - phase_Source
        // ��DA�źų�ǰԴ�ź�ʱ��diff > 0 �� ��Ҫ����DAƵ��
        // ��DA�ź��ͺ�Դ�ź�ʱ��diff < 0 �� ��Ҫ���DAƵ��
        float error = -diff;  // ��ת����
        
        // 3. ʹ��PID����������Ƶ�ʵ�����
        float freq_adjust = track_pid_update(&track_pid, error);
        
        // 4. Ӧ�õ������Ƶ�ʣ����޷���
        float new_freq = current_freq + freq_adjust;
        new_freq = fmaxf(fminf(new_freq, current_freq + 0.5f), current_freq - 0.5f);
        
        // 5. ����DA����
        da_channels[0].frequency = new_freq;
        DA_Apply_Settings();
 
        
        adc_flag = 0;
        stm32_adc_restart();
    }
}