/*
 * ECU.c
 *
 *  Created on: Aug 17, 2023
 *      Author: Salma Essam
 */

#include "uart.h"
#include <util/delay.h>
#include <avr/io.h>
#include "timer1.h"
#include "ex_eeprom.h"
#include "buzzer.h"
#include "dc_motor.h"

unsigned char g_tick = 0;

void PASS_CHECK(void){
	uint8 receive, key, i, read1, read2, check = 0;
	for (i = 6; i < 11; i++)
	{
		key = UART_recieveByte();   // Receive the key from HMI_ECU
		receive = EEPROM_writeByte((0x01) + i, key); // Save the key in the EEPROM
		_delay_ms(500);
	}
	for (i = 0; i < 5; i++)   //check the two passwords byte by byte
	{
		EEPROM_readByte((0x01) + i, &read1);
		_delay_ms(10);
		EEPROM_readByte((0x07) + i, &read2);
		_delay_ms(10);
		if (read1 == read2) //If the two bytes are matched increment the check counter
		{
			check++;
		}
	}
	if (check == 5) // the two passwords are matched if the check counter is equal 5
	{
		UART_sendByte(1); // Send 1 to HMI_ECU to let it know that the two passwords are matched
	}
	else
	{
		UART_sendByte(0); // Send 0 to HMI_ECU to let it know that the two passwords are not matched
	}
	check = 0;
}


void EEPROM(void){
	uint8 receive, key, i, read1, read2, check = 0;

	for (i = 0; i < 5; i++)
	{
		key = UART_recieveByte();	// Receive the key from HMI_ECU
		receive = EEPROM_writeByte((0x01) + i, key); // Save the key in the EEPROM
		_delay_ms(500);
	}

	for (i = 6; i < 11; i++)
	{
		key = UART_recieveByte(); // Receive the key from HMI_ECU
		receive = EEPROM_writeByte((0x01) + i, key); // Save the key in the EEPROM  in different address
		_delay_ms(500);
	}
	for (i = 0; i < 5; i++) // check the two passwords byte by byte
	{
		EEPROM_readByte((0x01) + i, &read1);
		_delay_ms(10);
		EEPROM_readByte((0x07) + i, &read2);
		_delay_ms(10);
		if (read1 == read2)
		{
			check++;
		}
	}
	if (check == 5)
	{
		UART_sendByte(1); //Send 1 to HMI_ECU to let it know that the two passwords are matched
	}
	else
	{
		UART_sendByte(0); //Send 0 to HMI_ECU to let it know that the two passwords are not matched
	}
	check = 0;
}

void TIMER1_ticks(){
	g_tick++;}


int main(void){
	uint8 byte_check;
	UART_ConfigType config1 = {EIGHT, DISABLED, ONE_BIT, BR7};
	Timer1_ConfigType config2 = {0, 31249, F_CPU_256, CTC};
	Timer1_init(&config2);
	UART_init(&config1);
	Timer1_setCallBack(TIMER1_ticks);
	DcMotor_Init();
	Buzzer_init();

	while (1){
		byte_check = UART_recieveByte();
		switch (byte_check){
		case '*': //user need to change the password or enter the password for the first time
            EEPROM();
            break;

		case '#': //user need to enter the password to either open the door or change password
			PASS_CHECK(); //call PASS_CHECK function to check whether the password entered matches the user password or not
			break;

		case '&': //user need to open the door
			DcMotor_Rotate(CW, 100);
			g_tick = 0;
			while (g_tick <= 15)  // open the door for 15 seconds
				;
			DcMotor_Rotate(STOP, 0); //door is hold for 3 seconds
			while (g_tick <= 18)
				;
			DcMotor_Rotate(A_CW, 100); //cloose the door for 15 seconds
			while (g_tick <= 33)
				;
			g_tick = 0;
			DcMotor_Rotate(STOP, 0);

		    break;

		case '$': // user entered the password wrong 3 consecutive time so that the ALARM must be ON
			g_tick = 0;
			Buzzer_on();			//activate the BUZZER for 1 minute
			while (g_tick <= 60)
				;

			g_tick = 0;
			Buzzer_off();
		    break;
		}
	}
}
