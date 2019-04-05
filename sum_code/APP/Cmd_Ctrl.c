
#include <string.h>
#include "Cmd_Ctrl.h"
#include "led.h"
#include "Finger.h"
#include "Environment.h"
#include "SIM900A.h"
#include "LockApp.h"
#include "ESP8266_WIFI.h"

void Cmd_Ctrl(void)
{
	//关于对led灯的控制
	if(NULL != strstr((const char*)cmd_ctrl_buf,"LED"))
	{
		LED_CmdCtl();
	}
	if(NULL != strstr((const char*)cmd_ctrl_buf,"FR_"))
	{
		FR_Cmd_Ctrl();
	}
	if(NULL != strstr((const char*)cmd_ctrl_buf,"EVR_"))
	{
		Evr_Cmd_Ctrl();
	}
	if(NULL != strstr((const char*)cmd_ctrl_buf,"SIM_"))
	{
		SIM_Ctrl();
	}
	if(NULL != strstr((const char*)cmd_ctrl_buf,"Lock"))
	{
		Lock_Cmd_Ctrl();
	}
}
