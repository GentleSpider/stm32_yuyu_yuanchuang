#include <stm32f10x.h>
#include <stdio.h>
#include <string.h>

#include "stm32_SPI_RC522.h"
#include "ESP8266_WIFI.h"

#include "RFID_IC.h"

/*ȫ�ֱ���*/
volatile u8 Item_Get_Flag = 0;
enum Item_Importance Item_Res;
char IC_Res[25];

unsigned char CT[2];//������
unsigned char SN[4]; //����
unsigned char RFID[16];			//���RFID 
//unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x07,0x80,0x29,0xff,0xff,0xff,0xff,0xff,0xff};
//����������
//unsigned char lxl_bit=0;
//unsigned char card1_bit=0;
//unsigned char card2_bit=0;
//unsigned char card3_bit=0;
//unsigned char card4_bit=0;
//unsigned char total=0;
//unsigned char lxl[4]={6,109,250,186};
//unsigned char card_1[4]={66,193,88,0};
//unsigned char card_2[4]={66,191,104,0};
//unsigned char card_3[4]={62,84,28,11};
//unsigned char card_4[4]={126,252,248,12};
//u8 KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};

/*��������*/
//void ShowID(u16 x,u16 y, u8 *p, u16 charColor, u16 bkColor);	 //��ʾ���Ŀ��ţ���ʮ��������ʾ
//void PutNum(u16 x,u16 y, u32 n1,u8 n0, u16 charColor, u16 bkColor);	//��ʾ����
//void Store(u8 *p,u8 store,u8 cash);//����Ҫ��һ������	


int Read_IC(char *IC_Res);
enum Item_Importance IC_Res_Match(char * IC_Res);


static void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,char *p);
	
//��ʾ�ַ���
//x,y:�������
//width,height:�����С  
//size:�����С
//*p:�ַ�����ʼ��ַ		  
static void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,char *p)
{         
	ESP8266_printf(p);
}

/*
**********************************************************
*@ function						: int Read_IC(char *IC_Res)
*@ Description				: ��ȡˢ��ʱIC����id	����ʹ����ʱ��ʼ��delay_init();
												����ʹ�ó�ʼ����Ƶģ��InitRc522();��		  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
**********************************************************
*/
int Read_IC(char *IC_Res)
{
	
	unsigned char status;
 
	status = PcdRequest(PICC_REQALL,CT);/*����*/
	if(status==MI_OK)//�����ɹ�
	{
			LCD_ShowString(0,30,200,16,16,"PcdRequest_MI_OK");
			status=MI_ERR;
			status = PcdAnticoll(SN);/*����ײ*/
			if (status==MI_OK)//���nײ�ɹ�
			{
				LCD_ShowString(150,30,200,16,16,"PcdAnticoll_MI_OK");
				status=MI_ERR;		
				//ShowID(0,200,SN,BLUE,WHITE); //��Һ��������ʾ����ID��
				sprintf(IC_Res,"ID:%02x %02x %02x %02x\n",SN[0],SN[1],SN[2],SN[3]);//���Ϳ���		
				return 1;
			}
			else 
				return 0;
	}		
	else 
		return 0;
	
}
/*
**********************************************************
*@ function						: enum Item_Importance IC_Res_Match(char * IC_Res)
*@ Description				: �б���Ʒ����Ҫ�̶�		  
*@ param RData				: IC_Res �洢RFID���Ž��
*@ return							: ������Ʒ����Ҫ�̶�
*@ version						: 1.0
**********************************************************
*/
enum Item_Importance IC_Res_Match(char * IC_Res)
{
	if(Item_Get_Flag == 0)
	{
		Item_Get_Flag = 1;
	}
	else
		Item_Get_Flag = 0;
	if(NULL != strstr((const char *)IC_Res,Item_Wallet))
	{
		return Important;
	}
	else
		return Common;
}

//int main(void)
//{
//	
//	unsigned char status;
//	unsigned char s=0x08;
//	u8 t,key;
//	u8 j; 

