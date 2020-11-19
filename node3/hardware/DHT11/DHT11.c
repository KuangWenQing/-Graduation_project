#include "DHT11.h"

static unsigned char DHT11ReadByte(void)
{
	unsigned char count=0 ;
	unsigned char i, datCode;
 
	for(i=0;i<8;i++)	 	
	{	
	    datCode <<= 1;
		count=0;
  		while( (!DHT11_DATA_BIT) && (count<250))	//�ȴ�50us�͵�ƽ����		
			count++;   		
		delay_25us(2);				//���50us��
		if (DHT11_DATA_BIT)			//���Ǹߵ�ƽ��˵��������1
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
	delay_1ms(19);				//��������18ms
	
	DHT11_DATA_BIT = 1;
	delay_25us(1);				//����20-40us
	DHT11_DATA_BIT = 1;
	
	if(!DHT11_DATA_BIT)			//�ӻ���Ӧ����
	{							//�͵�ƽ��Լ80us
		count = 0;
		while((!DHT11_DATA_BIT) && (count<250)) 
			count++; 					//�ȴ����ͽ���
		count = 0;				//���ͽ����󣬽������ߵ�ƽԼ80us
		
		while((DHT11_DATA_BIT) && (count<250)) 
			count++; 					//�ȴ��ߵ�ƽ����
		
		//��ʼ�������ݣ�ÿbit���ݶ�����50us�͵�ƽ��ʼ���ߵ�ƽ������
		//�жϸߵ�ƽ������ʱ���Ƿ����28us�������bitΪ1
		temp->Humidity_H 	= DHT11ReadByte() ; //Humidity_H
		temp->Humidity_L 	= DHT11ReadByte() ; //Humidity_L
		temp->Temperature_H = DHT11ReadByte() ; //Temperature_H
		temp->Temperature_L = DHT11ReadByte() ; //Temperature_L
		temp->DHT11_CRC 	= DHT11ReadByte() ; //DHT11_CRC
		
		
		DHT11_DATA_BIT = 1;	
	}
}





