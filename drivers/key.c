/****************(C) COPYRIGHT 2017 Cmoadne********************
// 文件名 : key.c
// 路径   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\drivers
// 作者   : Cmoande
// 日期   : 2017/05/15
// 描述   : 按键驱动，用于飞控校准
// 备注   :
// 版本   : V0.0  2017.5.15  初始版本
******************************************************************/ 

#include "key.h"
#include "data_transfer.h"
#include "usart.h"
#include "imu.h"
#include "mpu6050.h"
#include "ak8975.h"
#include "ms5611.h"
#include "rc.h"
#include "ctrl.h"
#include "time.h"
#include "usbd_user_hid.h"
#include "ultrasonic.h"
#include "led.h"

char key_mode = 0;

char fly_state_flag = 0;
char stop_mode_flag = 1; //默认为后退降落
u16 control_height = 1000; //起飞降落时候用的高度
char back_down_flag = 0;   //可以后退降落了   0 不能降落   1 可以降落   2 判断降落
//按键初始化函数
void KEY_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE, ENABLE);//

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4; //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);//


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
    //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//下拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA0

} 
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY1按下
//2，KEY2按下
//3，KEY3按下 
//4，KEY4按下 
//注意此函数有响应优先级,KEY1>KEY2>KEY3>KEY4!!
u8 KEY_Scan(u8 mode)
{	 
    static u8 key_up=1;//按键按松开标志
    if(mode)key_up=1;  //支持连按		  
    if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
    {
        key_up=0;
        if(KEY1==0)return 0x01;
        else if(KEY2==0)return 0x02;
        else if(KEY3==0)return 0x04;
        else if(KEY4==0)return 0x08;
    }else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	    
    return 0x00;// 无按键按下
}

#define KEY_TIMES 0
#define KEY_DOWN_DELAY_TIMES 200
#define KEY_DOWN_DELAY_TIMES_COMMOND 100

void Key_CALIBRATE(void)
{
    static u8 key = 0xff;          //按键返回值
    static u8 key_reserved = 0xff;
    static u8 key_times = 0;    //按键按下次数，消抖
    static u16 keydowndelay = 0; //按下以后记次数之后校准
    static u8 key_flag = 0;     //有按键按下正在计数延时校准的flag

    if (!key_flag)
    {
        key = KEY_Scan (1);
        if ( key)
        {
            if(key == key_reserved )
            {
                key_times++;
                if (key_times > KEY_TIMES)
                {
                    key_flag = 1;
                }
            }
            key_reserved = key;
        }
        else
            key_times = 0;
    }
    else
    {
        keydowndelay++;        
        if (keydowndelay > KEY_DOWN_DELAY_TIMES)
        {
            keydowndelay = 0;
            key_flag = 0;
            
            if(key_mode == 0)
            {
                switch(key)
                {
                case 0x01: LED1_ON;key_mode = 1;   break;          //USART_SendData(USART1,'\n');         //按键1        
                case 0x02: Mag_CALIBRATED = 1;  break;                       //4
                case 0x04: mpu6050.Acc_CALIBRATE = 1;  break;              //2
                case 0x08: mpu6050.Gyro_CALIBRATE = 1;break;                                   //3
                default:    break;
                }
            }
            else if(key_mode == 1)
            {
                switch(key)
                {
                case 0x01: LED1_OFF;key_mode = 0;  
                    break; //发送已经可以停机指令//mpu6050.Acc_CALIBRATE = 1;  break;   //1
                case 0x02: //Usart1SendSelect('3'); break;//mpu6050.Gyro_CALIBRATE = 1;break;                        //4
                    break;
                case 0x04: 
                if (fly_state_flag == 0)
                {
                    fly_state_flag = 1;  //起飞
                    stop_mode_flag = 0;  //原地停止
                }
                //Usart1SendSelect('1'); break; //发送已经可以停机指令//Mag_CALIBRATED = 1;  break;              //2
                    break;
                case 0x08: 
                if (fly_state_flag == 0)
                {
                    fly_state_flag = 1;  //起飞
                    stop_mode_flag = 1;  //后退停止
                }
                    //Usart1SendSelect('2'); break; //发送已经可以停机指令//USART_SendData(USART1,'\n');         //按键3
                default:    break;
                }
            }
        }
    }
}

#define DELAY_FLY_NUM 50   //50*100  起飞延时
#define BASIC_ONE_DELAY    170   //要求1降落延时
void fly_mode_duty(void)
{
    static int start_fly_delay = 0;   //按键后延时计数
    static char delay_finish_flag = 0;  //完成延时
    static char first_in_stop = 0;    //第一次进入降落判断

    static int basic_1_count = 0;
    if (fly_state_flag == 0)   //停止
    {

    }
    else if (fly_state_flag == 1)   //起飞
    {
        if (delay_finish_flag == 0)  //按键后延时一段时间起飞
        {
            start_fly_delay++;
            if (start_fly_delay > DELAY_FLY_NUM)
            {
                delay_finish_flag = 1;
                control_height = 1000;
                start_fly_delay = 0;
            }
        }
        else if (delay_finish_flag == 1)
        {
            control_height +=30;
            if (control_height > 1950)
            {
                fly_state_flag = 2;    //完成起飞
                delay_finish_flag = 0;
            }
        } 
    }
    else if (fly_state_flag == 2)  //飞行
    {
        if (stop_mode_flag == 0)  //按键1 原地降落 基本任务1要原地降落
        {
            basic_1_count++;
            if (basic_1_count > BASIC_ONE_DELAY)
            {
                basic_1_count = 0;
                fly_state_flag = 3;
            }
        }

    }
    else if (fly_state_flag == 3)  //降落
    {
        if (stop_mode_flag == 0)  //原地降落
        {
            if (first_in_stop == 0)   //第一次进入降落
            {
                control_height = 2000;  //高度最大
                first_in_stop = 1;
            }
            control_height -=20;
            if (control_height < 1030 )
            {
                if (ultra_distance < 300)
                {
                    fly_state_flag = 0;    //完成降落
                    first_in_stop = 0;  //便于下次
                    stop_mode_flag = 1;  //后退停止
                }
                else
                    control_height += 20;
            }
        }
        else if (stop_mode_flag == 1)  //后退降落
        {
            if (back_down_flag == 1)   //可以降落
            {
                if (first_in_stop == 0)   //第一次进入降落
                {
                    control_height = 2000;  //高度最大
                    first_in_stop = 1;
                }
                control_height -=20;
                if (control_height < 1030 )
                {
                    if (ultra_distance < 300)
                    {
                        fly_state_flag = 0;    //完成降落
                        first_in_stop = 0;  //便于下次
                        stop_mode_flag = 1;  //后退停止
                        back_down_flag = 0;  //重新清0
                    }
                    else
                        control_height += 20;
                }
            }
        }
        
    }
}

/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/



















