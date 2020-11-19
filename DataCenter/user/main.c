#include"lcd12864C.h"
#include"ds1302.h"
#include "nrf24l01.h"
#include "delay.h"

//-----------注意编码要转成ASCII编码--------------//
uchar code table1[] = {"一"};
uchar code table2[] = {"二"};
uchar code table3[] = {"三\xfd"};					//keil的bug，编译器会忽略0xfd值的字符串
uchar code table4[] = {"四"};
uchar code table5[] = {"五"};
uchar code table6[] = {"六"};
uchar code table7[] = {"天"};

uchar code node1[] = {"Ⅰ"};
uchar code node2[] = {"Ⅱ"};
uchar code node3[] = {"Ⅲ"};

uchar code temperature[] = {"℃"};
uchar code humidity[] = {"%RH"};
uchar code rain[] = {"雨"};
uchar code dosenot_receive[] = {"未上传数\xfd据"};
uchar code do_receive[] = "    全都收到    ";


uchar buf[6] = {0,0,0,0,0,0};
uchar buf1[6] = {0,0,0,0,0,0};
uchar buf2[6] = {0,0,0,0,0,0};
uchar buf3[6] = {0,0,0,0,0,0};

sbit key8 = P2^6;
sbit key7 = P2^7;
sbit key6 = P2^4;
sbit key5 = P2^5;
sbit key4 = P2^1;
sbit key3 = P2^3;
sbit key2 = P2^0;
sbit key1 = P2^2;

sbit speak = P1^0;

uchar temp_sec = 0;

uchar num5sec = 0;
char flag = 0;


void Int023Init()
{
	TMOD = 0X10;		
	TH1=(65536-50000)/256;	  
	TL1=(65536-50000)%256;
		ET1 = 1;
		TR1 = 1;
        //设置INT0
				IT0=1;//跳变沿触发方式（下降沿）
        EX0=1;//打开外部中断0  
	
        //设置INT1
        IT1=1;//跳变沿触发方式（下降沿）
        EX1=1;//打开外部中断1    
        EA=1;//打开总中断      
}

void displayDHT()
{
	LCD12864_SetWindow(0,2);
	LCD12864_DisplayString(temperature);
	LCD12864_SetWindow(0,4);
	LCD12864_DisplayString(humidity);
	LCD12864_SetWindow(0,7);
	LCD12864_DisplayString(rain);

		LCD12864_SetWindow(1,0);
		LCD12864_DisplayString(node1);
		LCD12864_WriteData('0' + buf1[1]/10);
		LCD12864_WriteData('0' + buf1[1]%10);
		LCD12864_WriteData('.');
		LCD12864_WriteData('0' + buf1[2]%10);		//温度只显示1位小数					
		LCD12864_SetWindow(1,4);
		LCD12864_WriteData('0' + buf1[3]/10);
		LCD12864_WriteData('0' + buf1[3]%10);
		LCD12864_WriteData('.');
		LCD12864_WriteData('0' + buf1[4]%10);		//湿度只显示1位小数	
		LCD12864_SetWindow(1,7);
		if((buf1[5] && 0x01) == 1)	
				LCD12864_WriteData('Y');
		else
				LCD12864_WriteData('N');
	
	
		LCD12864_SetWindow(2,0);
		LCD12864_DisplayString(node2);
		LCD12864_WriteData('0' + buf2[1]/10);
		LCD12864_WriteData('0' + buf2[1]%10);
		LCD12864_WriteData('.');
		LCD12864_WriteData('0' + buf2[2]%10);		//温度只显示1位小数					
		LCD12864_SetWindow(2,4);
		LCD12864_WriteData('0' + buf2[3]/10);
		LCD12864_WriteData('0' + buf2[3]%10);
		LCD12864_WriteData('.');
		LCD12864_WriteData('0' + buf2[4]%10);		//湿度只显示1位小数	
		LCD12864_SetWindow(2,7);
		if((buf2[5] && 0x01) == 1)	
				LCD12864_WriteData('Y');
		else
				LCD12864_WriteData('N');
	
		LCD12864_SetWindow(3,0);
		LCD12864_DisplayString(node3);
		LCD12864_WriteData('0' + buf3[1]/10);
		LCD12864_WriteData('0' + buf3[1]%10);
		LCD12864_WriteData('.');
		LCD12864_WriteData('0' + buf3[2]%10);		//温度只显示1位小数					
		LCD12864_SetWindow(3,4);
		LCD12864_WriteData('0' + buf3[3]/10);
		LCD12864_WriteData('0' + buf3[3]%10);
		LCD12864_WriteData('.');
		LCD12864_WriteData('0' + buf3[4]%10);		//湿度只显示1位小数	
		LCD12864_SetWindow(3,7);
		if((buf3[5] && 0x01) == 1)	
				LCD12864_WriteData('Y');
		else
				LCD12864_WriteData('N');
		delay_1ms(50);
}

