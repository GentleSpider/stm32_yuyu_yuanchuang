/*
**************************************************************
* @:file				: Finger.h
* @:brief				: Finger.c���ⲿ�ӿ��ļ�
* @:version			:	1.0
* @:author			: ����
* @:date				: 2018.2.11
***************************************************************
*/
#ifndef FINGER_H_INCLUDE
#define FINGER_H_INCLUDE

#include "as608.h"
#include "usart2_v2.h"

#define FR_Cmd_Ctrl_Buf usart2_cmd_buf

//��������
extern SysPara AS608Para;//ָ��ģ��AS608����
extern u16 ValidN;//ģ������Чģ�����
extern int volatile FR_Right;


//��������
extern u16 GET_NUM(void);
extern void Add_FR(void);
extern void press_FR(char *FR_Res);//ˢָ��
extern void Del_FR(void);
extern void FR_Init(void);
extern int Get_FR_Right(void);
extern void FR_Cmd_Ctrl(void);


#endif
