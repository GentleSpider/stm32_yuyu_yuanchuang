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
  * @brief  LED指示灯初始化函数
**/
void LED_Init(void)
{
	//麒麟座二版本
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

	// 麒麟座迷你1.1

		GPIO_InitTypeDef GPIO_InitStructure;
   
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		

		
}

/**
  * @brief  分析平台下发的LED控制命令
**/
void LED_CmdCtl(void)
{
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED11")))  //约定平台控制命令"LED11"为打开红色LED灯
		{
				LED_RED_ON;	
				red_value=1;
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED21")))  //约定平台控制命令"LED21"为打开绿色LED灯
		{
				LED_GREEN_ON;	
				green_value=1;
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED31")))  //约定平台控制命令"LED31"为打开黄色LED灯
		{
				LED_YELLOW_ON;
				yellow_value=1;			
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED41")))  //约定平台控制命令"LED41"为打开蓝色LED灯
		{
				LED_BLUE_ON;	
				blue_value=1;
		}
		
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED10")))  //约定平台控制命令"LED10"为关闭红色LED灯
		{
				LED_RED_OFF;	
				red_value=0;
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED20")))  //约定平台控制命令"LED20"为关闭绿色LED灯
		{
				LED_GREEN_OFF;
				green_value=0;	
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED30")))  //约定平台控制命令"LED30"为关闭黄色LED灯
		{
				LED_YELLOW_OFF;	
				yellow_value=0;
		}
		if((NULL != strstr((const char *)usart2_cmd_buf, "LED40")))  //约定平台控制命令"LED40"为关闭蓝色LED灯
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

