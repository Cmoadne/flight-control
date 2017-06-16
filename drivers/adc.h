#ifndef __ADC_H
#define __ADC_H	
#include "stm32f4xx.h"

u8 ADC_Duty(void);					   
u16  Get_Adc(u8 ch); 				//获得某个通道值 
void  Adc1CH0_Init(void);
#endif 















