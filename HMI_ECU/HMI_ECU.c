/*
 * HMI_ECU.c
 *
 *  Created on: Aug 17, 2023
 *      Author: Salma Essam
 */

#include <util/delay.h>
#include "timer1.h"
#include <avr/io.h>
#include "lcd.h"
#include "keypad.h"
#include "uart.h"

unsigned char g_tick = 0;
uint8 receive = 0;  //received byte by UART from CONTROL_ECU

void PASS(void){
	uint8 key, i;
	for (i = 0; i < 5; i++)
	{   //if any switch pressed for more than 500 ms it will considered more than one press
		key = KEYPAD_getPressedKey();
		if ((key <= 9) && (key >= 0))
		{
			LCD_displayCharacter('*'); //display the pressed keypad switch as '*'
			UART_sendByte(key);		   //send the pressed key to the CONTROL_ECU
		}
		else
		{
			i--; //to make the UART send five numbers only not including any characters
		}
		_delay_ms(500); //press time
	}
	key = KEYPAD_getPressedKey();
	if (key == '=') //'=' as an Enter button
	{
		receive = UART_recieveByte(); //receive a byte 1 or 0 to check that the two passwords are matched or not
		_delay_ms(500);
	}
}

void CHANGE_PASSWORD(void){
	uint8 key, i;
	for (i = 0; i < 5; i++){   //if any switch pressed for more than 500 ms it will considered more than one press
		key = KEYPAD_getPressedKey();
		if ((key <= 9) && (key >= 0))
		{
			LCD_displayCharacter('*');
			UART_sendByte(key);		   //send the pressed key to the CONTROL_ECU
		}
		else{i--;} //to make the UART send five numbers only not including any characters
		_delay_ms(500);
	}
	key = KEYPAD_getPressedKey();
	if (key == '=') //'=' as an Enter button
	{ // ask user to enter the same password
		LCD_clearScreen();
		LCD_displayStringRowColumn(0, 0, "Plz re-enter the");
		LCD_displayStringRowColumn(1, 0, "Same Pass:");
	}
	LCD_moveCursor(1, 11);
	for (i = 0; i < 5; i++){ //if any switch pressed for more than 500 ms it will considered more than one press */
		key = KEYPAD_getPressedKey();
		if ((key <= 9) && (key >= 0))
		{
			LCD_displayCharacter('*');
			UART_sendByte(key);
		}
		else{i--;}

		_delay_ms(500);
	}
	key = KEYPAD_getPressedKey();
	if (key == '=')
	{
		receive = UART_recieveByte();
		_delay_ms(500);
	}
}

void TIMER1_ticks(){
	g_tick++;
}

void OPEN_CLOSE_DOOR()
{
	UART_sendByte('&'); // to make CONTROL_ECU ready for using the door(motor)
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Door is");
	LCD_displayStringRowColumn(1, 0, "Unlocking");

	g_tick = 0; // start the counts again to get the needed 15 seconds
	while (g_tick <= 15)
		; //wait until the 15 seconds
	LCD_clearScreen();
	LCD_displayString("WARNING..."); //to warn user that the door will close in 3 seconds

	while (g_tick <= 18)
		; //wait until the 3 seconds
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Door is");
	LCD_displayStringRowColumn(1, 0, "Locking");

	while (g_tick <= 33)
		;

	g_tick = 0; //make counter zero again when it finished
}

void BUZZER()
{
	UART_sendByte('$'); //to make CONTROL_ECU ready for using the buzzer
	LCD_clearScreen();
	LCD_displayString("ERROR!!!!");
	g_tick = 0; //start counts again to get the needed 1 minute
	while (g_tick <= 60)
		;
	g_tick = 0;
}

