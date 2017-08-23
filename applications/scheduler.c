/****************(C) COPYRIGHT 2017 Cmoadne********************
// 文件名 : scheduler.c
// 路径   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\applications
// 作者   : Cmoande
// 日期   : 2017/05/15
// 描述   : 任务调度
// 备注   :
// 版本   : V0.0  2017.5.15  初始版本
******************************************************************/ 
#include "scheduler.h"
#include "include.h"
#include "time.h"
#include "mpu6050.h"
#include "ak8975.h"
#include "led.h"
#include "rc.h"
#include "imu.h"
#include "pwm_in.h"
#include "ctrl.h"
#include "ms5611.h"
#include "parameter.h"
#include "ultrasonic.h"
#include "height_ctrl.h"


#include "key.h"
#include "adc.h"
#include "beep.h"
#include "imagepid.h"
#include "usart3.h"

s16 loop_cnt;


loop_t loop;

void Loop_check()  //TIME INTTERRUPT
{
    loop.time++; //u16
    loop.cnt_2ms++;
    loop.cnt_5ms++;
    loop.cnt_10ms++;
    loop.cnt_20ms++;
    loop.cnt_50ms++;

    if( loop.check_flag == 1)
    {
        loop.err_flag ++;     //每累加一次，证明代码在预定周期内没有跑完。
    }
    else
    {	
        loop.check_flag = 1;	//该标志位在循环的最后被清零
    }
}

void Duty_1ms()
{
    //Get_Cycle_T(1);
    //LED_Display( LED_Brightness );								//20级led渐变显示
    ANO_DT_Data_Exchange();												//数传通信定时调用
}

float test[5];
void Duty_2ms()
{
    float inner_loop_time;

    inner_loop_time = Get_Cycle_T(0); 						//获取内环准确的执行周期

    //test[0] = GetSysTime_us()/1000000.0f;

    MPU6050_Read(); 															//读取mpu6轴传感器

    MPU6050_Data_Prepare( inner_loop_time );			//mpu6轴传感器数据处理

    CTRL_1( inner_loop_time ); 										//内环角速度控制。输入：执行周期，期望角速度，测量角速度，角度前馈；输出：电机PWM占空比。<函数未封装>

    RC_Duty( inner_loop_time , Rc_Pwm_In );				// 遥控器通道数据处理 ，输入：执行周期，接收机pwm捕获的数据。



    //test[1] = GetSysTime_us()/1000000.0f;
}

void Duty_5ms()
{
    float outer_loop_time;

    outer_loop_time = Get_Cycle_T(2);								//获取外环准确的执行周期

    //test[2] = GetSysTime_us()/1000000.0f;

    /*IMU更新姿态。输入：半个执行周期，三轴陀螺仪数据（转换到度每秒），三轴加速度计数据（4096--1G）；输出：ROLPITYAW姿态角*/
    IMUupdate(0.5f *outer_loop_time,mpu6050.Gyro_deg.x, mpu6050.Gyro_deg.y, mpu6050.Gyro_deg.z, mpu6050.Acc.x, mpu6050.Acc.y, mpu6050.Acc.z,&Roll,&Pitch,&Yaw);

    CTRL_2( outer_loop_time ); 											// 外环角度控制。输入：执行周期，期望角度（摇杆量），姿态角度；输出：期望角速度。<函数未封装>

    //test[3] = GetSysTime_us()/1000000.0f;
}

void Duty_10ms()
{
    if( MS5611_Update() ) 				//更新ms5611气压计数据
    {	
        baro_ctrl_start = 1;  //20ms
    }

    ANO_AK8975_Read();			//获取电子罗盘数据	


    //更改6.16 
#ifdef CMOADNE_BOARD        //匿名没有按键
    //if (!fly_ready)             //非解锁状态，按键校准
    Key_CALIBRATE ();
#endif 
}

void Duty_20ms()
{
    //更改6.16
    if(!fly_ready)
        Parameter_Save();

    pid_duty();
}

