/*
 * ex_eeprom.c
 *
 *  Created on: Aug 17, 2023
 *      Author: Salma Essam
 */

#include "ex_eeprom.h"
#include "twi.h"

uint8 EEPROM_writeByte(uint16 u16addr, uint8 u8data){
    TWI_start();//sent start byte
    if (TWI_getStatus() != TWI_START)
        return ERROR;
    /*Send the device address,want to get A8 A9 A10 address bits from the
      memory location address and R/W=0 (write) */
    TWI_writeByte((uint8)(0xA0 | ((u16addr & 0x0700) >> 7)));
    if (TWI_getStatus() != TWI_MT_SLA_W_ACK)
        return ERROR;

    TWI_writeByte((uint8)(u16addr));    // required memory location address
    if (TWI_getStatus() != TWI_MT_DATA_ACK)
        return ERROR;

    TWI_writeByte(u8data);
    if (TWI_getStatus() != TWI_MT_DATA_ACK)
        return ERROR;
    TWI_stop();

    return SUCCESS;
}

uint8 EEPROM_readByte(uint16 u16addr, uint8 *u8data){
    TWI_start();
    if (TWI_getStatus() != TWI_START)
        return ERROR;
    /*Send the device address,want to get A8 A9 A10 address bits from the
          memory location address and R/W=0 (write) */
    TWI_writeByte((uint8)((0xA0) | ((u16addr & 0x0700) >> 7)));
    if (TWI_getStatus() != TWI_MT_SLA_W_ACK)
        return ERROR;

    TWI_writeByte((uint8)(u16addr));    //required memory location address
    if (TWI_getStatus() != TWI_MT_DATA_ACK)
        return ERROR;

    TWI_start();
    if (TWI_getStatus() != TWI_REP_START)
        return ERROR;

    /*Send the device address,want to get A8 A9 A10 address bits from the
       memory location address and R/W=0 (write) */
    TWI_writeByte((uint8)((0xA0) | ((u16addr & 0x0700) >> 7) | 1));
    if (TWI_getStatus() != TWI_MT_SLA_R_ACK)
        return ERROR;

    *u8data = TWI_readByteWithNACK();
    if (TWI_getStatus() != TWI_MR_DATA_NACK)
        return ERROR;

    TWI_stop();

    return SUCCESS;
}
