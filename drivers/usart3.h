#ifndef __USART3_H
#define __USART3_H
#include "stdio.h"
#include "stm32f4xx_conf.h"

#define USART3_REC_LEN              20     //�����������ֽ��� 200
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart3_init(u32 bound);
void Usart3SendResult(char ch);

#endif


