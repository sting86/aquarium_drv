/*
 * commands.h
 *
 *  Created on: 22 lip 2017
 *      Author: Pawe�
 */

#ifndef DRV_1WIRE_COMMANDS_H_
#define DRV_1WIRE_COMMANDS_H_

enum OW_CMD {
	CMD_SEARCH_ROM=0xF0,
	CMD_MATCH_ROM =0x55,
	CMD_READ_ROM  =0x33,
	CMD_SKIP_ROM  =0xCC
};

#endif /* DRV_1WIRE_COMMANDS_H_ */
