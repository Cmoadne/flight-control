#include "vl53l0x.h"
#include "string.h"
#include "time.h"


u8 re_Buf_Data[8]={0},Receive_ok=0;
u8 sum = 0;   //校验
u16 vl53l0x_distance = 0;



void vl53l0x_Init(u32 br_num)
{
    USART_InitTypeDef USART_InitStructure;
    //USART_ClockInitTypeDef USART_ClockInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //开启USART3时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	


    //串口中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	


    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);

    //配置Pb10作为UART3　Tx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
    //配置PB11作为UART3　Rx
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

    //配置UART3
    //中断被屏蔽了
    USART_InitStructure.USART_BaudRate = br_num;       //波特率可以通过地面站配置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  //8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   //在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;    //禁用奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //硬件流控制失能
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  //发送、接收使能
    USART_Init(USART3, &USART_InitStructure);


    USART_ClearFlag(USART3, USART_FLAG_TC);
    //使能UART3接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    //使能USART3
    USART_Cmd(USART3, ENABLE); 
    //	//使能发送（进入移位）中断
    //	if(!(USART2->CR1 & USART_CR1_TXEIE))
    //	{
    //		USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 
    //	}


    Delay_ms(500);//等待模块初始化完成
    send_com(0x45);//发送读rgb指令

}

//发送一个字节数据
//input:byte,待发送的数据
void USART3_send_byte(uint8_t byte)
{
    while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);//等待发送完成
    USART3->DR=byte;	
}
//发送多字节数据
void USART_Send_bytes(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i=0;
    while(i<Length)
    {
        USART3_send_byte(Buffer[i++]);
    }
}
//发送多字节数据+校验和
void USART_Send(uint8_t *Buffer, uint8_t Length)
{
    uint8_t i=0;
    while(i<Length)
    {
        if(i<(Length-1))
            Buffer[Length-1]+=Buffer[i];//累加Length-1前的数据
        USART3_send_byte(Buffer[i++]);
    }
}
void send_3out(uint8_t *data,uint8_t length,uint8_t send)
{
    uint8_t TX_DATA[30]={0},i=0,k=0;
    TX_DATA[i++]=0X5A;//帧头
    TX_DATA[i++]=0X5A;//帧头
    TX_DATA[i++]=send;//功能字节
    TX_DATA[i++]=length;//数据个数
    for(k=0;k<length;k++)//存入数据到缓存TX_DATA
    {
        TX_DATA[i++]=(uint8_t)data[k];
    }
    USART_Send(TX_DATA,length+5);	
}
//发送一帧数据
void send_out(int16_t *data,uint8_t length,uint8_t send)
{
    uint8_t TX_DATA[30],i=0,k=0;
    memset(TX_DATA,0,(2*length+5));//清零缓存TX_DATA
    TX_DATA[i++]=0X5A;//帧头
    TX_DATA[i++]=0X5A;//帧头
    TX_DATA[i++]=send;//功能字节
    TX_DATA[i++]=2*length;//数据个数
    for(k=0;k<length;k++)//存入数据到缓存TX_DATA
    {
        TX_DATA[i++]=(uint16_t)data[k]>>8;
        TX_DATA[i++]=(uint16_t)data[k];
    }
    USART_Send(TX_DATA,2*length+5);	
}
void display(int16_t *num,u8 send,u8 count)
{
    u8 i=0;
    USART3_send_byte(0X0d);
    USART3_send_byte(0X0a);
    USART3_send_byte(send);
    USART3_send_byte(':');
    for(i=0;i<count;i++)
    {
        if(num[i]<0)
        {
            num[i]=-num[i];
            USART3_send_byte('-');
        }
        else
            USART3_send_byte('+');

        USART3_send_byte(0x30|(num[i]/10000));	
        USART3_send_byte(0x30|(num[i]%10000/1000));
        USART3_send_byte(0x30|(num[i]%1000/100));
        //USART_send_byte(0x2e);
        USART3_send_byte(0x30|(num[i]%100/10));
        USART3_send_byte(0x30|(num[i]%10));
        USART3_send_byte(',');	
    }
}

void send_com(u8 data)
{
    u8 bytes[3]={0};
    bytes[0]=0xa5;
    bytes[1]=data;//功能字节
    USART_Send(bytes,3);//发送帧头、功能字节、校验和
}
void CHeck(uint8_t *re_data)
{

}


void USART3_IRQHandler(void)
{
    static uint8_t i=0,rebuf[20]={0};
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//判断接收标志
    {
        rebuf[i++]=USART_ReceiveData(USART3);//读取串口数据，同时清接收标志
        if (rebuf[0]!=0x5a)//帧头不对
            i=0;	
        if ((i==2)&&(rebuf[1]!=0x5a))//帧头不对
            i=0;

        if(i>3)//i等于4时，已经接收到数据量字节rebuf[3]
        {
            if(i!=(rebuf[3]+5))//判断是否接收一帧数据完毕
                return;	
            switch(rebuf[2])//接收完毕后处理
            {
            case 0x15:
                if(!Receive_ok)//当数据处理完成后才接收新的数据
                {
                    memcpy(re_Buf_Data,rebuf,8);//拷贝接收到的数据
                    //Receive_ok=1;//接收完成标志

                    for(sum=0,i=0;i<(re_Buf_Data[3]+4);i++)//rgb_data[3]=3
                        sum+=re_Buf_Data[i];
                    if(sum==re_Buf_Data[i])//校验和判断
                    {
                        vl53l0x_distance=re_Buf_Data[4]<<8|re_Buf_Data[5];
                        send_3out(&re_Buf_Data[4],3,0x15);//上传给上位机
                    }
                }
                break;

            }
            i=0;//缓存清0
        }
    }

}

