#include "pwm_in.h"
#include "rc.h"
#include "include.h"
#include "key.h"
#include "ultrasonic.h"

static void NS_select(void);
u16 Rc_Pwm_In[8];

void PWM_IN_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PWMIN_P;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_PWMIN_S;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PWMIN_P;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_PWMIN_S;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	 

    //////////////////////////////////////////////////////////////////////////////////////////////
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);

    TIM3->PSC = (168/2)-1;

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    /* TIM enable counter */
    TIM_Cmd(TIM3, ENABLE);

    /* Enable the CC2 Interrupt Request */
    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_CC4, ENABLE);
    /////////////////////////////////////////////////////////////////////////////////////////////
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM3);

    TIM4->PSC = (168/2)-1;

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);

    /* TIM enable counter */
    TIM_Cmd(TIM4, ENABLE);

    /* Enable the CC2 Interrupt Request */
    TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
    TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);
    TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);
    TIM_ITConfig(TIM4, TIM_IT_CC4, ENABLE);
}


////通道映射  数字是实际通道，  0表示通道1，哪个是0就哪个是通道1，前面的宏定义不是实际通道
////我的板子
//#ifdef CMOADNE_BOARD
//
//#define ONE_CH  1
//#define TWO_CH  0
//#define THREE_CH  2
//#define FOUR_CH  3
//#define FIVE_CH  7
//#define SIX_CH  6
//#define SEVEN_CH  5
//#define EIGHT_CH  4
//
//#else
//
//#define ONE_CH  0
//#define TWO_CH  1
//#define THREE_CH  2
//#define FOUR_CH  3
//#define FIVE_CH  4
//#define SIX_CH  5
//#define SEVEN_CH  6
//#define EIGHT_CH  7
//
//#endif


void _TIM3_IRQHandler(void)	
{
    static u16 temp_cnt1,temp_cnt1_2,temp_cnt2,temp_cnt2_2,temp_cnt3,temp_cnt3_2,temp_cnt4,temp_cnt4_2;

    //Feed_Rc_Dog(1);//RC


    //我的通道1   ANO通道2
    if(TIM3->SR & TIM_IT_CC2) 
    {
        TIM3->SR = ~TIM_IT_CC2;
        TIM3->SR = ~TIM_FLAG_CC2OF;
        if(GPIOC->IDR & GPIO_Pin_7)
        {
            temp_cnt2 = TIM_GetCapture2(TIM3);
        }
        else
        {
            temp_cnt2_2 = TIM_GetCapture2(TIM3);
            if(temp_cnt2_2>=temp_cnt2)
                Rc_Pwm_In[1] = temp_cnt2_2-temp_cnt2;
            else
                Rc_Pwm_In[1] = 0xffff-temp_cnt2+temp_cnt2_2+1;
        }
    }  
    //我的通道2   ANO通道1
    if(TIM3->SR & TIM_IT_CC1) 
    {
        TIM3->SR = ~TIM_IT_CC1;//TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
        TIM3->SR = ~TIM_FLAG_CC1OF;
        if(GPIOC->IDR & GPIO_Pin_6)
        {
            temp_cnt1 = TIM_GetCapture1(TIM3);
        }
        else
        {
            temp_cnt1_2 = TIM_GetCapture1(TIM3);
            if(temp_cnt1_2>=temp_cnt1)
                Rc_Pwm_In[0] = temp_cnt1_2-temp_cnt1;
            else
                Rc_Pwm_In[0] = 0xffff-temp_cnt1+temp_cnt1_2+1;
        }
    }
    //我的通道3
    if(TIM3->SR & TIM_IT_CC3) 
    {
        TIM3->SR = ~TIM_IT_CC3;
        TIM3->SR = ~TIM_FLAG_CC3OF;
        if(GPIOB->IDR & GPIO_Pin_0)
        {
            temp_cnt3 = TIM_GetCapture3(TIM3);
        }
        else
        {
            temp_cnt3_2 = TIM_GetCapture3(TIM3);
            if(temp_cnt3_2>=temp_cnt3)
                Rc_Pwm_In[2] = temp_cnt3_2-temp_cnt3;
            else
                Rc_Pwm_In[2] = 0xffff-temp_cnt3+temp_cnt3_2+1;
        }
    }
    //我的通道4
    if(TIM3->SR & TIM_IT_CC4) 
    {
        TIM3->SR = ~TIM_IT_CC4;
        TIM3->SR = ~TIM_FLAG_CC4OF;
        if(GPIOB->IDR & GPIO_Pin_1)
        {
            temp_cnt4 = TIM_GetCapture4(TIM3);
        }
        else
        {
            temp_cnt4_2 = TIM_GetCapture4(TIM3);
            if(temp_cnt4_2>=temp_cnt4)
                Rc_Pwm_In[3] = temp_cnt4_2-temp_cnt4;
            else
                Rc_Pwm_In[3] = 0xffff-temp_cnt4+temp_cnt4_2+1;
        }
    }
}

