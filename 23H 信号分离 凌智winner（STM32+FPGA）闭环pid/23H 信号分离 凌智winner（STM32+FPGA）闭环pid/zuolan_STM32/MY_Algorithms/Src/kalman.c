#include "kalman.h"
void Kalman_init(kalman_state *state,float init_x,float init_p)//参数初始化
{
   	state->x = init_x;
    state->p = init_p;
    state->A = 1;
    state->H = 1;
    state->Q =0.03445;
    state->R =0.07;
}
float kalman_filter(kalman_state *state,float z_measure)//z_measure为测量值  卡尔曼滤波的五个步骤
{
	 state->x = state->A * state->x;//计算预测值
   state->p = state->A * state->A * state->p + state->Q;  // 计算协方差 p(n|n-1)=A^2*p(n-1|n-1)+q 
	 state->k = state->p * state->H / (state->p * state->H * state->H + state->R);//计算卡尔曼增益k
	 state->x = state->x + state->k* (z_measure - state->H * state->x);//计算出估计值
	 state->p = (1 - state->k * state->H) * state->p;//计算本次最优状态估计值的协方差矩阵，为下一次迭代做准备
	
	 return  state->x;
}

//////////////////////////////////////////////////////////
u8 last=1,current=0; 
u8 last1=1,current1=0; 
kalman_state state[10];
kalman_state state1;
float kalman( u8 i ,float z_measure )
{
		if(current!=last)
		{
			last=current;
			for(i=0;i<10;i++)
			{
				 Kalman_init(&state[i],z_measure,0.01);
			}
		}
		else
		{
			 z_measure=kalman_filter(&state[i],z_measure);
		}
		return z_measure;
}

float kalman_thd(float z_measure)
{
	if(current1!=last1)
		{
			last1=current1;
			Kalman_init(&state1,z_measure,1);
		}
		else
		{
			 z_measure=kalman_filter(&state1,z_measure);
		}
		return z_measure;
}

