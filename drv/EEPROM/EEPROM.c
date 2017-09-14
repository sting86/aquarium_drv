/*
 * EEPROM.c
 *
 *  Created on: 10 wrz 2017
 *      Author: Pawe³
 */

#include "avr/io.h"
#include "EEPROM.h"

void EEPROM_write(uint16_t uiAddress, uint8_t ucData) {
	/* Wait for completion of previous write */
	while (EECR & (1 << EEWE))
		;
	/* Set up address and data registers */
	EEAR = uiAddress;
	EEDR = ucData;
	/* Write logical one to EEMWE */
	EECR |= (1 << EEMWE);
	/* Start eeprom write by setting EEWE */
	EECR |= (1 << EEWE);
}

uint8_t EEPROM_read(uint16_t uiAddress) {
	/* Wait for completion of previous write */
	while (EECR & (1 << EEWE))
		;
	/* Set up address register */
	EEAR = uiAddress;
	/* Start eeprom read by writing EERE */
	EECR |= (1 << EERE);
	/* Return data from data register */
	return EEDR;
}

void EEPROM_clear() {
	/* Wait for completion of previous write */
	for (uint16_t i=0; i<1024; ++i) {
		while (EECR & (1 << EEWE))
			;
		/* Set up address and data registers */
		EEAR = i;
		EEDR = 0xFF;
		/* Write logical one to EEMWE */
		EECR |= (1 << EEMWE);
		/* Start eeprom write by setting EEWE */
		EECR |= (1 << EEWE);
	}
}
