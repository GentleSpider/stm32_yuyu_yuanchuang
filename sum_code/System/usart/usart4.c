
#include <stdlib.h>
#include <string.h>

#include "usart4.h"
#include "led.h"

volatile unsigned char  usart4_rcv_cmd_start=0;
volatile unsigned char  usart4_rcv_cmd_flag=0;
volatile unsigned char 	usart4_cmd_clr_flag = 1;

unsigned char  usart4_rcv_buf[USART4_MAX_RCV_LEN];
volatile unsigned int   usart4_rcv_len=0;

unsigned char  usart4_cmd_buf[USART4_MAX_CMD_LEN];
volatile unsigned int   usart4_cmd_len=0;

//volatile unsigned char fun_flag = 0;

void USART4_Cmd_Clr(void);

/**
  * @brief  USART4初始化函数
**/
void USART4_Init(u32 baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    /* config USART4 clock */   
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE); 

    /* USART4 GPIO config */
    /* Configure USART4 Tx (Pc.10) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);    
    /* Configure USART4 Rx (PC.11) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	//检测端口设置
//		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
//		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
//		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
      
    /* USART4 mode config */
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStructure); 
    USART_Cmd(UART4, ENABLE);
		
		//Enable USART4 receive interrupt
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); 
		
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
}

	
/**
  * @brief  USART4发送一个字符串
**/
void usart4_write(USART_TypeDef* USARTx, uint8_t *Data,uint32_t len)
{
    uint32_t i;
		USART_ClearFlag(UART4,USART_FLAG_TC); 
    for(i=0; i<len; i++)
    {                                         
        USART_SendData(USARTx, *Data++);
        while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
    }
}

/*
*******************************************************
* @:function			: void USART4_Cmd_Clr(void)
* @:description		: 清除串口4指令缓存区
* @:param					:	void
* @:return				: none
********************************************************
*/
void USART4_Cmd_Clr(void)
{
	memset(usart4_cmd_buf,0,usart4_cmd_len);
  usart4_cmd_len=0;
	usart4_cmd_clr_flag = 1;
}
/*
*******************************************************
* @:function			: void USART4_IRQHandler(void)
* @:description		: 串口4中断函数
* @:param					:	none
* @:return				: 返回需要输入的ID
********************************************************
*/
void USART4_IRQHandler(void)
{
		unsigned int data;

    if(UART4->SR & 0x0F)
    {
        // See if we have some kind of error
        // Clear interrupt (do nothing about it!)
        data = UART4->DR;
    }
		else if(UART4->SR & USART_FLAG_RXNE)   //Receive Data Reg Full Flag
    {		
        data = UART4->DR;
				usart4_rcv_buf[usart4_rcv_len++]=data;
					  
    }
		else
		{
				;
		}
}





