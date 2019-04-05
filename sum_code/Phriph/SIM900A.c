#include "SIM900A.h"
#include "usart1_v2.h"		
#include "delay.h"	 
#include "string.h" 
#include "math.h"
#include "stdio.h"
#include "ESP8266_WIFI.h"
//#include "STMFLASH.h"
//********************************************************************************
//��
//////////////////////////////////////////////////////////////////////////////////	

volatile u8 PhoneNum1[50] = "00310033003800390036003700360032003500330036";
volatile u8 Content1[50] = "4F6076848D3591CD726954C15DF288AB975E6CD562FF51FA";
volatile u8 PhoneNum2[50] = "00310033003800390036003600320038003600380037";
volatile u8 Content2[60] = "63077EB98BC6522B8FDE7EED51FA9519FF0C8BF76CE8610F5B895168";

u8 SIM900_CSQ[3];
u8 dtbuf[50];   								//��ӡ������	


//��������
//static void sim_at_response(u8 mode);
//static u8* sim900a_check_cmd(u8 *str);
static void SIM_write(u8 *cmd, uint32_t len);
static void PromptMsgSendStr(u8 *Data, u16 len);
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 sim900a_work_test(void);
u8 GSM_Dect(void);
u8 SIM_MAKE_CALL(u8 *number);
u8 sim900a_send_chmessage_zc(u8*number,u8*content);
u8 sim900a_word_test(void);
u8 SIM808_GPS_HANDLE(u8 * message);
u8 sim808_open_gps(void);

void SIM_Ctrl(void);

void Wait_CREG(void);
int Set_Text_Mode(void);
void Send_Text_Sms(unsigned char *number,unsigned char *Msg);
unsigned char SendSinaMsg(unsigned char *PhoneNumber,unsigned char *Msg);

//���ն���
u8 RcvMsgInit(void);
u8 RcvMsg_Ctrl(char *RcvMsg);


/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//usmart֧�ֲ��� 
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART2_RX_STA;
//     1,����USART2_RX_STA;
//static void sim_at_response(u8 mode)
//{
//	if(SIM_RX_STA&0X8000)		//���յ�һ��������
//	{ 
//		SIM_RX_BUF[SIM_RX_STA&0X7FFF]=0;//��ӽ�����
//		printf("%s",SIM_RX_BUF);	//���͵�����
//		if(mode)SIM_RX_STA=0;		
//	} 
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-SIM900A �������(���Ų��ԡ����Ų��ԡ�GPRS����)���ô���
//sim900a���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
//static u8* sim900a_check_cmd(u8 *str)
//{
//	char *strx=0;
//	if(SIM_RX_STA&0X8000)		//���յ�һ��������
//	{ 
//		SIM_RX_BUF[SIM_RX_STA&0X7FFF]=0;//��ӽ�����
//		strx=strstr((const char*)SIM_RX_BUF,(const char*)str);
//	} 
//	return (u8*)strx;
//}

static void SIM_write(u8 *cmd, uint32_t len)
{
	usart1_write(USART1, cmd, len);
}

static void PromptMsgSendStr(u8 *Data, u16 len)
{
	ESP8266_SendLenStr(Data, len);
}

static void sim900a_send_char(u8 data)
{
	USART_SendData(USART1,data);
}

//��sim900a��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
//static u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
//{
//	u8 res=0; 
//	SIM_RX_STA=0;
//	if((u32)cmd<=0XFF)
//	{
//		while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
//		USART2->DR=(u32)cmd;
//	}else u2_printf("%s\r\n",cmd);//��������
//	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
//	{
//		while(--waittime)	//�ȴ�����ʱ
//		{
//			delay_ms(10);
//			if(SIM_RX_STA&0X8000)//���յ��ڴ���Ӧ����
//			{
//				if(sim900a_check_cmd(ack))break;//�õ���Ч���� 
//				SIM_RX_STA=0;
//			} 
//		}
//		if(waittime==0)res=1; 
//	}
//	return res;
//} 

u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
		u8 res = 1;
		u8 count = 5;
    while(count)
    {
        memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
				SIM_rcv_len=0;
        SIM_write((unsigned char *)cmd, strlen((const char *)cmd));
        delay_ms(waittime*20);
        if((NULL != strstr((const char *)SIM_RX_BUF, (const char*)ack)))	//�ж��Ƿ���Ԥ�ڵĽ��
        {
						res = 0;
						break;
        }
        else
        {
            delay_ms(10);
        }
				count--;
    }
		return res;
} 

