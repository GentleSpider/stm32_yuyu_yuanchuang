#ifndef LOCK_APP_H
#define LOCK_APP_H
#include "usart2_v2.h"


#define Lock_Cmd_Buf usart2_cmd_buf

extern volatile unsigned char Lock_Cmd[20];

extern void Lock_Ctl(void);
extern void Lock_Cmd_Ctrl(void);


#endif


