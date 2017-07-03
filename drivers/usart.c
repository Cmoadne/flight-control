/****************(C) COPYRIGHT 2017 Cmoadne********************
// 文件名 : usart.c
// 路径   : K:\2016_2\EE\Train3_4\my_change\F407_FC_ANO\drivers
// 作者   : Cmoande
// 日期   : 2017/05/11
// 描述   : 串口驱动
// 备注   :
// 版本   : V0.0 2017.5.11  初始版本  适用于我的
增加了串口1接收代码
******************************************************************/ 
#include "usart.h"
#include "data_transfer.h"
#include "ultrasonic.h"

#include "include.h"

//我的新增代码开始
//我的include
#include "string.h"
#include "stdlib.h"
#include "imagepid.h"

//我的串口1初始化代码  用于自动控制
u16 RX_auto[CH_NUM] = {1500,1500,1000,1500,1000,1000,1000,1000}; 
char use_i_flag;  //是否定点使用I
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
char USART_RX_BUF_YAW[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
char USART_RX_BUF_PITCH[USART_REC_LEN];
char USART_RX_BUF_ROLL[USART_REC_LEN];

char USART_RX_BUF_GAS[USART_REC_LEN];       //油门
char USART_RX_BUF_ST[USART_REC_LEN];            //开始结束
char USART_RX_BUF_HEIGHT[USART_REC_LEN];        //高度
char USART_RX_BUF_KEY[USART_REC_LEN];       //按键

int key_value = 0;
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	
//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟

    //串口1对应引脚复用映射
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1

    //USART1端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

    //USART1 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART1, &USART_InitStructure); //初始化串口1

    USART_Cmd(USART1, ENABLE);  //使能串口1 

    USART_ClearFlag(USART1, USART_FLAG_TC);

#if EN_USART1_RX	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif

}

//  全局输出，CH_filter[],0横滚，1俯仰，2油门，3航向 范围：+-500	
#define UART_GET_YAW   3
#define UART_GET_PITCH 1
#define UART_GET_ROLL  0
#define UART_GET_THR   2 
#define UART_GET_HEIGHT  4   //高度控制模式

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
    u8 Res;
    static char uart1_selsect = 0;              //收到的是什么数据 

    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
        if(USART_RX_STA&0x4000)//接收到了0x0d
        {
            if(Res!=0x0a)
                USART_RX_STA=0;//接收错误,重新开始
            else 
            {

                switch(uart1_selsect)
                {
                case 'p':                   //PITCH
                    get_command = 1;
                    RX_auto[UART_GET_PITCH] = atoi(USART_RX_BUF_PITCH);
                    memset(USART_RX_BUF_PITCH, '\0', sizeof(USART_RX_BUF_PITCH));         //情况数组
                    break;
                case 'r':                   //ROLL
                    get_command = 1;
                    RX_auto[UART_GET_ROLL] = atoi(USART_RX_BUF_ROLL);
                    memset(USART_RX_BUF_ROLL, '\0', sizeof(USART_RX_BUF_ROLL));         //情况数组
                    break;
                case 'g':
                    RX_auto[UART_GET_THR] = atoi(USART_RX_BUF_GAS);
                    memset(USART_RX_BUF_GAS, '\0', sizeof(USART_RX_BUF_GAS)); 
                    break;
                case 'h':
                    RX_auto[UART_GET_HEIGHT] = atoi(USART_RX_BUF_HEIGHT);
                    memset(USART_RX_BUF_HEIGHT, '\0', sizeof(USART_RX_BUF_HEIGHT)); 
                    break;
                case 'k':
                    key_value = atoi(USART_RX_BUF_KEY);
                    //                    if(!fly_ready)
                    //                    {
                    switch(key_value)
                    {
                    case 4://server_duty_flag = 1;        //打舵机
                        break;
                    case 5://use_tags_height_flag = 1;
                        //use_i_flag = 1;
                        break;
                    case 6://use_tags_height_flag = 0;
                        // use_i_flag = 0;
                        break;
                        //校准
                    case 1://mpu6050.Acc_CALIBRATE = 1;
                        break;
                    case 2://mpu6050.Gyro_CALIBRATE = 1;
                        break;
                    case 3://Mag_CALIBRATED = 1;
                        break;
                    default:  
                        break;
                    }
                    //  }
                    key_value = 0;
                    memset(USART_RX_BUF_KEY, '\0', sizeof(USART_RX_BUF_KEY));
                case 'y':                   //YAW
                    RX_auto[UART_GET_YAW] = atoi(USART_RX_BUF_YAW);
                    memset(USART_RX_BUF_YAW, '\0', sizeof(USART_RX_BUF_YAW));         //情况数组
                    break;
                case 's':
                    //start_information = atoi(USART_RX_BUF_ST);
                    memset(USART_RX_BUF_ST, '\0', sizeof(USART_RX_BUF_ST));         //情况数组      
                    break;
                default: break;
                }
                uart1_selsect = 0;
                USART_RX_STA = 0;	//接收完成了 
            }
        }
        else //还没收到0X0D
        {	
            if(Res==0x0d)               //接收到0x0d
                USART_RX_STA|=0x4000;
            else
            {
                if (uart1_selsect == 0)  //第一个参数
                    uart1_selsect = Res;
                else
                {
                    switch(uart1_selsect)
                    {
                    case 'p':
                        USART_RX_BUF_PITCH[USART_RX_STA&0X3FFF]=Res ;
                        USART_RX_STA++; 
                        break;
                    case 'r':
                        USART_RX_BUF_ROLL[USART_RX_STA&0X3FFF]=Res ;
                        USART_RX_STA++; 
                        break;
                    case 'g':
                        USART_RX_BUF_GAS[USART_RX_STA&0X3FFF] = Res;
                        USART_RX_STA++; 
                        break;
                    case 'h':
                        USART_RX_BUF_HEIGHT[USART_RX_STA&0X3FFF]=Res ;
                        USART_RX_STA++;
                        break;
                    case 'k':
                        USART_RX_BUF_KEY[USART_RX_STA&0X3FFF]=Res ;
                        USART_RX_STA++;
                        break;
                    case 'y':
                        USART_RX_BUF_YAW[USART_RX_STA&0X3FFF]=Res ;
                        USART_RX_STA++; 
                        break;
                    case 's':
                        USART_RX_BUF_ST[USART_RX_STA&0X3FFF]=Res ;
                        USART_RX_STA++;
                        break;

                    default: uart1_selsect = 0;break;             
                    } 

                    if(USART_RX_STA>(USART_REC_LEN-1))
                        USART_RX_STA=0;//接收数据错误,重新开始接收	
                }

            }		 
        }

    } 
} 
#endif	

