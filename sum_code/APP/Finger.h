/*
**************************************************************
* @:file				: Finger.h
* @:brief				: Finger.c的外部接口文件
* @:version			:	1.0
* @:author			: 刘铸
* @:date				: 2018.2.11
***************************************************************
*/
#ifndef FINGER_H_INCLUDE
#define FINGER_H_INCLUDE

#include "as608.h"
#include "usart2_v2.h"

#define FR_Cmd_Ctrl_Buf usart2_cmd_buf

//变量声明
extern SysPara AS608Para;//指纹模块AS608参数
extern u16 ValidN;//模块内有效模板个数
extern int volatile FR_Right;


//函数声明
extern u16 GET_NUM(void);
extern void Add_FR(void);
extern void press_FR(char *FR_Res);//刷指纹
extern void Del_FR(void);
extern void FR_Init(void);
extern int Get_FR_Right(void);
extern void FR_Cmd_Ctrl(void);


#endif
