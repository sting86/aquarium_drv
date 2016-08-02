/*
 * kbd.c
 *
 *  Created on: 29 lip 2016
 *      Author: Pawe³
 */

#include "config.h"
#include "kbd.h"
#include "drv/port/port.h"
#include <string.h>
#include "drv/LCD/HD44780.h"

#define _X_PATERN 0b1000100010001
#define _Y_PATERN 0b1111


struct Kbd currentKeyboard;

uint8_t _y(uint16_t scan) {
	uint8_t y= 0;

	for (y = 0; y < 4; ++y) {
		if (_Y_PATERN<<(y*4) & scan) return y;
	}

	return 0;
}

uint8_t _x(uint16_t scan) {
	uint8_t x= 0;

	for (x = 0; x < 4; ++x) {
		if (_X_PATERN<<x & scan) return x;
	}

	return 0;
}

bool redisterd = false;
/**
 * bits No in kyeboard matrix
 * [0][1][2][3]
 * [4][5][6][7]
 * [8][9][A][B]
 * [C][D][E][F]
 *
 */

uint16_t scanState = 0;

void _debug(char *txt, uint8_t line) {
	LCD_GoTo(0, line);
	LCD_WriteText("                    ");
	LCD_GoTo(0, line);
	LCD_WriteText(txt);
}

Error Kbd_Scan () {
	Error ret = NO_ERROR;
	bool error = false;
	uint16_t scanTemp = 0;
	char dbg[21];

	if (currentKeyboard.callback != NULL) {
		uint8_t i, pressed = 0;

		for (i=0; i<3; i++) {
			switch (i) {
				case 0:
					PIN_CONFIG(LINE1_DDR, LINE1, PIN_OUTPUT);
					PIN_SET(LINE1_PORT,   LINE1, OUT_LO);
					PIN_CONFIG(LINE2_DDR, LINE2, PIN_INPUT);
					PIN_CONFIG(LINE3_DDR, LINE3, PIN_INPUT);
					break;

				case 1:
					PIN_CONFIG(LINE1_DDR, LINE1, PIN_INPUT);
					PIN_CONFIG(LINE2_DDR, LINE2, PIN_OUTPUT);
					PIN_SET(LINE2_PORT,   LINE2, OUT_LO);
					PIN_CONFIG(LINE3_DDR, LINE3, PIN_INPUT);
					break;

				case 2:
					PIN_CONFIG(LINE1_DDR, LINE1, PIN_INPUT);
					PIN_CONFIG(LINE2_DDR, LINE2, PIN_INPUT);
					PIN_CONFIG(LINE3_DDR, LINE3, PIN_OUTPUT);
					PIN_SET(LINE3_PORT,   LINE3, OUT_LO);
					break;

				case 3://used in /full 4x4 matrixes
					PIN_CONFIG(LINE1_DDR, LINE1, PIN_INPUT);
					PIN_CONFIG(LINE2_DDR, LINE2, PIN_INPUT);
					PIN_CONFIG(LINE3_DDR, LINE3, PIN_INPUT);
					break;
			}
			_delay_us(200);

			//TODO: simplicity - only set bits, not need to clear, cause scanTemp is already initialized as 0
			if (!(COLUMN1_PIN & (1<<COLUMN1))) {
				scanTemp |= (1<<scanMap[i][0]);
			} else {
				scanTemp &= ~(1<<scanMap[i][0]);
			}

			if (!(COLUMN2_PIN & (1<<COLUMN2))) {
				scanTemp |= (1<<scanMap[i][1]);
			} else {
				scanTemp &= ~(1<<scanMap[i][1]);
			}

			if (!(COLUMN3_PIN & (1<<COLUMN3))) {
				scanTemp |= (1<<scanMap[i][2]);
			} else {
				scanTemp &= ~(1<<scanMap[i][2]);
			}

			if (!(COLUMN4_PIN & (1<<COLUMN4))) {
				scanTemp |= (1<<scanMap[i][3]);
			} else {
				scanTemp &= ~(1<<scanMap[i][3]);
			}
		}

		for (i = 0; i<16; ++i) {
			if (scanTemp & (1<<i)) ++pressed;
		}

		if (pressed>1) error = true;

		if (!error && (scanTemp != scanState)) {
			if (scanState != 0) {
				currentKeyboard.callback(currentKeyboard.matrix[_y(scanState)][_x(scanState)], KBD_STATE_KEY_UP);
			}
			scanState = scanTemp;
			snprintf(dbg, 21, "Scan x=%d y=%d", _x(scanState), _y(scanState));
			_debug(dbg, 0);
			if (scanState != 0) {
				currentKeyboard.callback(currentKeyboard.matrix[_y(scanState)][_x(scanState)], KBD_STATE_KEY_DOWN);
			}
		} else if (error) {
			currentKeyboard.callback(KBD_KEY_UNDEFINIED, KBD_STATE_ERROR);
		}
	}

	return ret;
}


Error Kbd_Initialize () {
	Error ret = NO_ERROR;

	memset(&currentKeyboard, 0, sizeof(currentKeyboard));

	PIN_CONFIG(COLUMN1_DDR, COLUMN1, PIN_INPUT);
	PIN_SET(COLUMN1_PORT,   COLUMN1, IN_PULLUP);
	PIN_CONFIG(COLUMN2_DDR, COLUMN2, PIN_INPUT);
	PIN_SET(COLUMN2_PORT,   COLUMN2, IN_PULLUP);
	PIN_CONFIG(COLUMN3_DDR, COLUMN3, PIN_INPUT);
	PIN_SET(COLUMN3_PORT,   COLUMN3, IN_PULLUP);
	PIN_CONFIG(COLUMN4_DDR, COLUMN4, PIN_INPUT);
	PIN_SET(COLUMN4_PORT,   COLUMN4, IN_PULLUP);
	PIN_CONFIG(LINE1_DDR,   LINE1,   PIN_INPUT);
	PIN_CONFIG(LINE2_DDR,   LINE2,   PIN_INPUT);
	PIN_CONFIG(LINE3_DDR,   LINE3,   PIN_INPUT);
	//PIN_CONFIG(LINE4_DDR, LINE4,   PIN_INPUT);


	return ret;
}


Error Kbd_Register (struct Kbd kbd, enum KbdState stateMask) {
	Error ret = NO_ERROR;

	memcpy(&currentKeyboard, &kbd, sizeof(currentKeyboard));

	return ret;
}