int main(void)
{
	uint8 k;
	UART_ConfigType config1 = {EIGHT, DISABLED, ONE_BIT, BR7};
	Timer1_ConfigType config2 = {0, 999, F_CPU_1024, CTC};
	Timer1_init(&config2);
	UART_init(&config1);
	Timer1_setCallBack(TIMER1_ticks);
	LCD_init();
	LCD_displayString("Plz enter pass:");
	LCD_moveCursor(1, 0);
	UART_sendByte('*');

	//calling function CHANGE_PASSWORD to give the password to the system and save it
	CHANGE_PASSWORD();
     //first step
	if (receive == 1) // if the two passwords are matched receive 1
	{
		//Go to second step
	}
	//if the two passwords don't match repeat first step for second time
	else
	{
		LCD_clearScreen();
		LCD_displayString("Plz enter pass:");
		LCD_moveCursor(1, 0);
		UART_sendByte('*');
		CHANGE_PASSWORD();
		if (receive == 1)
		{
			//Go to second step
		}
		//if the two passwords don't match repeat step 1 for last time
		else{
			LCD_clearScreen();
			LCD_displayString("Plz enter pass:");
			LCD_moveCursor(1, 0);
			UART_sendByte('*');
			CHANGE_PASSWORD();
			if (receive == 1)
			{
				//Go to second step
			}
			//if the two passwords don't match DISPLAY an ERROR and close the system
			else
			{
				LCD_clearScreen();
				LCD_displayString("SYSTEM ERROR");
				return 0;
			}
		}
	}
	// second Step
	while (1)
	{//Display the main options on the screen
		LCD_displayStringRowColumn(0, 0, "+ : Open Door");
		LCD_displayStringRowColumn(0, 13, "   ");
		LCD_displayStringRowColumn(1, 0, "- : Change Pass");
		LCD_displayStringRowColumn(1, 15, "  ");
		k = KEYPAD_getPressedKey();
		if (k == '+') // you choose to open the door  //third Step 3
		{
			//Enter the password to be able to open the door
			LCD_clearScreen();
			LCD_displayString("Plz enter pass:");
			LCD_moveCursor(1, 0);
			// make CONTROL_ECU ready for reading the password and check it with the one saved before
			UART_sendByte('#');
			PASS();			//take password from user
			if (receive == 1) //Receive 1 for matched passwords
			{
				OPEN_CLOSE_DOOR(); 				//Open the door then close it
			}
			else //if two passwords don't match then repeat first step for the first time
			{

				LCD_clearScreen();
				LCD_displayString("Plz enter pass:");
				LCD_moveCursor(1, 0);
				UART_sendByte('#');
				PASS();
				if (receive == 1)
				{
					OPEN_CLOSE_DOOR();
				}
				else //if the two passwords don't match then repeat first step  for the second time
				{
					LCD_clearScreen();
					LCD_displayString("Plz enter pass:");
					LCD_moveCursor(1, 0);
					UART_sendByte('#');
					PASS();
					if (receive == 1) //Receive 1 for matched passwords
					{
						OPEN_CLOSE_DOOR();
					}
					else //if two passwords don't match for the last time then turn on the BUZZER  //Step five
					{
						BUZZER(); //turn on the buzzer
					}
				}
			}
		}
		else if (k == '-') //you choose to change the password  //Step four
		{
			//enter the password to be able to change the password
			LCD_clearScreen();
			LCD_displayString("Plz enter pass:");
			LCD_moveCursor(1, 0);
			UART_sendByte('#');
			PASS();
			if (receive == 1)
			{

				LCD_clearScreen();
				LCD_displayString("Plz enter pass:");
				LCD_moveCursor(1, 0);
				UART_sendByte('*');
				CHANGE_PASSWORD();
			}
			else // if two passwords don't match then repeat first step  for the first time
			{
				LCD_clearScreen();
				LCD_displayString("Plz enter pass:");
				LCD_moveCursor(1, 0);
				UART_sendByte('#');
				PASS();
				if (receive == 1)
				{
					LCD_clearScreen();
					LCD_displayString("Plz enter pass:");
					LCD_moveCursor(1, 0);
					UART_sendByte('*');
					CHANGE_PASSWORD();
				}
				else //if two passwords don't match then repeat first step for the second time
				{
					LCD_clearScreen();
					LCD_displayString("Plz enter pass:");
					LCD_moveCursor(1, 0);
					UART_sendByte('#');
					PASS();
					if (receive == 1)
					{
						LCD_clearScreen();
						LCD_displayString("Plz enter pass:");
						LCD_moveCursor(1, 0);
						UART_sendByte('*');
						CHANGE_PASSWORD();
					}
					else //if two passwords don't match then repeat first step  for the last time  //Step five
					{
						BUZZER();//turn on buzzer
					}
				}
			}
		}
	}
}
