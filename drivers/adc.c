/****************(C) COPYRIGHT 2017 Cmoadne********************
// �ļ��� : adc.c
// ·��   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\drivers
// ����   : Cmoande
// ����   : 2017/05/15
// ����   : �ɿذ�ADC��ѹ��������ѹ����
// ��ע   : 
// �汾   : V0.0 2017.5.15 ȥ��汾�������ڽ���
******************************************************************/ 

#include "adc.h"
	 
#define  ADC_TIMES   100
#define  BEEP_ADC  2787     //   2200/4096*2��5/0.163 = 10.5V   10.5V����  0.163�ǵ�·��ѹ��
u8 ADC_Duty(void)
{
    static u16 times = 0;
    static u32 sum_adc = 0;
    static u8  beep_flag = 0;
    u32 adcx = 0;

    sum_adc += Get_Adc(ADC_Channel_0);
    times ++;
    if (times == ADC_TIMES)
    {
        adcx = sum_adc/ADC_TIMES;
        if (adcx < BEEP_ADC)
            beep_flag = 1;       
        else
            beep_flag = 0;
        sum_adc = 0;
        times = 0;
    }
    
    return beep_flag;
    

}
			  
//���ADCֵ
//ch: @ref ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
//����ֵ:ת�����
u16 Get_Adc(u8 ch)   
{
    //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    

    ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������	

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

    return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}







void  Adc1CH0_Init(void)
{    
    GPIO_InitTypeDef  GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef       ADC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��

    //�ȳ�ʼ��ADC1ͨ��0 IO��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PA0 ͨ��0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  

    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 


    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
    ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
    ADC_InitStructure.ADC_ExternalTrigConv = 0;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
    ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��


    ADC_Cmd(ADC1, ENABLE);//����ADת����	

}				  


/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/






