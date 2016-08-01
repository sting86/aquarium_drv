/*
 * config.h
 *
 *  Created on: 29 lip 2016
 *      Author: Pawe³
 */

#include "avr/io.h"

#ifndef DRV_KBD_CONFIG_H_
#define DRV_KBD_CONFIG_H_
//k1, k2, k3, k4
#define LINE1_DDR  DDRB //PB5
#define LINE1_PORT PORTB
#define LINE1_PIN  PB5

#define LINE2_DDR  DDRB //PB6
#define LINE2_PORT PORTB
#define LINE2_PIN  PB6

#define LINE3_DDR  DDRC //PC5
#define LINE3_PORT PORTC
#define LINE3_PIN  PC5

#define LINE4_DDR
#define LINE4_PORT
#define LINE4_PIN

//k5, k6, k7, k8
#define COLUMN1_DDR  DDRC
#define COLUMN1_PORT PORTC
#define COLUMN1_PIN  PC4

#define COLUMN2_DDR  DDRD
#define COLUMN2_PORT PORTD
#define COLUMN2_PIN  PD6

#define COLUMN3_DDR  DDRB
#define COLUMN3_PORT PORTB
#define COLUMN3_PIN  PB1

#define COLUMN4_DDR  DDRD
#define COLUMN4_PORT PORTD
#define COLUMN4_PIN  PD4

#endif /* DRV_KBD_CONFIG_H_ */
