
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

////���ֽ�д��*******************************************

//void Single_WritePN532_v1(unsigned char REG_Address,unsigned char REG_data)
//{
//    i2c_Start();                  //��ʼ�ź�
//    i2c_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
//    i2c_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
//    i2c_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
//    i2c_Stop();                   //����ֹͣ�ź�
//}

////���ֽڶ�ȡ*****************************************
//unsigned char Single_ReadPN532_v1(unsigned char REG_Address)
//{ 
//    unsigned char REG_data;
//    i2c_Start();                          //��ʼ�ź�
//    i2c_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
//    i2c_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ        
//    i2c_Start();                          //��ʼ�ź�
//    i2c_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
//    REG_data=i2c_ReadByte();              //�����Ĵ�������
//    i2c_Ack();  
//    i2c_Stop();                           //ֹͣ�ź�
//    return REG_data; 
//}



/*
********************************************
*@ function						: inline void Send_Head(void)
*@ Description				: PN532����ָ���֡ͷ					  
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
*@ Description				: ����ͨ��IIC��PN532д����					  
*@ param RData				: cmd������������飬len����ĳ���
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
*@ Description				: ������ȡ����					  
*@ param RData				: buf����ȡ�����ݻ��棬len���ݵĳ���
*@ return							: none
*@ version						: 1.0
**********************************************************
*/
void Read_Data(u8 *buf, u8 len)
{
	 u8 i=0;  
   i2c_Start(); 
   i2c_SendByte(SlaveAddress+1);   //�����豸��ַ+д�ź�
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

//���ֽ�д��*******************************************

void Single_WritePN532_v2(unsigned char REG_Address,unsigned char REG_data)
{
    i2c_Start();                  //��ʼ�ź�
    //i2c_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    i2c_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
    i2c_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
    i2c_Stop();                   //����ֹͣ�ź�
}

//���ֽڶ�ȡ*****************************************
unsigned char Single_ReadPN532_v2(void)
{ 
    unsigned char REG_data;
    i2c_Start();                          //��ʼ�ź�
    //i2c_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    //i2c_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ        
    //i2c_Start();                          //��ʼ�ź�
    i2c_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
		REG_data = i2c_ReadByte();              //�����Ĵ�������
    i2c_Ack();  
    i2c_Stop();                           //ֹͣ�ź�
    return REG_data; 
}

/*
********************************************
*@ function						: unsigned char Mul_ReadPN532_v2(unsigned char REG_Address)
*@ Description				: stm32���I2C���ն���ֽ�����					  
*@ param RData				: �������ݵĵ�ַ
*@ return							: 1��OK��
*@ version						: 1.0
********************************************
*/
unsigned char Mul_ReadPN532_v2(void)
{ 
    u8 i = 0;
    i2c_Start();                          //��ʼ�ź�
    //i2c_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    //i2c_SendByte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ        
    //i2c_Start();                          //��ʼ�ź�
    i2c_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
		for(i=0; i<20; i++)
		{
			REG_data_Buff[i]=i2c_ReadByte();              //�����Ĵ�������
			i2c_Ack(); 
		}
     
    i2c_Stop();                           //ֹͣ�ź�
    return 1; 
}

void PN532_Active(void)
{
    i2c_Start();                  //��ʼ�ź�
    //i2c_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    i2c_SendByte(0x48);    //slave ��ַ 
		i2c_WaitAck();
    i2c_SendByte(0x55);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x55);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0xFF);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x03);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0xFD);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0xD4);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x14);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x01);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x17);       //���ͼ���ָ�� 
		i2c_WaitAck();
		i2c_SendByte(0x00);       //���ͼ���ָ�� 
		i2c_WaitAck();
    i2c_Stop(); 
}

