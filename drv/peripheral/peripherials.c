/*
 * peripherials.c
 *
 *  Created on: 29 lip 2016
 *      Author: Pawe³
 */


#include "peripherials.h"
#include "config.h"


Error Peripherials_Initialize () {
	Error ret=NO_ERROR;

//	PUMP_DDR  |= 1<<PUMP_PIN;
//	PUMP_PORT |= (1<<PUMP_PIN);
	PIN_CONFIG(PUMP_DDR, PUMP_PIN, PIN_OUTPUT);
	PIN_SET(PUMP_PORT, PUMP_PIN, OUT_HI);

	PIN_CONFIG(NIGHTLIGHT_DDR, NIGHTLIGHT_PIN, PIN_OUTPUT);
	PIN_SET(NIGHTLIGHT_PORT, NIGHTLIGHT_PIN, OUT_HI);

	PIN_CONFIG(FEEDER_DDR, FEEDER_PIN, PIN_OUTPUT);
	PIN_SET(FEEDER_PORT, FEEDER_PIN, OUT_HI);

	return ret;
}
Error Peripherials_Set (enum periph_type type, enum periph_state state){
	Error ret = NO_ERROR;
//	uint8_t buff;

	switch (type) {
		case PERIPH_PUMP:
			PIN_SET(PUMP_PORT, PUMP_PIN, state);
			break;

		case PERIPH_NIGHTLIGHT:
			PIN_SET(NIGHTLIGHT_PORT, NIGHTLIGHT_PIN, state);
			break;

		case PERIPH_FEEDER:
			PIN_SET(FEEDER_PORT, FEEDER_PIN, state);
			break;

		default:
			break;
	}

	return ret;
}