u8 sim900a_send_cmd_char(u8 cmd,u8 *ack,u16 waittime)
{
		u8 res = 1;
		u8 count = 5;
    while(count)
    {
        memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
				SIM_rcv_len=0;
        sim900a_send_char(cmd);
        delay_ms(waittime*7);
        if((NULL != strstr((const char *)SIM_RX_BUF, (const char*)ack)))	//�ж��Ƿ���Ԥ�ڵĽ��
        {
						res = 0;
						break;
        }
        else
        {
            delay_ms(10);
        }
				count--;
    }
		return res;
} 

u8 sim900a_work_test(void)
{
	if(sim900a_send_cmd((u8 *)"AT\r\n",(u8 *)"OK",100))
	{
		if(sim900a_send_cmd((u8 *)"AT\r\n",(u8 *)"OK",100))return SIM_COMMUNTION_ERR;	//ͨ�Ų���
	}		
	if(sim900a_send_cmd((u8 *)"AT+CPIN?\r\n",(u8 *)"READY",400))return SIM_CPIN_ERR;	//û��SIM��
	if(sim900a_send_cmd((u8 *)"AT+CREG?\r\n",(u8 *)"0,1",400))
	{
		if(strstr((const char*)SIM_RX_BUF,"0,5")==NULL)
		{
			 if(!sim900a_send_cmd((u8 *)"AT+CSQ\r\n",(u8 *)"OK",200))	
			 {
					memcpy(SIM900_CSQ,SIM_RX_BUF+15,2);
			 }
			 return SIM_CREG_FAIL;	//�ȴ����ŵ�����
		}
	}
	return SIM_OK;
}
u8 GSM_Dect(void)
{
	u8 res;
	res=sim900a_work_test();	
	switch(res)
	{
		case SIM_OK:
			PromptMsgSendStr("GSMģ���Լ�ɹ�",strlen("GSMģ���Լ�ɹ�"));
			break;
		case SIM_COMMUNTION_ERR:
			PromptMsgSendStr("��GSMģ��δͨѶ�ɹ�����ȴ�",strlen("��GSMģ��δͨѶ�ɹ�����ȴ�"));
			break;
		case SIM_CPIN_ERR:
			PromptMsgSendStr("û��⵽SIM��",strlen("û��⵽SIM��"));	
			break;
		case SIM_CREG_FAIL:
			PromptMsgSendStr("ע�������С�����",strlen("ע�������С�����"));	
			PromptMsgSendStr("��ǰ�ź�ֵ��",strlen("��ǰ�ź�ֵ��"));	PromptMsgSendStr(SIM900_CSQ,2);//PromptMsgSendStr("\r\n",2);
			break;		
		default:
			break;
	}
	return res;
}
u8 SIM_MAKE_CALL(u8 *number)
{
	u8 cmd[20];
	sprintf((char*)cmd,"ATD%s;\r\n",number);
	if(sim900a_send_cmd(cmd,(u8 *)"OK",200))	return SIM_MAKE_CALL_ERR;
	return SIM_OK;
}

u8 sim900a_send_chmessage_zc(u8*number,u8*content)
{
	u8 cmd[40];
	sprintf((char*)cmd,"AT+CMGS=\"%s\"\r\n",number);
	if(sim900a_send_cmd((u8 *)"AT+CMGF=1\r\n",(u8 *)"OK",100))	return SIM_CMGF_ERR;
	if(sim900a_send_cmd((u8 *)"AT+CSCS=\"UCS2\"\r\n",(u8 *)"OK",100))	return SIM_CSCS_ERR;
	if(sim900a_send_cmd((u8 *)"AT+CSCA?\r\n",(u8 *)"OK",100))	return SIM_CSCA_ERR;
	if(sim900a_send_cmd((u8 *)"AT+CSMP=17,167,0,25\r\n",(u8 *)"OK",100))	return SIM_CSMP_ERR;
	if(sim900a_send_cmd(cmd,(u8 *)">",100))	return SIM_CMGS_ERR;
	u1_printf("%s",content);
	sim900a_send_char(0x1A);
	//if(sim900a_send_cmd_char(0x1A,(u8 *)"+CMGS:",1500))	return SIM_CMGS_SEND_FAIL;
	return SIM_OK;
}

