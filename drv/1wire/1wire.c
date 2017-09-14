/*
 * 1wire.c
 *
 *  Created on: 15 maj 2017
 *      Author: Pawe³
 */

#include "1wire.h"
#include "1wire_internal.h"

#include "util/delay.h"

#include "avr/pgmspace.h"

#include <string.h>

#include "drv/LCD/HD44780.h"
#include "config.h"
#include "../../framework/crc8.h"

#define _1W_ONE() \
	OW_DDR &= ~(1<<OW_DATA)

#define _1W_ZERO() \
		OW_DDR |= (1<<OW_DATA)

#define WAIT()__asm__ __volatile__ ("nop")

struct owSearchStatus {
	uint8_t lastValue    :1;
	uint8_t forkDetected :1;
};

Error OW_Initialize() {

	_1W_ONE();

	return NO_ERROR;
}


Error _1w_reset() {

	_1W_ZERO();
	_delay_us(480);

	_1W_ONE();
	return NO_ERROR;
}

bool _1w_isDevice() {
	uint8_t i;
	bool ret = false;

	_1W_ONE();

	for (i = 0; i < 24; ++i) {
		if ((OW_PIN & (1<<OW_DATA)) == 0) ret = true;
		_delay_us(10);
	}

	return ret;
}

Error _1w_sendBit(uint8_t bit) {
	_1W_ZERO();

	_delay_us(1);

	if (bit != 0) {
		_1W_ONE();
		_delay_us(55);
	} else {
		_delay_us(60);
	}

	_1W_ONE();

	return NO_ERROR;
}

Error _1w_readBit(uint8_t *bit) {
	if (bit == NULL) return ERROR_INVALID_PARAMETER;

	_1W_ZERO();
	_delay_us(6);
//	WAIT();
//	WAIT();
//	WAIT();
//	WAIT();

	_1W_ONE();
	_delay_us(9);
	//WAIT();

	if (OW_PIN & (1<<OW_DATA)) {
		*bit = 1;
	} else {
		*bit = 0;
	}

	_delay_us(45);

	return NO_ERROR;
}

Error _1w_sendByte(uint8_t byte) {
	Error ret = NO_ERROR;
	int8_t i;
	for (i=0; i<8; ++i) {
		ret |= _1w_sendBit(byte&(1<<i));
	}

	return ret;
}

Error _1w_searchRom(struct OW_device* deviceList, uint8_t *cnt /*in-out*/, enum OW_CMD cmdId, enum OW_FamilyCodes familyCode) {
	Error ret = NO_ERROR;
	uint8_t bit, compl;
	uint8_t bitNo = 0, deviceNo = 0;
	int8_t i;
	uint8_t repeatIndex = 0;
	bool stop = false;
	struct OW_device foundDevice = {.dev.devFullID = 0};
	static struct owSearchStatus status[_1W_ROM_SIZE];

	memset(status, 0, sizeof(*status)*_1W_ROM_SIZE);

	do {
		bool b;

		foundDevice.dev.devFullID = 0;

		_1w_reset();
		b = _1w_isDevice();

		if (!b) break; //there is no more devices to find.

		_1w_sendByte(cmdId);

		if (repeatIndex) {
			for (i=0; i<=repeatIndex; ++i) {
				_1w_readBit(&bit);
				_1w_readBit(&compl);
				_1w_sendBit(status[i].lastValue);
				foundDevice.dev.devFullID |= ((uint64_t)status[i].lastValue<<i);
			}
			bitNo = repeatIndex+1;
		} else if (familyCode != OW_Family_NULL) {
			for (i=0; i<=7; ++i) {
				_1w_readBit(&bit);
				_1w_readBit(&compl);
				_1w_sendBit(familyCode & (1<<i));
				status[i].lastValue = (familyCode>>i) & 1;
			}
			foundDevice.dev.laseredRom.family = familyCode;
			bitNo = 7+1;
		}

		do {
			_1w_readBit(&bit);
			_1w_readBit(&compl);

			if (bit != 1 || compl != 1) {
				if (bit != compl) {
					foundDevice.dev.devFullID |= ((uint64_t)bit<<bitNo);
					_1w_sendBit(bit);
					status[bitNo].lastValue = bit;
				} else {
					if (status[bitNo].forkDetected == 0) {
						status[bitNo].forkDetected = 1;
						status[bitNo].lastValue = 1;
						foundDevice.dev.devFullID |= ((uint64_t)1<<bitNo);
						_1w_sendBit(1);
					} else {
						status[bitNo].lastValue = 0;
						_1w_sendBit(0);
					}
				}
				++bitNo;
			} else {
				ret = ERROR_BUS_ERROR;
				break;
			}
		} while (bitNo < 64);

		//find open fork
		for (i=bitNo-1; i>=0; --i) {
			if (status[i].forkDetected) {
				if (status[i].lastValue == 0) {
					status[i].forkDetected = 0;
				} else {
					status[i].lastValue = 0;
					repeatIndex = i;
					break;
				}
			}
		}
		if (i<0) stop = true;

		if (ret == NO_ERROR) {
			if (foundDevice.dev.devFullID != 0 && deviceNo < *cnt) {
				deviceList[deviceNo].dev.devFullID = foundDevice.dev.devFullID;
			}

			++deviceNo;
		}
	} while (!stop);

	*cnt = deviceNo;

	return ret;
}

Error OW_SearchRom(struct OW_device* deviceList, uint8_t *cnt /*in-out*/, enum OW_FamilyCodes familyCode) {
	return _1w_searchRom(deviceList, cnt, CMD_SEARCH_ROM, familyCode);
}
Error OW_DS18x20_SearchAlarm(struct OW_device* deviceList, uint8_t *cnt /*in-out*/, enum OW_FamilyCodes familyCode) {
	return _1w_searchRom(deviceList, cnt, CMD_SEARCH_ALARM, familyCode);
}

Error OW_MatchRom(struct OW_device* device) {
	Error ret = NO_ERROR;
	uint8_t i=0;
	uint64_t s = 0;

	if (device == NULL) return ERROR_INVALID_PARAMETER;

	_1w_sendByte(CMD_MATCH_ROM);

	for (; i<64; ++i) {
		ret = _1w_sendBit((uint8_t) ((device->dev.devFullID >> i) & 1));

		_delay_us(50);
		s+= ((device->dev.devFullID >> i) & 1)<<i;

	}

	//foundDevice.dev.devFullID |= ((uint64_t)bit<<bitNo);

	return ret;
}
