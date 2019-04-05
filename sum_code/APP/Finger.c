/*
**************************************************************
* @:file				: Finger.c
* @:brief				: 指纹功能,基于正点原子程序改编，
									可用于任何stm32开发板，测试as608指纹模块功能
									usart2用来与as608指纹模块通信，
									usart1用来输出提示语
									测试程序，录入指纹ID默认为88，删除指纹ID同样为88
									（ID的输入可以由本文件中的GET_NUM()函数，自行改写）
* @:version			:	1.0
* @:author			: 刘铸
* @:date				: 2018.2.11
***************************************************************
*/

#include <stm32f10x.h>
#include <stdio.h>
#include <string.h>
#include "delay.h"
#include "Float2char.h"
#include "as608.h"
#include "Finger.h"
#include "ESP8266_WIFI.h"
#include "SIM900A.h"

//变量声明

volatile unsigned char FR_Recongnition_Cnt = 0;
int volatile FR_Right = 0;
SysPara AS608Para;//指纹模块AS608参数
u16 ValidN;//模块内有效模板个数

//ensure=PS_ReadSysPara(&AS608Para);  //读AS608模块参数


//函数声明
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
u16 GET_NUM(void);
void Add_FR(void);
void press_FR(char *FR_Res);//刷指纹
void Del_FR(void);
int Get_FR_Right(void);
void FR_Init(void);
void FR_Cmd_Ctrl(void);

static void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
static void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
static void ShowErrMessage(u8 ensure);

//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
static void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{
	u8 array[15];
	Int2char(num,array);
	ESP8266_printf((char *)array);
}

//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用Show_Str显示
//len:指定要显示的宽度			  
static void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len)
{
	ESP8266_printf((char*)str);
}  

//显示确认码错误信息
static void ShowErrMessage(u8 ensure)
{
	Show_Str_Mid(0,120,(u8*)EnsureMessage(ensure),16,240);
}


/*
*******************************************
* @:function			: u16 GET_NUM(void)
* @:description		: 用来输入一个数字ID（区别指纹）
* @:param					:	none
* @:return				: 返回需要输入的ID
*******************************************
*/
u16 GET_NUM(void)
{
	u16 Res;
	Res = (FR_Cmd_Ctrl_Buf[4]-'0')*10+(FR_Cmd_Ctrl_Buf[5]-'0');
	return Res;
}


/*
*******************************************
* @:function			: void Add_FR(void)
* @:description		: 添加指纹 
* @:param					:	none
* @:return				: none
*******************************************
*/

void Add_FR(void)
{
	u8 i=0,ensure ,processnum=0;
	u16 ID;
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;
				Show_Str_Mid(0,100,(u8*)"Pleas touch finger!\n",16,240);//请按手指
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);//生成特征
					if(ensure==0x00)
					{
						Show_Str_Mid(0,120,(u8*)"Fingerprint correct\n",16,240);//指纹正确
						i=0;
						processnum=1;//跳到第二步						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
				Show_Str_Mid(0,100,"Pleas touch finger again!",16,240);//再按一次手指
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//生成特征			
					if(ensure==0x00)
					{

						Show_Str_Mid(0,120,"Fingerprint correct",16,240);//指纹正确
						i=0;
						processnum=2;//跳到第三步
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:
				Show_Str_Mid(0,100,"Compare twice fingerprint",16,240);//对比两次指纹
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					Show_Str_Mid(0,120,"Twice fingerprint are same",16,240);//两次指纹是一样的
					processnum=3;//跳到第四步
				}
				else 
				{
					Show_Str_Mid(0,100,"Compare fail,pleas touch again!",16,240);//对比失败，请重新按手指
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//跳回第一步		
				}
				delay_ms(1000);
				break;

			case 3:
				Show_Str_Mid(0,100,"Generate fingerprint template",16,240);//产生一个指纹模板
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					Show_Str_Mid(0,120,"Generate fingerprint success",16,240);//生成指纹模板成功
					processnum=4;//跳到第五步
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1000);
				break;
				
			case 4:	
				Show_Str_Mid(0,100,"Intput ID and save with ENTER!",16,240);//输入ID并按“Enter”保存
				Show_Str_Mid(0,120,"0=< ID <=299",16,240);	
				do
					ID=GET_NUM();
				while(!(ID<300));//输入DI必须小于300
				ensure=PS_StoreChar(CharBuffer2,ID);//储存模板
				if(ensure==0x00) 
				{								
					Show_Str_Mid(0,120,"Add fingerprint success!!!",16,240);//添加指纹成功
					PS_ValidTempleteNum(&ValidN);//读库指纹个数
					LCD_ShowNum(80,80,AS608Para.PS_max-ValidN,3,16);//显示剩余指纹个数
					delay_ms(1500);//延时后清除显示	
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		delay_ms(800);
		if(i==5)//超过5次没有按手指则退出
		{
			break;	
		}				
	}
}

//刷指纹
void press_FR(char *FR_Res)
{
	SearchResult seach;
	u8 ensure;
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			if(ensure==0x00)//搜索成功
			{				
				Show_Str_Mid(0,100,"Search fingerprint success",16,240);//搜索指纹成功
				sprintf(FR_Res,"Match ID:%d  Match score:%d",seach.pageID,seach.mathscore);//显示匹配指纹的ID和分数
			}
			else 
				ShowErrMessage(ensure);					
	  }
		else
			ShowErrMessage(ensure);
	 delay_ms(1000);//延时后清除显示
	}
		
}

