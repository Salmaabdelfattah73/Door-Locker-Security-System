/*
 * uart.h
 *
 *  Created on: Aug 17, 2023
 *      Author: Salma Essam
 */

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

typedef enum{
	FIVE,SIX,SEVEN,EIGHT,NINE=0x07
}UART_BitData;

typedef enum{
	DISABLED=0,EVEN=0x02,ODD=0x03
}UART_Parity;

typedef enum{
	ONE_BIT,TWO_BIT
}UART_StopBit;

typedef enum{
	BR1=10,BR2=300,BR3=600,BR4=1200,BR5=2400,BR6=4800,BR7=9600,BR8=14400,BR9=19200,BR10=38400,
	BR11=57600,BR12=115200,BR13=128000,BR14=256000
}UART_BaudRate;

typedef struct{
	UART_BitData bit_data;
	UART_Parity parity;
	UART_StopBit stop_bit;
	UART_BaudRate baud_rate;
}UART_ConfigType;


void UART_init(const UART_ConfigType * Config_Ptr);
void UART_sendByte(const uint8 data);
uint8 UART_recieveByte(void);
void UART_sendString(const uint8 *Str);
void UART_receiveString(uint8 *Str); // Receive until #


#endif /* UART_H_ */
