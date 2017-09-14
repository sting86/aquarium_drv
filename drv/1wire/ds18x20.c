/*
 * ds18x20.c
 *
 *  Created on: 9 wrz 2017
 *      Author: Pawe³
 */

#include "1wire_internal.h"
#include "1wire.h"

Error OW_DS18x20_StartConversion(struct OW_device* device) {
	Error ret = NO_ERROR;

	_1w_reset();
	_1w_isDevice();
	if (device == NULL) {
		_1w_sendByte(CMD_SKIP_ROM);
	} else {
		OW_MatchRom(device);
	}
	_1w_sendByte(CMD_CONVERT_TEMP);

	return ret;
}

Error OW_DS18x20_ReadTemp(struct OW_device* device, uint16_t *result) {
	Error ret = NO_ERROR;

	_1w_reset();
	_1w_isDevice();
	if (device == NULL) {
		_1w_sendByte(CMD_SKIP_ROM);
	} else {
		OW_MatchRom(device);
	}
	_1w_sendByte(CMD_READ_SCRATCHPAD);

	*result = 0;

	for (uint8_t a = 0; a < 16; a++) {
		uint8_t bit = 0;
		_1w_readBit(&bit);

		*result |= ((uint16_t) bit << a);
	}

	return ret;
}

Error OW_DS18b20_ConvertTemp(uint16_t stamp, int8_t *degrees, uint8_t *rest) {
	Error ret = NO_ERROR;

	if (degrees == NULL) return ERROR_INVALID_PARAMETER;

	*degrees = (uint8_t) ( stamp >> 4);

	if (rest) {
		*rest = (uint8_t) (((stamp & 0x0f) * 625)/100);
	}

	return ret;
}


Error OW_DS1820_ConvertTemp(uint16_t stamp, int8_t *degrees, uint8_t *rest) {
	Error ret = NO_ERROR;

	if (degrees == NULL) return ERROR_INVALID_PARAMETER;

	*degrees = (uint8_t) ( stamp >> 1);

	if (rest) {
		*rest = (uint8_t) ((stamp & 1) * 50);
	}

	return ret;
}
