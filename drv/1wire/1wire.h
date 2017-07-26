/*
 * 1wire.h
 *
 *  Created on: 15 maj 2017
 *      Author: Pawe³
 */

#ifndef DRV_1WIRE_1WIRE_H_
#define DRV_1WIRE_1WIRE_H_

#include "avr/io.h"

#include "config.h"
#include "commands.h"
#include "drv/port/port.h"
#include "framework/error.h"
#include "framework/typedefs.h"




struct OW_device {
	union {
		uint8_t id[8];
		struct {
			uint64_t family: 8;
			uint64_t sn    :48;
			uint64_t crc   : 8;
		} laseredRom;
		uint64_t devFullID;
	} dev;
};

struct OW {
  bool work;
};

Error OW_Initialize();
//Error OW_Init();
Error OW_Magic();


#endif /* DRV_1WIRE_1WIRE_H_ */
