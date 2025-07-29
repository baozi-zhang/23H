#include "key_app.h"
#include "ad_measure.h"
#include "my_fft.h"
#include "da_output.h"

uint8_t key_val = 0;
uint8_t key_old = 0;
uint8_t key_down = 0;
uint8_t key_up = 0;
float detected_freq = 0.0f;
// 当前AD采集频率
static float current_ad_freq = 2000000.0f; // 默认2MHz


uint8_t key_read(void)
{
	uint8_t temp = 0;
	
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_6) == GPIO_PIN_RESET)
		temp = 1;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4) == GPIO_PIN_RESET)
		temp = 2;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6) == GPIO_PIN_RESET)
		temp = 3;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9) == GPIO_PIN_RESET)
		temp = 4;
	// 可以添加更多按键...
	return temp;
}

/**
 * @brief 设置当前AD采集频率
 * 
 * @param freq 新的采集频率（Hz）
 */
void set_current_ad_frequency(float freq)
{
	current_ad_freq = freq;
}

/**
 * @brief 获取当前AD采集频率
 * 
 * @return float 当前采集频率（Hz）
 */
float get_current_ad_frequency(void)
{
	return current_ad_freq;
}

void key_proc(void)
{
	key_val = key_read();
	key_down = key_val & (key_old ^ key_val);
	key_up = ~key_val & (key_old ^ key_val );
	key_old = key_val;
	
	
  
	switch(key_down)
	{
		case 1: // 按键1：切换DA波形
		{
			// 使用静态变量保存当前波形状态
			static Waveform_t current_waveform = WAVE_SINE;
			
			// 循环切换波形
			switch (current_waveform)
			{
				case WAVE_SINE:
					current_waveform = WAVE_SQUARE;
					my_printf(&huart1,"DA切换为方波\r\n");
					break;
				case WAVE_SQUARE:
					current_waveform = WAVE_TRIANGLE;
					my_printf(&huart1,"DA切换为三角波\r\n");
					break;
				case WAVE_TRIANGLE:
					current_waveform = WAVE_SAWTOOTH;
					my_printf(&huart1,"DA切换为锯齿波\r\n");
					break;
				case WAVE_SAWTOOTH:
				default:
					current_waveform = WAVE_SINE;
					my_printf(&huart1,"DA切换为正弦波\r\n");
					break;
			}
			
			// 更新两个通道的波形配置
			da_channels[0].waveform = current_waveform;
			da_channels[1].waveform = current_waveform;
			
		
			
			// 将新配置应用到硬件
			DA_Apply_Settings();
			
	
		}
			break;
		
		case 2: // 按键2：输出当前缓冲区采集波形
		{
			for(int i=0;i<FIFO_SIZE;i++)
			{
				my_printf(&huart1,"%.4f\r\n", fifo_data1_f[i]);
			}
     	
		}
			break;
		
		case 3: // 按键3：调节DA1相位
		{
			
			// 使用静态变量保存当前相位，每次按键增加30度
			static uint16_t current_phase = 0;
			
			// 每次按键增加30度
			current_phase += 30;
			
			// 相位范围限制在0-359度
			if (current_phase >= 360)
			{
				current_phase = 0;
			}
			
			
			// 更新DA1的相位配置
			da_channels[0].phase = current_phase;
			
			my_printf(&huart1,"%d\r\n", current_phase);
			
			// 将新配置应用到硬件
			DA_Apply_Settings();
		
		}
			break;
		
		case 4: // 按键4：双峰检测分析并配置DA输出
		{
			// 执行双峰检测分析
			perform_dual_peak_analysis(fifo_data1_f, FIFO_SIZE);
			
			// 根据检测结果配置DA输出
			configure_da_output_from_peaks();
		}
			break;
		
	}
	
}


