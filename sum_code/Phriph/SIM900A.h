#ifndef __SIM900A_H__
#define __SIM900A_H__	 
#include "sys.h"
#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
#include "usart2_v2.h"
#include "usart1_v2.h"

//call
#define SIM_OK 0
#define SIM_COMMUNTION_ERR 0xff
#define SIM_CPIN_ERR 0xfe
#define SIM_CREG_FAIL 0xfd
#define SIM_MAKE_CALL_ERR 0Xfc

//message
#define SIM_CMGF_ERR 0xfa
#define SIM_CSCS_ERR 0xf9
#define SIM_CSCA_ERR 0xf8
#define SIM_CSMP_ERR 0Xf7
#define SIM_CMGS_ERR       0xf6
#define SIM_CMGS_SEND_FAIL       0xf5

// use define
#define SIM_RX_STA USART1_RX_STA
#define SIM_RX_BUF usart1_rcv_buf
#define SIM_rcv_len usart1_rcv_len
#define SIM_printf u1_printf
#define SIM_MAX_RCV_LEN USART1_MAX_RCV_LEN
#define SIM_Ctrl_Buf usart2_cmd_buf

extern volatile u8 PhoneNum1[50];
extern volatile u8 Content1[50];
extern volatile u8 PhoneNum2[50];
extern volatile u8 Content2[60];


extern u8 SIM900_CSQ[3];
extern u8 GSM_Dect(void);


extern u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
extern u8 sim900a_work_test(void);
extern u8 GSM_Dect(void);
extern u8 SIM_MAKE_CALL(u8 *number);
extern u8 sim900a_send_chmessage_zc(u8*number,u8*content);
extern u8 sim900a_word_test(void);


extern void Wait_CREG(void);
extern int Set_Text_Mode(void);
extern void Send_Text_Sms(unsigned char *number,unsigned char *Msg);
extern unsigned char SendSinaMsg(unsigned char *PhoneNumber,unsigned char *Msg);

//Ω” ’∂Ã–≈
extern u8 RcvMsgInit(void);
extern u8 RcvMsg_Ctrl(char *RcvMsg);

//GPS
extern u8 sim808_open_gps(void);
extern u8 SIM808_GPS_HANDLE(u8 * message);

extern void SIM_Ctrl(void);



#endif





