#ifndef USART1_V2_H_H
#define USART1_V2_H_H

#include <stm32f10x.h>

#define USART1_MAX_RCV_LEN  1024
#define USART1_MAX_CMD_LEN  256


extern void USART1_Init(u32 baud);
extern void usart1_write(USART_TypeDef* USARTx, uint8_t *Data,uint32_t len);
extern void u1_printf(char* fmt,...);

extern volatile unsigned char  usart1_rcv_cmd_start;
extern volatile unsigned char  usart1_rcv_cmd_flag;
extern volatile u8 usart1_irpt_flag;

extern unsigned char usart1_send_buf[256];
extern unsigned char  usart1_rcv_buf[USART1_MAX_RCV_LEN];
extern volatile unsigned int   usart1_rcv_len;

extern unsigned char  usart1_cmd_buf[USART1_MAX_CMD_LEN];
extern volatile unsigned int   usart1_cmd_len;



#endif

