/**
 * @file da_output.c
 * @brief DA输出驱动程序，用于控制双通道波形发生器
 * @details
 * 该文件实现了对FPGA控制的双通道DA输出的配置和管理。
 * 主要功能包括：
 * 1. 初始化DA通道的默认参数。
 * 2. 提供接口用于设置每个通道的波形类型、频率、幅度和相位。
 * 3. 将内存中的配置参数计算并写入到硬件寄存器中，以应用设置。
 * 4. 包含一个测试函数，用于周期性地切换波形以验证功能。
 *
 * @author 左岚
 * @date 2025-07-17
 */
#include "da_output.h"
#include "math.h"

// *********************************************************************************
// 3. 实例化一个含有两个DA结构体的数组，并全部初始化为0
// *********************************************************************************

/**
 * @brief DA通道配置参数存储数组
 * @details
 * 定义一个DA_Channel_t类型的数组，用于存储两个DA通道的配置信息，
 * 包括频率、幅度、相位和波形类型。
 * `[NUM_DA_CHANNELS]` 表示通道数量，通常为2。
 * `= {0}` 是一个聚合初始化，将数组的所有成员（两个结构体）的全部字段递归地初始化为0。
 */
DA_Channel_t da_channels[NUM_DA_CHANNELS] = {0};

/**
 * @brief 初始化DA的默认参数并应用
 * @details
 * 此函数在系统启动时调用，为两个DA通道设置一组预定义的默认值。
 * 设置完成后，调用 DA_Apply_Settings() 将这些默认值写入硬件，
 * 使得DA在开机后能立即输出确定的波形。
 */
void DA_Init(void)
{
    // 在DA_proc函数中的原始设定值
    // 设置DA1默认参数: 频率20kHz, 幅度2000, 相位180度, 正弦波
    DA_SetConfig(0, 20000.0f, 1000, 180, WAVE_SINE);

    // 设置DA2默认参数: 频率20kHz, 幅度2000, 相位0度, 正弦波
    DA_SetConfig(1, 20000.0f, 1000, 0, WAVE_SINE);

    // 将设置应用到硬件
    DA_Apply_Settings();
}

/**
 * @brief 设置指定DA通道的配置参数（仅修改内存中的值）
 * @details
 * 此函数用于更新存储在 `da_channels` 数组中的特定通道的配置参数。
 * 它不会直接影响硬件输出，需要后续调用 DA_Apply_Settings() 函数才能生效。
 * 这样做的好处是可以批量修改多个参数，然后一次性应用。
 * @param channel_index DA通道索引 (0 for DA1, 1 for DA2)
 * @param freq 频率 (Hz)
 * @param vpp 幅度 (通常是与DA位数相关的无单位数值)
 * @param angle 相位 (0-359度)
 * @param wave 波形类型 (枚举值，如 WAVE_SINE, WAVE_SQUARE 等)
 */
void DA_SetConfig(uint8_t channel_index, float freq, uint16_t vpp, uint16_t angle, Waveform_t wave)
{
    // 检查通道索引是否有效，防止数组越界访问
    if (channel_index >= NUM_DA_CHANNELS)
    {
        return; // 如果索引无效，则直接返回
    }
    // 将传入的参数保存到对应的通道结构体中
    da_channels[channel_index].frequency = freq;
    da_channels[channel_index].amplitude = vpp;
    da_channels[channel_index].phase = angle;
    da_channels[channel_index].waveform = wave;
}

/**
 * @brief 将内存中da_channels数组的配置写入到硬件寄存器
 * @details
 * 此函数是连接软件配置和硬件执行的关键。它执行以下操作：
 * 1. 停止FPGA波形生成，以安全地更新寄存器。
 * 2. 读取 `da_channels` 数组中两个通道的配置。
 * 3. 根据频率、时钟等参数计算出需要写入FPGA频率控制寄存器的值(M值)。
 * 4. 将计算出的频率、幅度和相位值写入对应的硬件寄存器。
 * 5. 将两个通道的波形类型编码并写入单个寄存器。
 * 6. 重新启动FPGA波形生成，使新配置生效。
 */
