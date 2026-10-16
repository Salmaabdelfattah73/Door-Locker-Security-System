/*
 * ex_eeprom.h
 *
 *  Created on: Aug 17, 2023
 *      Author: Salma Essam
 */

#ifndef EX_EEPROM_H_
#define EX_EEPROM_H_

#include "std_types.h"
#define ERROR 0
#define SUCCESS 1
uint8 EEPROM_writeByte(uint16 u16addr, uint8 u8data);
uint8 EEPROM_readByte(uint16 u16addr, uint8 *u8data);

#endif /* EX_EEPROM_H_ */
