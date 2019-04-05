


/**************************��������*******************/
unsigned char Dec2BCD(int Dec);
int BCD2Dec(unsigned char BCD);
unsigned char BCD2char(unsigned char BCD,unsigned char *array);
unsigned char Bit2char(unsigned char BCD,unsigned char *array);
/*
********************************************
*@ function			: Dec2BCD(int Dec)
*@ Description		: ��һ��ʮ������ת��ΪBCD��					  
*@ param 			: Dec ת����ʮ������
*@ return			: ����ת�����BCD��
********************************************
*/
unsigned char Dec2BCD(int Dec)
{
	 return ((Dec/10)<<4)+((Dec%10)&0x0f);
}
/*
********************************************
*@ function			: BCD2Dec(unsigned char BCD)
*@ Description		: ��һ��BCD��ת��Ϊʮ������					  
*@ param 			: BCD ��Ҫת����BCD��
*@ return			: �õ���ʮ������
********************************************
*/
int BCD2Dec(unsigned char BCD)
{
	 return ((BCD>>4)&0x0f)*10+(BCD&0x0f);
}

/*
********************************************
*@ function			: BCD2char(unsigned char BCD)
*@ Description		: ��һ��BCD��ת��Ϊ��λ�ַ���				  
*@ param 			: BCD ��Ҫת����BCD��,x,y��ʾ������
*@ return			: �ɹ�����1
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
*@ Description		: ��һ��BCD��ת��Ϊ��λ�ַ���				  
*@ param 			: BCD ��Ҫת����BCD��,x,y��ʾ������
*@ return			: �ɹ�����1
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


