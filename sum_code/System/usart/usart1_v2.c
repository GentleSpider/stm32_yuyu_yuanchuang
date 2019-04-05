
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "usart1_v2.h"
#include "led.h"

volatile unsigned char  usart1_rcv_cmd_start=0;
volatile unsigned char  usart1_rcv_cmd_flag=0;
volatile u8 usart1_irpt_flag = 0;


unsigned char  usart1_rcv_buf[USART1_MAX_RCV_LEN];
unsigned char usart1_send_buf[256];
volatile unsigned int   usart1_rcv_len=0;

unsigned char  usart1_cmd_buf[USART1_MAX_CMD_LEN];
volatile unsigned int   usart1_cmd_len=0;

//volatile unsigned char fun_flag = 0;

/**
  * @brief  USART2初始化函数
**/
void USART1_Init(u32 baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
//    /* config USART1 clock */   
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
//	

//    /* USART1 GPIO config */
//    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);    
//    /* Configure USART1 Rx (PA.10) as input floating */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);

		
		//映射到B6 B7
		    /* config USART1 clock */   
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
	

    /* USART1 GPIO config */
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//B6
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
    /* Configure USART1 Rx (PA.10) as input floating */
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //B7
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
		
      
    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure); 
    USART_Cmd(USART1, ENABLE);
		
		//Enable usart1 receive interrupt
		
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
		
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
//		USART_ITConfig(USART1, USART_IT_ORE, ENABLE);
}

	
/**
  * @brief  USART1发送一个字符串
**/
void usart1_write(USART_TypeDef* USARTx, uint8_t *Data,uint32_t len)
{
    uint32_t i;
		USART_ClearFlag(USART1,USART_FLAG_TC); 
    for(i=0; i<len; i++)
    {                                         
        USART_SendData(USARTx, *Data++);
        while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
    }
}

//串口1,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
//void u1_printf(char* fmt,...)  
//{  
//	u16 i,j; 
//	va_list ap; 
//	va_start(ap,fmt);
//	vsprintf((char*)usart1_send_buf,fmt,ap);
//	va_end(ap);
//	i=strlen((const char*)usart1_send_buf);		//此次发送数据的长度
//	for(j=0;j<i;j++)							//循环发送数据
//	{
//	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
//		USART_SendData(USART1,usart1_send_buf[j]); 
//	} 
//}

void u1_printf(char* fmt,...)  
{  
	u16 i; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)usart1_send_buf,fmt,ap);
	va_end(ap);
	i=strlen((const char*)usart1_send_buf);		//此次发送数据的长度
	usart1_write(USART1,usart1_send_buf,i); 
}
/*
*******************************************************
* @:function			: void USART1_IRQHandler(void)
* @:description		: 串口1中断函数
* @:param					:	none
* @:return				: 返回需要输入的ID
********************************************************
*/
//void USART1_IRQHandler(void)
//{
//		unsigned int data;

//    if(USART1->SR & 0x0F)
//    {
//        // See if we have some kind of error
//        // Clear interrupt (do nothing about it!)
//        data = USART1->DR;
//    }
//		else if(USART1->SR & USART_FLAG_RXNE)   //Receive Data Reg Full Flag
//    {		
//        data = USART1->DR;
//				usart1_rcv_buf[usart1_rcv_len++]=data;
//				 
//    }
//		else
//		{
//				;
//		}
//}
void USART1_IRQHandler(void)
{
	u8 data;
//	usart1_irpt_flag = 1;
//	if (USART_GetITStatus(USART1, USART_IT_ORE) == SET) 
//	{
//		USART_ClearITPendingBit(USART1,USART_IT_ORE);
//		USART_ReceiveData( USART1 );
//		
//	}
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		data = USART_ReceiveData(USART1);
		usart1_rcv_buf[usart1_rcv_len++] = data;
	}
	
}

