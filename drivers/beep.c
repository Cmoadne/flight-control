/****************(C) COPYRIGHT 2017 Cmoadne********************
// �ļ��� : beep.c
// ·��   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\drivers
// ����   : Cmoande
// ����   : 2017/05/15
// ����   : ������PWM��ʼ��
// ��ע   :
// �汾   : V0.0 2017.5.15 ȥ��汾�����ڽ���
******************************************************************/ 

#include "beep.h" 

//��ʼ��PF8Ϊ�����		    
//BEEP IO��ʼ��
void BEEP_Init(void)
{   
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOFʱ��
  
  //��ʼ����������Ӧ����GPIOF8
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
	
  GPIO_ResetBits(GPIOB,GPIO_Pin_9);  //��������Ӧ����GPIOF8���ͣ� 
}



void TIM11_PWM_Init(u32 arr,u32 psc)
{		 					 
    //�˲������ֶ��޸�IO������

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE);  	//TIM14ʱ��ʹ��    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ��PORTFʱ��	

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_TIM11); //GPIOF9����Ϊ��ʱ��14

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;           //GPIOF9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
    GPIO_Init(GPIOB,&GPIO_InitStructure);              //��ʼ��PF9

    TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

    TIM_TimeBaseInit(TIM11,&TIM_TimeBaseStructure);//��ʼ����ʱ��14

    //��ʼ��TIM14 Channel1 PWMģʽ	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ե�
    TIM_OC1Init(TIM11, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1

    TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

    TIM_ARRPreloadConfig(TIM11,ENABLE);//ARPEʹ�� 

    TIM_Cmd(TIM11, ENABLE);  //ʹ��TIM14

    		TIM_SetCompare1(TIM11,400);	//�޸ıȽ�ֵ���޸�ռ�ձ�  
}  

/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/


