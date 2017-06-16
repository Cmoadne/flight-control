/****************(C) COPYRIGHT 2017 Cmoadne********************
// 文件名 : ultrasonic.c
// 路径   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\drivers
// 作者   : Cmoande
// 日期   : 2017/05/15
// 描述   : 超声波驱动
// 备注   : 初始化后，每100ms 调用一次duty 把irqn放入串口中断，通过ultra_start_f判断状态
// 版本   : V0.0 2017.5.15 初始化版本
******************************************************************/ 
#include "include.h"
#include "ultrasonic.h"
#include "usart.h"


void Ultrasonic_Init()
{
  Uart5_Init(9600);			//串口5初始化，函数参数为波特率
	
}

s8 ultra_start_f;
u8 ultra_time;
u8 ultra_ok = 0;
void Ultra_Duty()
{
	u8 temp[3];

	ultra_time++;
	ultra_time = ultra_time%2;
	
	
	if( ultra_time == 0 )  //100ms//改用发送中断，节省时间。
	{
/*
		UART5->DR = 0xe8;   //ks103地址（可设置）
		while( (UART5->SR & USART_FLAG_TXE) == 0 );
		
		UART5->DR = 0x02;   //++++
		while( (UART5->SR & USART_FLAG_TXE) == 0 );

		UART5->DR = 0xbc;  //70ms,带温度补偿
		while( (UART5->SR & USART_FLAG_TXE) == 0 );
*/	
	#if defined(USE_KS103)
		temp[0] = 0xe8;
		temp[1] = 0x02;
		temp[2] = 0xbc;
		Uart5_Send(temp ,3);
	#elif defined(USE_US100)
		temp[0] = 0x55;
		Uart5_Send(temp ,1);
	#endif
///////////////////////////////////////////////
		ultra_start_f = 1;
	}
}

u16 ultra_distance,ultra_distance_old;
s16 ultra_delta;
void Ultra_Get(u8 com_data)
{
	static u8 ultra_tmp;
	
	if( ultra_start_f == 1 )
	{
		ultra_tmp = com_data;
		ultra_start_f = 2;
	}
	else if( ultra_start_f == 2 )
	{
		ultra_distance = (ultra_tmp<<8) + com_data;

        if (ultra_distance==0 || ultra_distance > 3500)
        {
            ultra_distance = MAX_HEIGHT_CONTROL + 300;
        }

		ultra_start_f = 0;
		ultra_ok = 1;
	}
	
	ultra_delta = ultra_distance - ultra_distance_old;
	
	ultra_distance_old = ultra_distance;
	
}

/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/

