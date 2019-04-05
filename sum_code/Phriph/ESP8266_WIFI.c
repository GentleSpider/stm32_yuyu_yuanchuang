
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "delay.h"
#include "usart2_v2.h"
#include "ESP8266_WIFI.h"
#include "Float2char.h"

void ESP8266_Rst(void);
void ESP8266_AP_Init(void);
void SendCmd(char* cmd, char* result, int timeOut);
void TCP_SendData_test(void);
int ESP8266_printf(char* cmd);
int ESP8266_sprintf(char *fmt, ...);
int ESP8266_SendLenStr(u8 *cmd,u16 len);

static void ESP8266_write(uint8_t *Data,uint32_t len)
{
	usart2_write(USART2, Data, len);
}

void ESP8266_Rcv_Clr()
{
	memset(ESP8266_rcv_buf,0,sizeof(ESP8266_rcv_buf));
	ESP8266_rcv_len = 0;
}

static void mDelay(uint32_t i)
{
		uint32_t j=0;
		for(;i>0;i--)
		{
				for(j=0;j<12000;j++);
		}
}

/*
**************************************************************
*@ funciton				: void ESP8266_AP_Init(void)
*@ brief					: 设置ESP8266模块为AP模式（默认ip 192.168.4.1 端口8080）
*@ param 					: none
*@ return					: none
***********************************************************************
*/

void ESP8266_AP_Init(void)
{
		ESP8266_Rst();
	
//		SendCmd("AT\r\n","OK",1000); 			
//	
//		SendCmd("AT+RST\r\n","OK",2000);	
//	
//		SendCmd("AT+CWMODE=2\r\n","OK",1000);		

//		SendCmd("AT+CWSAP=\"esp-test\",\"llllllll\",1,3\r\n","OK",1000);	

//		SendCmd("AT+CIPMUX=1\r\n","OK",2200);	

//		SendCmd("AT+CIPSERVER=1,8080\r\n","OK",2200);

//		SendCmd("AT+CIPSTO=2880\r\n","OK",1000);


		SendCmd(AT,"OK",1000); 			

		SendCmd(RST,"OK",2000);	

		SendCmd(CWMODE2,"OK",1000);		

		SendCmd(CWSAP,"OK",1000);
		
//		SendCmd("AT+CIPAP=192.168.5.1\r\n","OK",1000);

		SendCmd(CIPMUX,"OK",2200);	

		SendCmd(CIPSERVER,"OK",2200);

		SendCmd(CIPSTO,"OK",1000);

}

/**
  * @brief  ESP8266硬件复位 PB4
**/
void ESP8266_Rst(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    //Pb5--对应ESP8266的reset引脚;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		GPIO_ResetBits(GPIOB,GPIO_Pin_4);
		mDelay(100);
		GPIO_SetBits(GPIOB,GPIO_Pin_4);
		mDelay(100);
}

/**
  * @brief  发送一条AT指令
**/
void SendCmd(char* cmd, char* result, int timeOut)
{
    while(1)
    {
        memset(ESP8266_rcv_buf,0,sizeof(ESP8266_rcv_buf));
				ESP8266_rcv_len=0;
        ESP8266_write((unsigned char *)cmd, strlen((const char *)cmd));
        mDelay(timeOut);
       
        if((NULL != strstr((const char *)ESP8266_rcv_buf, result)))	//判断是否有预期的结果
        {
            break;
        }
        else
        {
            mDelay(100);
        }
    }
}

/*
**************************************************************
*@ funciton				: int ESP8266_printf(char* cmd)
*@ brief					: 通过ESP8266发送字符串
*@ param 					: cmd 发送的字符串
*@ return					: 1发送成功 0发送失败
***********************************************************************
*/

