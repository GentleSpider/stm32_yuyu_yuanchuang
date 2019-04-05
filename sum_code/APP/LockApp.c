#include <string.h>
#include <stdlib.h>
#include "LockApp.h"
#include "lock.h"
#include "SIM900A.h"
#include "Finger.h"
#include "ESP8266_WIFI.h"


volatile unsigned char Lock_Cmd[20] = "zimakaimen";

void Lock_Ctl(void);
void Lock_Cmd_Ctrl(void);

unsigned char Lock_Cmd_Set(void);
unsigned char Lock_Time_Set(void);

/*
********************************************
*@ function						: void Lock_Ctl(void)
*@ Description				: 短信开锁和指纹开锁的功能实现					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/
void Lock_Ctl(void)
{
		if(RcvMsg_Ctrl((char *)Lock_Cmd))
		{
			LockOpen();
		}
		if(FR_Right == 1)
		{
			LockOpen();
			FR_Right = 0;
		}
}
/*
********************************************
*@ function						: void Lock_Ctl(void)
*@ Description				: 短信开锁和指纹开锁的功能实现					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/
void Lock_Cmd_Ctrl(void)
{
	if(NULL != strstr((const char*)Lock_Cmd_Buf,(const char*)"_Open"))
	{
		LockOpen();
	}
	if(NULL != strstr((const char*)Lock_Cmd_Buf,(const char*)"_Close"))
	{
		LockClose();
	}
	if(NULL != strstr((const char*)Lock_Cmd_Buf,(const char*)"_Cmd_"))
	{
		if(NULL != strstr((const char*)Lock_Cmd_Buf,(const char*)"_Cmd_Set:"))
		{
			Lock_Cmd_Set();
		}
		if(NULL != strstr((const char*)Lock_Cmd_Buf,(const char*)"_Cmd_View"))
		{
			ESP8266_sprintf("Lock_Cmd:%s",(char*)Lock_Cmd);
		}
		
	}
	if(NULL != strstr((const char*)Lock_Cmd_Buf,(const char*)"_Time_"))
	{
		if(NULL != strstr((const char*)Lock_Cmd_Buf,(const char*)"_Time_Set:"))
		{
			Lock_Time_Set();
		}
		if(NULL != strstr((const char*)Lock_Cmd_Buf,(const char*)"_Time_View"))
		{
			ESP8266_sprintf("Lock_Delay_Time:%d",Lock_Time_Delay_N);
		}
	}
}
/*
********************************************
*@ function						: unsigned char Lock_Cmd_Set(void)
*@ Description				: 改变开锁的指令					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/
unsigned char Lock_Cmd_Set(void)
{
	char *p = NULL;
	unsigned char *p_cmd = NULL;
	memset((void *)Lock_Cmd,0,sizeof(Lock_Cmd));
	p_cmd = (unsigned char*)Lock_Cmd;
	p = strchr((const char*)Lock_Cmd_Buf,':');
	p = p+1;
	while(*p != '}')
	{
		*p_cmd = *p;
		p_cmd++;
		p++;
	}
	*p_cmd = '\0';
	ESP8266_sprintf("Lock_Cmd:%s",(char*)Lock_Cmd);
	return 1;
}

/*
********************************************
*@ function						: unsigned char Lock_Time_Set(void)
*@ Description				: 改变开锁的时间					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/
unsigned char Lock_Time_Set(void)
{
	char *p = NULL;
	p = strchr((const char*)Lock_Cmd_Buf,':');
	p = p+1;
	Lock_Time_Delay_N = atoi(p);
	ESP8266_sprintf("Lock_Delay_Time:%d",Lock_Time_Delay_N);
	return 1;
}




