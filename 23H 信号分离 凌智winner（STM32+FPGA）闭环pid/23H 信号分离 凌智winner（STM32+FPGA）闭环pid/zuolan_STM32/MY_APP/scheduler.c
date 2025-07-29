#include "scheduler.h"


uint8_t task_num;
typedef struct {
    void (*task_func)(void);  
    uint32_t rate_ms;         
    uint32_t last_run;      
} task_t;

u32 i=0;


void uart_proc(void)
{
	
	my_printf(&huart1,"%f,%d\r\n",vol_amp2/2*10,output); 
}


static task_t scheduler_task[] =
{
    {ad_proc, 1, 0},        
		{key_proc,10,0},
		{stm32_adc_proc,1,0}
		// {wave_test,20,0},  
   // {DA_proc, 10, 0},        
    //{AD9959_proc, 1200, 0},   
    //{uart_proc, 10, 0},  
		//{Pid_Proc,1,0},
};


void scheduler_init(void)
{ 
    task_num = sizeof(scheduler_task) / sizeof(task_t);
}



void scheduler_run(void)
{
    for (uint8_t i = 0; i < task_num; i++)
    {
        
        uint32_t now_time = HAL_GetTick();

       
        if (now_time >= scheduler_task[i].rate_ms + scheduler_task[i].last_run)
        {
            
            scheduler_task[i].last_run = now_time;

           
            scheduler_task[i].task_func();
        }
    }
}
