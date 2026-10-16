/*
 * buzzer.c
 *
 *  Created on: Aug 17, 2023
 *      Author: Salma Essam
 */
#include "gpio.h"
#include"buzzer.h"

void Buzzer_init(){
	GPIO_setupPinDirection(PORTA_ID, PIN7_ID, PIN_OUTPUT);
	GPIO_writePin(PORTA_ID, PIN7_ID, LOGIC_LOW);
}

void Buzzer_on(void){
	GPIO_writePin(PORTA_ID, PIN7_ID, LOGIC_HIGH);
}

void Buzzer_off(void){
	GPIO_writePin(PORTA_ID, PIN7_ID, LOGIC_LOW);
}
