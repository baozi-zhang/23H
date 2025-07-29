#ifndef __KALMAN_H
#define __KALMAN_H
#include "bsp_system.h"

typedef struct
{
	float x;// 系统的状态
	float A;//系统参数
	float H;//系统参数
	float Q;//过程噪声协方差
	float R;//测量噪声协方差
  float	p;//误差协方差
	float k;
}kalman_state;
 

void Kalman_init(kalman_state *state,float init_x,float init_p);//参数初始化
float kalman_filter(kalman_state *state,float z_measure);//z_measure为测量值  卡尔曼滤波的五个步骤
float kalman( u8 i ,float z_measure );
float kalman_thd(float z_measure);
#endif
