#ifndef __BEEP_H
#define __BEEP_H	 
#include "stm32f4xx.h"

#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
//LED�˿ڶ���  MEM_ADDR(BITBAND(addr, bitnum))
#define BEEP PBout(9)	// ����������IO 

void BEEP_Init(void);//��ʼ��	

void TIM11_PWM_Init(u32 arr,u32 psc);
#endif

















