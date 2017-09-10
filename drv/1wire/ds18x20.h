/*
 * ds18x20.h
 *
 *  Created on: 9 wrz 2017
 *      Author: Pawe³
 */

#ifndef DRV_1WIRE_DS18X20_H_
#define DRV_1WIRE_DS18X20_H_

#include "drv/port/port.h"
#include "framework/error.h"
#include "framework/typedefs.h"

#include "1wire.h"

Error OW_DS18x20_SearchAlarm(struct OW_device* deviceList, uint8_t *cnt /*in-out*/, enum OW_FamilyCodes familyCode);

Error OW_DS18x20_StartConversion(struct OW_device* device);
Error OW_DS18x20_ReadTemp(struct OW_device* device, uint16_t *result);
Error OW_DS18b20_ConvertTemp(uint16_t stamp, int8_t *degrees, uint8_t *rest);
Error OW_DS1820_ConvertTemp(uint16_t stamp, int8_t *degrees, uint8_t *rest);


#endif /* DRV_1WIRE_DS18X20_H_ */
