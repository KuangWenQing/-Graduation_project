//晶振为11.0592MHz
#include "delay.h"
	
//x=1就延时1ms
void delay_1ms(int x)
{
	int i,j;
	for(i = x; i>0; i--)
		for(j=111; j>0; j--);
}

	
//x=1就延时25us
void delay_25us(int x)
{
	int j;
	for(j=x;j>0;j--)
		_nop_();
}