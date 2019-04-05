
#include <stm32f10x.h>
#include "I2C_IO.h"
#include "PN532_IIC.h"

unsigned char REG_data_Buff[50];

void Single_WritePN532_v2(unsigned char REG_Address,unsigned char REG_data);
unsigned char Single_ReadPN532_v2(void);
unsigned char Mul_ReadPN532_v2(void);
void PN532_Active(void);
void Write_Cmd(u8 *cmd, u8 len);
void Read_Data(u8 *buf, u8 len);

////单字节写入*******************************************

//void Single_WritePN532_v1(unsigned char REG_Address,unsigned char REG_data)
//{
//    i2c_Start();                  //起始信号
//    i2c_SendByte(SlaveAddress);   //发送设备地址+写信号
//    i2c_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
//    i2c_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
//    i2c_Stop();                   //发送停止信号
//}

////单字节读取*****************************************
//unsigned char Single_ReadPN532_v1(unsigned char REG_Address)
//{ 
//    unsigned char REG_data;
//    i2c_Start();                          //起始信号
//    i2c_SendByte(SlaveAddress);           //发送设备地址+写信号
//    i2c_SendByte(REG_Address);            //发送存储单元地址，从0开始        
//    i2c_Start();                          //起始信号
//    i2c_SendByte(SlaveAddress+1);         //发送设备地址+读信号
//    REG_data=i2c_ReadByte();              //读出寄存器数据
//    i2c_Ack();  
//    i2c_Stop();                           //停止信号
//    return REG_data; 
//}



/*
********************************************
*@ function						: inline void Send_Head(void)
*@ Description				: PN532发送指令的帧头					  
*@ param RData				: none
*@ return							: none
*@ version						: 1.0
********************************************
*/

//inline void Send_Head(void)
//{
//	i2c_SendByte(0x00);
//	i2c_WaitAck();
//	i2c_SendByte(0x00);
//	i2c_WaitAck();
//	i2c_SendByte(0xff);
//	i2c_WaitAck();
//}

/*
**********************************************************
*@ function						: void Write_Cmd(u8 *cmd, u8 len)
*@ Description				: 主机通过IIC对PN532写命令					  
*@ param RData				: cmd包含命令的数组，len命令的长度
*@ return							: none
*@ version						: 1.0
**********************************************************
*/
void Write_Cmd(u8 *cmd, u8 len)
{
	uint8_t checksum;
	uint8_t i=0;
	
  len++;

  i2c_Start();
	i2c_SendByte(SlaveAddress);
	i2c_WaitAck();
  checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;
  i2c_SendByte(PN532_PREAMBLE);
	i2c_WaitAck();
  i2c_SendByte(PN532_PREAMBLE);
	i2c_WaitAck();
  i2c_SendByte(PN532_STARTCODE2);
	i2c_WaitAck();

  i2c_SendByte(len);
	i2c_WaitAck();
  i2c_SendByte(~len + 1);
	i2c_WaitAck();

  i2c_SendByte(PN532_HOSTTOPN532);
	i2c_WaitAck();
	checksum += PN532_HOSTTOPN532;

  for (i=0; i<len-1; i++)
  {
    i2c_SendByte(cmd[i]);
		i2c_WaitAck();
		checksum += cmd[i];
  }

  i2c_SendByte(~checksum);
	i2c_WaitAck();
  i2c_SendByte(PN532_POSTAMBLE);
	i2c_WaitAck();

    // I2C STOP
    i2c_Stop();

}

/*
**********************************************************
*@ function						: void Read_Data(u8 *buf, u8 len)
*@ Description				: 主机读取数据					  
*@ param RData				: buf：读取的数据缓存，len数据的长度
*@ return							: none
*@ version						: 1.0
**********************************************************
*/
void Read_Data(u8 *buf, u8 len)
{
	 u8 i=0;  
   i2c_Start(); 
   i2c_SendByte(SlaveAddress+1);   //发送设备地址+写信号
	 i2c_WaitAck();
   for (i=0; i<len; i++)
   {
			buf[i] = i2c_ReadByte();
			i2c_Ack(); 
			if((len!=6)&&i==3&&(buf[0]==0)&&(buf[1]==0)&&(buf[2]==0xFF)){
            len = buf[i]+6;
      }
    }
	 i2c_Stop(); 

}

//单字节写入*******************************************

void Single_WritePN532_v2(unsigned char REG_Address,unsigned char REG_data)
{
    i2c_Start();                  //起始信号
    //i2c_SendByte(SlaveAddress);   //发送设备地址+写信号
    i2c_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
    i2c_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    i2c_Stop();                   //发送停止信号
}

//单字节读取*****************************************
unsigned char Single_ReadPN532_v2(void)
{ 
    unsigned char REG_data;
    i2c_Start();                          //起始信号
    //i2c_SendByte(SlaveAddress);           //发送设备地址+写信号
    //i2c_SendByte(REG_Address);            //发送存储单元地址，从0开始        
    //i2c_Start();                          //起始信号
    i2c_SendByte(SlaveAddress+1);         //发送设备地址+读信号
		REG_data = i2c_ReadByte();              //读出寄存器数据
    i2c_Ack();  
    i2c_Stop();                           //停止信号
    return REG_data; 
}

/*
********************************************
*@ function						: unsigned char Mul_ReadPN532_v2(unsigned char REG_Address)
*@ Description				: stm32软件I2C接收多个字节数据					  
*@ param RData				: 接收数据的地址
*@ return							: 1（OK）
*@ version						: 1.0
********************************************
*/
unsigned char Mul_ReadPN532_v2(void)
{ 
    u8 i = 0;
    i2c_Start();                          //起始信号
    //i2c_SendByte(SlaveAddress);           //发送设备地址+写信号
    //i2c_SendByte(REG_Address);            //发送存储单元地址，从0开始        
    //i2c_Start();                          //起始信号
    i2c_SendByte(SlaveAddress+1);         //发送设备地址+读信号
		for(i=0; i<20; i++)
		{
			REG_data_Buff[i]=i2c_ReadByte();              //读出寄存器数据
			i2c_Ack(); 
		}
     
    i2c_Stop();                           //停止信号
    return 1; 
}

void PN532_Active(void)
{
    i2c_Start();                  //起始信号
    //i2c_SendByte(SlaveAddress);   //发送设备地址+写信号
    i2c_SendByte(0x48);    //slave 地址 
		i2c_WaitAck();
    i2c_SendByte(0x55);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x55);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0xFF);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x03);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0xFD);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0xD4);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x14);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x01);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x17);       //发送激活指令 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //发送激活指令 
		i2c_WaitAck();
    i2c_Stop(); 
}

