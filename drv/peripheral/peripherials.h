/*
 * peripherials.h
 *
 *  Created on: 29 lip 2016
 *      Author: Pawe³
 */

#ifndef DRV_PERIPHERAL_PERIPHERIALS_H_
#define DRV_PERIPHERAL_PERIPHERIALS_H_

#include "../../framework/error.h"

enum periph_type {
	PERIPH_PUMP,

	PERIPH_COUNT
};

enum periph_state {
	PERIPH_STATE_ON,
	PERIPH_STATE_OFF,

	PERIPH_STATE_COUNT
};

Error Peripherials_Initialize ();
Error Peripherials_Set (enum periph_type, enum periph_state);

#endif /* DRV_PERIPHERAL_PERIPHERIALS_H_ */
