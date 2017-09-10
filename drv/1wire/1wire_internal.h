/*
 * 1wire_internal.h
 *
 *  Created on: 9 wrz 2017
 *      Author: Pawe³
 */

#ifndef DRV_1WIRE_1WIRE_INTERNAL_H_
#define DRV_1WIRE_1WIRE_INTERNAL_H_

#include "1wire.h"

#define _1W_ROM_SIZE (64)

Error _1w_reset();
bool _1w_isDevice();
Error _1w_sendBit(uint8_t bit);
Error _1w_readBit(uint8_t *bit);
Error _1w_sendByte(uint8_t byte);
Error _1w_searchRom(struct OW_device* deviceList, uint8_t *cnt /*in-out*/, enum OW_CMD cmdId, enum OW_FamilyCodes familyCode);


#endif /* DRV_1WIRE_1WIRE_INTERNAL_H_ */
