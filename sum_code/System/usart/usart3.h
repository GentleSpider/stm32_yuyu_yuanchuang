#ifndef USART3_H_H
#define USART3_H_H

#include <stm32f10x.h>

#define USART3_MAX_RCV_LEN  1024
#define USART3_MAX_CMD_LEN  256


extern void USART3_Init(void);
extern void usart3_write(uint8_t *Data,uint32_t len);

extern volatile unsigned char  usart3_rcv_cmd_start;
extern volatile unsigned char  usart3_rcv_cmd_flag;

extern unsigned char  usart3_rcv_buf[USART3_MAX_RCV_LEN];
extern volatile unsigned int   usart3_rcv_len;

extern unsigned char  usart3_cmd_buf[USART3_MAX_CMD_LEN];
extern volatile unsigned int   usart3_cmd_len;



#endif