//检测是否上传数据
void check_upload_data()
{
		LCD12864_SetWindow(3,0);
			
		if(buf1[0] != 1)
				LCD12864_DisplayString(node1);
		else
		{LCD12864_WriteData(' '); LCD12864_WriteData(' ');}
		
		if(buf2[0] != 2)
				LCD12864_DisplayString(node2);
		else
		{LCD12864_WriteData(' '); LCD12864_WriteData(' ');}
		
		if(buf3[0] != 3)
				LCD12864_DisplayString(node3);
		else
		{LCD12864_WriteData(' '); LCD12864_WriteData(' ');}
		
		if((buf1[0] != 1) || (buf2[0] != 2) || (buf3[0] != 3))
		{
				LCD12864_SetWindow(3,3);
				LCD12864_DisplayString(dosenot_receive);
		}
		else
		{
				LCD12864_SetWindow(3,0);
				LCD12864_DisplayString(do_receive);
		}
		
		if((buf1[5] && buf2[5] && buf3[5] && 0x01) == 1)//三个模块都有雨，则显示下雨
		{
				LCD12864_SetWindow(1,0);
				LCD12864_DisplayString(rain);
		}
		else
		{
				LCD12864_SetWindow(1,0);
				LCD12864_WriteData(' ');
				LCD12864_WriteData(' ');
		}
				
		LCD12864_SetWindow(1,5);
		LCD12864_WriteData(' ');
		LCD12864_WriteData(' ');
		LCD12864_WriteData(' ');
		LCD12864_WriteData(' ');
		LCD12864_WriteData(' ');
		LCD12864_WriteData(' ');
	
		buf1[0] = 0;
		buf2[0] = 0;
		buf3[0] = 0;
}


