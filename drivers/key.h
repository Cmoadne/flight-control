#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f4xx.h"

/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
#define KEY1 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) //PE4
#define KEY2 		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)	//PC14 
#define KEY3 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5) //PE5
#define KEY4 	    GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)	//PC13


/*���淽ʽ��ͨ��λ��������ʽ��ȡIO*/
/*
#define KEY0 		PEin(4)   	//PE4
#define KEY1 		PEin(3)		//PE3 
#define KEY2 		PEin(2)		//P32
#define WK_UP 	PAin(0)		//PA0
*/


// //����������  ����״̬  0  �ڵ���  1 ��ɹ���  2 ���й���  3 ͣ������  �л�������2��ֱ��״̬��0��
//����������  ����Ѿ������� �Ҹ߶ȴ���50cm��ô���� ���й�����
//���й�������� �߶ȴ���ĳ��ֵ����ʼ�������ҿ���ֵ �������1500

//���й�����2  ֱ������1500  �߶�1980
//
extern char fly_state_flag;

extern char stop_mode_flag; //���Ϊ0 ԭ��ֹͣ  Ϊ1 ��Ҫ�����һ��

extern u16 control_height; //��ɽ���ʱ���õĸ߶�
extern char back_down_flag;
#define KEY1_PRES 	0x01
#define KEY2_PRES	0x02
#define KEY3_PRES	0x04
#define KEY4_PRES   0x08

void KEY_Init(void);	//IO��ʼ��
u8 KEY_Scan(u8);  		//����ɨ�躯��	
void Key_CALIBRATE(void);

void fly_mode_duty(void);

#endif
