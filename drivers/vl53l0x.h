#ifndef __VL530LX_H
#define __VL530LX_H
#include "stm32f4xx.h"


void vl53l0x_Init(u32 br_num);
void USART3_send_byte(uint8_t byte);
void send_3out(uint8_t *data,uint8_t length,uint8_t send);
void send_out(int16_t *data,uint8_t length,uint8_t send);
void display(int16_t *num,u8 send,u8 count);
void CHeck(uint8_t *re_data);
void send_com(u8 data);
void USART_Send(uint8_t *Buffer, uint8_t Length);
void USART_Send_bytes(uint8_t *Buffer, uint8_t Length);

extern u8 re_Buf_Data[8],Receive_ok;
#endif



