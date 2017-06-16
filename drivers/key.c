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
            switch(key)
            {
            case 0x01: mpu6050.Acc_CALIBRATE = 1;  break;   //
            case 0x02: mpu6050.Gyro_CALIBRATE = 1;break;
            case 0x04: Mag_CALIBRATED = 1;  break;
            case 0x08: //USART_SendData(USART1,'\n');         //按键4
                break;
            default:    break;
            }
        }
    }
}


/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/



