//时钟
void LcdDisplay()
{
	char i;
	
	if(temp_sec == TIME[0])
		return;
	
	else
	{
	//设置显示位置在第3行第3个字处
		LCD12864_WriteCmd(0x8a);
		if(TIME[2]/16)
			LCD12864_WriteData('0'+TIME[2]/16);				//时
		else
			LCD12864_WriteData(' ');
		LCD12864_WriteData('0'+(TIME[2]&0x0f));		
		
		LCD12864_SetWindow(2,4);
		if(TIME[1]/16)
			LCD12864_WriteData('0'+TIME[1]/16);				//分
		else
			LCD12864_WriteData(' ');
		LCD12864_WriteData('0'+(TIME[1]&0x0f));	
		
		LCD12864_SetWindow(2,6);
		if(TIME[0]/16)
			LCD12864_WriteData('0'+TIME[0]/16);				//秒
		else
			LCD12864_WriteData(' ');
		LCD12864_WriteData('0'+(TIME[0]&0x0f));
		
		temp_sec = TIME[0];
	
	//设置显示位置在第1行第3个字处
		LCD12864_WriteCmd(0x82);			
		LCD12864_WriteData('0'+TIME[6]/16);			//年
		LCD12864_WriteData('0'+(TIME[6]&0x0f));
		LCD12864_SetWindow(0,4);
		LCD12864_WriteData('0'+TIME[4]/16);			//月	
		LCD12864_WriteData('0'+(TIME[4]&0x0f));
		LCD12864_SetWindow(0,6);
		LCD12864_WriteData('0'+TIME[3]/16);			//日
		LCD12864_WriteData('0'+(TIME[3]&0x0f));	

		LCD12864_SetWindow(1,4);
		i = 0;
		switch(TIME[5]&0x07)									//星期	
		{
			case 1:while(table1[i++] != '\0')				//table先将i的值取走，table[i]比较，然后i++
							LCD12864_WriteData(table1[i-1]);		//此时i已经+1，所以i-1
						break;
			case 2:while(table2[i] != '\0')						//了解了i++和++i
							LCD12864_WriteData(table2[i++]);	//代码优化了
						break;
			case 3:while(table3[i] != '\0')
							LCD12864_WriteData(table3[i++]);
						break;
			case 4:while(table4[i] != '\0')
							LCD12864_WriteData(table4[i++]);
						break;
			case 5:while(table5[i] != '\0')
							LCD12864_WriteData(table5[i++]);
						break;
			
			case 6:while(table6[i] != '\0')
							LCD12864_WriteData(table6[i++]);
						break;
			case 7:while(table7[i] != '\0')
							LCD12864_WriteData(table7[i++]);
						break;
		}	
	}	
}

