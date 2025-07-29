/**
 * @file commond_init.h
 * @brief 项目通用初始化与宏定义头文件
 * @details
 * 该文件是整个项目的核心基础头文件之一，包含了被广泛使用的定义：
 * 1.  常用数据类型的别名，简化代码书写。
 * 2.  基于Cortex-M4内核位带(Bit-Band)操作的GPIO快速访问宏，实现高效I/O。
 * 3.  与FPGA通信相关的硬件寄存器地址映射。
 * 4.  FPGA主控制寄存器的位定义（命令宏）。
 * 5.  系统中使用的重要常量，如FIFO大小、时钟频率等。
 *
 * @author 左岚
 * @date 2025-07-17
 */
#ifndef __COMMOND_INIT_H__
#define __COMMOND_INIT_H__

// 引入STM32F4系列的主头文件，提供了所有外设寄存器、数据类型等基础定义。
#include "stm32f4xx.h"

//-----------------------------------------------------------------
// 1. 定义一些常用的数据类型短关键字，提高代码可读性和编写效率
//-----------------------------------------------------------------
// s: signed, u: unsigned, c: const, v: volatile (__IO, __I)
typedef int32_t  s32;
typedef int16_t  s16;
typedef int8_t   s8;

typedef const int32_t sc32;
typedef const int16_t sc16;
typedef const int8_t  sc8;

typedef __IO int32_t vs32; // volatile, 可读可写
typedef __IO int16_t vs16;
typedef __IO int8_t  vs8;

typedef __I int32_t vsc32; // volatile const, 只读
typedef __I int16_t vsc16;
typedef __I int8_t  vsc8;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;
typedef const uint16_t uc16;
typedef const uint8_t  uc8;

typedef __IO uint32_t vu32; // volatile, 可读可写
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32; // volatile const, 只读
typedef __I uint16_t vuc16;
typedef __I uint8_t  vuc8;

//-----------------------------------------------------------------
// 2. 位带(Bit-Band)操作, 实现对单个GPIO引脚的原子操作，避免读改写指令
//-----------------------------------------------------------------
// 计算指定地址的某个位在位带别名区的地址
#define BITBAND(addr, bitnum)  ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
// 将地址强制转换为一个可操作的指针
#define MEM_ADDR(addr)         *((volatile unsigned long *)(addr))
// 获取位带别名区地址的指针，对该指针的读写即是对原始地址单个位的读写
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

//-----------------------------------------------------------------
// 3. GPIO寄存器地址定义，用于位带操作
//-----------------------------------------------------------------
#define GPIO_ODR_OFFSET 20 // GPIO 输出数据寄存器(ODR)相对于端口基地址的偏移量
#define GPIO_IDR_OFFSET 16 // GPIO 输入数据寄存器(IDR)相对于端口基地址的偏移量

// 计算指定GPIO端口的ODR和IDR的绝对地址
#define GPIO_ODR_ADDR(port) ((port##_BASE) + GPIO_ODR_OFFSET)
#define GPIO_IDR_ADDR(port) ((port##_BASE) + GPIO_IDR_OFFSET)

// 通用宏：获取指定GPIO端口某个引脚的位带操作地址
#define GPIOx_OUT(port, n) BIT_ADDR(GPIO_ODR_ADDR(port), n) // 输出引脚
#define GPIOx_IN(port, n)  BIT_ADDR(GPIO_IDR_ADDR(port), n)  // 输入引脚

// 为每个GPIO端口定义独立的快速访问宏，例如 PAout(5)=1; 即可将PA5置1
#define PAout(n) GPIOx_OUT(GPIOA, n)
#define PAin(n)  GPIOx_IN(GPIOA, n)

#define PBout(n) GPIOx_OUT(GPIOB, n)
#define PBin(n)  GPIOx_IN(GPIOB, n)

#define PCout(n) GPIOx_OUT(GPIOC, n)
#define PCin(n)  GPIOx_IN(GPIOC, n)

#define PDout(n) GPIOx_OUT(GPIOD, n)
#define PDin(n)  GPIOx_IN(GPIOD, n)

#define PEout(n) GPIOx_OUT(GPIOE, n)
#define PEin(n)  GPIOx_IN(GPIOE, n)

#define PFout(n) GPIOx_OUT(GPIOF, n)
#define PFin(n)  GPIOx_IN(GPIOF, n)

#define PGout(n) GPIOx_OUT(GPIOG, n)
#define PGin(n)  GPIOx_IN(GPIOG, n)

#define PHout(n) GPIOx_OUT(GPIOH, n)
#define PHin(n)  GPIOx_IN(GPIOH, n)

#define PIout(n) GPIOx_OUT(GPIOI, n)
#define PIin(n)  GPIOx_IN(GPIOI, n)

#define PJout(n) GPIOx_OUT(GPIOJ, n)
#define PJin(n)  GPIOx_IN(GPIOJ, n)

#define PKout(n) GPIOx_OUT(GPIOK, n)
#define PKin(n)  GPIOx_IN(GPIOK, n)

