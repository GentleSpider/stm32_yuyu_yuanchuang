 /*
 *********************************************************
*@ file			: Float2char.c
*@ Description	: float型转字符串，便于显示。也包含int转字符串
*@ version		: 1
*@ author		: liuzhu
*@ data			: 2017.3.18
****************************************************************
*/
/******************************函数声明*************************/
unsigned char Float2char(float value , unsigned char *array);
unsigned char Int2char(int value,unsigned char *array);
unsigned char UnInt2char(unsigned int value,unsigned char *array);

/*
**************************************************************
*@ funciton			: Float2char
*@ brief			: change a float data to a string 
						in order to send data by usart easily
*@ param value		: the float data which is needed to change
*@ param array		: the array which is used to store changed data(String)
*@ return				: i (the length of changed data)
***********************************************************************
*/
/*
unsigned char Float2char(float valueture , unsigned char *array)
{
	float value = valueture;
	int IntegerPart;
	float DecimalPart;
	unsigned char i=0;
	unsigned char j=0;
	unsigned char k=0;
	char temp;
	unsigned char NegertiveFlag;
	//分离整数和小数
	if(value < 0)
	{
		NegertiveFlag = 1;
		value = (-value);
	}
	if(value > 0)
	{
		IntegerPart = (int)value;
		DecimalPart = value - IntegerPart;
	}
	if(value == 0)
	{
		IntegerPart = 0;
		DecimalPart = 0;
	}
	//处理整数部分
	if(IntegerPart == 0)
	{
		array[0] = '0';
		array[1] = '.';
		i=1;
	}
	else
	{
		while(IntegerPart > 0)
		{
			array[i] = IntegerPart % 10 + '0';
			IntegerPart /= 10;
			i++;
		}
		i--;
		//整理整数结果
		for(j=0;j<i/2;j++)
		{
			temp = array[j];
			array[j] = array[i-j];
			array[i-j] = temp;
		}
		i++;
		array[i] = '.';
	}
	
	//convert decimalpart
	i++;
	array[i++] = (int)(DecimalPart * 10) % 10 + '0';  //保留两位小数
	array[i++] = (int)(DecimalPart * 100) % 10 + '0';
	//array[i++] = (int)(DecimalPart * 1000) % 10 + '0';
	//array[i++] = (int)(DecimalPart * 10000) % 10 + '0';
	array[i] = '\0';
	//fix the result
	if(NegertiveFlag == 1)
	{
		for(k=i+1;k>0;k--)
		{
			array[k] = array[k-1];
		}
		array[0] = '-';
		i = i+1;
	}
	return i;
}*/

unsigned char Float2char(float valueture , unsigned char *array)
{
	float value = valueture;
	int IntegerPart;
	float DecimalPart;
	unsigned char i=0;
	unsigned char j=0;
	unsigned char k=0;
	char temp;
	unsigned char NegertiveFlag = 0;
	//分离整数和小数
	if(value < 0)
	{
		NegertiveFlag = 1;
		value = (-value);
	}
	if(value >= 0)
	{
		IntegerPart = (int)value;
		DecimalPart = value - IntegerPart;
	}
	
	//if(value == 0)
	//{
	//	IntegerPart = 0;
		//DecimalPart = value - IntegerPart;
	//	DecimalPart = 0;
	//}
	
	//处理整数部分
	if(IntegerPart == 0)
	{
		array[0] = '0';
		array[1] = '.';
		i=1;
	}
	else
	{
		while(IntegerPart > 0)
		{
			array[i] = IntegerPart % 10 + '0';
			IntegerPart /= 10;
			i++;
		}
		i--;
			//整理整数结果
		for(j=0;j<=i/2;j++)
		{
			temp = array[j];
			array[j] = array[i-j];
			array[i-j] = temp;
		}
		i++;
		array[i] = '.';
	}

	//convert decimalpart
	i++;
	array[i++] = (int)(DecimalPart * 10) % 10 + '0';  //保留两位小数
	array[i++] = (int)(DecimalPart * 100) % 10 + '0';
	//array[i++] = (int)(DecimalPart * 1000) % 10 + '0';
	//array[i++] = (int)(DecimalPart * 10000) % 10 + '0';
	
	//fix the result
	if(NegertiveFlag == 1)
	{
		for(k=i+1;k>0;k--)
		{
			array[k] = array[k-1];
		}
		array[0] = '-';
		i = i+1;
	}
	array[i] = '\0';
	return i;
}

