/*
 * 1wire.c
 *
 *  Created on: 15 maj 2017
 *      Author: Pawe³
 */

#include "1wire.h"

#include "util/delay.h"

#include "avr/pgmspace.h"

#include "drv/LCD/HD44780.h"
#include "config.h"
#include "../../framework/crc8.h"

static Error _1w_reset(); //pill down for 480us data bus
static bool  _1w_isDevice(); //Sample the bus for slave response (240us)
static Error _1w_sendBit(uint8_t);//pull down for 6us then release for next 54us (1)
                                  //or pull down for 60us(0)
static Error _1w_readBit(uint8_t *bit);//drive the bus low for 6us, then after 9us sample bus for read.

static Error _1w_sendByte(uint8_t byte);
static Error _1w_searchRom(struct OW_device* deviceList, uint8_t *cnt /*in-out*/, uint8_t cmdId, enum OW_FamilyCodes familyCode);

//#define _1W_ONE() \
//		PIN_CONFIG(OW_DDR, OW_DATA, PIN_INPUT);\
//		PIN_SET(OW_PORT,   OW_DATA, IN_HI_Z);
////		PIN_CONFIG(OW_DDR, PA1, PIN_INPUT);
////		PIN_SET(OW_PORT,   PA1, IN_HI_Z)
//
//#define _1W_ZERO() \
//		PIN_CONFIG(OW_DDR, OW_DATA, PIN_OUTPUT);\
//		PIN_SET(OW_PORT,   OW_DATA, OUT_LO);
//		PIN_CONFIG(OW_DDR, PA1, PIN_OUTPUT);
//		PIN_SET(OW_PORT,   PA1, OUT_LO)

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


Error OW_Magic() {
	char text[21];
	Error ret;

	struct OW_device list[4];
	uint8_t cnt = 2, cnt2;

	ret = _1w_searchRom(list, &cnt, CMD_SEARCH_ROM, OW_Family_DS18B20);//OW_Family_NULL);
	cnt2=4-cnt;
	ret = _1w_searchRom(&list[cnt], &cnt2, CMD_SEARCH_ROM, OW_Family_DS1820);//OW_Family_NULL);
	cnt += cnt2;

	if (cnt>0) {

		uint16_t temp = 0;
		//ow_reset();
		_1w_reset();
		_1w_isDevice();
		//ow_write_byte(0xCC);
		//OW_MatchRom(&list[0]);
		//ow_write_byte(0x44);
		_1w_sendByte(0xCC);
		_1w_sendByte(CMD_CONVERT_TEMP);
		//for (uint8_t a = 0; a < ; a++) _delay_ms(30);

		_delay_ms(500);
//		ow_reset();
//		ow_write_byte(0xCC);
//		ow_write_byte(0xBE);
		_1w_reset();
		_1w_isDevice();
		//OW_MatchRom(&list[0]);
		_1w_sendByte(0xCC);
		_1w_sendByte(0xbe);
		for (uint8_t a = 0; a < 16; a++) {
			uint8_t bit = 0;
			_1w_readBit(&bit);

			temp |= ((uint16_t) bit << a);
		}


		//		lsb = ow_read_byte();
		//		msb = ow_read_byte();

				LCD_GoTo(0, 1);
				snprintf_P(text, 21, PSTR("Temp: %x"), temp);
				LCD_WriteText(text);
	} else {
				LCD_GoTo(0, 1);
				snprintf_P(text, 21, PSTR("No device found :("));
				LCD_WriteText(text);
	}

//	if (cnt>0) {
//		uint8_t i;
//		for (i=0; i<MIN(cnt, 4); ++i) {
//			LCD_GoTo(0, i);
//			snprintf_P(text, 21, PSTR("%02X%02X%02X%02X%02X%02X%02X%02X %d%02X"), list[i].dev.id[0], list[i].dev.id[1], list[i].dev.id[2], list[i].dev.id[3], list[i].dev.id[4], list[i].dev.id[5], list[i].dev.id[6], list[i].dev.id[7], ret, list[i].dev.laseredRom.family);
//			LCD_WriteText(text);
//		}
//	} else {
//		LCD_GoTo(0, 1);
//		snprintf_P(text, 21, PSTR("No device found :("));
//		LCD_WriteText(text);
//	}

	return ret;
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

	_delay_us(1);

	if (bit != 0) {
		_1W_ONE();
		_delay_us(20);
	} else {
		_delay_us(60);
	}

	_1W_ONE();

	return NO_ERROR;
}

static Error _1w_readBit(uint8_t *bit) {
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

static Error _1w_sendByte(uint8_t byte) {
	Error ret = NO_ERROR;
	int8_t i;
	for (i=0; i<8; ++i) {
		ret |= _1w_sendBit(byte&(1<<i));
	}

	return ret;
}

static Error _1w_searchRom(struct OW_device* deviceList, uint8_t *cnt /*in-out*/, enum OW_CMD cmdId, enum OW_FamilyCodes familyCode) {
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
Error OW_SearchAlarm(struct OW_device* deviceList, uint8_t *cnt /*in-out*/, enum OW_FamilyCodes familyCode) {
	return _1w_searchRom(deviceList, cnt, CMD_SEARCH_ALARM, familyCode);
}

Error OW_MatchRom(struct OW_device* device) {
	Error ret = NO_ERROR;
	uint8_t i=0;
	uint64_t s = 0;
	char text[21];

	if (device == NULL) return ERROR_INVALID_PARAMETER;

	_1w_sendByte(CMD_MATCH_ROM);

	for (; i<64; ++i) {
		ret = _1w_sendBit((uint8_t) ((device->dev.devFullID >> i) & 1));

		_delay_us(50);
		s+= ((device->dev.devFullID >> i) & 1)<<i;

	}
	LCD_GoTo(0, 2);
	snprintf(text, 21, "%lX%lX", (uint32_t) (device->dev.devFullID>>32), (uint32_t) device->dev.devFullID);
	LCD_WriteText(text);
	LCD_GoTo(0, 0);
	snprintf(text, 21, "%lX%lX", (uint32_t) (s>>32), (uint32_t) s);
	LCD_WriteText(text);
	//foundDevice.dev.devFullID |= ((uint64_t)bit<<bitNo);

	return ret;
}