u8 sim808_open_gps(void)
{
	if(sim900a_send_cmd((u8 *)"AT+CGNSPWR=1\r\n",(u8 *)"OK",100))	 return 1;
		return 0;
}

u8 sim900a_word_test(void)
{
	if(sim900a_send_cmd((u8 *)"AT\r\n",(u8 *)"OK",100))
	{
		if(sim900a_send_cmd((u8 *)"AT\r\n",(u8 *)"OK",100))
			return 1;	//ͨ�Ų���
	}		
	if(sim900a_send_cmd((u8 *)"AT+CPIN?\r\n",(u8 *)"READY",200))return 2;	//û��SIM��
	if(sim900a_send_cmd((u8 *)"AT+CREG?\r\n",(u8 *)"0,1",200))
	{
		 if(!sim900a_send_cmd((u8 *)"AT+CSQ\r\n",(u8 *)"OK",100))	
		 {
				memcpy(SIM900_CSQ,SIM_RX_BUF+15,2);
		 }
		 return 3;	//�ȴ����ŵ�����
	}	
	return 0;
}

//SIM808 GPS���ݽ������쳣����
u8 SIM808_GPS_HANDLE(u8 * message)
{
	u8 res,sta,count;	
	char *p1,*p2;
	if(!sim900a_send_cmd((u8 *)"AT+CGNSINF\r\n",(u8 *)"OK",300))
	{
		if((p1=(char*)strstr((const char*)SIM_RX_BUF,"+CGNSINF:")),(p1!=NULL))//
		{		
			if((p2=(char*)strstr((const char*)p1,"OK")),(p2!=NULL))//?????
			{
				*p2=0;//?????
				p2=strtok((p1),",");
				p2=(char*)strtok(NULL,",");	//��ȡ�Ƿ�λ��ʾ
				//�������
				message[0]='0';
				p2=(char*)strtok(NULL,",");
				//res=strlen(p2);						
				sprintf((char*)message+1,",%s",(u8*)p2); 	//UTC TIME
				//sprintf(message,%s,p2);//memcpy(message,p2,strlen(p2));						
				p2=(char*)strtok(NULL,",");
				sprintf((char*)message+strlen((char *)message),",%s",(u8*)p2);	//LAT
				//memcpy(message+strlen(message),p2,strlen(p2));
				p2=(char*)strtok(NULL,",");					
				sprintf((char*)message+strlen((char *)message),",%s,0001",(u8*)p2);	//LONG
				//memcpy(message+strlen(message),p2,strlen(p2));
				if(*p2=='1')		//��λ
				{		
						sta=0;	//��ЧGPS����
				}	
				else 	//δ��λ
				{
					if(sim900a_send_cmd((u8 *)"AT+CGNSPWR?\r\n",(u8 *)"+CGNSPWR: 1",100))//�жϵ�Դ�Ƿ��
					{
						sim900a_send_cmd((u8 *)"AT+CGNSPWR=1\r\n",(u8 *)"OK",100);
					}
					sta=1;	//����ЧGPS����
				}					
			}
		}			
	}
	else 
	{
		sprintf((char*)message,"%s","0,0,0,0");	//��ʱ
		res=1;
		count = 5;
		while(res && count)
		{
			res=sim900a_word_test();	//���¼��
			count--;
		}
		sta=2;	//��GPS����
	}
	return sta;
}

