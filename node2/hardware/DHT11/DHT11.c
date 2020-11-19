#include "DHT11.h"

static unsigned char DHT11ReadByte(void)
{
	unsigned char count=0 ;
	unsigned char i, datCode;
 
	for(i=0;i<8;i++)	 	
	{	
	    datCode <<= 1;
		count=0;
  		while( (!DHT11_DATA_BIT) && (count<250))	//等待50us低电平结束		
			count++;   		
		delay_25us(2);				//如果50us后
		if (DHT11_DATA_BIT)			//还是高电平，说明数据是1
		{	
			datCode |= 0x01;
			count=0;
			while((DHT11_DATA_BIT) &&(count<250))
				count++;
		}	
	}
	return datCode;	
}

void DHT11ReadInfo(DHT11_info *temp)
{
	unsigned char count;
	
	DHT11_DATA_BIT = 0;
	delay_1ms(19);				//拉低至少18ms
	
	DHT11_DATA_BIT = 1;
	delay_25us(1);				//拉高20-40us
	DHT11_DATA_BIT = 1;
	
	if(!DHT11_DATA_BIT)			//从机响应拉低
	{							//低电平大约80us
		count = 0;
		while((!DHT11_DATA_BIT) && (count<250)) 
			count++; 					//等待拉低结束
		count = 0;				//拉低结束后，将持续高电平约80us
		
		while((DHT11_DATA_BIT) && (count<250)) 
			count++; 					//等待高电平结束
		
		//开始传输数据，每bit数据都是以50us低电平开始，高电平结束，
		//判断高电平持续的时间是否大于28us，大则该bit为1
		temp->Humidity_H 	= DHT11ReadByte() ; //Humidity_H
		temp->Humidity_L 	= DHT11ReadByte() ; //Humidity_L
		temp->Temperature_H = DHT11ReadByte() ; //Temperature_H
		temp->Temperature_L = DHT11ReadByte() ; //Temperature_L
		temp->DHT11_CRC 	= DHT11ReadByte() ; //DHT11_CRC
		
		
		DHT11_DATA_BIT = 1;	
	}
}





