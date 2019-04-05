#include <stm32f10x.h>
#include "delay.h"
#include "lock.h"

volatile int Lock_Time_Delay_N = 11;


/*
********************************************
*@ function						: LockPinInit
*@ Description				: �����صĳ�ʼ������Ϊ��LED��ʹ����ͬ�˿ڣ����Դ˳�������δʹ��					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/
unsigned char LockPinInit(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		return 1;
}
/*
********************************************
*@ function						: unsigned char LockOpen(void)
*@ Description				: ������������ʱʱ�䣬�Լ����Žӷ����Ӿ������					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/
unsigned char LockOpen(void)
{
	int i=0;
	GPIO_SetBits(LOCK_PORT_1,LOCK_PIN_1);
	GPIO_ResetBits(LOCK_PORT_2,LOCK_PIN_2);
	for(i=Lock_Time_Delay_N;i>0;i--)
	{
		delay_ms(100);
	}
	GPIO_SetBits(LOCK_PORT_2,LOCK_PIN_2);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	return 1;
	
}
/*
********************************************
*@ function						: unsigned char LockClose(void)
*@ Description				: ���Ĺرճ��򣬾�����ʱʱ������Žӷ��Ӿ������					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/
unsigned char LockClose(void)
{
	int i=0;
	GPIO_ResetBits(LOCK_PORT_1,LOCK_PIN_1);
	GPIO_SetBits(LOCK_PORT_2,LOCK_PIN_2);
	for(i=Lock_Time_Delay_N;i>0;i--)
	{
		delay_ms(100);
	}
	GPIO_SetBits(LOCK_PORT_1,LOCK_PIN_1);	
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
	return 1;
}

