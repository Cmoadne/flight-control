/****************(C) COPYRIGHT 2017 Cmoadne********************
// 文件名 : init.c
// 路径   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\applications
// 作者   : Cmoande
// 日期   : 2017/05/10
// 描述   : 外设初始化
// 备注   :
// 版本   : V0.0  2017.6.16  Cmoadne  初始代码  对比去年代码
                *本次更改点
                      48 波特率50000->115200   Usart2_Init(500000);			//串口2初始化，函数参数为波特率   
                      新增KEYinit初始化按键
                      新增串口上层控制
                      增加ADC初始化

******************************************************************/ 
#include "include.h"
#include "pwm_out.h"
#include "mpu6050.h"
#include "i2c_soft.h"
#include "led.h"
#include "ctrl.h"
#include "ms5611.h"
#include "ak8975.h"
#include "ultrasonic.h"

#include "key.h"
#include "adc.h"
#include "beep.h"

u8 All_Init()
{
	NVIC_PriorityGroupConfig(NVIC_GROUP);		//中断优先级组别设置
	
	SysTick_Configuration(); 	//滴答时钟
	
	I2c_Soft_Init();					//初始化模拟I2C
	
	PWM_IN_Init();						//初始化接收机采集功能
	
	PWM_Out_Init(400);				//初始化电调输出功能	
	
	Usb_Hid_Init();						//飞控usb接口的hid初始化
	
	MS5611_Init();						//气压计初始化
	
	Delay_ms(400);						//延时
	
	MPU6050_Init(20);   			//加速度计、陀螺仪初始化，配置20hz低通
	
	LED_Init();								//LED功能初始化
	
    //更改6.16
	Usart2_Init(115200);			//串口2初始化，函数参数为波特率
	//Usart2_Init(256000);


    uart_init(115200);          //树莓派串口

    KEY_Init();                 //按键初始化

    Adc1CH0_Init();             //adc1 CH0

    TIM11_PWM_Init(400-1,168-1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz. 蜂鸣器
    //结束6.16
	
	//TIM_INIT();
	
	Para_Init();							//参数初始化
	
	Delay_ms(100);						//延时
	
	Ultrasonic_Init();   			//超声波初始化
	
	Cycle_Time_Init();
	
	ak8975_ok = !(ANO_AK8975_Run());
	
	if( !mpu6050_ok )
	{
		LED_MPU_Err();
	}
	else if( !ak8975_ok )
	{
		LED_Mag_Err();
	}
	else if( !ms5611_ok )
	{
		LED_MS5611_Err();
	}
 	return (1);
}

/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/
