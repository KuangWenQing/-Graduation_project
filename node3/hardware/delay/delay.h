#ifndef  __DELAY_H_
#define  __DELAY_H_

#include<reg52.h>
#include<intrins.h>

#define uchar unsigned char
#define uint unsigned int

//����Ϊ12MHz�պ�5us
#define delay_5us();		_nop_();_nop_();_nop_();_nop_();_nop_();

void delay_1ms(int x);
void delay_25us(int x);


#endif
