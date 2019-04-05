
#include <stdlib.h>
#include <string.h>

#include "usart2_v2.h"
#include "led.h"

volatile unsigned char  usart2_rcv_cmd_start=0;
volatile unsigned char  usart2_rcv_cmd_flag=0;
volatile unsigned char 	usart2_cmd_clr_flag = 1;

unsigned char  usart2_rcv_buf[USART2_MAX_RCV_LEN];
volatile unsigned int   usart2_rcv_len=0;

unsigned char  usart2_cmd_buf[USART2_MAX_CMD_LEN];
volatile unsigned int   usart2_cmd_len=0;

//volatile unsigned char fun_flag = 0;

void USART2_Cmd_Clr(void);

/**
  * @brief  USART2初始化函数
**/
void USART2_Init(u32 baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    /* config USART2 clock */   
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 

    /* USART2 GPIO config */
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	//检测端口设置
//		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
//		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
//		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
      
    /* USART2 mode config */
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure); 
    USART_Cmd(USART2, ENABLE);
		
		//Enable usart2 receive interrupt
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 
		
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
}

	
/**
  * @brief  USART2发送一个字符串
**/
void usart2_write(USART_TypeDef* USARTx, uint8_t *Data,uint32_t len)
{
    uint32_t i;
		USART_ClearFlag(USART2,USART_FLAG_TC); 
    for(i=0; i<len; i++)
    {                                         
        USART_SendData(USARTx, *Data++);
        while( USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET );
    }
}

/*
*******************************************************
* @:function			: void USART2_Cmd_Clr(void)
* @:description		: 清除串口2指令缓存区
* @:param					:	void
* @:return				: none
********************************************************
*/
void USART2_Cmd_Clr(void)
{
	memset(usart2_cmd_buf,0,usart2_cmd_len);
  usart2_cmd_len=0;
	usart2_cmd_clr_flag = 1;
}
/*
*******************************************************
* @:function			: void USART2_IRQHandler(void)
* @:description		: 串口2中断函数
* @:param					:	none
* @:return				: 返回需要输入的ID
********************************************************
*/
void USART2_IRQHandler(void)
{
		unsigned int data;

    if(USART2->SR & 0x0F)
    {
        // See if we have some kind of error
        // Clear interrupt (do nothing about it!)
        data = USART2->DR;
    }
		else if(USART2->SR & USART_FLAG_RXNE)   //Receive Data Reg Full Flag
    {		
        data = USART2->DR;
				usart2_rcv_buf[usart2_rcv_len++]=data;
				
				if(data=='{') //约定平台下发的控制命令以'{'为开始符，‘}’为控制命令结束符，读者可以自定义自己的开始符合结束符
				{
						usart2_rcv_cmd_start=1;
				}
				if(usart2_rcv_cmd_start==1 && usart2_cmd_clr_flag == 1)
				{
						usart2_cmd_buf[usart2_cmd_len++]=data;
						if((data=='}')||(usart2_cmd_len>=USART2_MAX_CMD_LEN-1))
						{
								usart2_rcv_cmd_start=0;
								usart2_cmd_clr_flag = 0;
						}
				}	  
    }
		else
		{
				;
		}
}





