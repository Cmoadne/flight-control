/****************(C) COPYRIGHT 2017 Cmoadne********************
// 文件名 : adc.c
// 路径   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\drivers
// 作者   : Cmoande
// 日期   : 2017/05/15
// 描述   : 飞控板ADC电压采样，低压报警
// 备注   : 
// 版本   : V0.0 2017.5.15 去年版本，适用于今年
******************************************************************/ 

#include "adc.h"
	 
#define  ADC_TIMES   100
#define  BEEP_ADC  2787     //   2200/4096*2。5/0.163 = 10.5V   10.5V报警  0.163是电路分压比
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
			  
//获得ADC值
//ch: @ref ADC_channels 
//通道值 0~16取值范围为：ADC_Channel_0~ADC_Channel_16
//返回值:转换结果
u16 Get_Adc(u8 ch)   
{
    //设置指定ADC的规则组通道，一个序列，采样时间
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    

    ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

    return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}







void  Adc1CH0_Init(void)
{    
    GPIO_InitTypeDef  GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef       ADC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1时钟

    //先初始化ADC1通道0 IO口
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PA0 通道0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟输入
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//不带上下拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  

    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束	 


    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz 
    ADC_CommonInit(&ADC_CommonInitStructure);//初始化

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式	
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
    ADC_InitStructure.ADC_ExternalTrigConv = 0;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
    ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1 
    ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化


    ADC_Cmd(ADC1, ENABLE);//开启AD转换器	

}				  


/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/






