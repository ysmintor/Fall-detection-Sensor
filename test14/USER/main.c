#pragma once
#include "stm32f10x.h"
#include "usart1.h"
#include "SysTick.h"
#include "usart2.h"
#include "misc.h"
#include "IIC.h"
#include "led.h"
#include "getdata.h"
int main(void)
{
	/* 串口2初始化 */
//	USART1_Config();
	USART2_Config();
//	NVIC_Configuration();
    NVIC_Configuration1();
	/* 配置SysTick 为1us中断一次 */
	SysTick_Init();	
	 
	/* Init I2C and MPU6050*/
  	i2cInit();
	LED_GPIO_Config();
	if (mpuDMPinit())
	{
		// dmp设备初始化失败
		printf("f");
	}
	// dmp设备初始化成功
	else printf("i");		
	// 检测算法
	checkAlgo();
	return 0; 
}
