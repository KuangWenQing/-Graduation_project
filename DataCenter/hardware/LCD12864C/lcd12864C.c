#include"lcd12864C.h"

uchar code table[] = {"星期"};
uchar code table_bolang[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x09,0x00,0x00,0x90,
0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};

uchar code table_maohao[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0x80,0x01,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};


/**************************************************************
* 函 数 名         : LCD12864_Delay1ms
* 函数功能		   : 延时1MS
* 输    入         : c
* 输    出         : 无
**************************************************************/

void LCD12864_Delay1ms(uint c)
{
    uchar a,b;
	for(; c>0; c--)
	{
	    for(b=199; b>0; b--)
		{
	        for(a=1; a>0; a--);
		}
	}
}

/****************发一个字节********************/
void LCD12864_SendByte(unsigned char zdata)  
{  
    unsigned char i;  
    for(i = 0;i < 8;i ++)  
    {  
        if((zdata<<i) & 0x80)  
        {  
            LCD12864_SID = 1;  
        }  
        else  
        {  
            LCD12864_SID = 0;  
        }  
        LCD12864_SCLK = 0; 
        LCD12864_SCLK = 1;    
    }     
} 

/************写命令***************/
void  LCD12864_WriteCmd(unsigned char com)  
{  
      LCD12864_SendByte(0xf8);  
      LCD12864_SendByte(com & 0xf0);     
      LCD12864_SendByte((com << 4) & 0xf0);
}  

/************写数据***************/   
void LCD12864_WriteData(unsigned char dat)  
{  
     
    LCD12864_SendByte(0xfa); 
    LCD12864_SendByte(dat & 0xf0);   
    LCD12864_SendByte((dat << 4) & 0xf0);
}  

void LCD12864_Init()  
{        
	//LCD12864_PSB = 0;				//串行//硬件已经拉低
	LCD12864_CS =1;
   
	LCD12864_WriteCmd(0x30);  //选择基本指令操作
	LCD12864_WriteCmd(0x0c);  //显示开，关光标
	LCD12864_ClearScreen();		//清除LCD12864的显示内容  
	LCD12864_ClearScreen_picture();

	
	Draw_TX(0x80,0x83,table_bolang);
	Draw_TX(0x80,0x85,table_bolang);
	
	Draw_TX(0x80,0x8b,table_maohao);
	Draw_TX(0x80,0x8d,table_maohao);
	
	LCD12864_WriteCmd(0x81);	//设置显示位置在第1行第2个字处
	LCD12864_WriteData('2');
	LCD12864_WriteData('0');
	LCD12864_SetWindow(1, 2);	//设置显示位置在第2行第3个字处
	LCD12864_DisplayString(table);
}    
void LCD12864_ClearScreen()  
{  
    LCD12864_WriteCmd(0x01); 
		LCD12864_Delay1ms(1);			//清屏后一定要延时
}  

void LCD12864_SetWindow(unsigned char x, unsigned char y)  
{  
    unsigned char pos;  
    if(x == 0)  
				x = 0x80;  
    else if(x == 1)  
				x = 0x90;  
    else if(x == 2)  
				x = 0x88;  
    else if(x == 3)  
				x = 0x98;  
    pos = x + y;  
    LCD12864_WriteCmd(pos);         
}  

void LCD12864_DisplayString(const uchar *s)  
{    
    unsigned char  i = 0;  
    while(s[i]!='\0')  
    {   
        LCD12864_WriteData(s[i]);  
        i++;  
    }  
}

/******************************************* 
函数名称：Draw_TX 
功 能：显示一个16*16大小的图形 
参 数：	Yaddr--Y地址 	第一行0x80
				Xaddr--X地址 
dp--指向图形数据存放地址的指针 
返回值 ：无 
********************************************/ 
void Draw_TX(uchar Yaddr,uchar Xaddr,const uchar *dp) 
{ 
	uchar j; 
	uchar k=0; 
	
	LCD12864_WriteCmd(0x34); //使用扩展指令集，关闭绘图显示 
	
	for(j=0;j<16;j++) 
	{ 
		LCD12864_WriteCmd(Yaddr++); //Y地址 
		LCD12864_WriteCmd(Xaddr); //X地址 
		LCD12864_WriteData(dp[k++]); 
		LCD12864_WriteData(dp[k++]); 
		
	} 
	LCD12864_WriteCmd(0x36); //打开绘图显示 	
	LCD12864_WriteCmd(0x30); //回到基本指令集模式 
	LCD12864_WriteCmd(0x0c);  //显示开，关光标
} 

void LCD12864_ClearScreen_picture(void)
{
	uchar i,j;

	LCD12864_WriteCmd(0x34);	  //开启拓展指令集

	for(i=0;i<32;i++)			  //因为LCD有纵坐标32格所以写三十二次
	{
		LCD12864_WriteCmd(0x80+i);		  //先写入纵坐标Y的值
		LCD12864_WriteCmd(0x80);		  //再写入横坐标X的值
		for(j=0;j<32;j++)		  //横坐标有16位，每位写入两个字节的的数据，也
		{						  //就写入32次以为当写入两个字节之后横坐标会自
			LCD12864_WriteData(0x00);	  //动加1，所以就不用再次写入地址了。
		}
	}

	LCD12864_WriteCmd(0x36);	 //0x36扩展指令里面打开绘图显示
	LCD12864_WriteCmd(0x30);	 //恢复基本指令集
}
