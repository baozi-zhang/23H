#ifndef __CMD_TO_FUN_H__
#define __CMD_TO_FUN_H__
#include "commond_init.h"


void CTRL_INIT(void);

/**
 * @brief 启动DA波形发生器。
 */
void DA_FPGA_START(void);

/**
 * @brief 停止DA波形发生器。
 */
void DA_FPGA_STOP(void);

void AD_FREQ_CLR_ENABLE(int ch);
void AD_FREQ_CLR_DISABLE(int ch);
void AD_FREQ_START(int ch);
void AD_FREQ_STOP(int ch);

void AD_FREQ_SET(int ch);
void AD_FIFO_WRITE_ENABLE(int ch);
void AD_FIFO_WRITE_DISABLE(int ch);
void AD_FIFO_READ_ENABLE(int ch);
void AD_FIFO_READ_DISABLE(int ch);
#endif //__CMD_TO_FUN_H__