int ESP8266_printf(char* cmd)
{
	int count,OK_Flag;
	char AT_send[25];
	unsigned char cmd_len_str[5];
	int cmd_len = strlen((const char*)cmd);
	ESP8266_Rcv_Clr();
	memset(AT_send,0,20);
	Int2char(cmd_len,cmd_len_str);
	strcat(AT_send,"AT+CIPSEND=0,");
	strcat(AT_send,(char *)cmd_len_str);
	strcat(AT_send,"\r\n");
	ESP8266_write((unsigned char*)AT_send, strlen((const char*)AT_send));
	for(count=0;count<40;count++)
	{
			mDelay(100);
			if((NULL != strstr((const char *)ESP8266_rcv_buf,">")))
			{
					break;
			}
	}
	
	ESP8266_Rcv_Clr();
	for(count=0;count<10; count++)
	{
		ESP8266_write((unsigned char*)cmd, strlen((const char*)cmd));
		mDelay(500);
		if((NULL != strstr((const char *)ESP8266_rcv_buf, "SEND OK")))	//判断是否有预期的结果
		{
			OK_Flag = 1;
			break;
		}	
	}
	OK_Flag = 0;
	return OK_Flag;
}
/*
**************************************************************
*@ funciton				: int ESP8266_sprintf(char *fmt, ...)
*@ brief					: 打印数据
*@ param 					: none
*@ return					: none
***********************************************************************
*/
int ESP8266_sprintf(char *fmt, ...)
{
	char Array[100];
	int cnt;
	va_list ap; 
	va_start(ap,fmt);
	cnt = vsprintf((char*)Array,fmt,ap);
	va_end(ap);
	ESP8266_printf(Array);
	return cnt;
}
/*
**************************************************************
*@ funciton				: int ESP8266_SendLenStr(u8 *cmd,u16 len)
*@ brief					: 发送指定长度的字符串
*@ param 					: none
*@ return					: none
***********************************************************************
*/
int ESP8266_SendLenStr(u8 *cmd,u16 len)
{
	int count;
	int OK_Flag = 0;
	char AT_send[25];
	unsigned char cmd_len_str[5];
	ESP8266_Rcv_Clr();
	memset(AT_send,0,20);
	Int2char(len,cmd_len_str);
	strcat(AT_send,"AT+CIPSEND=0,");
	strcat(AT_send,(char *)cmd_len_str);
	strcat(AT_send,"\r\n");
	ESP8266_write((unsigned char*)AT_send, strlen((const char*)AT_send));
	for(count=0;count<40;count++)
	{
			mDelay(100);
			if((NULL != strstr((const char *)ESP8266_rcv_buf,">")))
			{
					break;
			}
	}
	
	ESP8266_Rcv_Clr();
	for(count=0;count<10; count++)
	{
		ESP8266_write((unsigned char*)cmd, strlen((const char*)cmd));
		mDelay(500);
		if((NULL != strstr((const char *)ESP8266_rcv_buf, "SEND OK")))	//判断是否有预期的结果
		{
			OK_Flag = 1;
			break;
		}	
	}
	return OK_Flag;
}
/*
**************************************************************
*@ funciton				: void TCP_SendData_test(void)
*@ brief					: 测试ESP8266的发送和接收功能
*@ param 					: none
*@ return					: none
***********************************************************************
*/

void TCP_SendData_test(void)
{
	int count;
		ESP8266_write("AT+CIPSEND=0,5\r\n",strlen("AT+CIPSEND=0,5\r\n"));  //向ESP8266发送数据透传指令
		for(count=0;count<40;count++)
		{
				mDelay(100);
				if((NULL != strstr((const char *)ESP8266_rcv_buf,">")))
				{
						break;
				}
		}	
	ESP8266_write("Hello\r\n", strlen("Hello\r\n"));
	while(1)
	{
		if((NULL != strstr((const char *)ESP8266_rcv_buf, "SEND OK")))	//判断是否有预期的结果
		{
			break;
		}	
		else
			mDelay(100);
	}

}

/////////////////////测试用//////////////////////////////
//	unsigned char Start_Flag;
//int main(void)
//{
//		Start_Flag = 0;
//		LED_Init();    //LED指示灯初始化函数
//		USART2_Init(115200); //USART2串口初始化函数
//				Start_Flag = 1;
//				if(Start_Flag == 1)
//				{
//						led_off();
//						ESP8266_AP_Init();
//						LED_GREEN_ON;
//						while(1)
//						{
//							if(usart2_cmd_clr_flag == 0)
//							{
//								LED_CmdCtl();
//								USART2_Cmd_Clr();
//								
//							}
//							if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==0)
//							{
//								ESP8266_printf("Hello");
//							}
//						}

//				}
//}



