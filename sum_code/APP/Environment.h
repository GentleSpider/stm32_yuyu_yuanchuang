#ifndef ENVIRONMENT_H_INCLUDE
#define ENVIRONMENT_H_INCLUDE


#define Evr_Cmd_Buf usart2_cmd_buf

extern void Evr_Init(void);
extern void Send_PressAndHeight(void);
extern void Send_TempAndHumidity(void);
extern void Evr_Cmd_Ctrl(void);

#endif