//我的新增代码结束

void Usart2_Init(u32 br_num)
{
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); //开启USART2时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);	

    //串口中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	


    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

    //配置PD5作为USART2　Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
    //配置PD6作为USART2　Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOD, &GPIO_InitStructure); 

    //配置USART2
    //中断被屏蔽了
    USART_InitStructure.USART_BaudRate = br_num;       //波特率可以通过地面站配置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
    //配置USART2时钟
    USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;  //时钟低电平活动
    USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;  //SLCK引脚上时钟输出的极性->低电平
    USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;  //时钟第二个边沿进行数据捕获
    USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable; //最后一位数据的时钟脉冲不从SCLK输出

    USART_Init(USART2, &USART_InitStructure);
    USART_ClockInit(USART2, &USART_ClockInitStruct);

    //使能USART2接收中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    //使能USART2
    USART_Cmd(USART2, ENABLE); 
    //	//使能发送（进入移位）中断
    //	if(!(USART2->CR1 & USART_CR1_TXEIE))
    //	{
    //		USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 
    //	}


}

u8 TxBuffer[256];
u8 TxCounter=0;
u8 count=0; 

u8 Rx_Buf[256];	//串口接收缓存

void Usart2_IRQ(void)
{
    u8 com_data;

    if(USART2->SR & USART_SR_ORE)//ORE中断
    {
        com_data = USART2->DR;
    }

    //接收中断
    if( USART_GetITStatus(USART2,USART_IT_RXNE) )
    {
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);//清除中断标志

        com_data = USART2->DR;
        ANO_DT_Data_Receive_Prepare(com_data);
    }
    //发送（进入移位）中断
    if( USART_GetITStatus(USART2,USART_IT_TXE ) )
    {

        USART2->DR = TxBuffer[TxCounter++]; //写DR清除中断标志          
        if(TxCounter == count)
        {
            USART2->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }


        //USART_ClearITPendingBit(USART2,USART_IT_TXE);
    }



}

void Usart2_Send(unsigned char *DataToSend ,u8 data_num)
{
    u8 i;
    for(i=0;i<data_num;i++)
    {
        TxBuffer[count++] = *(DataToSend+i);
    }

    if(!(USART2->CR1 & USART_CR1_TXEIE))
    {
        USART_ITConfig(USART2, USART_IT_TXE, ENABLE); //打开发送中断
    }

}



void Uart5_Init(u32 br_num)
{
    USART_InitTypeDef USART_InitStructure;
    //USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); //开启USART2时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);

    //串口中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	


    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

    //配置PC12作为UART5　Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
    //配置PD2作为UART5　Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOD, &GPIO_InitStructure); 

    //配置UART5
    //中断被屏蔽了
    USART_InitStructure.USART_BaudRate = br_num;       //波特率可以通过地面站配置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
    USART_Init(UART5, &USART_InitStructure);



    //使能UART5接收中断
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
    //使能USART5
    USART_Cmd(UART5, ENABLE); 
    //	//使能发送（进入移位）中断
    //	if(!(USART2->CR1 & USART_CR1_TXEIE))
    //	{
    //		USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 
    //	}

}
u8 Tx5Buffer[256];
u8 Tx5Counter=0;
u8 count5=0; 

void Uart5_IRQ(void)
{
    u8 com_data;

    //接收中断
    if( USART_GetITStatus(UART5,USART_IT_RXNE) )
    {
        USART_ClearITPendingBit(UART5,USART_IT_RXNE);//清除中断标志

        com_data = UART5->DR;

        Ultra_Get(com_data);
    }

    //发送（进入移位）中断
    if( USART_GetITStatus(UART5,USART_IT_TXE ) )
    {

        UART5->DR = Tx5Buffer[Tx5Counter++]; //写DR清除中断标志

        if(Tx5Counter == count5)
        {
            UART5->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }


        //USART_ClearITPendingBit(USART2,USART_IT_TXE);
    }

}

void Uart5_Send(unsigned char *DataToSend ,u8 data_num)
{
    u8 i;
    for(i=0;i<data_num;i++)
    {
        Tx5Buffer[count5++] = *(DataToSend+i);
    }

    if(!(UART5->CR1 & USART_CR1_TXEIE))
    {
        USART_ITConfig(UART5, USART_IT_TXE, ENABLE); //打开发送中断
    }

}
/******************* (C) COPYRIGHT 2017 Cmoadne *****END OF FILE************/

