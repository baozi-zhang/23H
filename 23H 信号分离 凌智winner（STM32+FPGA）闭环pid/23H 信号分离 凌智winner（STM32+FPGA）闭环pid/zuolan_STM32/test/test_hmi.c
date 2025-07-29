#include "my_hmi.h"
#include "math.h"
void main()
{
    char char_data[6] = "hello";
    int int_data = 10;
    float float_data = 10.5;
    int wave_data[1024];
    for (int i = 0; i < 1024; i++)
    {
        // 构建一个周期的正弦波，并且把幅度设置为0~200
        wave_data[i] = (sin(i * 2 * 3.14 / 1024) + 1) * 100;
    }
    HMI_Send_String("char_data", char_data);
    HMI_Send_Int("int_data", int_data);
    HMI_Send_Float("float_data", float_data, 1);
    for (int i = 0; i < 1024; i++)
    {
        HMI_Write_Wave_Low("low_wave", 0, wave_data[i]);
    }
    HMI_Wave_Clear("low_wave", 0);
    
    HMI_Write_Wave_Fast("fast_wave", 0, 1024, wave_data);
}