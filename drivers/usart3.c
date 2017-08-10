#include "include.h"
#include "usart3.h"	
#include "stdlib.h"
#include "string.h"
#include "usart.h"
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
char USART3_RX_BUF[USART3_REC_LEN] = {'\0','\0','\0','\0','\0','\0'};     //���ջ���,���USART3_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART3_RX_STA=0;       //����״̬���	

//��ʼ��IO ����1 
//bound:������
void uart3_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��

    //����3��Ӧ���Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10����ΪUSART3
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11����ΪUSART3

    //USART3�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //GPIOB10��GPIOB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
    GPIO_Init(GPIOB,&GPIO_InitStructure); //��ʼ��PB10��PB11

    //USART3 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART3, &USART_InitStructure); //��ʼ������3

    USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3

    //USART_ClearFlag(USART1, USART_FLAG_TC);


    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�

    //Usart3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����1�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����


}

u8 Tx3Buffer[256];
u8 Tx3Counter=0;
u8 count3=0; 

u8 stop_flag = 0;


void USART3_IRQHandler(void)                	//����3�жϷ������
{
    u8 Res;

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        USART_ClearITPendingBit(USART3,USART_IT_RXNE);//����жϱ�־
        Res =USART_ReceiveData(USART3);//(USART3->DR);	//��ȡ���յ�������

        if((USART3_RX_STA&0x8000)==0)//����δ���
        {
            if(USART3_RX_STA&0x4000)//���յ���0x0d
            {
                if(Res!=0x0a)USART3_RX_STA=0;//���մ���,���¿�ʼ
                else 
                {
                    USART3_RX_STA|=0x8000;	//��������� 

                    if(USART3_RX_BUF[0]==0x00)  //���֡ͷ
                    {
                        //������� 
                        if(USART3_RX_BUF[1] == 0x10)  //ͣ����־
                        {
                            //beep_alarm_flag = 1;
                            Usart1SendSelect('7');  //�����Ѿ�����ͣ��ָ��
                            
                        }
                        //if(USART3_RX_BUF[1] == 0x20)  //�رձ���
                        //    beep_alarm_flag = 0;
                        memset(USART3_RX_BUF, '\0', sizeof(USART3_RX_BUF));         //�������

                    }
                    USART3_RX_STA = 0;

                }
            }
            else //��û�յ�0X0D
            {	
                if(Res==0x0d)USART3_RX_STA|=0x4000;
                else
                {
                    USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
                    USART3_RX_STA++;
                    if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
                }		 
            }
        }   		 
    } 


    //���ͣ�������λ���ж�
    if( USART_GetITStatus(USART3,USART_IT_TXE ) )
    {

        USART3->DR = Tx3Buffer[Tx3Counter++]; //дDR����жϱ�־

        if(Tx3Counter == count3)
        {
            USART3->CR1 &= ~USART_CR1_TXEIE;		//�ر�TXE�������жϣ��ж�
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
        USART_ITConfig(USART3, USART_IT_TXE, ENABLE); //�򿪷����ж�
    }

}

/**
* @brief  ����������С��ֹͣ��Ϣ
* @param  NONE
* @retval NONE
* @brief  ����֡Ϊ[0x00,0x17,0x0d,0x0a]
*/
void Usart3SendResult(char ch)
{
    unsigned char result[4] = {0x00, ch, 0x0d, 0x0a};
    Uart3_Send(result, 4);
    //    while ((USART3->SR & 0X40) == 0); //ѭ������,ֱ���������
    //    USART3->DR = 0x00;
    //    while ((USART3->SR & 0X40) == 0); //ѭ������,ֱ���������
    //    USART3->DR = 0x17;
    //    while ((USART3->SR & 0X40) == 0); //ѭ������,ֱ���������
    //    USART3->DR = 0x0d;
    //    while ((USART3->SR & 0X40) == 0); //ѭ������,ֱ���������
    //    USART3->DR = 0x0a;
}





