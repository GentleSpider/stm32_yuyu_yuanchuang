#ifndef USART2_V2_H_H
#define USART2_V2_H_H

#include <stm32f10x.h>

#define USART2_MAX_RCV_LEN  1024
#define USART2_MAX_CMD_LEN  256


extern void USART2_Init(u32 baud);
extern void usart2_write(USART_TypeDef* USARTx, uint8_t *Data,uint32_t len);
extern void USART2_Cmd_Clr(void);

extern volatile unsigned char  usart2_rcv_cmd_start;
extern volatile unsigned char  usart2_rcv_cmd_flag;
extern volatile unsigned char 	usart2_cmd_clr_flag;

extern unsigned char  usart2_rcv_buf[USART2_MAX_RCV_LEN];
extern volatile unsigned int   usart2_rcv_len;

extern unsigned char  usart2_cmd_buf[USART2_MAX_CMD_LEN];
extern volatile unsigned int   usart2_cmd_len;



#endif

