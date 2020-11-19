#include "nrf24l01.h"

/**************说明***************
1、产自Nordic的芯片叫NRF24L01，而产自南京的芯片叫Si24R1
2、两个模块的技术资料基本一样。感觉Si24R1无线模块就是NRF24L01无线模块，
同样的程序可以驱动这两个模块。而且，实现的功能也一样，
也就是说，这两个芯片的寄存器地址、内容、操作命令等基本一样。
更多内容见下面网站：
http://blog.sina.com.cn/s/blog_6fd8c5830102v9yi.html
http://www.dnsj88.com/Products/si24r1qgzd.html
**********************************/

// 定义一个静态发送地址
//u8 code ADDRESS0[ADDR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x01};  
//u8 code ADDRESS1[ADDR_WIDTH] = {0x0C,0x0C,0x0C,0x0C,0x01};
u8 code ADDRESS2[ADDR_WIDTH] = {0x0D,0x0C,0x0C,0x0C,0x01};

/*****************SPI时序函数********************
读数据时，byte写0

其实不论读写数据，该函数都会返回byte，
只是配置成写数据MISO上的电平不会变。
************************************************/
static u8 SPI_RW(u8 byte)
{
	u8 bit_ctr;
	for(bit_ctr=0; bit_ctr<8; bit_ctr++)
	{
		MOSI = (byte & 0x80);		//MCU to NRF24L01	
		byte = (byte << 1);    
		
		SCK = 1;     
		
		byte |= MISO;               //NRF24L01 to MCU     
		
		SCK = 0;                                
	}
	return(byte);                              
}


/********************************************************
函数功能：SI24R1引脚初始化                
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_Init(void)
{
	SCK = 0; 			//SPI时钟线拉低
	CSN = 1;				
	CE 	= 0;				
	IRQ = 1;
}


/********************************************************
函数功能：写寄存器的值（单字节）                
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
					value:寄存器的值
返回  值：状态寄存器的值
*********************************************************/
u8 SI24R1_Write_Reg(u8 reg, u8 value)
{
	u8 status;

	CSN = 0;                 
	status = SPI_RW(reg);				
	SPI_RW(value);
	CSN = 1;  
	
	return(status);
}


/********************************************************
函数功能：写寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
					pBuf:写数据首地址
					bytes:写数据字节数
返回  值：状态寄存器的值
*********************************************************/
u8 SI24R1_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes)
{
	u8 status,byte_ctr;

  CSN = 0;                                  			
  status = SPI_RW(reg);                          
  for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)     
    SPI_RW(*pBuf++);
  CSN = 1;                                      	

  return(status);       
}							  					   


/********************************************************
函数功能：读取寄存器的值（单字节）                  
入口参数：reg:寄存器映射地址（格式：READ_REG｜reg）
返回  值：寄存器值
*********************************************************/
u8 SI24R1_Read_Reg(u8 reg)
{
 	u8 value;

	CSN = 0;    
	SPI_RW(reg);			
	value = SPI_RW(0);
	CSN = 1;              

	return(value);
}


/********************************************************
函数功能：读取寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（READ_REG｜reg）
					pBuf:接收缓冲区的首地址
					bytes:读取字节数
返回  值：状态寄存器的值
*********************************************************/
u8 SI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes)
{
	u8 status,byte_ctr;

	CSN = 0;                                        
	status = SPI_RW(reg);                           
	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
		pBuf[byte_ctr] = SPI_RW(0);   			//读取数据，低字节在前
	CSN = 1;                                        

	return(status);    
}


/********************************************************
函数功能：SI24R1接收模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
/*void SI24R1_RX_Mode(void)
{
	CE = 0;
	SI24R1_Write_Buf(WRITE_REG + RX_ADDR_P2, ADDRESS2, ADDR_WIDTH);	// 接收设备接收通道0使用和发送设备相同的发送地址
	SI24R1_Write_Reg(WRITE_REG + EN_AA, 0x01); 			// 使能接收通道0自动应答
	SI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);      // 使能接收通道0
	SI24R1_Write_Reg(WRITE_REG + RF_CH, 40);          	// 选择射频通道0x40
	SI24R1_Write_Reg(WRITE_REG + RX_PW_P0, PLOAD_WIDTH); //接收通道0选择和发送通道相同有效数据宽度
	SI24R1_Write_Reg(WRITE_REG + RF_SETUP, 0x07);	// 数据传输率1Mbps，发射功率7dBm
	SI24R1_Write_Reg(WRITE_REG + CONFIG, 0x0f);	// CRC使能，16位CRC校验，上电，接收模式
	SI24R1_Write_Reg(WRITE_REG + STATUS, 0xff); //清除所有的中断标志位
	CE = 1;                                     // 拉高CE启动接收设备
}						
*/