/*******************************************************************************
* ������ : Wait_CREG
* ����   : �ȴ�ģ��ע��ɹ�
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void Wait_CREG(void)
{
	char *p;
	int count = 5;
	
	while(count)        			
	{        
		memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
		SIM_rcv_len=0;
		SIM_printf("AT\r\n");//��ѯģ������ע��״̬
		delay_ms(500);  	
		if(NULL != strstr((const char*)SIM_RX_BUF,"OK"))
		{
				break;
		}
		count--;
	}
	ESP8266_printf((char*)SIM_RX_BUF);
	count = 5;
  while(count)        			
	{        
		memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
		SIM_rcv_len=0;
		SIM_printf("AT+CREG?\r\n");//��ѯģ������ע��״̬
		delay_ms(500);  	
		p = strchr((const char*)SIM_RX_BUF,':');
		if(NULL != p)
		{
			if((*(p+4) == '1') || *(p+4) == '5')
				break;
		}
		count--;
	}
	
}

/*******************************************************************************
* ������ : Set_Text_Mode
* ����   : ���ö���ΪTEXT�ı�ģʽ
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
int Set_Text_Mode(void)
{
	int stat = 1;
	sim900a_send_cmd("ATE0\r\n","OK",30);										  //ȡ������	
//	Second_AT_Command("AT+CMGF=0","OK",3);								//����PDUģʽ	
//	Second_AT_Command("AT+CPMS=\"SM\",\"SM\",\"SM\"","OK",3);//���в�������SIM���н���
	if(sim900a_send_cmd("AT+CMGF=1\r\n","OK",30)) stat = 0;
	if(sim900a_send_cmd("AT+CSCS=\"UCS2\"\r\n","OK",30)) stat = 0;
	if(sim900a_send_cmd("AT+CSMP=17,167,0,25\r\n","OK",30)) stat = 0;
	return stat;
	/*
	sim900a_send_cmd("AT+CMGS=\"00310033003800390036003600320038003600380037\"",">",3);
	UART2_SendString("4E2D56FD4F60597D");
	delay_ms(100);
	UART2_SendData(0X1A); 
	*/
}

/*******************************************************************************
* ������ : Send_Text_Sms
* ����   : ����PDU�ı�����
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
void Send_Text_Sms(unsigned char *number,unsigned char *Msg)
{
	int count = 5;
	unsigned char  Send_Buf[70];
	memset(Send_Buf,0,70);//���Send_Buf
	strcpy((char *)Send_Buf,"AT+CMGS=\"");//��Ҫ���͵ĵ绰�����Լ���ظ�ʽ���Ƹ�Send_Buf
	strcat((char *)Send_Buf,(char *)number);
	strcat((char *)Send_Buf,"\"\r\n");
	SIM_write(Send_Buf,strlen((const char*)Send_Buf)); //�������ݳ��ȣ�27������ļ��㷽�������ڵ��ԱȽϣ����յ���>���ŷ��Ͷ�������
	while(count)
	{
		if(NULL != strchr((const char *)SIM_RX_BUF, '>'))
			break;
		delay_ms(500);
		count--;
	}
	SIM_printf((char *)Msg);     //���Ͷ�������
	delay_ms(500);
	sim900a_send_char(0X1A);          //���ͽ�����
}

/*
********************************************
*@ function			: unsigned char SendSinaMsg()
*@ Description		: ������Ϣ					  
*@ param 			: NONE
*@ return			: �ɹ�����1
********************************************
*/
unsigned char SendSinaMsg(unsigned char *PhoneNumber,unsigned char *Msg)
{
	ESP8266_printf("Enter MSG");
	Wait_CREG();    //��ѯ�ȴ�ģ��ע��ɹ�
	ESP8266_printf("CREG Finish");
	if(!Set_Text_Mode()) ESP8266_printf("Mode Wrong");;//���ö���Ϊucs2ģʽ
	ESP8266_printf("Mode Finish");
	Send_Text_Sms(PhoneNumber,Msg);//����һ������Ϣ
	ESP8266_printf("Send Finish");
	//��������Ϊ���룬��UCS2����ת��ΪIRA
	delay_ms(200);
	while(1)
	{
		if(!sim900a_send_cmd("AT+CMGF=1\r\n","OK",30)) 
			if(!sim900a_send_cmd("AT+CSCS=\"IRA\"\r\n","OK",30))
			{
				ESP8266_printf("IRA ok");
				break;
			}	
			else ESP8266_printf("IRA error");
		else ESP8266_printf("CMGF_1 error");
	}

	return 1;
}
/*
********************************************
*@ function				: void SIM_Ctrl(void)
*@ Description		: ָ�����					  
*@ param 					: NONE
*@ return					: none
********************************************
*/
void SIM_Ctrl(void)
{
		if(NULL != strstr((const char*)SIM_Ctrl_Buf,"_MSG"))
		{
			SendSinaMsg((unsigned char*)PhoneNum1,(unsigned char*)Content1);
			ESP8266_printf("���ŷ������");
		}
		if(NULL != strstr((const char*)SIM_Ctrl_Buf,"_GPSO"))
		{
			sim808_open_gps();
			ESP8266_printf("OPEN GPS");
		}
		if(NULL != strstr((const char*)SIM_Ctrl_Buf,"_GPSG"))
		{
			u8 GPS_MSG[50];
			SIM808_GPS_HANDLE(GPS_MSG);
			ESP8266_sprintf("GPS_Data:%s",GPS_MSG);
		}
		if(NULL != strstr((const char*)SIM_Ctrl_Buf,"_GPSA"))
		{
			u8 GPS_MSG[50];
			sim808_open_gps();
			ESP8266_printf("OPEN GPS");
			SIM808_GPS_HANDLE(GPS_MSG);
			ESP8266_sprintf("GPS_Data:%s",GPS_MSG);
		}
		if(NULL != strstr((const char*)SIM_Ctrl_Buf,"_NUMS:"))
		{
			u8 i;
			char *p = strchr((const char*)SIM_Ctrl_Buf,':');
			for(i=0; i<=11; i++)
			{
				PhoneNum1[i*4-1] = *(p+i);
			}
			ESP8266_sprintf("NumSet_Finish:%s",PhoneNum1);
		}
}

