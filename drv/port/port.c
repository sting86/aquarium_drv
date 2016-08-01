/*
 * port.c
 *
 *  Created on: 31 lip 2016
 *      Author: Pawe³
 */
#include "avr/io.h"
#include "port.h"

void _set(volatile uint8_t* addr, uint8_t pin, uint8_t value) {
	if (value) {
		*addr |= 1<<pin;
	} else {
		*addr &= ~(1<<pin);
	}
}

void Pin_config(volatile uint8_t* ddr, uint8_t pin, enum pin_config config) {
	_set(ddr, pin, config);
}

void Pin_set(volatile uint8_t* port, uint8_t pin, enum pin_state state) {
	_set(port, pin, state);
}
