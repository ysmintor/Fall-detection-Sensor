#include "IIC.h"


#define SCL_set  GPIO_SetBits(GPIOB , GPIO_Pin_6)    //   GPIOE->BSRR = GPIO_Pin_8 /* GPIO_SetBits(GPIOB , GPIO_Pin_6)   */
#define SCL_reset  GPIO_ResetBits(GPIOB , GPIO_Pin_6) /* GPIO_ResetBits(GPIOB , GPIO_Pin_10) */

#define SDA_set  GPIO_SetBits(GPIOB , GPIO_Pin_7)//   GPIOE->BSRR = GPIO_Pin_9 /* GPIO_SetBits(GPIOB , GPIO_Pin_11)   */
#define SDA_reset  GPIO_ResetBits(GPIOB , GPIO_Pin_7) /* GPIO_ResetBits(GPIOB , GPIO_Pin_11) */

//#define SCL_read      GPIOE->IDR  & GPIO_Pin_8 /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_10) */
#define SDA_read  GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_7)//   GPIOE->IDR  & GPIO_Pin_9 /* GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_11) */

static void I2C_delay(void)
{
    volatile int i = 7;
    while (i)
        i--;
}
 static u8 I2C_Start(void)
{
	SDA_set;
	SCL_set;
	I2C_delay();
	if (!SDA_read)
	return false;
	SDA_reset;
	I2C_delay();
	if (SDA_read)
	return false;
	SDA_reset;
	I2C_delay();
	return true;
}

static void I2C_Stop(void)
{
	SCL_reset;
	I2C_delay();
	SDA_reset;
	I2C_delay();
	SCL_set;
	I2C_delay();
	SDA_set;
	I2C_delay();
}

static void I2C_Ack(void)
{
	SCL_reset;
	I2C_delay();
	SDA_reset;
	I2C_delay();
	SCL_set;
	I2C_delay();
	SCL_reset;
	I2C_delay();
}

static void I2C_NoAck(void)
{
	SCL_reset;
	I2C_delay();
	SDA_set;
	I2C_delay();
	SCL_set;
	I2C_delay();
	SCL_reset;
	I2C_delay();
}

static u8 I2C_WaitAck(void)
{
	SCL_reset;
	I2C_delay();
	SDA_set;
	I2C_delay();
	SCL_set;
	I2C_delay();
	if (SDA_read) {
		SCL_reset;
		return false;
	}
	SCL_reset;
	return true;
}

static void I2C_SendByte(u8 byte)
{
	u8 i = 8;
	while (i--) {
		SCL_reset;
		I2C_delay();
		if (byte & 0x80)
		SDA_set;
		else
		SDA_reset;
		byte <<= 1;
		I2C_delay();
		SCL_set;
		I2C_delay();
	}
	SCL_reset;
}

static u8 I2C_ReceiveByte(void)
{
	u8 i = 8;
	u8 byte = 0;

	SDA_set;
	while (i--) {
		byte <<= 1;
		SCL_reset;
		I2C_delay();
		SCL_set;
		I2C_delay();
		if (SDA_read) {
			byte |= 0x01;
		}
	}
	SCL_reset;
	return byte;
}

void i2cInit(void)
{
	//RCC->APB2ENR|=1<<6;   //使能PORTE口时钟
	RCC->APB2ENR|=1<<3;   //使能PORTB口时钟  
	//GPIOE->CRH&=0XFFFFFF00; 
	//GPIOE->CRH|=0X00000055;//IO状态设置
	GPIOB->CRL&=0X00FFFFFF;
	GPIOB->CRL|=0X55FFFFFF;
}

u8 i2cWriteBuffer(u8 addr, u8 reg, u8 len, u8 * data)
{
	int i;
	if (!I2C_Start())
	return false;
	I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);
	if (!I2C_WaitAck()) {
		I2C_Stop();
		return false;
	}
	I2C_SendByte(reg);
	I2C_WaitAck();
	for (i = 0; i < len; i++) {
		I2C_SendByte(data[i]);
		if (!I2C_WaitAck()) {
			I2C_Stop();
			return false;
		}
	}
	I2C_Stop();
	return true;
}
/////////////////////////////////////////////////////////////////////////////////
int i2cwrite(u8 addr, u8 reg, u8 len, u8 * data)
{
	if(i2cWriteBuffer(addr,reg,len,data))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//return FALSE;
}
int i2cread(u8 addr, u8 reg, u8 len, u8 *buf)
{
	if(i2cRead(addr,reg,len,buf))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	//return FALSE;
}
//////////////////////////////////////////////////////////////////////////////////
u8 i2cWrite(u8 addr, u8 reg, u8 data)
{
	if (!I2C_Start())
	return false;
	I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);
	if (!I2C_WaitAck()) {
		I2C_Stop();
		return false;
	}
	I2C_SendByte(reg);
	I2C_WaitAck();
	I2C_SendByte(data);
	I2C_WaitAck();
	I2C_Stop();
	return true;
}

u8 i2cRead(u8 addr, u8 reg, u8 len, u8 *buf)
{
	if (!I2C_Start())
	return false;
	I2C_SendByte(addr << 1 | I2C_Direction_Transmitter);
	if (!I2C_WaitAck()) {
		I2C_Stop();
		return false;
	}
	I2C_SendByte(reg);
	I2C_WaitAck();
	I2C_Start();
	I2C_SendByte(addr << 1 | I2C_Direction_Receiver);
	I2C_WaitAck();
	while (len) {
		*buf = I2C_ReceiveByte();
		if (len == 1)
		I2C_NoAck();
		else
		I2C_Ack();
		buf++;
		len--;
	}
	I2C_Stop();
	return true;
}

u16 i2cGetErrorCounter(void)
{
	// TODO maybe fix this, but since this is test code, doesn't matter.
	return 0;
}
