#ifndef __MY_HMI_H__
#define __MY_HMI_H__
#include "my_usart.h"
void HMI_Send_String(char *obj_name, char *show_data);
void HMI_Send_Int(char *obj_name, int show_data);
void HMI_Send_Float(char *obj_name, float show_data, int point_index);
void HMI_Wave_Clear(char *obj_name, int ch);
void HMI_Write_Wave_Low(char *obj_name, int ch, int val);
void HMI_Write_Wave_Fast(char *obj_name, int ch, int len, int *val);
#endif // __MY_HMI_H__
