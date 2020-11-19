#ifndef __LCD12864C_H
#define __LCD12864C_H

//--包含头文件---//
#include<reg52.h>
#include<intrins.h>

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif


sbit LCD12864_SID   = P0^1;  
sbit LCD12864_SCLK  = P0^2;  
sbit LCD12864_CS  = P0^0;


//---声明全局函数--//
void LCD12864_Delay1ms(uint c);
void LCD12864_SendByte(unsigned char zdata);
void LCD12864_WriteCmd(uchar com);
void LCD12864_WriteData(uchar dat);
void LCD12864_Init();
void LCD12864_ClearScreen();
void LCD12864_SetWindow(unsigned char x,unsigned char y);
void LCD12864_DisplayString(const uchar *s) ;

void Draw_TX(uchar Yaddr,uchar Xaddr,const uchar *dp);
void LCD12864_ClearScreen_picture(void);

#endif
