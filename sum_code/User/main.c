/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��3.5.0�汾�⽨�Ĺ���ģ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO-MINI STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
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
  * @brief  ������
  * @param  ��
  * @retval ��
  */

int main(void)
{
		delay_init();
		LED_Init();    //LEDָʾ�Ƴ�ʼ������ ����ָʾ�ƶ�Ӧ������������
		USART2_Init(115200); //USART2���ڳ�ʼ������
		//��ʼ��ESP8266
		led_off();
		ESP8266_AP_Init();
		LED_GREEN_ON;
		//�ȴ�ȷ�����ӳɹ���ָ��
		while(1)
		{
			if(NULL != strstr((const char*)ESP8266_rcv_buf,"CS"))
			{
				USART2_Cmd_Clr();
				break;
			}
		}
		ESP8266_printf("���");
		
		//����ʼ��
		LockPinInit();
		ESP8266_printf("Lock_Finish");
		
		//��ʼ��ָ��ģ��
		usart3_init(9600);
		ESP8266_printf("Usart3_Finish");
		FR_Init();
		ESP8266_printf("Finger_Finish");

		
//		//��ʼ��RFIDģ��
//		InitRc522();
//		LED_BLUE_ON;
//		//��ʼ������������
//		Evr_Init();
//		LED_RED_ON;
		
		//��ʼ��GSMģ��
		USART1_Init(9600);
		ESP8266_printf("Uart1_Finish");
		GSM_Dect();   //ģ���Լ�
		RcvMsgInit(); // ���ն��ų�ʼ��
		ESP8266_printf("GSM_Finish");
		
		
		
		//����������
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