void DA_Apply_Settings(void)
{
    // 停止FPGA的DA输出，防止在更新参数时产生错误波形
    DA_FPGA_STOP();

    // --- 计算并设置DA1 ---
    // 计算DA1的频率控制字M。该值与设定的频率、FPGA主时钟、DA FIFO大小等相关。
    unsigned int M_DA1 = DA_FREQ_CONSTANT * da_channels[0].frequency / FPGA_BASE_CLK * DA_FIFO_SIZE;
    // 将32位的M值拆分为高16位和低16位，分别写入对应的硬件寄存器
    DA1_H = M_DA1 >> 16;
    DA1_L = M_DA1 & 0x0000FFFF;
    // 设置DA1的幅度
    DA1_VPP = da_channels[0].amplitude;
    // 将0-359度的相位角度归一化到0-359范围内
    uint16_t normalized_angle1 = da_channels[0].phase % 360;
    // 将角度值转换为FPGA内部所需的相位寄存器值，2.844f是转换系数 (可能等于 1024/360)
    DA1_PHASE = (uint16_t)(roundf(normalized_angle1 * 2.844f));

    // --- 计算并设置DA2 ---
    // 计算DA2的频率控制字M
    unsigned int M_DA2 = DA_FREQ_CONSTANT * da_channels[1].frequency / FPGA_BASE_CLK * DA_FIFO_SIZE;
    // 拆分并写入DA2的频率控制寄存器
    DA2_H = M_DA2 >> 16;
    DA2_L = M_DA2 & 0x0000FFFF;
    // 设置DA2的幅度
    DA2_VPP = da_channels[1].amplitude;
    // 归一化DA2的相位角度
    uint16_t normalized_angle2 = da_channels[1].phase % 360;
    // 将角度值转换为DA2的相位寄存器值
    DA2_PHASE = (uint16_t)(roundf(normalized_angle2 * 2.844f));

    // --- 设置组合的波形寄存器 ---
    // 将两个通道的波形类型枚举值合并到一个16位寄存器中。
    // DA1的波形设置在低8位，DA2的波形设置在高8位。
    // 使用直接赋值而不是 `|=` 操作，可以避免读-改-写操作可能带来的时序问题或意外的旧值干扰。
    DA_WAVEFORM = (da_channels[1].waveform << 8) | da_channels[0].waveform;

    // 重新启动FPGA的DA输出，应用新的设置
    DA_FPGA_START();
}

// ------------------- 测试函数更新 -------------------

// 用于非阻塞延时的计时器变量，记录上次波形切换的时间
uint32_t wave_tick = 0;

/**
 * @brief 波形变换测试函数 (已停用，改为按键1控制)
 * @details
 * 原测试函数，用于演示和验证波形切换功能。
 * 现已改为静态函数，不再由调度器调用。
 * 波形切换功能已移至按键1处理中。
 */
#if 0  // 已弃用的函数，保留用于参考
static void wave_test(void)
{
    // 使用 `uwTick` (一个系统级的毫秒计时器) 实现非阻塞延时
    // 如果距离上次执行的时间间隔小于3000ms，则直接返回
    if (uwTick - wave_tick < 3000)
    {
        return;
    }
    // 更新时间戳，为下一次计时做准备
    wave_tick = uwTick;

    // 使用静态变量保存当前波形状态，使其在函数调用之间保持其值
    // 注意类型已变为Waveform_t，与配置结构体一致
    static Waveform_t current_waveform = WAVE_SINE;

    // 使用 switch-case 结构来循环切换波形
    switch (current_waveform)
    {
    case WAVE_SINE:
        current_waveform = WAVE_SQUARE;
        break;
    case WAVE_SQUARE:
        current_waveform = WAVE_TRIANGLE;
        break;
    case WAVE_TRIANGLE:
        current_waveform = WAVE_SAWTOOTH;
        break;
    case WAVE_SAWTOOTH:
    default: // 如果是锯齿波或任何其他意外情况，则回到正弦波
        current_waveform = WAVE_SINE;
        break;
    }

    // 更新两个通道的波形配置（只更新波形，其他参数如频率、幅度、相位保持不变）
    da_channels[0].waveform = current_waveform;
    da_channels[1].waveform = current_waveform;

    // 将新配置应用到硬件
    DA_Apply_Settings();
}
#endif  // 已弃用的函数结束
