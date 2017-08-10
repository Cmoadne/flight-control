/****************(C) COPYRIGHT 2017 Cmoadne********************
// �ļ��� : key.c
// ·��   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\drivers
// ����   : Cmoande
// ����   : 2017/05/15
// ����   : �������������ڷɿ�У׼
// ��ע   :
// �汾   : V0.0  2017.5.15  ��ʼ�汾
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
char stop_fly_flag = 0;
//������ʼ������
void KEY_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE, ENABLE);//

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4; //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOE, &GPIO_InitStructure);//


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
    //  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;//����
    GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA0

} 
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY1����
//2��KEY2����
//3��KEY3���� 
//4��KEY4���� 
//ע��˺�������Ӧ���ȼ�,KEY1>KEY2>KEY3>KEY4!!
u8 KEY_Scan(u8 mode)
{	 
    static u8 key_up=1;//�������ɿ���־
    if(mode)key_up=1;  //֧������		  
    if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
    {
        key_up=0;
        if(KEY1==0)return 0x01;
        else if(KEY2==0)return 0x02;
        else if(KEY3==0)return 0x04;
        else if(KEY4==0)return 0x08;
    }else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	    
    return 0x00;// �ް�������
}

#define KEY_TIMES 0
#define KEY_DOWN_DELAY_TIMES 200
#define KEY_DOWN_DELAY_TIMES_COMMOND 100

void Key_CALIBRATE(void)
{
    static u8 key = 0xff;          //��������ֵ
    static u8 key_reserved = 0xff;
    static u8 key_times = 0;    //�������´���������
    static u16 keydowndelay = 0; //�����Ժ�Ǵ���֮��У׼
    static u8 key_flag = 0;     //�а����������ڼ�����ʱУ׼��flag

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
                case 0x01: LED1_ON;key_mode = 1;   break;          //USART_SendData(USART1,'\n');         //����1        
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
                case 0x01: LED1_OFF;key_mode = 0;  break; //�����Ѿ�����ͣ��ָ��//mpu6050.Acc_CALIBRATE = 1;  break;   //1
                case 0x02: //Usart1SendSelect('3'); break;//mpu6050.Gyro_CALIBRATE = 1;break;                                 //4
                case 0x04: //Usart1SendSelect('1'); break; //�����Ѿ�����ͣ��ָ��//Mag_CALIBRATED = 1;  break;              //2
                case 0x08: //Usart1SendSelect('2'); break; //�����Ѿ�����ͣ��ָ��//USART_SendData(USART1,'\n');         //����3
                default:    break;
                }
            }
        }
    }
}


/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/



















