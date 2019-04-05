#ifndef RFID_IC_H_INCLUDE
#define RFID_IC_H_INCLUDE

#define		CARD1		"70 da 4b a8"
#define		CARD2		"f0 d8 ac 56"
#define 	CARD3		"fb e4 8e a9"

#define Item_Wallet CARD1


enum Item_Importance {Important,Common};
extern char IC_Res[25];


/*全局变量*/
extern enum Item_Importance Item_Res;
extern unsigned char CT[2];//卡类型
extern unsigned char SN[4]; //卡号
extern unsigned char RFID[16];			//存放RFID 
//extern unsigned char RFID1[16];

extern volatile u8 Item_Get_Flag;

extern int Read_IC(char *IC_Res);
extern enum Item_Importance IC_Res_Match(char * IC_Res);
#endif

