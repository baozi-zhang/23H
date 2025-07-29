#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H

/*
****************************************************************************************
 *
 * 
 * 
 *                        _oo0oo_
 *                       o8888888o
 *                       88" . "88
 *                       (| -_- |)
 *                       0\  =  /0
 *                     ___/`---'\___
 *                   .' \\|     |// '.
 *                  / \\|||  :  |||// \
 *                 / _||||| -:- |||||- \
 *                |   | \\\  - /// |   |
 *                | \_|  ''\---/''  |_/ |
 *                \  .-\__  '-'  ___/-. /
 *              ___'. .'  /--.--\  `. .'___
 *           ."" '<  `.___\_<|>_/___.' >' "".
 *          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *          \  \ `_.   \_ __\ /__ _/   .-` /  /
 *      =====`-.____`.___ \_____/___.-`___.-'=====
 *                        `=---='
 * 
 * 
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *            佛祖保佑     永不宕机     永无BUG
 *
****************************************************************************************
*/

#include "main.h"
#include "dac.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "stm32f4xx_hal.h"
#include "scheduler.h"
#include "ad_measure.h"
#include "freq_measure.h"
#include "my_fft.h"
#include "my_usart.h"
#include "string.h"
#include "my_usart_pack.h"
#include "da_output.h"
#include "phase_measure.h"
#include "AD9959.h"
#include "my_filter.h"
#include "arm_math.h"
#include "commond_init.h"
#include "app_pid.h"
#include "key_app.h"
#include "da_output.h"
#include "kalman.h"
#include "arm_math.h"

extern u32 Modulated_wave;
extern u8 mdoe_flag;
extern u16 Phase;
extern u32 val;
extern u8 Carrier_indx;
extern int32_t output;
extern u32 pid_vin;
extern float detected_freq;
extern u16 fifo_data1[FIFO_SIZE], fifo_data2[FIFO_SIZE];       // 采样结果
#endif
