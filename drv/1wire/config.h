/*
 * config.h
 *
 *  Created on: 15 maj 2017
 *      Author: Pawe³
 */

#ifndef DRV_1WIRE_CONFIG_H_
#define DRV_1WIRE_CONFIG_H_

enum OW_FamilyCodes {
	OW_Family_DS1820 = 0x10,
};

#define OW_PORT PORTA
#define OW_DDR  DDRA
#define OW_PIN  PINA
#define OW_DATA PA0

#endif /* DRV_1WIRE_CONFIG_H_ */
