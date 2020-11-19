#ifndef __DH11_H__
#define __DH11_H__

#include "delay.h"

sbit DHT11_DATA_BIT = P1^0;

typedef struct DHT11_information{
	unsigned char Humidity_H;
	unsigned char Humidity_L;
	unsigned char Temperature_H;
	unsigned char Temperature_L;
	unsigned char DHT11_CRC;
}DHT11_info;


static unsigned char DHT11ReadByte(void);
void DHT11ReadInfo(DHT11_info *temp);


#endif