//-----------------------------------------------------------------
// 4. FPGA主控制寄存器(CTRL_DATA)的位定义
//-----------------------------------------------------------------
enum
{
    DA_FREQ_EN     = 1,    // bit 0:  1=启动DA波形输出
    AD1_FREQ_EN    = 4,    // bit 2:  1=启动AD1采样时钟
    AD2_FREQ_EN    = 8,    // bit 3:  1=启动AD2采样时钟
    AD1_FIFO_WR    = 16,   // bit 4:  1=允许AD1向FIFO写入数据
    AD1_FIFO_RD    = 32,   // bit 5:  1=允许MCU从AD1的FIFO读取数据
    AD2_FIFO_WR    = 64,   // bit 6:  1=允许AD2向FIFO写入数据
    AD2_FIFO_RD    = 128,  // bit 7:  1=允许MCU从AD2的FIFO读取数据
    AD1_FREQ_CLR   = 256,  // bit 8:  0=触发清除AD1测频计数器 (低电平有效)
    AD1_FREQ_START = 512,  // bit 9:  1=启动AD1测频计数
    AD2_FREQ_CLR   = 1024, // bit 10: 0=触发清除AD2测频计数器 (低电平有效)
    AD2_FREQ_START = 2048  // bit 11: 1=启动AD2测频计数
};

//-----------------------------------------------------------------
// 5. FPGA 寄存器地址映射
// (通过FSMC/FMC将FPGA内部寄存器映射到STM32的内存地址空间)
//-----------------------------------------------------------------
// 基础宏：计算FPGA寄存器的绝对地址。基地址0x64000000, 偏移量addr是16位字地址。
#define reg_addr(addr) ((uint32_t *)(0x64000000 + ((addr) << 1)))

// --- 寄存器定义 (区分读写功能) ---

// 地址 1: 总控制 (STM32 -> FPGA)
#define CTRL_DATA     *(vu16 *)reg_addr(1) // 全局控制寄存器 (写操作)

// 地址 2, 3:
// 写: DA1 输出频率控制字 (STM32 -> FPGA)
#define DA1_H         *(vu16 *)reg_addr(2)
#define DA1_L         *(vu16 *)reg_addr(3)
// 读: AD1 频率测量基准(base)计数器结果 (FPGA -> STM32)
#define BASE1_FREQ_H  *(vu16 *)reg_addr(2)
#define BASE1_FREQ_L  *(vu16 *)reg_addr(3)

// 地址 4, 5:
// 写: DA2 输出频率控制字 (STM32 -> FPGA)
#define DA2_H         *(vu16 *)reg_addr(4)
#define DA2_L         *(vu16 *)reg_addr(5)
// 读: AD2 频率测量基准(base)计数器结果 (FPGA -> STM32)
#define BASE2_FREQ_H  *(vu16 *)reg_addr(4)
#define BASE2_FREQ_L  *(vu16 *)reg_addr(5)

// 地址 6, 7:
// 写: AD1 采样频率控制字 (STM32 -> FPGA)
#define AD1_FS_H      *(vu16 *)reg_addr(6)
#define AD1_FS_L      *(vu16 *)reg_addr(7)
// 读: AD1 采样结果与完成标志 (FPGA -> STM32)
#define AD1_DATA_SHOW *(vu16 *)reg_addr(6) // 读此地址获取AD1采样结果
#define AD1_FULL_FLAG *(vu16 *)reg_addr(7) // 读此地址获取AD1采样完成标志

// 地址 8, 9:
// 写: AD2 采样频率控制字 (STM32 -> FPGA)
#define AD2_FS_H      *(vu16 *)reg_addr(8)
#define AD2_FS_L      *(vu16 *)reg_addr(9)
// 读: AD2 采样结果与完成标志 (FPGA -> STM32)
#define AD2_DATA_SHOW *(vu16 *)reg_addr(8) // 读此地址获取AD2采样结果
#define AD2_FULL_FLAG *(vu16 *)reg_addr(9) // 读此地址获取AD2采样完成标志

// 地址 10, 11:
// 写: DA1, DA2 相位控制字 (STM32 -> FPGA)
#define DA1_PHASE     *(vu16 *)reg_addr(10)
#define DA2_PHASE     *(vu16 *)reg_addr(11)
// 读: AD1 频率测量信号计数器结果 (FPGA -> STM32)
#define AD1_FREQ_H    *(vu16 *)reg_addr(10)
#define AD1_FREQ_L    *(vu16 *)reg_addr(11)

// 地址 12, 13:
// 写: DA1, DA2 波形选择 (STM32 -> FPGA)
#define DA_WAVEFORM   *(vu16 *)reg_addr(12) // 低8位DA1, 高8位DA2
// 读: AD2 频率测量信号计数器结果 (FPGA -> STM32)
#define AD2_FREQ_H    *(vu16 *)reg_addr(12)
#define AD2_FREQ_L    *(vu16 *)reg_addr(13)

// 地址 14, 15: DA1, DA2 峰峰值控制 (STM32 -> FPGA, 只写)
#define DA1_VPP       *(vu16 *)reg_addr(14)
#define DA2_VPP       *(vu16 *)reg_addr(15)

//-----------------------------------------------------------------
// 6. 系统级常量定义
//-----------------------------------------------------------------
// AD模块参数
#define FIFO_SIZE     1024        // AD FIFO 缓冲区的深度（点数）
#define FIFO_SIZE_N   1024.0f     // AD FIFO 深度的浮点数形式，用于计算
#define FREQ_CONSTANT 4294967296.0f // AD采样频率设置常数, 等于 2^32

// DA模块参数
#define DA_FIFO_SIZE     1024        // DA FIFO 缓冲区的深度（点数）
#define DA_FREQ_CONSTANT 4294967296.0f // DA输出频率设置常数, 等于 2^32

// FFT处理参数
#define FFT_LENGTH       1024        // FFT运算的点数

// FPGA系统时钟
#define FPGA_BASE_CLK    150000000.0f // FPGA的主工作时钟频率 (150 MHz)

#endif // __COMMOND_INIT_H__