/*
unsigned char Float2char(float Value,unsigned char *array) 
{
 unsigned int IntegerPart;
 float DecimalPart;
 unsigned int i = 0;
 unsigned int j = 0;
 char temp;

 //分离整数部分与小数部分：
 //整数部分保存在IntegerPart中
 //小数部分保存在DecimalPart中
 if (Value>=1)
 {
 IntegerPart = (unsigned int)Value;
 DecimalPart = Value-IntegerPart;
 }
 else 
 {
 IntegerPart = 0;
 DecimalPart = Value-IntegerPart;
 }


 //转换整数部分
 if (IntegerPart == 0)
 {
 array[0] = 0+48;
 array[1] = '.';
 i = 1;
 }
 else
 {
 while(IntegerPart>0)
 {
 array[i] = IntegerPart%10+48;
 IntegerPart = IntegerPart/10;
 i++;
 }
 i--;

 //修正转换结果的顺序
 for (j=0;j+1<=(i+1)/2;j++)
	 {
 temp = array[j];
 array[j] = array[i-j];
 array[i-j] = temp;
 }
 i++;
 array[i] = '.';
 }


 //转换小数部分，此处设置最多转换到第四位小数
 i++;
 array[i++] = (unsigned int)(DecimalPart*10)%10+48;
 array[i++] = (unsigned int)(DecimalPart*100)%10+48;
 //array[i++] = (unsigned int)(DecimalPart*1000)%10+48;
// if (5 == i)
 //array[i++] = (unsigned int)(DecimalPart*10000)%10+48;

 array[i] = '\0'; //结束符

 return i-1	;

}
*/
/*
***************************************************
*@ function			: Int2char
*@ brief			: 将整形数据转化为字符型
*@ param value 		: 需要被转化的整形数据
*@ param *array		: 存放字符的数组指针
*@ return			: 返回字节长度
*@ version			: 2.0
****************************************************
*/

unsigned char Int2char(int value,unsigned char *array)
{
	unsigned char i=0;
	unsigned char k=0;
	unsigned char Negertive;//,ZeroFlag;
	unsigned char temp,j;
	/*
	if((value>0)&&(value<10))
	{
		ZeroFlag=1;	
	}
	*/
	if(value < 0)
	{
		Negertive = 1;
		value = (-value);
	}
		
	if(value == 0)
	{
		array[0] = '0';
		array[1] = '\0';
		i = 1;
	}
	else
	{
		while(value > 0)
		{
			array[i] = value % 10 + '0';
			value /= 10;
			i++;
			/*
			if(ZeroFlag == 1)
			{
				ZeroFlag = 0;
				array[i] = '0';//array[i-1];
				//array[i-1] = '0';
				//i = i+1;
			}
			*/
		}
		i--;

	}
	//整理整数结果
	/*
	for(j=0;j<i/2;j++)
	{
		temp = array[i];
		array[j] = array[i-1-j];
		array[i-1-j] = temp;
	}
	*/
	for(j=0;j<=i/2;j++)
	{
		temp = array[j];
		array[j] = array[i-j];
		array[i-j] = temp;
	}
	i++;
	array[i] = '\0';	
	if(Negertive == 1)
	{
		for(k=i+1;k > 0;k--)
		{
			array[k]=array[k-1];
		}
		array[0] = '-';
		i=i+1;
		Negertive = 0;
	}
	return i;
}

/*
***************************************************
*@ function			: UnInt2char
*@ brief				: 将无符号整形数据转化为字符型
*@ param value 		: 需要被转化的整形数据
*@ param *array		: 存放字符的数组指针
*@ return			: 返回字节长度
****************************************************
*/

unsigned char UnInt2char(unsigned int value,unsigned char *array)
{
	unsigned char i=0;
	unsigned char j=0;
	unsigned char temp;
	if(value == 0)
	{
		array[0] = '0';
		array[1] = '\0';
		i = 1;
	}
	else
	{
		while(value > 0)
		{
			array[i] = value % 10 + '0';
			value /= 10;
			i++;
		}
		array[i] = '\0';
	}
	//整理整数结果
	for(j=0;j<i/2;j++)
	{
		temp = array[i];
		array[j] = array[i-1-j];
		array[i-1-j] = temp;
	}
	return i;
}



