#ifndef __AD9959_H__
#define __AD9959_H__

#include "main.h"
#include "commond_init.h"
#include "bsp_system.h"
// 应用参考值
#define FRE_REF 8.5904963602764 // 频率参考值，用于频率控制字计算
#define POW_REF 45.51111111     // 相位参考值，用于相位控制字计算

// 晶振频率：25 MHz
#define Crystal 25000000

// AD9959寄存器地址定义
#define CSR 0x00   // 通道选择寄存器
#define FR1 0x01   // 功能控制寄存器1
#define FR2 0x02   // 功能控制寄存器2
#define CFR 0x03   // 通道功能寄存器
#define CFTW0 0x04 // 通道频率控制字0
#define CPOW0 0x05 // 通道相位补偿字0
#define ACR 0x06   // 幅度控制寄存器
#define LSRR 0x07  // 线性扫描速率寄存器：[7:0]上升，[15:8]下降
#define RDW 0x08   // 上升步进频率控制字
#define FDW 0x09   // 下降步进频率控制字
#define CW1 0x0a   // 通道字1
#define CW2 0x0b   // 通道字2
#define CW3 0x0c   // 通道字3
#define CW4 0x0d   // 通道字4
#define CW5 0x0e   // 通道字5
#define CW6 0x0f   // 通道字6
#define CW7 0x10   // 通道字7
#define CW8 0x11   // 通道字8
#define CW9 0x12   // 通道字9
#define CW10 0x13  // 通道字10
#define CW11 0x14  // 通道字11
#define CW12 0x15  // 通道字12
#define CW13 0x16  // 通道字13
#define CW14 0x17  // 通道字14
#define CW15 0x18  // 通道字15

// GPIO控制函数声明
// P0 高低电平控制
void AD9959_P0_H(void); ///< 拉高通道0的GPIO引脚
void AD9959_P0_L(void); ///< 拉低通道0的GPIO引脚

// P1 高低电平控制
void AD9959_P1_H(void); ///< 拉高通道1的GPIO引脚
void AD9959_P1_L(void); ///< 拉低通道1的GPIO引脚

// UP 高低电平控制
void AD9959_UP_H(void); ///< 拉高更新引脚
void AD9959_UP_L(void); ///< 拉低更新引脚

// PDC 高低电平控制
void AD9959_PDC_H(void); ///< 拉高省电模式引脚
void AD9959_PDC_L(void); ///< 拉低省电模式引脚

// SDIO0 高低电平控制
void AD9959_SDIO0_H(void); ///< 拉高数据引脚SDIO0
void AD9959_SDIO0_L(void); ///< 拉低数据引脚SDIO0

// P2 高低电平控制
void AD9959_P2_H(void); ///< 拉高通道2的GPIO引脚
void AD9959_P2_L(void); ///< 拉低通道2的GPIO引脚

// SCK 高低电平控制
void AD9959_SCK_H(void); ///< 拉高时钟引脚
void AD9959_SCK_L(void); ///< 拉低时钟引脚

// CS 高低电平控制
void AD9959_CS_H(void); ///< 拉高片选引脚
void AD9959_CS_L(void); ///< 拉低片选引脚

// RST 高低电平控制
void AD9959_RST_H(void); ///< 拉高复位引脚
void AD9959_RST_L(void); ///< 拉低复位引脚

// SDIO1 高低电平控制
void AD9959_SDIO1_H(void); ///< 拉高数据引脚SDIO1
void AD9959_SDIO1_L(void); ///< 拉低数据引脚SDIO1

// P3 高低电平控制
void AD9959_P3_H(void); ///< 拉高通道3的GPIO引脚
void AD9959_P3_L(void); ///< 拉低通道3的GPIO引脚

// SDIO2 高低电平控制
void AD9959_SDIO2_H(void); ///< 拉高数据引脚SDIO2
void AD9959_SDIO2_L(void); ///< 拉低数据引脚SDIO2

// SDIO3 高低电平控制
void AD9959_SDIO3_H(void); ///< 拉高数据引脚SDIO3
void AD9959_SDIO3_L(void); ///< 拉低数据引脚SDIO3

// 功能函数声明
void AD9959_Start(void);                                                                                                   ///< 启动AD9959设备
void AD9959_Reset(void);                                                                                                   ///< 复位AD9959设备
void AD9959_IO_UpDate(void);                                                                                               ///< 更新AD9959寄存器
void AD9959_Init(void);                                                                                                    ///< 初始化AD9959设备
void AD9959_WByte(unsigned char byte);                                                                                     ///< 写入一个字节到AD9959
void AD9959_Set_Fre(u32 Fout);                                                                                             ///< 设置输出频率
void AD9959_Set_Pha(float Pout);                                                                                           ///< 设置输出相位
void AD9959_WRrg(u8 reg, u32 _data);                                                                                       ///< 向寄存器写入数据
void AD9959__Sweep_Fre(u8 Channel, u32 FreS, u32 FreE, float FTstep, float RTstep, u32 FFstep, u32 RFstep, u8 DWELL);      ///< 配置频率扫描
void AD9959__Sweep_Amp(u8 Channel, u16 ampS, u32 ampE, float FTstep, float RTstep, u32 FFstep, u32 RFstep, u8 DWELL);      ///< 配置幅度扫描
void AD9959_Sweep_Phase(u8 Channel, u16 phaseS, u32 phaseE, float FTstep, float RTstep, u32 FFstep, u32 RFstep, u8 DWELL); ///< 配置相位扫描
void AD9959__Sweep_Trigger(u8 Channel);                                                                                    ///< 触发扫描
void AD9959_Single_Output(u8 Channel, u32 Fout, float Pout, u16 amp);                                                      ///< 单通道波形输出
void AD9959_Set_Amp(u16 amp);                                                                                              ///< 设置输出幅度
void AD9959_Ch(u8 Channel);                                                                                                ///< 选择输出通道
void AD9959_proc(void);                                                                                                     ///< 输出逻辑设置
#endif // __AD9959_H__
