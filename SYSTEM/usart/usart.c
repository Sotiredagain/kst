#include "Usart.h"

unsigned char revbuf[512];
int revint=0;
void usart_gpio()
{
	//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//����gpio�ṹ��
	GPIO_InitTypeDef GPIO_InitStruct;
	//����tx��
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
		GPIO_Init(GPIOA,&GPIO_InitStruct);
	//����rx��
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
		GPIO_Init(GPIOA,&GPIO_InitStruct);
}


void usart1_init(uint32_t Baud)
{
	usart_gpio();
	//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	//����ṹ��
	USART_InitTypeDef USART_InitStruct;
	//���ô��ڲ���������֡��ʽ��
	USART_InitStruct.USART_BaudRate=Baud;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
  //��ʼ������
	USART_Init(USART1,&USART_InitStruct);
	
	//����nvic�ṹ��
	NVIC_InitTypeDef NVIC_InitStruct;
	//����nvic
	NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=15;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	
	NVIC_Init(&NVIC_InitStruct);
	//���ڽ����ж�ʹ��
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//�򿪴���
	USART_Cmd(USART1,ENABLE);
}
void Usart_SentString(const char* String)
{
	int i=0;
	while(String[i]!='\0')
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1,String[i]);
		i++;
	}
}
void usart2_init(uint32_t Baud)
{
		//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//����gpio�ṹ��
	GPIO_InitTypeDef GPIO_InitStruct;
	//����tx��
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
		GPIO_Init(GPIOA,&GPIO_InitStruct);
	//����rx��
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;
		GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
		GPIO_Init(GPIOA,&GPIO_InitStruct);
	//��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//����ṹ��
	USART_InitTypeDef USART_InitStruct;
	//���ô��ڲ���������֡��ʽ��
	USART_InitStruct.USART_BaudRate=Baud;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
  //��ʼ������
	USART_Init(USART2,&USART_InitStruct);
	
	//����nvic�ṹ��
	NVIC_InitTypeDef NVIC_InitStruct;
	//����nvic
	NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=15;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	
	NVIC_Init(&NVIC_InitStruct);
	//���ڽ����ж�ʹ��
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	//�򿪴���
	USART_Cmd(USART2,ENABLE);
}
void Usart1_SentString(const char* String)
{
	int i=0;
	while(String[i]!='\0')
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1,String[i]);
		i++;
	}
}
void Usart2_SentString(const char* String)
{
		while(*String)
	{
		 USART_SendData(USART2,*String++);
		 while( USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET );		
	}
}
void USART1_IRQHandler()
{
	unsigned char data;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)== SET)
	{
		data=USART_ReceiveData(USART1);
		USART_SendData(USART1,data);
	}
}
void USART2_IRQHandler()
{

	if(USART_GetITStatus(USART2,USART_IT_RXNE)== SET)
	{
		revbuf[revint++]=USART_ReceiveData(USART2);
	}
}
