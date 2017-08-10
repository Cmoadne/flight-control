#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"
#include "stm32f4xx_conf.h"

#define USART3_REC_LEN              20     //定义最大接收字节数 200
//如果想串口中断接收，请不要注释以下宏定义
void uart3_init(u32 bound);
void Usart3SendResult(char ch);

#endif


