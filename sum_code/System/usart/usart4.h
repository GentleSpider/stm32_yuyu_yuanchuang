#ifndef USART2_V2_H_H
#define USART2_V2_H_H

#include <stm32f10x.h>

#define USART4_MAX_RCV_LEN  1024
#define USART4_MAX_CMD_LEN  256


extern void USART4_Init(u32 baud);
extern void usart4_write(USART_TypeDef* USARTx, uint8_t *Data,uint32_t len);
extern void USART4_Cmd_Clr(void);

extern volatile unsigned char  usart4_rcv_cmd_start;
extern volatile unsigned char  usart4_rcv_cmd_flag;
extern volatile unsigned char 	usart4_cmd_clr_flag;

extern unsigned char  usart4_rcv_buf[USART4_MAX_RCV_LEN];
extern volatile unsigned int   usart4_rcv_len;

extern unsigned char  usart4_cmd_buf[USART4_MAX_CMD_LEN];
extern volatile unsigned int   usart4_cmd_len;



#endif

