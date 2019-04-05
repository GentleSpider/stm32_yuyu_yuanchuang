//#include <hidef.h>           /* common defines and macros */
//#include "derivative.h"      /* derivative-specific definitions */

#include <stm32f10x.h>

#define WHO_AM_I 0x0F

#define CTRL_REG1 0x20     
#define CTRL_REG2 0x21     
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

#define        SlaveAddress   0xD2          
//定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改

#define SCL PTM_PTM2 
#define SDA PTM_PTM5 

#define SCL_dir DDRM_DDRM2
#define SDA_dir DDRM_DDRM5

#define  BUS_CLOCK                   48000000           //总线频率
#define  OSC_CLOCK                   16000000           //晶振频率

#define somenop(); asm("nop"); asm("nop");/*asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");*/\
                   //asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");\
                   //asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");\
                   //asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

//asm("nop");晶振频率的倒数
char BUF[8];                         //接收数据缓存区 
unsigned char ge,shi,bai,qian,wan;           //显示变量
int dis_data1,dis_data2,dis_data3;//变量
unsigned char TxBuf[10] = {0,0,0,0,0,0,0,0,0,0};


/*************************************************************/
/*                      初始化锁相环                         */
/*************************************************************/

void INIT_PLL(void) 
{
    CLKSEL &= 0x7f;       //set OSCCLK as sysclk
    PLLCTL &= 0x8F;       //Disable PLL circuit
    CRGINT &= 0xDF;
    
    #if(BUS_CLOCK == 48000000) 
      SYNR = 0x45;
    #elif(BUS_CLOCK == 40000000) 
      SYNR = 0x44;
    #elif(BUS_CLOCK == 32000000)
      SYNR = 0x43;     
    #elif(BUS_CLOCK == 24000000)
      SYNR = 0x42;
    #endif 

    REFDV = 0x81;         //PLLCLK=2×OSCCLK×(SYNR+1)/(REFDV+1)＝64MHz ,fbus=32M
    PLLCTL =PLLCTL|0x70;  //Enable PLL circuit
    asm NOP;
    asm NOP;
    while(!(CRGFLG&0x08)); //PLLCLK is Locked already
    CLKSEL |= 0x80;        //set PLLCLK as sysclk
}

/*************************************************************/
/*                        延时函数                           */
/*************************************************************/
void delay(void) 
{
 unsigned int i;
 for(i=0;i<5000;i++);
}

/*************************************************************/
/*                        初始化IIC                          */
/*************************************************************/
void INIT_IIC(void) 
{

 SCL_dir = 1;
 SCL_dir = 1;
 SCL = 1;
 SDA = 1;
}


/*************************************************************/
/*                         启动IIC                           */
/*************************************************************/
void I2Cstart(void) 
{
  SCL_dir = 1;
  SDA_dir = 1;
  SDA = 1;
  somenop();
  SCL = 1;
  somenop();
  somenop();
  SDA = 0;
  somenop();
  somenop();
  SCL = 0;
}


/**************************************
接收应答信号
**************************************/
void L3G4200D_RecvACK()
{
    SCL_dir = 1;
    SDA_dir = 0;
 
    SCL = 1;                    //拉高时钟线
    somenop();
    somenop();                 //延时
    while(SDA==1);
    SCL = 0;                    //拉低时钟线
    somenop();
    somenop();                 //延时

}
/*************************************************************/
/*                       IIC发送数据                         */
/*************************************************************/
void I2Csend(byte data) 
{
 unsigned char i=8;
 
 SCL_dir = 1;
 SDA_dir = 1;
 
 for (i=0; i<8; i++)         //8位计数器
 {
        SDA = (data&0x80)>>7;               //送数据口
        data=data<<1;
        SCL = 1;                //拉高时钟线
       somenop();
        somenop();             //延时
        SCL = 0;                //拉低时钟线
        somenop();
        somenop();             //延时
 }
    
 L3G4200D_RecvACK();
}




/*************************************************************/
/*                       IIC接收数据                         */
/*************************************************************/
signed char I2Creceive(void) 
{ 
    byte i;
    byte dat = 0;
    
    SCL_dir = 1;
    SDA_dir = 0;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;                              //这里的1是 00000001
        SCL = 1;                //拉高时钟线
        somenop();
        somenop();             //延时
        dat |= SDA;             //读数据      SDA  00000000 or 00000001         
        SCL = 0;                //拉低时钟线
        somenop();
        somenop();             //延时
    }
    return dat;
}

/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void I2Csendack(char ack)
{
    SCL_dir = 1;
    SDA_dir = 1;
    if(ack==1) 
    {     
       SDA = 1;                  //写应答信号
       SCL = 1;                    //拉高时钟线
       somenop();
       somenop();
       SCL = 0;                    //拉低时钟线
       somenop();
       somenop();
    } 
    else
    {
       SDA = 0;                  //写应答信号
       SCL = 1;                    //拉高时钟线
       somenop();
       somenop();
       SCL = 0;                    //拉低时钟线
       somenop();
       somenop();
    }
}





/*************************************************************/
/*                         停止IIC                           */
/*************************************************************/
void I2Cstop(void) 
{
  SCL_dir = 1;
  SDA_dir = 1;
  
  SCL = 1;
  SDA = 0;
  somenop();
  somenop();
  SDA = 1; 
  somenop(); 
  somenop();
}