// 	delay_init();	    	 //��ʱ������ʼ��	  
//	usart_init(9600);				
//	InitRc522();				//��ʼ����Ƶ��ģ��
//  while(1) 
//	{	
//			status = PcdRequest(PICC_REQALL,CT);/*����*/
//			if(status==MI_OK)//�����ɹ�
//			{
//			 LCD_ShowString(0,30,200,16,16,"PcdRequest_MI_OK");
//			 status=MI_ERR;
//			 status = PcdAnticoll(SN);/*����ײ*/
//			}
//			if (status==MI_OK)//���nײ�ɹ�
//			{
//				LCD_ShowString(150,30,200,16,16,"PcdAnticoll_MI_OK");
//				status=MI_ERR;		
//				//ShowID(0,200,SN,BLUE,WHITE); //��Һ��������ʾ����ID��
//				printf("ID:%02x %02x %02x %02x\n",SN[0],SN[1],SN[2],SN[3]);//���Ϳ���
////				LCD_ShowString(0,100,200,16,16,"The Card ID is:");
////				for(j=0;j<4;j++)
////				{
////					 LCD_ShowNum(0,116+j*16,SN[j],3,16);
////				}
////				if((SN[0]==lxl[0])&&(SN[1]==lxl[1])&&(SN[2]==lxl[2])&&(SN[3]==lxl[3]))
////				{
////					lxl_bit=1;
////					LCD_ShowString(0,0,200,16,16,"The User is:lxl");
////				}
////				if((SN[0]==card_1[0])&&(SN[1]==card_1[1])&&(SN[2]==card_1[2])&&(SN[3]==card_1[3]))
////				{
////					card1_bit=1;
////					LCD_ShowString(0,0,200,16,16,"The User is:card_1");
////				}
////				if((SN[0]==card_2[0])&&(SN[1]==card_2[1])&&(SN[2]==card_2[2])&&(SN[3]==card_2[3]))
////				{
////					card2_bit=1;
////					LCD_ShowString(0,0,200,16,16,"The User is:card_2");
////				}
////				
////				if((SN[0]==card_3[0])&&(SN[1]==card_3[1])&&(SN[2]==card_3[2])&&(SN[3]==card_3[3]))
////				{
////					card3_bit=1;
////					LCD_ShowString(0,0,200,16,16,"The User is:card_3");
////				}
////				if((SN[0]==card_4[0])&&(SN[1]==card_4[1])&&(SN[2]==card_4[2])&&(SN[3]==card_4[3]))
////				{
////					card4_bit=1;
////					LCD_ShowString(0,0,200,16,16,"The User is:card_4");
////				}
////				total=card1_bit+card2_bit+card3_bit+card4_bit+lxl_bit;
////				LCD_ShowString(0,16,200,16,16,"total:");
////				LCD_ShowNum(46,16,total,2,16);

////				status =PcdSelect(SN);
//				//Reset_RC522();
//		
//			}
////			else
////			{
////			}
////			if(status==MI_OK)//�x���ɹ�
////			{
////			 //LCD_ShowString(0,50,200,16,16,"PcdAnticoll_MI_OK");
////			 LCD_ShowString(0,48,200,16,16,"PcdSelect_MI_OK  ");
////			 status=MI_ERR;
////			 status =PcdAuthState(0x60,0x09,KEY,SN);
////			 }
////			 if(status==MI_OK)//��C�ɹ�
////			 {
////			  LCD_ShowString(0,64,200,16,16,"PcdAuthState_MI_OK  ");
////			  status=MI_ERR;
////			  status=PcdRead(s,RFID);
////			  //status=PcdWrite(s,RFID1);
////			  }

////			if(status==MI_OK)//�x���ɹ�
////			 {
////			  LCD_ShowString(0,80,200,16,16,"READ_MI_OK");
////			  status=MI_ERR;
////			  delay_ms(100);
////			 }
////			if(key==2)
////			{
////			for(t=0;t<16;t++)                                        //������Ƶģ��
////	        {
////	         USART_ClearFlag(USART1,USART_FLAG_TC);                 //��ȡUSART_SR
////	         USART_SendData(USART1,RFID[t]);                         //�򴮿�2��������
////	         while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET); //�ȴ����ͽ���
////         	}
////			}
//			//else LCD_ShowString(0,80,200,16,16,"READ_MI_ERR ");
////////////////////////////////////////////////////////////////////////////////////////////////////////
//			  /*if(status==MI_OK)
//			  {
//			   LCD_ShowString(120,80,200,16,16,"WRITE_MI_OK");
//			   status=MI_ERR;
//			  }*/
//			  //else LCD_ShowString(120,80,200,16,16,"WRITE_MI_ERR");
//			  //Reset_RC522();
//	}
//			
//}
