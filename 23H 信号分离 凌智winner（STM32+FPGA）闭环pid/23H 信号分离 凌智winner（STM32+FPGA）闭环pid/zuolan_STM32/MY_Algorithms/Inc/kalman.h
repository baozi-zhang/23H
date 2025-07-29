#ifndef __KALMAN_H
#define __KALMAN_H
#include "bsp_system.h"

typedef struct
{
	float x;// ϵͳ��״̬
	float A;//ϵͳ����
	float H;//ϵͳ����
	float Q;//��������Э����
	float R;//��������Э����
  float	p;//���Э����
	float k;
}kalman_state;
 

void Kalman_init(kalman_state *state,float init_x,float init_p);//������ʼ��
float kalman_filter(kalman_state *state,float z_measure);//z_measureΪ����ֵ  �������˲����������
float kalman( u8 i ,float z_measure );
float kalman_thd(float z_measure);
#endif
