#include "SIM900A.h"
#include "usart1_v2.h"		
#include "delay.h"	 
#include "string.h" 
#include "math.h"
#include "stdio.h"
#include "ESP8266_WIFI.h"
//#include "STMFLASH.h"
//********************************************************************************
//无
//////////////////////////////////////////////////////////////////////////////////	

volatile u8 PhoneNum1[50] = "00310033003800390036003700360032003500330036";
volatile u8 Content1[50] = "4F6076848D3591CD726954C15DF288AB975E6CD562FF51FA";
volatile u8 PhoneNum2[50] = "00310033003800390036003600320038003600380037";
volatile u8 Content2[60] = "63077EB98BC6522B8FDE7EED51FA9519FF0C8BF76CE8610F5B895168";

u8 SIM900_CSQ[3];
u8 dtbuf[50];   								//打印缓存器	


//函数声明
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

//接收短信
u8 RcvMsgInit(void);
u8 RcvMsg_Ctrl(char *RcvMsg);


/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//usmart支持部分 
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART2_RX_STA;
//     1,清零USART2_RX_STA;
//static void sim_at_response(u8 mode)
//{
//	if(SIM_RX_STA&0X8000)		//接收到一次数据了
//	{ 
//		SIM_RX_BUF[SIM_RX_STA&0X7FFF]=0;//添加结束符
//		printf("%s",SIM_RX_BUF);	//发送到串口
//		if(mode)SIM_RX_STA=0;		
//	} 
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-SIM900A 各项测试(拨号测试、短信测试、GPRS测试)共用代码
//sim900a发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
//static u8* sim900a_check_cmd(u8 *str)
//{
//	char *strx=0;
//	if(SIM_RX_STA&0X8000)		//接收到一次数据了
//	{ 
//		SIM_RX_BUF[SIM_RX_STA&0X7FFF]=0;//添加结束符
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

//向sim900a发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
//static u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
//{
//	u8 res=0; 
//	SIM_RX_STA=0;
//	if((u32)cmd<=0XFF)
//	{
//		while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
//		USART2->DR=(u32)cmd;
//	}else u2_printf("%s\r\n",cmd);//发送命令
//	if(ack&&waittime)		//需要等待应答
//	{
//		while(--waittime)	//等待倒计时
//		{
//			delay_ms(10);
//			if(SIM_RX_STA&0X8000)//接收到期待的应答结果
//			{
//				if(sim900a_check_cmd(ack))break;//得到有效数据 
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
        if((NULL != strstr((const char *)SIM_RX_BUF, (const char*)ack)))	//判断是否有预期的结果
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
        if((NULL != strstr((const char *)SIM_RX_BUF, (const char*)ack)))	//判断是否有预期的结果
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
		if(sim900a_send_cmd((u8 *)"AT\r\n",(u8 *)"OK",100))return SIM_COMMUNTION_ERR;	//通信不上
	}		
	if(sim900a_send_cmd((u8 *)"AT+CPIN?\r\n",(u8 *)"READY",400))return SIM_CPIN_ERR;	//没有SIM卡
	if(sim900a_send_cmd((u8 *)"AT+CREG?\r\n",(u8 *)"0,1",400))
	{
		if(strstr((const char*)SIM_RX_BUF,"0,5")==NULL)
		{
			 if(!sim900a_send_cmd((u8 *)"AT+CSQ\r\n",(u8 *)"OK",200))	
			 {
					memcpy(SIM900_CSQ,SIM_RX_BUF+15,2);
			 }
			 return SIM_CREG_FAIL;	//等待附着到网络
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
			PromptMsgSendStr("GSM模块自检成功",strlen("GSM模块自检成功"));
			break;
		case SIM_COMMUNTION_ERR:
			PromptMsgSendStr("与GSM模块未通讯成功，请等待",strlen("与GSM模块未通讯成功，请等待"));
			break;
		case SIM_CPIN_ERR:
			PromptMsgSendStr("没检测到SIM卡",strlen("没检测到SIM卡"));	
			break;
		case SIM_CREG_FAIL:
			PromptMsgSendStr("注册网络中。。。",strlen("注册网络中。。。"));	
			PromptMsgSendStr("当前信号值：",strlen("当前信号值："));	PromptMsgSendStr(SIM900_CSQ,2);//PromptMsgSendStr("\r\n",2);
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
			return 1;	//通信不上
	}		
	if(sim900a_send_cmd((u8 *)"AT+CPIN?\r\n",(u8 *)"READY",200))return 2;	//没有SIM卡
	if(sim900a_send_cmd((u8 *)"AT+CREG?\r\n",(u8 *)"0,1",200))
	{
		 if(!sim900a_send_cmd((u8 *)"AT+CSQ\r\n",(u8 *)"OK",100))	
		 {
				memcpy(SIM900_CSQ,SIM_RX_BUF+15,2);
		 }
		 return 3;	//等待附着到网络
	}	
	return 0;
}

//SIM808 GPS数据解析及异常处理
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
				p2=(char*)strtok(NULL,",");	//获取是否定位标示
				//填充数据
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
				if(*p2=='1')		//定位
				{		
						sta=0;	//有效GPS数据
				}	
				else 	//未定位
				{
					if(sim900a_send_cmd((u8 *)"AT+CGNSPWR?\r\n",(u8 *)"+CGNSPWR: 1",100))//判断电源是否打开
					{
						sim900a_send_cmd((u8 *)"AT+CGNSPWR=1\r\n",(u8 *)"OK",100);
					}
					sta=1;	//无有效GPS数据
				}					
			}
		}			
	}
	else 
	{
		sprintf((char*)message,"%s","0,0,0,0");	//超时
		res=1;
		count = 5;
		while(res && count)
		{
			res=sim900a_word_test();	//重新检测
			count--;
		}
		sta=2;	//无GPS数据
	}
	return sta;
}

