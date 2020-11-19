#include "delay.h"
#include "DHT11.h"
#include "nrf24l01.h"

sbit fire = P1^1;
sbit water =P1^2;

DHT11_info dht11_data;

uchar num3sec = 0;			//3秒

void Int3Init()
{
	TMOD = 0X10;		//定时器1设置为工作方式1
	
	TH1=(65536-50000)/256;	  
	TL1=(65536-50000)%256;
		ET1 = 1;
		TR1 = 1;
    EA=1;  
}

/******************主函数*********************/
void main()
{
	uchar buf[6] = {0};
	
	buf[0] = 0x02;
	buf[5] = 0;
	
	delay_1ms(500);
	SI24R1_Init();
	SI24R1_TX_Mode_multichannel();
	Int3Init();
	while(1)
	{
		if(water)
			buf[5] &= 0xfe;
		else
			buf[5] |= 0x01;
		if(fire)
			buf[5] &= 0x7f;
		else
		{
			buf[5] |= 0x80;
			SI24R1_TxPacket(buf);
			delay_1ms(500);
		}

		if(num3sec > 60)							//3秒时间到
		{
			num3sec = 0;
			DHT11ReadInfo(&dht11_data);
			
			if(dht11_data.DHT11_CRC == dht11_data.Humidity_H + dht11_data.Humidity_L + dht11_data.Temperature_H + dht11_data.Temperature_L)
			{	
				buf[1] = dht11_data.Temperature_H;
				buf[2] = dht11_data.Temperature_L;
				buf[3] = dht11_data.Humidity_H;
				buf[4] = dht11_data.Humidity_L;
			}
			SI24R1_TxPacket(buf);
		}
	}
}

void T1_time()interrupt 3			
{
	TH1=(65536-50000)/256;	  
	TL1=(65536-50000)%256;	  
	num3sec++;
	if(num3sec > 66)							//3秒时间到
			num3sec = 0;
	
}