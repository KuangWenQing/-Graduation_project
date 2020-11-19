#include"lcd12864C.h"

uchar code table[] = {"����"};
uchar code table_bolang[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x09,0x00,0x00,0x90,
0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};

uchar code table_maohao[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x01,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0x80,0x01,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};


/**************************************************************
* �� �� ��         : LCD12864_Delay1ms
* ��������		   : ��ʱ1MS
* ��    ��         : c
* ��    ��         : ��
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

/****************��һ���ֽ�********************/
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

/************д����***************/
void  LCD12864_WriteCmd(unsigned char com)  
{  
      LCD12864_SendByte(0xf8);  
      LCD12864_SendByte(com & 0xf0);     
      LCD12864_SendByte((com << 4) & 0xf0);
}  

/************д����***************/   
void LCD12864_WriteData(unsigned char dat)  
{  
     
    LCD12864_SendByte(0xfa); 
    LCD12864_SendByte(dat & 0xf0);   
    LCD12864_SendByte((dat << 4) & 0xf0);
}  

void LCD12864_Init()  
{        
	//LCD12864_PSB = 0;				//����//Ӳ���Ѿ�����
	LCD12864_CS =1;
   
	LCD12864_WriteCmd(0x30);  //ѡ�����ָ�����
	LCD12864_WriteCmd(0x0c);  //��ʾ�����ع��
	LCD12864_ClearScreen();		//���LCD12864����ʾ����  
	LCD12864_ClearScreen_picture();

	
	Draw_TX(0x80,0x83,table_bolang);
	Draw_TX(0x80,0x85,table_bolang);
	
	Draw_TX(0x80,0x8b,table_maohao);
	Draw_TX(0x80,0x8d,table_maohao);
	
	LCD12864_WriteCmd(0x81);	//������ʾλ���ڵ�1�е�2���ִ�
	LCD12864_WriteData('2');
	LCD12864_WriteData('0');
	LCD12864_SetWindow(1, 2);	//������ʾλ���ڵ�2�е�3���ִ�
	LCD12864_DisplayString(table);
}    
void LCD12864_ClearScreen()  
{  
    LCD12864_WriteCmd(0x01); 
		LCD12864_Delay1ms(1);			//������һ��Ҫ��ʱ
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
�������ƣ�Draw_TX 
�� �ܣ���ʾһ��16*16��С��ͼ�� 
�� ����	Yaddr--Y��ַ 	��һ��0x80
				Xaddr--X��ַ 
dp--ָ��ͼ�����ݴ�ŵ�ַ��ָ�� 
����ֵ ���� 
********************************************/ 
void Draw_TX(uchar Yaddr,uchar Xaddr,const uchar *dp) 
{ 
	uchar j; 
	uchar k=0; 
	
	LCD12864_WriteCmd(0x34); //ʹ����չָ����رջ�ͼ��ʾ 
	
	for(j=0;j<16;j++) 
	{ 
		LCD12864_WriteCmd(Yaddr++); //Y��ַ 
		LCD12864_WriteCmd(Xaddr); //X��ַ 
		LCD12864_WriteData(dp[k++]); 
		LCD12864_WriteData(dp[k++]); 
		
	} 
	LCD12864_WriteCmd(0x36); //�򿪻�ͼ��ʾ 	
	LCD12864_WriteCmd(0x30); //�ص�����ָ�ģʽ 
	LCD12864_WriteCmd(0x0c);  //��ʾ�����ع��
} 

void LCD12864_ClearScreen_picture(void)
{
	uchar i,j;

	LCD12864_WriteCmd(0x34);	  //������չָ�

	for(i=0;i<32;i++)			  //��ΪLCD��������32������д��ʮ����
	{
		LCD12864_WriteCmd(0x80+i);		  //��д��������Y��ֵ
		LCD12864_WriteCmd(0x80);		  //��д�������X��ֵ
		for(j=0;j<32;j++)		  //��������16λ��ÿλд�������ֽڵĵ����ݣ�Ҳ
		{						  //��д��32����Ϊ��д�������ֽ�֮����������
			LCD12864_WriteData(0x00);	  //����1�����ԾͲ����ٴ�д���ַ�ˡ�
		}
	}

	LCD12864_WriteCmd(0x36);	 //0x36��չָ������򿪻�ͼ��ʾ
	LCD12864_WriteCmd(0x30);	 //�ָ�����ָ�
}
