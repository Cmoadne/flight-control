/****************(C) COPYRIGHT 2017 Cmoadne********************
// 文件名 : include.h
// 路径   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\applications
// 作者   : Cmoande
// 日期   : 2017/05/10
// 描述   :
// 备注   :
// 版本   : V0.0  2017.5.10  Cmoadne  初始代码,对比去年代码
                    *本次更改点
                            41   关闭USB数据传输     注释
                            120  增加我的宏定义
                    *增加点            
                            100  最大上升、下降速度
                    *不同点
                            104  遥控能控制最大角速度  MAX_CTRL_ANGLE  去年代码25.0f
                            110  Yaw能控制最大角速度   MAX_CTRL_YAW_SPEED 去年代码450.0f            
******************************************************************/ 

#ifndef _INCLUDE_H_
#define _INCLUDE_H_

#include "stm32f4xx.h"
#include "scheduler.h"
#include "time.h"
#include "init.h"
#include "parameter.h"
#include "pwm_in.h"
#include "usart.h"
#include "usbd_user_hid.h"
#include "data_transfer.h"

//================系统===================
#define CTRL_HEIGHT 1       //0失能，1使能

#define USE_US100           //使用us100型号超声波 
// #define USE_KS103					//使用ks103型号超声波

#define MAXMOTORS 		(4)		//电机数量
#define GET_TIME_NUM 	(5)		//设置获取时间的数组数量
#define CH_NUM 				(8) 	//接收机通道数量

#define USE_TOE_IN_UNLOCK 0 // 0：默认解锁方式，1：外八解锁方式
#define ANO_DT_USE_USART2 	//开启串口2数传功能
//#define ANO_DT_USE_USB_HID	//开启飞控USBHID连接上位机功能
//=======================================
/***************中断优先级******************/
#define NVIC_GROUP NVIC_PriorityGroup_3		//中断分组选择
#define NVIC_PWMIN_P			1		//接收机采集
#define NVIC_PWMIN_S			1
#define NVIC_TIME_P       2		//暂未使用
#define NVIC_TIME_S       0
#define NVIC_UART_P				5		//暂未使用
#define NVIC_UART_S				1
#define NVIC_UART2_P			3		//串口2中断
#define NVIC_UART2_S			1
/***********************************************/

//================传感器===================
#define ACC_ADJ_EN 									//是否允许校准加速度计,(定义则允许)

#define OFFSET_AV_NUM 	50					//校准偏移量时的平均次数。
#define FILTER_NUM 			10					//滑动平均滤波数值个数

#define TO_ANGLE 				0.06103f 		//0.061036 //   4000/65536  +-2000   ???

#define FIX_GYRO_Y 			1.02f				//陀螺仪Y轴固有补偿
#define FIX_GYRO_X 			1.02f				//陀螺仪X轴固有补偿

#define TO_M_S2 				0.23926f   	//   980cm/s2    +-8g   980/4096
#define ANGLE_TO_RADIAN 0.01745329f //*0.01745 = /57.3	角度转弧度

#define MAX_ACC  4096.0f						//+-8G		加速度计量程
#define TO_DEG_S 500.0f      				//T = 2ms  默认为2ms ，数值等于1/T

enum
{
 A_X = 0,
 A_Y ,
 A_Z ,
 G_Y ,
 G_X ,
 G_Z ,
 TEM ,
 ITEMS ,
};

// CH_filter[],0横滚，1俯仰，2油门，3航向		
enum
{
 ROL= 0,
 PIT ,
 THR ,
 YAW ,
 AUX1 ,
 AUX2 ,
 AUX3 ,
 AUX4 ,
};
//=========================================

//================控制=====================
#define MAX_CTRL_ANGLE			25.0f										//遥控能达到的最大角度
#define ANGLE_TO_MAX_AS 		30.0f										//角度误差N时，期望角速度达到最大（可以通过调整CTRL_2的P值调整）
#define CTRL_2_INT_LIMIT 		0.5f *MAX_CTRL_ANGLE		//外环积分幅度

#define MAX_CTRL_ASPEED 	 	300.0f									//ROL,PIT允许的最大控制角速度
#define MAX_CTRL_YAW_SPEED 	450.0f									//YAW允许的最大控制角速度
#define CTRL_1_INT_LIMIT 		0.5f *MAX_CTRL_ASPEED		//内环积分幅度


#define MAX_PWM				100			//	最大PWM输出为100%油门
#define MAX_THR       80 			//	油门通道最大占比80%，留20%给控制量
#define READY_SPEED   20			//	解锁后电机转速20%油门
//=========================================



//==================我的宏定义====================
//  全局输出，CH_filter[],0横滚，1俯仰，2油门，3航向 范围：+-500	
#define CMOADNE_BOARD               //有宏定义就是用自己的飞控板，没有就是用其他飞控板

#ifdef CMOADNE_BOARD            //ANO不用报警
//#define LOW_VOLATAGE_ALARM          //是否开启抵押报警
#endif

#define NS_NO_SIGNAL    0           //无信号
#define NS_MAN        1             //完全人
#define NS_COMPUTER   2             //完全电脑   一般用不着
#define NS_FINISH_AUTO  3         //完成测试的
#define NS_NOW_ONE_TEST   4   //当前测试



#define HEIGHT_CTRL_DEATHZOOM   50  //高度控制死区，5cm以内不控制
#define HEIGHT_UP_THR           600 //高度上升油门值
#define HEIGHT_KEEP             500 //高度保持油门值
#define HEIGHT_DOWN_THR         400 //高度下降油门值
#define HEIGHT_THR_CTRL_MAX     100 //油门最大调解量


//#define ULTRA_SPEED 		 300    // mm/s
//#define ULTRA_MAX_HEIGHT 1500   // mm
//#define ULTRA_INT        300    // 积分幅度


#define ULTRA_SPEED 		 800    // mm/s                                         //改了啊啊啊  300
#define ULTRA_INT        300    // 积分幅度
#define MAX_HEIGHT_CONTROL 1230   // mm  最高高度
#define MAN_ULTRA_SPEED       100       //MM/S
#define MIN_HEIGHT_CONTROL  300     //最低高度
#define ULTRA_ERROR         200     //上下两次大于，丢弃值


//#define UNLINE_PID     //跟踪使用非线性pid
#define ROLL_MID   1500
#define PITCH_MID  1500

#endif

/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/

