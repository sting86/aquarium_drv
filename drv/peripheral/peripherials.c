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

	return ret;
}
Error Peripherials_Set (enum periph_type type, enum periph_state state){
	Error ret = NO_ERROR;
//	uint8_t buff;

	switch (type) {
		case PERIPH_PUMP:
			PIN_SET(PUMP_PORT, PUMP_PIN, state);
//			buff = PUMP_PORT &(~(1<<PUMP_PIN));
//			buff |= (state<<PUMP_PIN);
//			PUMP_PORT = buff;
			break;

		default:
			break;
	}

	return ret;
}
