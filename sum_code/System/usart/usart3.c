#include "usart3.h"

volatile unsigned char  usart3_rcv_cmd_start=0;
volatile unsigned char  usart3_rcv_cmd_flag=0;

unsigned char  usart3_rcv_buf[USART3_MAX_RCV_LEN];
volatile unsigned int   usart3_rcv_len=0;

unsigned char  usart3_cmd_buf[USART3_MAX_CMD_LEN];
volatile unsigned int   usart3_cmd_len=0;


void USART3_Init(void);
void usart3_write(uint8_t *Data,uint32_t len);

/**
  * @brief  USART2初始化函数
**/
void USART3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    /* config USART3 clock */   
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 

    /* USART3 GPIO config */
    /* Configure USART3 Tx (PB.10) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    
    /* Configure USART3 Rx (PB.11) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	//检测端口设置
//		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
//		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
//		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
      
    /* USART3 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure); 
    USART_Cmd(USART3, ENABLE);
		
		//Enable usart3 receive interrupt
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 
		
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
}

	
/**
  * @brief  USART2发送一个字符串
**/
void usart3_write(uint8_t *Data,uint32_t len)
{
    uint32_t i;
		USART_ClearFlag(USART3,USART_FLAG_TC); 
    for(i=0; i<len; i++)
    {                                         
        USART_SendData(USART3, *Data++);
        while( USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET );
    }
}