#define BEEP_COUNT_NUMS  3
#define SEND_NUM     2
char send_count_3 = 0; 
void Duty_50ms()
{
    //更改6.16
    if(NS != NS_MAN)
        get_ultra_expect_height ();
    else
        if (!fly_ready)      
            Mode();
//结束6.16
    //LED_Duty();								//LED任务
    Ultra_Duty();

    //更改6.16
    static char beep_count = 0;
    static char beep_flag = 0;
#ifdef  LOW_VOLATAGE_ALARM
    if (ADC_Duty() )//|| start_information || server_duty_flag)
    {
        beep_count++;
        if (beep_count > BEEP_COUNT_NUMS)
        {
            beep_count = 0;
            if (beep_flag == 1)
            {
                TIM_SetCompare1(TIM11,400);	//修改比较值，修改占空比 
                beep_flag = 0;
            }
            else
            {
                beep_flag = 1;
                TIM_SetCompare1(TIM11,200);	//修改比较值，修改占空比 
            }
        }
   
    }
    else
        TIM_SetCompare1(TIM11,400);	//修改比较值，修改占空比  
 
#endif
    if (start_information||beep_alarm_flag)
    {
        beep_count++;
        if (beep_count > BEEP_COUNT_NUMS)
        {
            beep_count = 0;
            if (beep_flag == 1)
            {
                TIM_SetCompare1(TIM11,400);	//修改比较值，修改占空比 
                beep_flag = 0;
            }
            else
            {
                beep_flag = 1;
                TIM_SetCompare1(TIM11,200);	//修改比较值，修改占空比 
            }
        }
    }
    else if (command_beep_flag||(fly_state_flag == 3)||(fly_state_flag == 1)) //自动起飞降落报警之后可能删掉
    {
        beep_count++;
        if (beep_count > 1)
        {
            beep_count = 0;
            if (beep_flag == 1)
            {
                TIM_SetCompare1(TIM11,400);	//修改比较值，修改占空比 
                beep_flag = 0;
            }
            else
            {
                beep_flag = 1;
                TIM_SetCompare1(TIM11,200);	//修改比较值，修改占空比 
            }
        }
    }
    else
        TIM_SetCompare1(TIM11,400);	//修改比较值，修改占空比  



    if (beep_alarm_flag)
    {
        LED2_ON;
        LED3_ON;
        LED4_ON;
    }
    else
    {
        LED2_OFF;
        LED3_OFF;
        LED4_OFF;
    }


    fly_mode_duty ();   //3档
    
    
    send_count_3++;
    if(get_tag_flag)
    {
        if ((((RX_auto[0]-1500)*(RX_auto[0]-1500) + (RX_auto[1]-1500)*(RX_auto[1]-1500) + (ultra_distance/10)*(ultra_distance/10)) < 22500)&& 
            (((RX_auto[0]-1500)*(RX_auto[0]-1500) + (RX_auto[1]-1500)*(RX_auto[1]-1500) + (ultra_distance/10)*(ultra_distance/10)) > 2500))
        {
            beep_alarm_flag = 1;
            if (send_count_3 > SEND_NUM)
            {
                Usart3SendResult(0x10);   //报警
                send_count_3 = 0;
            }
        }
        else
        {         
           beep_alarm_flag = 0;
           if (send_count_3 > SEND_NUM)
           {
               Usart3SendResult(0x20);   //不报警
               send_count_3 = 0;
           }       
        }
    }
    else
    {         
        beep_alarm_flag = 0;
        if (send_count_3 > SEND_NUM)
        {
            Usart3SendResult(0x20);   //不报警
            send_count_3 = 0;
        }     
    }

    //结束6.16
}


void Duty_Loop()   					//最短任务周期为1ms，总的代码执行时间需要小于1ms。
{

    if( loop.check_flag == 1 )
    {
        loop_cnt = time_1ms;

        Duty_1ms();							//周期1ms的任务

        if( loop.cnt_2ms >= 2 )
        {
            loop.cnt_2ms = 0;
            Duty_2ms();						//周期2ms的任务
        }
        if( loop.cnt_5ms >= 5 )
        {
            loop.cnt_5ms = 0;
            Duty_5ms();						//周期5ms的任务
        }
        if( loop.cnt_10ms >= 10 )
        {
            loop.cnt_10ms = 0;
            Duty_10ms();					//周期10ms的任务
        }
        if( loop.cnt_20ms >= 20 )
        {
            loop.cnt_20ms = 0;
            Duty_20ms();					//周期20ms的任务
        }
        if( loop.cnt_50ms >= 50 )
        {
            loop.cnt_50ms = 0;
            Duty_50ms();					//周期50ms的任务
        }

        loop.check_flag = 0;		//循环运行完毕标志
    }
}


/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/