//按键
void key_change_time()
{
	char i = 0, address = 0, val  = 0,temp = 0;
	LCD12864_SetWindow(0,2);
	LCD12864_WriteCmd(0x0f);  //显示开，开光标
	while(1)
	{
		while(!key1)			//改变值
		{
			speak = 1;
			i = 1;
		}
		if(i == 1)
		{		
			temp++;
			val++;
			if(temp / 10)			//由于ds1302采用的BCD码
				if((temp % 10) == 0)
					val = val + 6;
		
			if(address == 0)					//地址在年
			{	
				if(temp > 99)						//超过2099年
				{val = 0;	temp = 0;}
				TIME[6] = val;				
				LCD12864_WriteData('0'+TIME[6]/16);
				LCD12864_WriteData('0'+(TIME[6]&0x0f));		
				LCD12864_SetWindow(0,2);
			}
			else if(address == 1)		//地址在月
			{	
				if(temp > 12)					//超过12月
				{val = 1;	temp = 1;}
				TIME[4] = val;						
				LCD12864_WriteData('0'+TIME[4]/16);
				LCD12864_WriteData('0'+(TIME[4]&0x0f));
				LCD12864_SetWindow(0,4);
			}
			else if(address == 2)		//地址在日
			{
				if(temp > 31)					//超过31日
					{val = 1;	temp = 1;}
				TIME[3] = val;			
				LCD12864_WriteData('0'+TIME[3]/16);
				LCD12864_WriteData('0'+(TIME[3]&0x0f));
				LCD12864_SetWindow(0,6);
			}
			else if(address == 3)			//地址在星期
			{
				if(temp > 7)
					{	val = 1;	temp = 1;}
				TIME[5] = val;				
				LCD12864_WriteData('0'+TIME[5]/16);
				LCD12864_WriteData('0'+(TIME[5]&0x0f));
				LCD12864_SetWindow(1,4);
			}
			else if(address == 4)		//地址在时
			{
					if(temp > 23)
						{	val = 0;	temp = 0;}
					TIME[2] = val;						
					LCD12864_WriteData('0'+TIME[2]/16);
					LCD12864_WriteData('0'+(TIME[2]&0x0f));
					LCD12864_SetWindow(2,2);
			}
			else if(address == 5)		//地址在分
			{
				if(temp > 59)
					{	val = 0;	temp = 0;}
				TIME[1] = val;			
				LCD12864_WriteData('0'+TIME[1]/16);
				LCD12864_WriteData('0'+(TIME[1]&0x0f));
				LCD12864_SetWindow(2,4);		
			}
			
			else if(address == 6)		//地址在秒
			{
				if(temp > 59)
					{	val = 0;	temp = 0;}
				TIME[0] = val;						
				LCD12864_WriteData('0'+TIME[0]/16);
				LCD12864_WriteData('0'+(TIME[0]&0x0f));
				LCD12864_SetWindow(2,6);
			}
			
			i = 0;
		}
		while(!key2)			//改变值
		{
			speak = 1;
			i = 2;
		}
		if(i == 2)
		{		
			temp--;
			val--;
			if((temp % 10) == 9)			//由于ds1302采用的BCD码
					val = val - 6;
		
			if(address == 0)					//地址在年
			{	
				if((temp < 0))						//小于2000年
					{	val = 0x99;	temp = 99;}
				TIME[6] = val;				
				LCD12864_WriteData('0'+TIME[6]/16);
				LCD12864_WriteData('0'+(TIME[6]&0x0f));		
				LCD12864_SetWindow(0,2);
			}
			else if(address == 1)		//地址在月
			{	
				if(temp < 1)					//小于1月
					{	val = 0x12;	temp = 12;}
				TIME[4] = val;						
				LCD12864_WriteData('0'+TIME[4]/16);
				LCD12864_WriteData('0'+(TIME[4]&0x0f));
				LCD12864_SetWindow(0,4);
			}
			else if(address == 2)		//地址在日
			{
				if(temp < 1)					//小于1日
					{	val = 0x31;	temp = 31;}
				TIME[3] = val;			
				LCD12864_WriteData('0'+TIME[3]/16);
				LCD12864_WriteData('0'+(TIME[3]&0x0f));
				LCD12864_SetWindow(0,6);
			}
			else if(address == 3)			//地址在星期
			{
				if(temp < 1)
					{	val = 0x07;	temp = 7;}
				TIME[5] = val;			
				LCD12864_WriteData('0'+TIME[5]/16);
				LCD12864_WriteData('0'+(TIME[5]&0x0f));
				LCD12864_SetWindow(1,4);
			}
			else if(address == 4)		//地址在时
			{
					if(temp < 0)
						{	val = 0x23;	temp = 23;}
					TIME[2] = val;						
					LCD12864_WriteData('0'+TIME[2]/16);
					LCD12864_WriteData('0'+(TIME[2]&0x0f));
					LCD12864_SetWindow(2,2);
			}
			else if(address == 5)		//地址在分
			{
				if(temp < 0)
					{	val = 0x59;	temp = 59;}
				TIME[1] = val;			
				LCD12864_WriteData('0'+TIME[1]/16);
				LCD12864_WriteData('0'+(TIME[1]&0x0f));
				LCD12864_SetWindow(2,4);		
			}
			
			else if(address == 6)		//地址在秒
			{
				if(temp < 0)
					{	val = 0x59;	temp = 59;}
				TIME[0] = val;						
				LCD12864_WriteData('0'+TIME[0]/16);
				LCD12864_WriteData('0'+(TIME[0]&0x0f));
				LCD12864_SetWindow(2,6);
			}	
			
			i = 0;
		}

		while(!key3)			//改变地址
		{
			speak = 1;
			i = 3;
		}
		if(i == 3)
		{
			address++;
			if(address == 0)					//地址在年
				LCD12864_SetWindow(0,2);
			else if(address == 1)		//地址在月
				LCD12864_SetWindow(0,4);
			else if(address == 2)		//地址在日
				LCD12864_SetWindow(0,6);	
			else if(address == 3)			//地址在星期
				LCD12864_SetWindow(1,4);
			else if(address == 4)		//地址在时
				LCD12864_SetWindow(2,2);
			else if(address == 5)		//地址在分
				LCD12864_SetWindow(2,4);
			else if(address == 6)		//地址在秒
				LCD12864_SetWindow(2,6);
			i = 0;
		}
		while(!key4)			//改变地址
		{
			speak = 1;
			i = 4;
		}
		if(i == 4)
		{
			if(address == 0)
				address = 7;
			address--;
			
			if(address == 0)					//地址在年
				LCD12864_SetWindow(0,2);
			else if(address == 1)		//地址在月
				LCD12864_SetWindow(0,4);
			else if(address == 2)		//地址在日
				LCD12864_SetWindow(0,6);	
			else if(address == 3)			//地址在星期
				LCD12864_SetWindow(1,4);
			else if(address == 4)		//地址在时
				LCD12864_SetWindow(2,2);
			else if(address == 5)		//地址在分
				LCD12864_SetWindow(2,4);
			else if(address == 6)		//地址在秒
				LCD12864_SetWindow(2,6);
			
			i = 0;
		}
		
		speak = 0;
		
		if(address == 7)
			break;	
	}
	
	Ds1302WriteTime();
	LCD12864_WriteCmd(0x0c);  //显示开，关光标
	flag = 1;
}