void _TIM4_IRQHandler(void)		
{
    static u16 temp_cnt1,temp_cnt1_2,temp_cnt2,temp_cnt2_2,temp_cnt3,temp_cnt3_2,temp_cnt4,temp_cnt4_2;

    //Feed_Rc_Dog(1);//RC

    //我的通道5  ANO8
    if(TIM4->SR & TIM_IT_CC4) 
    {
        TIM4->SR = ~TIM_IT_CC4;
        TIM4->SR = ~TIM_FLAG_CC4OF;
        if(GPIOD->IDR & GPIO_Pin_15)
        {
            temp_cnt4 = TIM_GetCapture4(TIM4);
        }
        else
        {
            temp_cnt4_2 = TIM_GetCapture4(TIM4);
            if(temp_cnt4_2>=temp_cnt4)
                Rc_Pwm_In[7] = temp_cnt4_2-temp_cnt4;
            else
                Rc_Pwm_In[7] = 0xffff-temp_cnt4+temp_cnt4_2+1;
        }
    }
    //我的通道6 ANO7
    if(TIM4->SR & TIM_IT_CC3) 
    {
        TIM4->SR = ~TIM_IT_CC3;
        TIM4->SR = ~TIM_FLAG_CC3OF;
        if(GPIOD->IDR & GPIO_Pin_14)
        {
            temp_cnt3 = TIM_GetCapture3(TIM4);
        }
        else
        {
            temp_cnt3_2 = TIM_GetCapture3(TIM4);
            if(temp_cnt3_2>=temp_cnt3)
                Rc_Pwm_In[6] = temp_cnt3_2-temp_cnt3;
            else
                Rc_Pwm_In[6] = 0xffff-temp_cnt3+temp_cnt3_2+1;
        }
    }
    //我的通道7  ANO6
    if(TIM4->SR & TIM_IT_CC2) 
    {
        TIM4->SR = ~TIM_IT_CC2;
        TIM4->SR = ~TIM_FLAG_CC2OF;
        if(GPIOD->IDR & GPIO_Pin_13)
        {
            temp_cnt2 = TIM_GetCapture2(TIM4);
        }
        else
        {
            temp_cnt2_2 = TIM_GetCapture2(TIM4);
            if(temp_cnt2_2>=temp_cnt2)
                Rc_Pwm_In[5] = temp_cnt2_2-temp_cnt2;
            else
                Rc_Pwm_In[5] = 0xffff-temp_cnt2+temp_cnt2_2+1;
        }
    }
    //我的通道8   ANO5
    if(TIM4->SR & TIM_IT_CC1) 
    {
        TIM4->SR = ~TIM_IT_CC1;//TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
        TIM4->SR = ~TIM_FLAG_CC1OF;
        if(GPIOD->IDR & GPIO_Pin_12)
        {
            temp_cnt1 = TIM_GetCapture1(TIM4);
        }
        else
        {
            temp_cnt1_2 = TIM_GetCapture1(TIM4);
            if(temp_cnt1_2>=temp_cnt1)
                Rc_Pwm_In[4] = temp_cnt1_2-temp_cnt1;
            else
                Rc_Pwm_In[4] = 0xffff-temp_cnt1+temp_cnt1_2+1;
        }
    }

    NS_select ();

}

#ifdef CMOADNE_BOARD
#define NS_CH  6
#else
#define NS_CH  5
#endif
//暂时全部喂1
char last_NS = NS_MAN;
static void NS_select(void)
{
    if (Rc_Pwm_In[NS_CH] < 1200)
    {
        Feed_Rc_Dog(NS_MAN);               //3                   //1档自动
        last_NS = NS_MAN;
        fly_state_flag = 0;
    }//NS_MAN
    else if(Rc_Pwm_In[NS_CH] < 1800)
    {
        //        height_ctrl_mode = 0;
        //Feed_Rc_Dog(NS_NOW_TEST);        //1                            //2档
        height_ctrl_mode = 2;               //超声波定高
        Feed_Rc_Dog(NS_NOW_ONE_TEST);               //3                   //1档自动
        last_NS = NS_NOW_ONE_TEST;
        fly_state_flag = 0;
    }
    else                                                //3档
    {
        //Feed_Rc_Dog(NS_FINISH_RASPBERRY);                                 //最底下那档手控pr，yaw自动
        ////Feed_Rc_Dog(NS_MAN_HEIGHT_CONTROL);          //5   人控高度
        height_ctrl_mode = 2;     //超声波控高
        Feed_Rc_Dog(NS_FINISH_AUTO);               //3                   //1档自动
        if((last_NS == NS_NOW_ONE_TEST) && (fly_ready == 1) && (ultra_distance > 500))
        {
            fly_state_flag = 2;    //飞行中手动从二档切换三档
        }
        last_NS = NS_FINISH_AUTO;
    }
}
/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/
