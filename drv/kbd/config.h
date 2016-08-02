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
#define LINE1_PIN  PINB
#define LINE1      PB5

#define LINE2_DDR  DDRB //PB6
#define LINE2_PORT PORTB
#define LINE2_PIN  PINB
#define LINE2      PB6

#define LINE3_DDR  DDRC //PC5
#define LINE3_PORT PORTC
#define LINE3_PIN  PINC
#define LINE3      PC5

#define LINE4_DDR
#define LINE4_PORT
#define LINE4_PIN
#define LINE4

//k5, k6, k7, k8
#define COLUMN1_DDR  DDRC
#define COLUMN1_PORT PORTC
#define COLUMN1_PIN  PINC
#define COLUMN1      PC4

#define COLUMN2_DDR  DDRD
#define COLUMN2_PORT PORTD
#define COLUMN2_PIN  PIND
#define COLUMN2      PD6

#define COLUMN3_DDR  DDRB
#define COLUMN3_PORT PORTB
#define COLUMN3_PIN  PINB
#define COLUMN3      PB1

#define COLUMN4_DDR  DDRD
#define COLUMN4_PORT PORTD
#define COLUMN4_PIN  PIND
#define COLUMN4      PD4
/**
 * uint8_t scanMap[4][4] = {
 *		{ 0,  1,  2,  3},
 *		{ 4,  5,  6,  7},
 *		{ 8,  9, 10, 11},
 *		{12, 13, 14, 15},
 *};
 *
 * When PCB is designed in simple way, then use map as above
 * i. e.
 *         K1     K2
 *     S1  |  S2  |
 *    ,-/--*  ,-/-*
 *K3--'----+--'   |
 *     S3  |  S4  |
 *    ,-/--'  ,-/-'
 *K4--'-------'
 * but, when matrix on PCB is not the same as on blueprint
 * then You have to config Your own map.
 * Values are counted in following way:
 * map[x][y] = (PATERN_X << x') & (PATERN_Y << (4*y'))
 * where:
 * PATERN_X = 1000100010001b
 * PATERN_Y = 1111b
 * x, y - position in map matrix
 * x', y' - position of key in real
 */


uint8_t scanMap[4][4] = {
		{ 2,  6, 10, 14},
		{ 5,  4, 13,  1},
		{ 9,  8, 12, 11},
		{ 0,  3,  7, 13},
};

#endif /* DRV_KBD_CONFIG_H_ */
