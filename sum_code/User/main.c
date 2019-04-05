/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   用3.5.0版本库建的工程模板
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO-MINI STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include <string.h>
#include "delay.h"
#include "led.h"
#include "usart1_v2.h"
#include "usart2_v2.h"
#include "usart3_v2.h"
#include "lock.h"

#include "stm32_SPI_RC522.h"
#include "ESP8266_WIFI.h"
#include "RFID_IC.h"
#include "Finger.h"
#include "SIM900A.h"
#include "Environment.h"
#include "Cmd_Ctrl.h"
#include "LockApp.h"



/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */

int main(void)
{
		delay_init();
		LED_Init();    //LED指示灯初始化函数 蓝黄指示灯对应锁的两个引脚
		USART2_Init(115200); //USART2串口初始化函数
		//初始化ESP8266
		led_off();
		ESP8266_AP_Init();
		LED_GREEN_ON;
		//等待确认连接成功的指令
		while(1)
		{
			if(NULL != strstr((const char*)ESP8266_rcv_buf,"CS"))
			{
				USART2_Cmd_Clr();
				break;
			}
		}
		ESP8266_printf("你好");
		
		//锁初始化
		LockPinInit();
		ESP8266_printf("Lock_Finish");
		
		//初始化指纹模块
		usart3_init(9600);
		ESP8266_printf("Usart3_Finish");
		FR_Init();
		ESP8266_printf("Finger_Finish");

		
//		//初始化RFID模块
//		InitRc522();
//		LED_BLUE_ON;
//		//初始化环境传感器
//		Evr_Init();
//		LED_RED_ON;
		
		//初始化GSM模块
		USART1_Init(9600);
		ESP8266_printf("Uart1_Finish");
		GSM_Dect();   //模块自检
		RcvMsgInit(); // 接收短信初始化
		ESP8266_printf("GSM_Finish");
		
		
		
		//程序主流程
		while(1)
		{
			Get_FR_Right();

			Lock_Ctl();
			if(usart2_cmd_clr_flag == 0)
			{
				Cmd_Ctrl();
				USART2_Cmd_Clr();
								
			}

		}

	
}

/*********************************************END OF FILE**********************/

