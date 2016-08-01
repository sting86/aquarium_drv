/*
 * port.h
 *
 *  Created on: 30 lip 2016
 *      Author: Pawe³
 */

#ifndef DRV_PORT_PORT_H_
#define DRV_PORT_PORT_H_

enum pin_config {
	PIN_INPUT,
	PIN_OUTPUT,
};

enum pin_state {
	OUT_LO = 0,
	OUT_HI = 1,
	IN_HI_Z = 0,
	IN_PULLUP = 1,
};

void Pin_config(volatile uint8_t* ddr, uint8_t pin, enum pin_config config);
void Pin_set(volatile uint8_t* port, uint8_t pin, enum pin_state state);

//macros
#define PIN_CONFIG(DDR, PIN, PIN_CONFIG) Pin_config(&DDR, PIN, PIN_CONFIG)
#define PIN_SET(PORT, PIN, VALUE) Pin_set(&PORT, PIN, VALUE)

#endif /* DRV_PORT_PORT_H_ */