/*
********************************************
*@ function						: u8 RcvMsgInit(void)
*@ Description				: ���ն��ų�ʼ������Ҫ���豸��ʼ��֮��ʹ��					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/
u8 RcvMsgInit(void)
{
	if((sim900a_send_cmd((u8 *)"AT\r\n",(u8 *)"OK",10))) return 0;
	if((sim900a_send_cmd((u8 *)"ATE0\r\n",(u8 *)"OK",10))) return 0;
	if((sim900a_send_cmd((u8 *)"AT+CNMI=2,1\r\n",(u8 *)"OK",10))) return 0;  //AT+CNMI=2,1//���÷��������뻺�淽ʽ����
	if((sim900a_send_cmd((u8 *)"AT+CMGF=1\r\n",(u8 *)"OK",10))) return 0;		//AT+CMGF=1//�����ı�ģ��
	//������ջ���
	memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
	SIM_rcv_len=0;
	return 1;
	
}

/*
********************************************
*@ function						: u8 RcvMsgInit(void)
*@ Description				: ���ն��ų�ʼ������Ҫ���豸��ʼ��֮��ʹ��					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/
u8 RcvMsg_Ctrl(char *RcvMsg)
{
	char *p1 = NULL;
	int MsgPos = 0;
	char SendBuf[12];
	//��ѯ�Ƿ���յ�����
	if(NULL != strstr((const char*)SIM_RX_BUF,"+CMTI: \"SM\""))
	{
		delay_ms(100);
		p1 = strstr((const char*)SIM_RX_BUF,"SM");
		MsgPos = p1[4]-'0';
		if(p1[5]>='0' && p1[5]<='9')
		{
			MsgPos = MsgPos*10+(p1[5]-'0');
		}
//		if(MsgPos>0 && MsgPos<50)
//		{
//			//�������
//			memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
//			SIM_rcv_len=0;
//			//���Ͳ�ѯ����ָ��
//			if((sim900a_send_cmd((u8*)"AT+CPMS=\"SM\"\r\n",(u8*)"OK",100))) return 0;
//			p1 = strchr((const char*)SIM_RX_BUF,':');
//			MsgPos = p1[2]-'0';
//			if(p1[3]>='0' && p1[3]<='9')
//			{
//				MsgPos = MsgPos*10+(p1[3]-'0');
//			}
//		}
		ESP8266_sprintf("msgpos:%d", MsgPos);
	}
	else 
		return 0;
	//���ϲ�ѯָ��
	memset(SendBuf,0,sizeof(SendBuf));
	sprintf(SendBuf,"AT+CMGR=%d\r\n",MsgPos);
	ESP8266_printf(SendBuf);
	//��ѯ�ö���
	if((sim900a_send_cmd((u8*)SendBuf,(u8*)"OK",100))) return 0;
	//��֤����
	if(NULL != (char*)strstr((const char*)SIM_RX_BUF,RcvMsg))
	{
			//�������
			memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
			SIM_rcv_len=0;
			//�������
			memset(SendBuf,0,sizeof(SendBuf));
			sprintf(SendBuf,"AT+CMGD=%d\r\n",MsgPos);
			if((sim900a_send_cmd((u8*)SendBuf,(u8*)"OK",100))) return 0;
		//���سɹ�ֵ
			return 1;
	}
	else 
		return 0;
}