//删除指纹
void Del_FR(void)
{
	u8  ensure;
	u16 num;
	Show_Str_Mid(0,100,"Delete fingerprint",16,240);//显示删除指纹
	Show_Str_Mid(0,120,"Input ID and touch Enter!",16,240);//显示输入ID并按下“Enter”
	Show_Str_Mid(0,140,"0=< ID <=299",16,240);
	delay_ms(50);
	num=GET_NUM();//获取返回的数值
	if(num==0xFFFF)
	printf("Hello\n");
	else if(num==0xFF00)
		ensure=PS_Empty();//清空指纹库
	else 
		ensure=PS_DeletChar(num,1);//删除单个指纹
	if(ensure==0)
	{
		Show_Str_Mid(0,140,"Delete fingerprint success!!!",16,240);//删除指纹成功		
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1500);//延时后清除显示
	PS_ValidTempleteNum(&ValidN);//读库指纹个数
	LCD_ShowNum(80,80,AS608Para.PS_max-ValidN,3,16);//显示剩余指纹个数

}

/*
*******************************************************
* @:function			: void Get_FR_Right(void)
* @:description		: 用来刷指纹获取权限
* @:param					:	none
* @:return				: 返回需要输入的ID
********************************************************
*/
int Get_FR_Right(void)
{
	char FR_Res[30];
	if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
	{
		press_FR(FR_Res);//刷指纹	
		if(NULL != strstr((const char*)FR_Res,"Match ID"))
		{
			ESP8266_printf("Get FR_Right");
			FR_Right = 1;
			FR_Recongnition_Cnt = 0;
		}
		else
		{
			FR_Recongnition_Cnt++;
			if(FR_Recongnition_Cnt == 4)
			{
				FR_Recongnition_Cnt = 0;
				SendSinaMsg((unsigned char*)PhoneNum1, (unsigned char*)Content2);
				
			}
		}
	}	
	return FR_Right;
}
/*
*******************************************************
* @:function			: void FR_Init(void)
* @:description		: 指纹模块初始化
* @:param					:	none
* @:return				: none
********************************************************
*/
void FR_Init(void)
{
	u8 ensure;
	PS_StaGPIO_Init();//初始化PA6读状态引脚
	
	
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		Show_Str_Mid(0,40,"Cannot connect with AS608!",16,240);
		delay_ms(1000);
		Show_Str_Mid(0,40,"Try to connect again....",16,240);	
		delay_ms(1000);	  
	}
	
	Show_Str_Mid(0,40,"Connect success!",16,240);//通讯成功/
	
	ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	if(ensure!=0x00)
		ShowErrMessage(ensure);//显示确认码错误信息	
	
	ensure=PS_ReadSysPara(&AS608Para);  //读AS608模块参数
	if(ensure==0x00)
	{
		ESP8266_sprintf("RemainNum:%d    Level:%d",AS608Para.PS_max-ValidN,AS608Para.PS_level);//显示剩余指纹数量和安全等级
	}
	else
		ShowErrMessage(ensure);	
}

/*
*******************************************************
* @:function			: void FR_Cmd_Ctrl(void)
* @:description		: 通过命令执行相应的指纹程序，指令例子：FR_88_Add，FR_88_Del
* @:param					:	none
* @:return				: 返回需要输入的ID
********************************************************
*/
void FR_Cmd_Ctrl(void)
{
	if(NULL != strstr((const char*)FR_Cmd_Ctrl_Buf,"_Add"))
	{
		Add_FR();
	}
	if(NULL != strstr((const char*)FR_Cmd_Ctrl_Buf,"_Del"))
	{
		Del_FR();
	}
	if(NULL != strstr((const char*)FR_Cmd_Ctrl_Buf,"_RIGHT_GET"))
	{
		FR_Right = 1;
		ESP8266_printf("Get FR_Right");
	}
	if(NULL != strstr((const char*)FR_Cmd_Ctrl_Buf,"_RIGHT_LOST"))
	{
		FR_Right = 0;
		ESP8266_printf("FR_Lost_Right,解除指纹权限");
	}
}

/*
*******************************************************
* @:function			: int main(void)
* @:description		: 测试as608模块的主程序，
										注意再使用as608模块前如要与其先握手
* @:param					:	none
* @:return				: 返回需要输入的ID
********************************************************
*/
////测试用主函数
//int main(void)
//{
//	u8 ensure;
//	delay_init();
//	usart_init((u32)115200);
//	usart2_init((u32)57600);
//	
//	PS_StaGPIO_Init();//初始化PA6读状态引脚

//	
//	while(PS_HandShake(&AS608Addr))//与AS608模块握手
//	{
//		Show_Str_Mid(0,40,"Cannot connect with AS608!",16,240);
//		delay_ms(1000);
//		Show_Str_Mid(0,40,"Try to connect again....",16,240);	
//		delay_ms(1000);	  
//	}
//	
//	Show_Str_Mid(0,40,"Connect success!",16,240);//通讯成功
//	
//	ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
//	if(ensure!=0x00)
//		ShowErrMessage(ensure);//显示确认码错误信息	
//	
//	ensure=PS_ReadSysPara(&AS608Para);  //读AS608模块参数
//	if(ensure==0x00)
//	{
//		printf("RemainNum:%d    Level:%d",AS608Para.PS_max-ValidN,AS608Para.PS_level);//显示剩余指纹数量和安全等级
//	}
//	else
//		ShowErrMessage(ensure);	
//	
//	Add_FR();
//	
//	if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
//	{
//		press_FR();//刷指纹			
//	}	
//	Del_FR();
//  while(1);
//}
