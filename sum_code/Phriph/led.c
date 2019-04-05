#include <stm32f10x.h>
#include <stddef.h>
#include <string.h>
#include "led.h"
#include "usart2_v2.h"


volatile unsigned char  red_value=0;
volatile unsigned char  yellow_value=0;
volatile unsigned char  blue_value=0;
volatile unsigned char  green_value=0;
unsigned int j;


void LED_Init(void);
void LED_CmdCtl(void);
void led_off(void);

/**
  * @brief  LEDָʾ�Ƴ�ʼ������
**/
void LED_Init(void)
{
	//���������汾
//    GPIO_InitTypeDef GPIO_InitStructure;
//   
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);
//		
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//		GPIO_Init(GPIOA, &GPIO_InitStructure);

	// ����������1.1

		GPIO_InitTypeDef GPIO_InitStructure;
   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		

		
}

/**
  * @brief  ����ƽ̨�·���LED��������
**/
void LED_CmdCtl(void)
{
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED11")))  //Լ��ƽ̨��������"LED11"Ϊ�򿪺�ɫLED��
		{
				LED_RED_ON;	
				red_value=1;
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED21")))  //Լ��ƽ̨��������"LED21"Ϊ����ɫLED��
		{
				LED_GREEN_ON;	
				green_value=1;
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED31")))  //Լ��ƽ̨��������"LED31"Ϊ�򿪻�ɫLED��
		{
				LED_YELLOW_ON;
				yellow_value=1;			
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED41")))  //Լ��ƽ̨��������"LED41"Ϊ����ɫLED��
		{
				LED_BLUE_ON;	
				blue_value=1;
		}
		
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED10")))  //Լ��ƽ̨��������"LED10"Ϊ�رպ�ɫLED��
		{
				LED_RED_OFF;	
				red_value=0;
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED20")))  //Լ��ƽ̨��������"LED20"Ϊ�ر���ɫLED��
		{
				LED_GREEN_OFF;
				green_value=0;	
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED30")))  //Լ��ƽ̨��������"LED30"Ϊ�رջ�ɫLED��
		{
				LED_YELLOW_OFF;	
				yellow_value=0;
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED40")))  //Լ��ƽ̨��������"LED40"Ϊ�ر���ɫLED��
		{
				LED_BLUE_OFF;	
				blue_value=0;
		}
		if(NULL != strstr((const char*)usart2_cmd_buf, "LED_OFF"))
		{
			led_off();
		}
		
		if((NULL != strstr((const char *)usart2_cmd_buf, "123456")))  //????????"KEY1"???
		{
				j=2500000;
				LED_GREEN_ON;
				LED_YELLOW_OFF;
				while(j--);
				LED_GREEN_OFF;
				//usart2_rcv_buf[usart2_rcv_len]=0;
		}
		
		if((NULL != strstr((const char *)usart2_cmd_buf, "KEY0")))  //????????"KEY0"???
		{
				j=2500000;
				LED_YELLOW_ON;
				LED_GREEN_OFF;
				while(j--);
				LED_YELLOW_OFF;
				//usart2_rcv_buf[usart2_rcv_len]=0;
		}
}

void led_off(void)
{
	LED_RED_OFF;
	LED_GREEN_OFF;
	LED_YELLOW_OFF;
	LED_BLUE_OFF;
}

