#ifndef LOCK_INCLUDE_H
#define LOCK_INCLUDE_H

#define LOCK_PORT_1 GPIOA
#define LOCK_PORT_2 GPIOA
#define LOCK_PIN_1 GPIO_Pin_11
#define LOCK_PIN_2 GPIO_Pin_12

extern volatile int Lock_Time_Delay_N;

extern unsigned char LockPinInit(void);
extern unsigned char LockOpen(void);
extern unsigned char LockClose(void);


#endif