//单字节写入*******************************************

void Single_WriteL3G4200D(unsigned char REG_Address,unsigned char REG_data)
{
    I2Cstart();                  //起始信号
    I2Csend(SlaveAddress);   //发送设备地址+写信号
    I2Csend(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
    I2Csend(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    I2Cstop();                   //发送停止信号
}

//单字节读取*****************************************
unsigned char Single_ReadL3G4200D(unsigned char REG_Address)
{ 
    unsigned char REG_data;
    I2Cstart();                          //起始信号
    I2Csend(SlaveAddress);           //发送设备地址+写信号
    I2Csend(REG_Address);            //发送存储单元地址，从0开始        
    I2Cstart();                          //起始信号
    I2Csend(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=I2Creceive();              //读出寄存器数据
    I2Csendack(1);  
    I2Cstop();                           //停止信号
    return REG_data; 
}
//*************************************************



//*****************************************************************

//初始化L3G4200D，根据需要请参考pdf，第27页，进行修改************************
void InitL3G4200D()
{
   Single_WriteL3G4200D(CTRL_REG1, 0x0f);   //  0x0f=00001111  普通模式   X Y Z 启用。
   Single_WriteL3G4200D(CTRL_REG2, 0x00);   //  选择高通滤波模式和高通截止频率  此为普通模式
   Single_WriteL3G4200D(CTRL_REG3, 0x08);   //  0x08=0000 1000    DRDY/INT2 数据准备(0: Disable; 1: Enable)默认0 
   Single_WriteL3G4200D(CTRL_REG4, 0x00 );  //  选择量程    满量程选择（默认 00）（00：250dps)
   Single_WriteL3G4200D(CTRL_REG5, 0x00);   //   篎FIFO使能，高通滤波使
}



void SCI_Init() 
{
  SCI1BD = BUS_CLOCK/16/9600;   //设置SCI1波特率为9600
  SCI1CR1 = 0x00;       //设置SCI1为正常模式，八位数据位，无奇偶校验
  SCI1CR2 = 0x0c;       //允许发送和接收数据，禁止中断功能*/
        MODRR|=0X80;
  
}
void SCI_send(unsigned char data) 
{
  while(!SCI1SR1_TDRE);       //等待发送数据寄存器（缓冲器）为空
  SCI1DRL = data;
}



/*************************************************************/
/*                  串口发送字符串函数                       */
/*************************************************************/
void send_string(unsigned char *putchar) 
{
  while(*putchar!=0x00)       //判断字符串是否发送完毕
  {
   
   SCI_send(*putchar++);  
  }
}

unsigned char SCI_receive()
{
        unsigned char a;
        while(!SCI1SR1_RDRF);
        a=SCI1DRL;
        return a;
}



void SCI_sendstring(unsigned char *a,unsigned char stringlen) 
{ 
    unsigned char i;
  for(i=0;i<=stringlen;i++)
        SCI_send(*(a++));
}

unsigned int CRC_Check(unsigned char *Buf, unsigned char crc_count)
{
    unsigned short CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;

    for (i=0;i<crc_count; i++)
    {      
        CRC_Temp ^= Buf[i];
        for(j=0;j<8;j++) 
        {
            if (CRC_Temp & 0x01)
            CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
            CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}

void WirelessSerial(int a,int b,int c,int d)
{
  int num;
  unsigned char *data;
  unsigned char nrf_flag = 0,error;
  int *pdat;

#if 1
  num = (int)a;    //Speed
  data = (unsigned char *)&num;
  TxBuf[1] = *data;
  data++;
  TxBuf[0] = *data;
       
  num = (int)b;      //MotorPWM
  data = (unsigned char *)&num;
  TxBuf[3] = *data;
  data++;
  TxBuf[2] = *data;

  num = (int)c;        //angle
  data = (unsigned char *)&num;
  TxBuf[5] = *data;
  data++;
  TxBuf[4] = *data;
  
  num = (int)d;
  data = (unsigned char *)&num;
  TxBuf[7] = *data;
  data++;
  TxBuf[6] = *data;
#endif

  num = CRC_Check(TxBuf,8);
  TxBuf[8] = num & 0xFF;
  TxBuf[9] = num >> 8;

  SCI_sendstring(TxBuf,9);
}

void main(void) 
{
  DisableInterrupts;
   
  INIT_PLL();
  
  INIT_IIC();
  
  SCI_Init();
  
  InitL3G4200D();
  
  EnableInterrupts; 

  for(;;)
  {
      
      BUF[0]= Single_ReadL3G4200D(OUT_X_L);
      BUF[1]= Single_ReadL3G4200D(OUT_X_H);
      dis_data1=(BUF[1]<<8)+BUF[0]; 
  
      
      BUF[2]= Single_ReadL3G4200D(OUT_Y_L);
      BUF[3]= Single_ReadL3G4200D(OUT_Y_H);
      dis_data2=(BUF[3]<<8)+BUF[2]; 
      
      
      BUF[4]= Single_ReadL3G4200D(OUT_Z_L);
      BUF[5]= Single_ReadL3G4200D(OUT_Z_H);
      dis_data3=(BUF[5]<<8)+BUF[4]; 
      
            
      WirelessSerial(dis_data1,0,0,0);
      
      
     
      
      
  }
}
