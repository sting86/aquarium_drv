/*
 * 1wire.c
 *
 *  Created on: 15 maj 2017
 *      Author: Pawe�
 */

#include "1wire.h"

#include "util/delay.h"

#include "drv/LCD/HD44780.h"
#include "../../framework/crc8.h"

static Error _1w_reset(); //pill down for 480us data bus
static bool  _1w_isDevice(); //Sample the bus for slave response (240us)
static Error _1w_sendBit(uint8_t);//pull down for 6us then release for next 54us (1)
                                  //or pull down for 60us(0)
static Error _1w_readBit(uint8_t *bit);//drive the bus low for 6us, then after 9us sample bus for read.

static Error _1w_sendByte(uint8_t byte);
static Error _1w_searchRom(struct OW_device* deviceList, uint8_t *cnt);

#define _1W_ONE() \
		PIN_CONFIG(OW_DDR, OW_DATA, PIN_INPUT);\
		PIN_SET(OW_PORT,   OW_DATA, IN_HI_Z);\
//		PIN_CONFIG(OW_DDR, PA1, PIN_INPUT);\
//		PIN_SET(OW_PORT,   PA1, IN_HI_Z)

#define _1W_ZERO() \
		PIN_CONFIG(OW_DDR, OW_DATA, PIN_OUTPUT);\
		PIN_SET(OW_PORT,   OW_DATA, OUT_LO);\
//		PIN_CONFIG(OW_DDR, PA1, PIN_OUTPUT);\
//		PIN_SET(OW_PORT,   PA1, OUT_LO)

#define WAIT()__asm__ __volatile__ ("nop")

struct owSearchStatus {
	uint8_t lastValue    :1;
	uint8_t forkDetected :1;
};

Error OW_Initialize() {

	_1W_ONE();

	return NO_ERROR;
}


Error OW_Magic() {
//	bool b;
	char text[21];
	Error ret;
//	_1w_reset();
//	b = _1w_isDevice();
	struct OW_device list[2];
	uint8_t cnt = 2;

	//_1w_sendByte(CMD_SEARCH_ROM);

	ret = _1w_searchRom(list, &cnt);

	if (cnt>0) {
		LCD_GoTo(0, 1);
		snprintf(text, 21, "%02X%02X%02X%02X%02X%02X%02X%02X %d%02X", list[0].dev.id[0], list[0].dev.id[1], list[0].dev.id[2], list[0].dev.id[3], list[0].dev.id[4], list[0].dev.id[5], list[0].dev.id[6], list[0].dev.id[7], ret, list[0].dev.laseredRom.family);
		LCD_WriteText(text);
		LCD_GoTo(0, 3);
		//snprintf(text, 21, "A: %08lx 0x%02X", (uint32_t) list[0].dev.laseredRom.sn, crc8(list[0].dev.id, 7));
		snprintf(text, 21, "%02X%02X%02X%02X%02X%02X%02X%02X %d%02X", list[1].dev.id[0], list[1].dev.id[1], list[1].dev.id[2], list[1].dev.id[3], list[1].dev.id[4], list[1].dev.id[5], list[1].dev.id[6], list[1].dev.id[7], ret, list[1].dev.laseredRom.family);
		LCD_WriteText(text);
//		LCD_GoTo(0, 3);
//		snprintf(text, 21, "A: %-15ld", ++i);
//		LCD_WriteText(text);
	} else {
		LCD_GoTo(0, 1);
		snprintf(text, 21, "No device found :(");
		LCD_WriteText(text);
	}

	return NO_ERROR;
}


static Error _1w_reset() {

	_1W_ZERO();
	_delay_us(480);

	_1W_ONE();
	return NO_ERROR;
}

static bool _1w_isDevice() {
	uint8_t i;
	bool ret = false;

	_1W_ONE();

	for (i = 0; i < 24; ++i) {
		if ((OW_PIN & (1<<OW_DATA)) == 0) ret = true;
		_delay_us(10);
	}

	return ret;
}

static Error _1w_sendBit(uint8_t bit) {
	_1W_ZERO();

	//WAIT();

	if (bit != 0) {
		_1W_ONE();
	}

	_delay_us(50);

	_1W_ONE();

	return NO_ERROR;
}

static Error _1w_readBit(uint8_t *bit) {
	if (bit == NULL) return ERROR_INVALID_PARAMETER;

	_1W_ZERO();
	//_delay_us(6);

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

static Error _1w_sendByte(uint8_t byte) {
	Error ret = NO_ERROR;
	int8_t i;
	for (i=0; i<8; ++i) {
		ret |= _1w_sendBit(byte&(1<<i));
	}

	return ret;
}

static Error _1w_searchRom(struct OW_device* deviceList, uint8_t *cnt /*in-out*/) {
	Error ret = NO_ERROR;
	uint8_t bit, compl;
	uint8_t bitNo = 0, deviceNo = 0;
	int8_t i;
	uint8_t repeatIndex = 0;
	bool stop = false;
	struct OW_device foundDevice = {.dev.devFullID = 0};
	static struct owSearchStatus status[64] = {0};

	do {
		bool b;

		foundDevice.dev.devFullID = 0;

		_1w_reset();
		b = _1w_isDevice();

		if (!b) break; //there is no more devices to find.

		_1w_sendByte(CMD_SEARCH_ROM);

		if (repeatIndex) {
			for (i=0; i<=repeatIndex; ++i) {
				_1w_readBit(&bit);
				_1w_readBit(&compl);
				_1w_sendBit(status[i].lastValue);
				foundDevice.dev.devFullID |= ((uint64_t)status[i].lastValue<<i);
			}
			bitNo = repeatIndex+1;
		}

		do {
			_1w_readBit(&bit);
			PIN_CONFIG(OW_DDR, PA1, PIN_OUTPUT);
			PIN_SET(OW_PORT,   PA1, OUT_LO);

			_1w_readBit(&compl);
			PIN_CONFIG(OW_DDR, PA1, PIN_INPUT);
			PIN_SET(OW_PORT,   PA1, IN_HI_Z);

			if (bit != 1 || compl != 1) {
				if (bit != compl) {
					foundDevice.dev.devFullID |= ((uint64_t)bit<<bitNo);
					_1w_sendBit(bit);
					status[bitNo].lastValue = bit;
				} else {//TODO: here should be the magic with finding another devices in another branches
					//_1w_sendBit(0);
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

		if (foundDevice.dev.devFullID != 0 && deviceNo < *cnt) {
			deviceList[deviceNo].dev.devFullID = foundDevice.dev.devFullID;
		}
		++deviceNo;
	} while (!stop);

	*cnt = deviceNo;

	return ret;
}



