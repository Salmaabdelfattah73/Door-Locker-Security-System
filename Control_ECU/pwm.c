/*
 * pwm.c
 *
 *  Created on: Aug 17, 2023
 *      Author: Salma Essam
 */


#include "gpio.h"
#include "pwm.h"
#include <avr/io.h>

void PWM_Timer0_Start(uint8 duty_cycle){
	TCNT0 = 0;
	OCR0 = duty_cycle;
	GPIO_setupPinDirection(PORTB_ID, PIN3_ID, PIN_OUTPUT); //OC0/PB3 as output pin
	TCCR0 = (1 << WGM01) | (1 << WGM00) | (1 << COM01) | (1 << CS01);
}
