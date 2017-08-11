#ifndef _USART_H
#define _USART_H

#include "stm32f4xx.h"

//自增
#define USART_REC_LEN  			20  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

extern u16 RX_auto[];               //上层控制的数组

extern char  USART_RX_BUF_YAW[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern char  USART_RX_BUF_PITCH[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern char  USART_RX_BUF_ROLL[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符

extern u16 USART_RX_STA;         		//接收状态标记
extern char use_i_flag;
extern int start_information;
extern u8 beep_alarm_flag;
extern u8 command_beep_flag;
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void Usart1SendSelect(char ch);

//结束b


extern u8 Rx_Buf[];
void Usart2_Init(u32 br_num);
void Usart2_IRQ(void);
void Usart2_Send(unsigned char *DataToSend ,u8 data_num);

void Uart5_Init(u32 br_num);
void Uart5_IRQ(void);
void Uart5_Send(unsigned char *DataToSend ,u8 data_num);
#endif