/********************************************************
函数功能：SI24R1发送模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
/*void SI24R1_TX_Mode(void)
{
	CE = 0;
	SI24R1_Write_Buf(WRITE_REG + TX_ADDR, ADDRESS2, ADDR_WIDTH);     // 写入发送地址
	SI24R1_Write_Buf(WRITE_REG + RX_ADDR_P2, ADDRESS2, ADDR_WIDTH);  // 为了应答接收设备，接收通道0地址和发送地址相同

	SI24R1_Write_Reg(WRITE_REG + EN_AA, 0x01);		// 使能接收通道0自动应答
	SI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);  // 使能接收通道0
	SI24R1_Write_Reg(WRITE_REG + SETUP_RETR, 0x0a); // 自动重发延时等待250us+86us，自动重发10次
	SI24R1_Write_Reg(WRITE_REG + RF_CH, 40);        // 选择射频通道0x40
	SI24R1_Write_Reg(WRITE_REG + RF_SETUP, 0x07);   // 数据传输率1Mbps，发射功率7dBm
	SI24R1_Write_Reg(WRITE_REG + CONFIG, 0x0e);     // CRC使能，16位CRC校验，上电
	CE = 1;
}
*/
void SI24R1_TX_Mode_multichannel(void)
{
	CE = 0;
/*
	SI24R1_Write_Reg(WRITE_REG + FEATURE, 0x04); 			//特征寄存器第三位(EN_DPL)置1
	SI24R1_Write_Reg(WRITE_REG + DYNPD, 0x3f); 				//使能6个通道都是“动态负载长度”
*/
	
	SI24R1_Write_Reg(WRITE_REG + EN_AA, 0x3f);				//使能6个通道自动应答
	SI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x3f);		//开启6个通道
	SI24R1_Write_Reg(WRITE_REG + RF_CH, 40);          // 选择射频通道0x40
	SI24R1_Write_Reg(WRITE_REG + RF_SETUP, 0x07);			// 数据传输率1Mbps，发射功率7dBm
//	SI24R1_Write_Reg(WRITE_REG + SETUP_AW, 0x03);			//5个字节的地址宽度
	
	SI24R1_Write_Buf(WRITE_REG + TX_ADDR, ADDRESS2, ADDR_WIDTH);
	SI24R1_Write_Buf(WRITE_REG + RX_ADDR_P0, ADDRESS2, ADDR_WIDTH);
	
	SI24R1_Write_Reg(WRITE_REG + CONFIG, 0x0e);	// CRC使能，16位CRC校验，上电，发送模式
}

/********************************************************
函数功能：读取接收数据                       
入口参数：rxbuf:接收数据存放首地址
返回  值：0:接收到数据
          1:没有接收到数据
*********************************************************/
/*u8 SI24R1_RxPacket(u8 *rxbuf)
{
	u8 state;
	state = SI24R1_Read_Reg(STATUS);  			       	//读取状态寄存器的值    
		
	CE = 0;
	SI24R1_Write_Reg(WRITE_REG+STATUS,state);			//清除RX_DS中断标志

	if(state & RX_DR)								    //接收到数据
	{
		SI24R1_Read_Buf(RD_RX_PLOAD,rxbuf,PLOAD_WIDTH);	//读取数据
		SI24R1_Write_Reg(FLUSH_RX,0xff);				//清除RX FIFO寄存器
		CE = 1;
		return 0; 
	}	   
	CE =1;
	return 1;                                          	//没收到任何数据
}
*/

/********************************************************
函数功能：发送一个数据包                      
入口参数：txbuf:要发送的数据
返回  值：0x10:达到最大重发次数，发送失败 
          0x20:发送成功            
          0xff:发送失败                  
*********************************************************/
u8 SI24R1_TxPacket(u8 *txbuf)
{
	u8 state = 11;
	CE=0;												//CE拉低，使能SI24R1配置
  SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, PLOAD_WIDTH);	//写数据到TX FIFO,32个字节
 	CE=1;												//CE置高，使能发送	   
	
	while((IRQ) && (state--));								//等待发送完成
	state = SI24R1_Read_Reg(STATUS);  				//读取状态寄存器的值	   
	SI24R1_Write_Reg(WRITE_REG+STATUS, state); 		//清除TX_DS或MAX_RT中断标志
	if(state&MAX_RT)								//达到最大重发次数
	{
		SI24R1_Write_Reg(FLUSH_TX,0xff);			//清除TX FIFO寄存器 
		return MAX_RT; 
	}
	if(state&TX_DS)						//发送完成
	{
		return TX_DS;
	}
	return 0XFF;						//发送失败
}

