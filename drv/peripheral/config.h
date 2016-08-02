/*
 * config.h
 *
 *  Created on: 29 lip 2016
 *      Author: Pawe³
 */

#ifndef DRV_PERIPHERAL_CONFIG_H_
#define DRV_PERIPHERAL_CONFIG_H_

#include "avr/io.h"

#define PUMP_PORT PORTA
#define PUMP_DDR  DDRA
#define PUMP_PIN  PA6

#define NIGHTLIGHT_PORT PORTA
#define NIGHTLIGHT_DDR  DDRA
#define NIGHTLIGHT_PIN  PA2

#define FEEDER_PORT PORTA
#define FEEDER_DDR  DDRA
#define FEEDER_PIN  PA3



#endif /* DRV_PERIPHERAL_CONFIG_H_ */
