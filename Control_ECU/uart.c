/*
 * uart.c
 *
 *  Created on: Aug 17, 2023
 *      Author: Salma Essam
 */


#include "uart.h"
#include <avr/io.h>
#include "common_macros.h"

void UART_init(const UART_ConfigType * Config_Ptr){
	uint16 ubrr_value = 0;
	UCSRA = (1<<U2X);
	UCSRB = (1<<RXEN) | (1<<TXEN);
    UCSRC = (1<<URSEL);
	UCSRC = (UCSRC & 0xF9) | ((Config_Ptr->bit_data)<<1);
	UCSRC = (UCSRC & 0xCF) | ((Config_Ptr->parity)<<4);
	UCSRC = (UCSRC & 0xF7) | ((Config_Ptr->stop_bit)<<3);

	ubrr_value = (uint16)(((F_CPU / (Config_Ptr->baud_rate * 8UL))) - 1); //Calculate the UBRR register value

	UBRRH = ubrr_value>>8;
	UBRRL = ubrr_value;
}

void UART_sendByte(const uint8 data){
	 while(BIT_IS_CLEAR(UCSRA,UDRE)){}
   	UDR = data;
}

uint8 UART_recieveByte(void){
	//RXC flag is set when the UART receive data so wait until this flag is set to one
	while(BIT_IS_CLEAR(UCSRA,RXC)){}
    return UDR;
}

void UART_sendString(const uint8 *Str){
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}}

void UART_receiveString(uint8 *Str){
	uint8 i = 0;
	Str[i] = UART_recieveByte();  //receive the first byte
	while(Str[i] != '#') //receive the whole string until the '#'
	{
		i++;
		Str[i] = UART_recieveByte();
	}
	Str[i] = '\0';  //After receiving the whole string plus the '#', replace the '#' with '\0'
}
