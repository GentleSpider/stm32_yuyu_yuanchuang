#ifndef DEC2BCD_H
#define DEC2BCD_H



extern unsigned char Dec2BCD(int Dec);
extern int BCD2Dec(unsigned char BCD);
extern unsigned char BCD2char(unsigned char BCD,unsigned char *array);
extern unsigned char Bit2char(unsigned char BCD,unsigned char *array);

#endif