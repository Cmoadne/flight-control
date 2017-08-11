#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f4xx.h"

/*下面的方式是通过直接操作库函数方式读取IO*/
#define KEY1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) //PE4
#define KEY2 		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)	//PC14 
#define KEY3 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5) //PE5
#define KEY4 	    GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)	//PC13


/*下面方式是通过位带操作方式读取IO*/
/*
#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 
#define KEY2 		PEin(2)		//P32
#define WK_UP 	PAin(0)		//PA0
*/


// //都是在三档  飞行状态  0  在地上  1 起飞过程  2 飞行过程  3 停机过程  切换到其他2档直接状态回0；
//二档切三档  如果已经解锁了 且高度大于50cm那么是在 飞行过程中
//飞行过程中如果 高度大于某个值，开始接收左右控制值 否则就是1500

//飞行过程中2  直接油门1500  高度1980
//
extern char fly_state_flag;

extern char stop_mode_flag; //如果为0 原地停止  为1 需要往后飞一点

extern u16 control_height; //起飞降落时候用的高度
extern char back_down_flag;
#define KEY1_PRES 	0x01
#define KEY2_PRES	0x02
#define KEY3_PRES	0x04
#define KEY4_PRES   0x08

void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8);  		//按键扫描函数	
void Key_CALIBRATE(void);

void fly_mode_duty(void);

#endif
