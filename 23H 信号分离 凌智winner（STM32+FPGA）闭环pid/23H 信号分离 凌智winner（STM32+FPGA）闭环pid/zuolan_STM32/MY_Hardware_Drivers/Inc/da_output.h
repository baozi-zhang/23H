/**
 * @file da_output.h
 * @brief DA输出模块的公共接口和类型定义
 * @details
 * 该头文件定义了控制双通道DA输出所需的数据结构、枚举、宏，
 *以及所有外部可调用函数的原型。它为其他模块提供了与DA功能
 *交互所需的全部接口。
 *
 * @author 左岚
 * @date 2025-07-17
 */
#ifndef __DA_OUTPUT_H__
#define __DA_OUTPUT_H__

#include "commond_init.h" // 引入通用的初始化和基本类型定义（如 uint16_t 等）
#include "bsp_system.h"   // 引入板级支持包的系统定义，可能包含硬件相关的宏

// *********************************************************************************
// 1. 使用enum管理波形类型
// *********************************************************************************
/**
 * @brief DA输出的波形类型枚举
 * @details
 * 使用枚举类型来明确、安全地表示DA可以生成的各种波形。
 * 这比使用宏定义或魔术数字（magic numbers）更具可读性和类型安全性。
 * 这些枚举值通常直接对应FPGA寄存器中的设定值。
 */
typedef enum
{
    WAVE_SINE = 0,     // 正弦波
    WAVE_SQUARE = 1,   // 方波
    WAVE_TRIANGLE = 2, // 三角波
    WAVE_SAWTOOTH = 3  // 锯齿波
} Waveform_t;

// *********************************************************************************
// 2. 定义DA输出的结构体
// *********************************************************************************
/**
 * @brief 单个DA通道的配置参数结构体
 * @details
 * 将一个DA通道所有可配置的参数聚合到一个结构体中。
 * 这样做可以方便地在函数间传递配置，并使代码结构更清晰。
 */
typedef struct
{
    float frequency;     // 输出频率 (单位: Hz)
    uint16_t amplitude;  // 幅度 (通常是一个与DAC分辨率相关的无单位数值，代表峰峰值Vpp)
    uint16_t phase;      // 相位 (单位: 度, 范围: 0-359)
    Waveform_t waveform; // 波形类型 (使用上面定义的枚举 Waveform_t)
} DA_Channel_t;

/**
 * @brief 定义系统中DA通道的总数
 * @details
 * 使用宏定义来指定DA通道的数量，便于代码的维护。
 * 如果将来硬件更改（例如增加到4通道），只需修改此处的定义即可。
 */
#define NUM_DA_CHANNELS 2

// *********************************************************************************
// 函数原型声明
// *********************************************************************************

/**
 * @brief 初始化DA的默认参数并应用
 * @details
 * 该函数应在系统启动时调用一次，为所有DA通道设置一个已知的初始状态。
 */
void DA_Init(void);

/**
 * @brief 设置指定DA通道的完整配置（仅更新内存中的结构体）
 * @details
 * 调用此函数来修改特定通道的配置参数。
 * 注意：此函数只更新软件中的 `da_channels` 数组，不会立即影响硬件输出。
 * @param channel_index DA通道索引 (0 for DA1, 1 for DA2)
 * @param freq 目标频率 (单位: Hz)
 * @param vpp 目标幅度 (原始值)
 * @param angle 目标相位 (单位: 度, 0-359)
 * @param wave 目标波形类型 (Waveform_t 枚举值)
 */
void DA_SetConfig(uint8_t channel_index, float freq, uint16_t vpp, uint16_t angle, Waveform_t wave);

/**
 * @brief 将内存中的DA配置参数应用到硬件寄存器
 * @details
 * 在通过 `DA_SetConfig` 修改了一个或多个通道的配置后，
 * 必须调用此函数，才能将这些更改写入FPGA或相关硬件，使其真正生效。
 * @note  这是一个关键函数，是软件配置与硬件输出之间的桥梁。
 */
void DA_Apply_Settings(void);

/**
 * @brief 波形变换测试函数
 * @details
 * 一个用于演示和测试的示例函数，它会周期性地改变DA输出的波形。
 * 已更新为使用新的 `DA_Channel_t` 结构体和 `DA_Apply_Settings` 接口。
 */
void wave_test(void);

/**
 * @brief 外部变量声明
 * @details
 * 使用 `extern` 关键字声明全局的DA通道配置数组。
 * 这使得其他 `.c` 文件在包含了本头文件后，能够访问到这个在 `da_output.c`
 * 中定义的数组，从而可以读取当前的DA设置。
 */
extern DA_Channel_t da_channels[NUM_DA_CHANNELS];

#endif // __DA_OUTPUT_H__
