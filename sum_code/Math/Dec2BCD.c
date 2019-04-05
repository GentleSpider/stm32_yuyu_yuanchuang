


/**************************函数声明*******************/
unsigned char Dec2BCD(int Dec);
int BCD2Dec(unsigned char BCD);
unsigned char BCD2char(unsigned char BCD,unsigned char *array);
unsigned char Bit2char(unsigned char BCD,unsigned char *array);
/*
********************************************
*@ function			: Dec2BCD(int Dec)
*@ Description		: 将一个十进制数转化为BCD码					  
*@ param 			: Dec 转化的十进制数
*@ return			: 返回转化后的BCD码
********************************************
*/
unsigned char Dec2BCD(int Dec)
{
	 return ((Dec/10)<<4)+((Dec%10)&0x0f);
}
/*
********************************************
*@ function			: BCD2Dec(unsigned char BCD)
*@ Description		: 将一个BCD码转化为十进制数					  
*@ param 			: BCD 想要转化的BCD码
*@ return			: 得到的十进制数
********************************************
*/
int BCD2Dec(unsigned char BCD)
{
	 return ((BCD>>4)&0x0f)*10+(BCD&0x0f);
}

/*
********************************************
*@ function			: BCD2char(unsigned char BCD)
*@ Description		: 将一个BCD码转化为二位字符串				  
*@ param 			: BCD 想要转化的BCD码,x,y显示的坐标
*@ return			: 成功返回1
********************************************
*/
unsigned char BCD2char(unsigned char BCD,unsigned char *array)
{
	//unsigned char array[3];
	array[0] = ((BCD>>4)&0x0f)+'0';
	array[1] = 	(BCD&0x0f)+'0';
	array[2] = '\0';
	//LCD_P6x8Str(x,y,array);
	return 1;
}

/*
********************************************
*@ function			: Bit2char(unsigned char BCD)
*@ Description		: 将一个BCD码转化为二位字符串				  
*@ param 			: BCD 想要转化的BCD码,x,y显示的坐标
*@ return			: 成功返回1
********************************************
*/
unsigned char Bit2char(unsigned char BCD,unsigned char *array)
{
	//unsigned char array[3];
	array[0] = ((BCD>>7)&0x01)+'0';
	array[1] = ((BCD>>6)&0x01)+'0';
	array[2] = ((BCD>>5)&0x01)+'0';
	array[3] = ((BCD>>4)&0x01)+'0';
	array[4] = ((BCD>>3)&0x01)+'0';
	array[5] = ((BCD>>2)&0x01)+'0';
	array[6] = ((BCD>>1)&0x01)+'0';
	array[7] = ((BCD)&0x01)+'0';
	array[8] = '\0';
	//LCD_P6x8Str(x,y,array);
	return 1;
}


