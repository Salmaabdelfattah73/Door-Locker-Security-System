/*
 * timer1.c
 *
 *  Created on: Aug 17, 2023
 *      Author: Salma Essam
 */


#include "timer1.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "gpio.h"

static volatile void (*g_callBackPtr)(void) = NULL_PTR;

ISR(TIMER1_OVF_vect)
{
	if (g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

ISR(TIMER1_COMPA_vect)
{
	if (g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

void Timer1_init(const Timer1_ConfigType *Config_Ptr){
	TCNT1 = Config_Ptr->initial_value;
	TCCR1A = (1 << FOC1A) | (1 << FOC1B) | (1 << COM1A1);
	if (Config_Ptr->mode == NORMAL)
	{
		TCCR1A = (TCCR1A & 0xFC) | (Config_Ptr->mode);
		TIMSK |= (1 << TOIE1);
	}
	else if (Config_Ptr->mode == CTC)
	{
		TCCR1B = (TCCR1B & 0xE7) | ((Config_Ptr->mode) << 1);
		GPIO_setupPinDirection(PORTD_ID, PIN5_ID, PIN_OUTPUT);
		OCR1A = Config_Ptr->compare_value;
		TIMSK |= (1 << OCIE1A);
	}
	TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->prescaler);
	SREG |= (1 << 7);
}

void Timer1_deInit(void){
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	TIMSK &= ~(1 << TOIE1);
	TIMSK &= ~(1 << OCIE1A);
}

void Timer1_setCallBack(void (*a_ptr)(void))
{
	  g_callBackPtr = a_ptr;
}
