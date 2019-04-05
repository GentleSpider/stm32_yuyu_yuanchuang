/*
**************************************************************
* @:file				: Finger.c
* @:brief				: ָ�ƹ���,��������ԭ�ӳ���ı࣬
									�������κ�stm32�����壬����as608ָ��ģ�鹦��
									usart2������as608ָ��ģ��ͨ�ţ�
									usart1���������ʾ��
									���Գ���¼��ָ��IDĬ��Ϊ88��ɾ��ָ��IDͬ��Ϊ88
									��ID����������ɱ��ļ��е�GET_NUM()���������и�д��
* @:version			:	1.0
* @:author			: ����
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

//��������

volatile unsigned char FR_Recongnition_Cnt = 0;
int volatile FR_Right = 0;
SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чģ�����

//ensure=PS_ReadSysPara(&AS608Para);  //��AS608ģ�����


//��������
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
u16 GET_NUM(void);
void Add_FR(void);
void press_FR(char *FR_Res);//ˢָ��
void Del_FR(void);
int Get_FR_Right(void);
void FR_Init(void);
void FR_Cmd_Ctrl(void);

static void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
static void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
static void ShowErrMessage(u8 ensure);

//��ʾ����,��λΪ0,����ʾ
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//color:��ɫ 
//num:��ֵ(0~4294967295);	 
static void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{
	u8 array[15];
	Int2char(num,array);
	ESP8266_printf((char *)array);
}

//��ָ����ȵ��м���ʾ�ַ���
//����ַ����ȳ�����len,����Show_Str��ʾ
//len:ָ��Ҫ��ʾ�Ŀ��			  
static void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len)
{
	ESP8266_printf((char*)str);
}  

//��ʾȷ���������Ϣ
static void ShowErrMessage(u8 ensure)
{
	Show_Str_Mid(0,120,(u8*)EnsureMessage(ensure),16,240);
}


/*
*******************************************
* @:function			: u16 GET_NUM(void)
* @:description		: ��������һ������ID������ָ�ƣ�
* @:param					:	none
* @:return				: ������Ҫ�����ID
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
* @:description		: ���ָ�� 
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
				Show_Str_Mid(0,100,(u8*)"Pleas touch finger!\n",16,240);//�밴��ָ
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);//��������
					if(ensure==0x00)
					{
						Show_Str_Mid(0,120,(u8*)"Fingerprint correct\n",16,240);//ָ����ȷ
						i=0;
						processnum=1;//�����ڶ���						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
				Show_Str_Mid(0,100,"Pleas touch finger again!",16,240);//�ٰ�һ����ָ
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//��������			
					if(ensure==0x00)
					{

						Show_Str_Mid(0,120,"Fingerprint correct",16,240);//ָ����ȷ
						i=0;
						processnum=2;//����������
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:
				Show_Str_Mid(0,100,"Compare twice fingerprint",16,240);//�Ա�����ָ��
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					Show_Str_Mid(0,120,"Twice fingerprint are same",16,240);//����ָ����һ����
					processnum=3;//�������Ĳ�
				}
				else 
				{
					Show_Str_Mid(0,100,"Compare fail,pleas touch again!",16,240);//�Ա�ʧ�ܣ������°���ָ
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//���ص�һ��		
				}
				delay_ms(1000);
				break;

			case 3:
				Show_Str_Mid(0,100,"Generate fingerprint template",16,240);//����һ��ָ��ģ��
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					Show_Str_Mid(0,120,"Generate fingerprint success",16,240);//����ָ��ģ��ɹ�
					processnum=4;//�������岽
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1000);
				break;
				
			case 4:	
				Show_Str_Mid(0,100,"Intput ID and save with ENTER!",16,240);//����ID������Enter������
				Show_Str_Mid(0,120,"0=< ID <=299",16,240);	
				do
					ID=GET_NUM();
				while(!(ID<300));//����DI����С��300
				ensure=PS_StoreChar(CharBuffer2,ID);//����ģ��
				if(ensure==0x00) 
				{								
					Show_Str_Mid(0,120,"Add fingerprint success!!!",16,240);//���ָ�Ƴɹ�
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
					LCD_ShowNum(80,80,AS608Para.PS_max-ValidN,3,16);//��ʾʣ��ָ�Ƹ���
					delay_ms(1500);//��ʱ�������ʾ	
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		delay_ms(800);
		if(i==5)//����5��û�а���ָ���˳�
		{
			break;	
		}				
	}
}

//ˢָ��
void press_FR(char *FR_Res)
{
	SearchResult seach;
	u8 ensure;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				Show_Str_Mid(0,100,"Search fingerprint success",16,240);//����ָ�Ƴɹ�
				sprintf(FR_Res,"Match ID:%d  Match score:%d",seach.pageID,seach.mathscore);//��ʾƥ��ָ�Ƶ�ID�ͷ���
			}
			else 
				ShowErrMessage(ensure);					
	  }
		else
			ShowErrMessage(ensure);
	 delay_ms(1000);//��ʱ�������ʾ
	}
		
}

//ɾ��ָ��
void Del_FR(void)
{
	u8  ensure;
	u16 num;
	Show_Str_Mid(0,100,"Delete fingerprint",16,240);//��ʾɾ��ָ��
	Show_Str_Mid(0,120,"Input ID and touch Enter!",16,240);//��ʾ����ID�����¡�Enter��
	Show_Str_Mid(0,140,"0=< ID <=299",16,240);
	delay_ms(50);
	num=GET_NUM();//��ȡ���ص���ֵ
	if(num==0xFFFF)
	printf("Hello\n");
	else if(num==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
	else 
		ensure=PS_DeletChar(num,1);//ɾ������ָ��
	if(ensure==0)
	{
		Show_Str_Mid(0,140,"Delete fingerprint success!!!",16,240);//ɾ��ָ�Ƴɹ�		
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1500);//��ʱ�������ʾ
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	LCD_ShowNum(80,80,AS608Para.PS_max-ValidN,3,16);//��ʾʣ��ָ�Ƹ���

}

/*
*******************************************************
* @:function			: void Get_FR_Right(void)
* @:description		: ����ˢָ�ƻ�ȡȨ��
* @:param					:	none
* @:return				: ������Ҫ�����ID
********************************************************
*/
int Get_FR_Right(void)
{
	char FR_Res[30];
	if(PS_Sta)	 //���PS_Sta״̬���������ָ����
	{
		press_FR(FR_Res);//ˢָ��	
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
* @:description		: ָ��ģ���ʼ��
* @:param					:	none
* @:return				: none
********************************************************
*/
void FR_Init(void)
{
	u8 ensure;
	PS_StaGPIO_Init();//��ʼ��PA6��״̬����
	
	
	while(PS_HandShake(&AS608Addr))//��AS608ģ������
	{
		Show_Str_Mid(0,40,"Cannot connect with AS608!",16,240);
		delay_ms(1000);
		Show_Str_Mid(0,40,"Try to connect again....",16,240);	
		delay_ms(1000);	  
	}
	
	Show_Str_Mid(0,40,"Connect success!",16,240);//ͨѶ�ɹ�/
	
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	if(ensure!=0x00)
		ShowErrMessage(ensure);//��ʾȷ���������Ϣ	
	
	ensure=PS_ReadSysPara(&AS608Para);  //��AS608ģ�����
	if(ensure==0x00)
	{
		ESP8266_sprintf("RemainNum:%d    Level:%d",AS608Para.PS_max-ValidN,AS608Para.PS_level);//��ʾʣ��ָ�������Ͱ�ȫ�ȼ�
	}
	else
		ShowErrMessage(ensure);	
}

/*
*******************************************************
* @:function			: void FR_Cmd_Ctrl(void)
* @:description		: ͨ������ִ����Ӧ��ָ�Ƴ���ָ�����ӣ�FR_88_Add��FR_88_Del
* @:param					:	none
* @:return				: ������Ҫ�����ID
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
		ESP8266_printf("FR_Lost_Right,���ָ��Ȩ��");
	}
}

/*
*******************************************************
* @:function			: int main(void)
* @:description		: ����as608ģ���������
										ע����ʹ��as608ģ��ǰ��Ҫ����������
* @:param					:	none
* @:return				: ������Ҫ�����ID
********************************************************
*/
////������������
//int main(void)
//{
//	u8 ensure;
//	delay_init();
//	usart_init((u32)115200);
//	usart2_init((u32)57600);
//	
//	PS_StaGPIO_Init();//��ʼ��PA6��״̬����

//	
//	while(PS_HandShake(&AS608Addr))//��AS608ģ������
//	{
//		Show_Str_Mid(0,40,"Cannot connect with AS608!",16,240);
//		delay_ms(1000);
//		Show_Str_Mid(0,40,"Try to connect again....",16,240);	
//		delay_ms(1000);	  
//	}
//	
//	Show_Str_Mid(0,40,"Connect success!",16,240);//ͨѶ�ɹ�
//	
//	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
//	if(ensure!=0x00)
//		ShowErrMessage(ensure);//��ʾȷ���������Ϣ	
//	
//	ensure=PS_ReadSysPara(&AS608Para);  //��AS608ģ�����
//	if(ensure==0x00)
//	{
//		printf("RemainNum:%d    Level:%d",AS608Para.PS_max-ValidN,AS608Para.PS_level);//��ʾʣ��ָ�������Ͱ�ȫ�ȼ�
//	}
//	else
//		ShowErrMessage(ensure);	
//	
//	Add_FR();
//	
//	if(PS_Sta)	 //���PS_Sta״̬���������ָ����
//	{
//		press_FR();//ˢָ��			
//	}	
//	Del_FR();
//  while(1);
//}
