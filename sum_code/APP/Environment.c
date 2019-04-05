#include "string.h"
#include "usart2_v2.h"
#include "DH11.h"
#include "BMP280.h"
#include "ESP8266_WIFI.h"
#include "Environment.h"

void Evr_Init(void);
void Send_PressAndHeight(void);
void Send_TempAndHumidity(void);
void Evr_Cmd_Ctrl(void);

/*
**************************************************************
*@ funciton				: void Evr_Init(void)
*@ brief					: 环境设置初始化
*@ param 					: none
*@ return					: none
***********************************************************************
*/
void Evr_Init(void)
{
	DHT11_Init();
	bmp280Init();
}
/*
**************************************************************
*@ funciton				: void Send_TempAndHumidity(void)
*@ brief					: 读取温度和湿度
*@ param 					: none
*@ return					: none
***********************************************************************
*/
void Send_TempAndHumidity(void)
{
	DHT11_Data_TypeDef DHT11_Data;
	DHT11_Read_TempAndHumidity(&DHT11_Data);
	ESP8266_sprintf("Temp:%d.%d",DHT11_Data.temp_int,DHT11_Data.temp_deci);
	ESP8266_sprintf("Humidity:%d.%d",DHT11_Data.humi_int,DHT11_Data.humi_deci);
}
/*
**************************************************************
*@ funciton				: void Send_PressAnd_Height(void)
*@ brief					: 读取压强和海拔
*@ param 					: none
*@ return					: none
***********************************************************************
*/
void Send_PressAndHeight(void)
{
	float bmp280_press,bmp280_temp,high;
	bmp280GetData(&bmp280_press,&bmp280_temp,&high);  //bm280取值函数
	ESP8266_sprintf("Press:%.2f,BMP_Temp:%.2f,Height:%.2f",bmp280_press,bmp280_temp,high);
}
/*
**************************************************************
*@ funciton				: void Evr_Cmd_Ctrl(void)
*@ brief					: 环境的命令控制
*@ param 					: none
*@ return					: none
***********************************************************************
*/
void Evr_Cmd_Ctrl(void)
{
	if(NULL != strstr((const char*)Evr_Cmd_Buf,"T_H"))
	{
		Send_TempAndHumidity();
	}
	if(NULL != strstr((const char*)Evr_Cmd_Buf,"P_H"))
	{
		Send_PressAndHeight();
	}
	if(NULL != strstr((const char*)Evr_Cmd_Buf,"ALL"))
	{
		Send_TempAndHumidity();
		Send_PressAndHeight();
	}
}

