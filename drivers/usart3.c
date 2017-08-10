#include "include.h"
#include "usart3.h"	
#include "stdlib.h"
#include "string.h"
#include "usart.h"
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
char USART3_RX_BUF[USART3_REC_LEN] = {'\0','\0','\0','\0','\0','\0'};     //接收缓冲,最大USART3_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART3_RX_STA=0;       //接收状态标记	

//初始化IO 串口1 
//bound:波特率
void uart3_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟

    //串口3对应引脚复用映射
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3

    //USART3端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10与GPIOB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化PB10，PB11

    //USART3 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART3, &USART_InitStructure); //初始化串口3

    USART_Cmd(USART3, ENABLE);  //使能串口3

    //USART_ClearFlag(USART1, USART_FLAG_TC);


    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断

    //Usart3 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、


}

u8 Tx3Buffer[256];
u8 Tx3Counter=0;
u8 count3=0; 

u8 stop_flag = 0;


void USART3_IRQHandler(void)                	//串口3中断服务程序
{
    u8 Res;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);//清除中断标志
        Res =USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据

        if((USART3_RX_STA&0x8000)==0)//接收未完成
        {
            if(USART3_RX_STA&0x4000)//接收到了0x0d
            {
                if(Res!=0x0a)USART3_RX_STA=0;//接收错误,重新开始
                else 
                {
                    USART3_RX_STA|=0x8000;	//接收完成了 

                    if(USART3_RX_BUF[0]==0x00)  //检查帧头
                    {
                        //程序语句 
                        if(USART3_RX_BUF[1] == 0x10)  //停车标志
                        {
                            //beep_alarm_flag = 1;
                            Usart1SendSelect('7');  //发送已经可以停机指令
                            
                        }
                        //if(USART3_RX_BUF[1] == 0x20)  //关闭报警
                        //    beep_alarm_flag = 0;
                        memset(USART3_RX_BUF, '\0', sizeof(USART3_RX_BUF));         //情况数组

                    }
                    USART3_RX_STA = 0;

                }
            }
            else //还没收到0X0D
            {	
                if(Res==0x0d)USART3_RX_STA|=0x4000;
                else
                {
                    USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
                    USART3_RX_STA++;
                    if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
                }		 
            }
        }   		 
    } 


    //发送（进入移位）中断
    if( USART_GetITStatus(USART3,USART_IT_TXE ) )
    {

        USART3->DR = Tx3Buffer[Tx3Counter++]; //写DR清除中断标志

        if(Tx3Counter == count3)
        {
            USART3->CR1 &= ~USART_CR1_TXEIE;		//关闭TXE（发送中断）中断
        }


        //USART_ClearITPendingBit(USART2,USART_IT_TXE);
    }
} 

void Uart3_Send(unsigned char *DataToSend ,u8 data_num)
{
    u8 i;
    for(i=0;i<data_num;i++)
    {
        Tx3Buffer[count3++] = *(DataToSend+i);
    }

    if(!(USART3->CR1 & USART_CR1_TXEIE))
    {
        USART_ITConfig(USART3, USART_IT_TXE, ENABLE); //打开发送中断
    }

}

/**
* @brief  串口三发送小车停止信息
* @param  NONE
* @retval NONE
* @brief  发送帧为[0x00,0x17,0x0d,0x0a]
*/
void Usart3SendResult(char ch)
{
    unsigned char result[4] = {0x00, ch, 0x0d, 0x0a};
    Uart3_Send(result, 4);
    //    while ((USART3->SR & 0X40) == 0); //循环发送,直到发送完毕
    //    USART3->DR = 0x00;
    //    while ((USART3->SR & 0X40) == 0); //循环发送,直到发送完毕
    //    USART3->DR = 0x17;
    //    while ((USART3->SR & 0X40) == 0); //循环发送,直到发送完毕
    //    USART3->DR = 0x0d;
    //    while ((USART3->SR & 0X40) == 0); //循环发送,直到发送完毕
    //    USART3->DR = 0x0a;
}





