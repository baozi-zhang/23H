#ifndef __MY_USART_PACK_H__
#define __MY_USART_PACK_H__
#include "my_usart.h"
#include "stdint.h"

// 定义帧头和帧尾
#define FRAME_HEADER 0xA5
#define FRAME_TAIL 0x5A
#define MIN_FRAME_LENGTH 3

// 定义数据类型
typedef enum
{
    TYPE_BYTE,
    TYPE_SHORT,
    TYPE_INT,
    TYPE_FLOAT
} DataType;

// 定义解析模板的最大长度
#define MAX_VARIABLES 10

// 接口函数声明
void SetParseTemplate(DataType *templateArray, void **variableArray, uint16_t count);
void ParseFrame(uint8_t *buffer, uint16_t length);
uint16_t PrepareFrame(uint8_t *buffer, uint16_t maxLength);
void SendFrame(uint8_t *frame, uint16_t length);

#endif // __MY_USART_PACK_H__