void main()
{

	flag = 1;
	delay_1ms(500);
	SI24R1_Init();
	Ds1302Init();
	LCD12864_Init();
	speak = 0;
	
	SI24R1_RX_Mode_multichannel();
	
	Int023Init();
	while(1)
	{
		if(flag == 0)
			key_change_time();
		if(flag == 1)
		{				
					Ds1302ReadTime();
					LcdDisplay();
		}
			
		if(flag == 5)
		{
			LCD12864_ClearScreen();
			displayDHT();
			delay_1ms(3500);
			LCD12864_Init();
			flag = 1;		
		}
		
		if(num5sec > 80)			//4秒检测1次节点是否上传数据
		{
			num5sec = 0;
			check_upload_data();
		}	
	}
}

void Key_5678() interrupt 0
{
	while(!key5)
	{
		speak = 1;
		flag = 0;
	}
	
	while(!key8)
	{
		speak = 1;
		flag = 5;
	}
	
	while(!key6)
	{
		speak = 1;
		buf1[5] = 0;
		buf2[5] = 0;
		buf3[5] = 0;
	}
	speak = 0;

}

void ReceivedData() interrupt 2
{
		SI24R1_RxPacket(buf);
		speak = 0;
		if(buf[0] == 1)
		{
				buf1[0] = buf[0];
				buf1[1] = buf[1];
				buf1[2] = buf[2];
				buf1[3] = buf[3];
				buf1[4] = buf[4];
				buf1[5] = buf[5];
				if(buf1[5] > 1)
				{
					speak = 1;
					LCD12864_SetWindow(1,5);
					LCD12864_DisplayString(node1);
				}
		}
		if(buf[0] == 2)
		{
				buf2[0] = buf[0];
				buf2[1] = buf[1];
				buf2[2] = buf[2];
				buf2[3] = buf[3];
				buf2[4] = buf[4];
				buf2[5] = buf[5];	
				if(buf2[5] > 1)
				{
					speak = 1;
					LCD12864_SetWindow(1,6);
					LCD12864_DisplayString(node2);
				}
		}
		if(buf[0] == 3)
		{
				buf3[0] = buf[0];
				buf3[1] = buf[1];
				buf3[2] = buf[2];
				buf3[3] = buf[3];
				buf3[4] = buf[4];
				buf3[5] = buf[5];
				if(buf3[5] > 1)
				{
					speak = 1;
					LCD12864_SetWindow(1,7);
					LCD12864_DisplayString(node3);
				}
		}
		buf[0] = 0;
}

void T1_time()interrupt 3			
{
	TH1=(65536-50000)/256;	  
	TL1=(65536-50000)%256;	  
	num5sec++;
	if(num5sec > 103)
			num5sec = 0;
}