/*******************************************************************************
* 函数名 : Wait_CREG
* 描述   : 等待模块注册成功
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Wait_CREG(void)
{
	char *p;
	int count = 5;
	
	while(count)        			
	{        
		memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
		SIM_rcv_len=0;
		SIM_printf("AT\r\n");//查询模块网络注册状态
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
		SIM_printf("AT+CREG?\r\n");//查询模块网络注册状态
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
* 函数名 : Set_Text_Mode
* 描述   : 设置短信为TEXT文本模式
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
int Set_Text_Mode(void)
{
	int stat = 1;
	sim900a_send_cmd("ATE0\r\n","OK",30);										  //取消回显	
//	Second_AT_Command("AT+CMGF=0","OK",3);								//设置PDU模式	
//	Second_AT_Command("AT+CPMS=\"SM\",\"SM\",\"SM\"","OK",3);//所有操作都在SIM卡中进行
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
* 函数名 : Send_Text_Sms
* 描述   : 发送PDU文本短信
* 输入   : 
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/
void Send_Text_Sms(unsigned char *number,unsigned char *Msg)
{
	int count = 5;
	unsigned char  Send_Buf[70];
	memset(Send_Buf,0,70);//清空Send_Buf
	strcpy((char *)Send_Buf,"AT+CMGS=\"");//将要发送的电话号码以及相关格式复制给Send_Buf
	strcat((char *)Send_Buf,(char *)number);
	strcat((char *)Send_Buf,"\"\r\n");
	SIM_write(Send_Buf,strlen((const char*)Send_Buf)); //发送数据长度：27（具体的计算方法看串口调试比较）接收到“>”才发送短信内容
	while(count)
	{
		if(NULL != strchr((const char *)SIM_RX_BUF, '>'))
			break;
		delay_ms(500);
		count--;
	}
	SIM_printf((char *)Msg);     //发送短信内容
	delay_ms(500);
	sim900a_send_char(0X1A);          //发送结束符
}

/*
********************************************
*@ function			: unsigned char SendSinaMsg()
*@ Description		: 发送信息					  
*@ param 			: NONE
*@ return			: 成功返回1
********************************************
*/
unsigned char SendSinaMsg(unsigned char *PhoneNumber,unsigned char *Msg)
{
	ESP8266_printf("Enter MSG");
	Wait_CREG();    //查询等待模块注册成功
	ESP8266_printf("CREG Finish");
	if(!Set_Text_Mode()) ESP8266_printf("Mode Wrong");;//设置短信为ucs2模式
	ESP8266_printf("Mode Finish");
	Send_Text_Sms(PhoneNumber,Msg);//发送一条短消息
	ESP8266_printf("Send Finish");
	//重新设置为编码，将UCS2编码转化为IRA
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
*@ Description		: 指令控制					  
*@ param 					: NONE
*@ return					: none
********************************************
*/
void SIM_Ctrl(void)
{
		if(NULL != strstr((const char*)SIM_Ctrl_Buf,"_MSG"))
		{
			SendSinaMsg((unsigned char*)PhoneNum1,(unsigned char*)Content1);
			ESP8266_printf("短信发送完毕");
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
*@ Description				: 接收短信初始化，需要在设备初始化之后使用					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/
u8 RcvMsgInit(void)
{
	if((sim900a_send_cmd((u8 *)"AT\r\n",(u8 *)"OK",10))) return 0;
	if((sim900a_send_cmd((u8 *)"ATE0\r\n",(u8 *)"OK",10))) return 0;
	if((sim900a_send_cmd((u8 *)"AT+CNMI=2,1\r\n",(u8 *)"OK",10))) return 0;  //AT+CNMI=2,1//设置非请求结果码缓存方式发送
	if((sim900a_send_cmd((u8 *)"AT+CMGF=1\r\n",(u8 *)"OK",10))) return 0;		//AT+CMGF=1//进入文本模块
	//清理接收缓存
	memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
	SIM_rcv_len=0;
	return 1;
	
}

/*
********************************************
*@ function						: u8 RcvMsgInit(void)
*@ Description				: 接收短信初始化，需要在设备初始化之后使用					  
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
	//查询是否接收到短信
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
//			//清除缓存
//			memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
//			SIM_rcv_len=0;
//			//发送查询短信指令
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
	//整合查询指令
	memset(SendBuf,0,sizeof(SendBuf));
	sprintf(SendBuf,"AT+CMGR=%d\r\n",MsgPos);
	ESP8266_printf(SendBuf);
	//查询该短信
	if((sim900a_send_cmd((u8*)SendBuf,(u8*)"OK",100))) return 0;
	//验证短信
	if(NULL != (char*)strstr((const char*)SIM_RX_BUF,RcvMsg))
	{
			//清除缓存
			memset(SIM_RX_BUF,0,sizeof(SIM_RX_BUF));
			SIM_rcv_len=0;
			//清理短信
			memset(SendBuf,0,sizeof(SendBuf));
			sprintf(SendBuf,"AT+CMGD=%d\r\n",MsgPos);
			if((sim900a_send_cmd((u8*)SendBuf,(u8*)"OK",100))) return 0;
		//返回成功值
			return 1;
	}
	else 
		return 0;
}


