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


struct Kbd currentKeyboard;
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
uint8_t scanMap[4][4] = {
		{ 0,  1,  2,  3},
		{ 4,  5,  6,  7},
		{ 8,  9, 10, 11},
		{12, 13, 14, 15},
};


Error Kbd_Scan () {
	Error ret = NO_ERROR;

	if (currentKeyboard.callback != NULL) {
		uint8_t i, pressed = 0;
		for (i=0; i<4; i++) {
			switch (i) {
				case 0:
					PIN_CONFIG(LINE1_DDR,   LINE1_PIN,   PIN_OUTPUT);
					PIN_CONFIG(LINE2_DDR,   LINE2_PIN,   PIN_INPUT);
					PIN_CONFIG(LINE3_DDR,   LINE3_PIN,   PIN_INPUT);
//					LINE1_DDR |= 1<<LINE1_PIN;
//					LINE2_DDR &= ~(1<<LINE2_PIN);
//					LINE3_DDR &= ~(1<<LINE3_PIN);
					break;
//
				case 1:
					PIN_CONFIG(LINE1_DDR,   LINE1_PIN,   PIN_INPUT);
					PIN_CONFIG(LINE2_DDR,   LINE2_PIN,   PIN_OUTPUT);
					PIN_CONFIG(LINE3_DDR,   LINE3_PIN,   PIN_INPUT);
//					LINE1_DDR &= ~(1<<LINE1_PIN);
//					LINE2_DDR |= 1<<LINE2_PIN;
//					LINE3_DDR &= ~(1<<LINE3_PIN);
					break;
//
				case 2:
					PIN_CONFIG(LINE1_DDR,   LINE1_PIN,   PIN_INPUT);
					PIN_CONFIG(LINE2_DDR,   LINE2_PIN,   PIN_INPUT);
					PIN_CONFIG(LINE3_DDR,   LINE3_PIN,   PIN_OUTPUT);
//					LINE1_DDR &= ~(1<<LINE1_PIN);
//					LINE2_DDR &= ~(1<<LINE2_PIN);
//					LINE3_DDR |= 1<<LINE3_PIN;
					break;
//
				case 3://used in /full 4x4 matrixes
					PIN_CONFIG(LINE1_DDR,   LINE1_PIN,   PIN_INPUT);
					PIN_CONFIG(LINE2_DDR,   LINE2_PIN,   PIN_INPUT);
					PIN_CONFIG(LINE3_DDR,   LINE3_PIN,   PIN_INPUT);
//					LINE1_DDR &= ~(1<<LINE1_PIN);
//					LINE2_DDR &= ~(1<<LINE2_PIN);
//					LINE3_DDR &= ~(1<<LINE3_PIN);
					break;
			}

			if (COLUMN1_PORT & (1<<COLUMN1_PIN)) {
				if ((scanState & (1<<scanMap[i][0])) == 0) {
					scanState |= (1<<scanMap[i][0]);
					currentKeyboard.callback(currentKeyboard.matrix[i][0], KBD_STATE_KEY_DOWN);
				}
			} else {
				if (scanState & (1<<scanMap[i][0])) {
					scanState &= ~(1<<scanMap[i][0]);
					currentKeyboard.callback(currentKeyboard.matrix[i][0], KBD_STATE_KEY_UP);
				}
			}

			if (COLUMN2_PORT & (1<<COLUMN2_PIN)) {
				if ((scanState & (1<<scanMap[i][1])) == 0) {
					scanState |= (1<<scanMap[i][1]);
					currentKeyboard.callback(currentKeyboard.matrix[i][1], KBD_STATE_KEY_DOWN);
				}
			} else {
				if (scanState & (1<<scanMap[i][1])) {
					scanState &= ~(1<<scanMap[i][1]);
					currentKeyboard.callback(currentKeyboard.matrix[i][1], KBD_STATE_KEY_UP);
				}
			}

			if (COLUMN3_PORT & (1<<COLUMN3_PIN)) {
				if ((scanState & (1<<scanMap[i][2])) == 0) {
					scanState |= (1<<scanMap[i][2]);
					currentKeyboard.callback(currentKeyboard.matrix[i][2], KBD_STATE_KEY_DOWN);
				}
			} else {
				if (scanState & (1<<scanMap[i][2])) {
					scanState &= ~(1<<scanMap[i][2]);
					currentKeyboard.callback(currentKeyboard.matrix[i][2], KBD_STATE_KEY_UP);
				}
			}

			if (COLUMN4_PORT & (1<<COLUMN4_PIN)) {
				if ((scanState & (1<<scanMap[i][3])) == 0) {
					scanState |= (1<<scanMap[i][3]);
					currentKeyboard.callback(currentKeyboard.matrix[i][3], KBD_STATE_KEY_DOWN);
				}
			} else {
				if (scanState & (1<<scanMap[i][3])) {
					scanState &= ~(1<<scanMap[i][3]);
					currentKeyboard.callback(currentKeyboard.matrix[i][3], KBD_STATE_KEY_UP);
				}
			}
		}
	}

	return ret;
}


Error Kbd_Initialize () {
	Error ret = NO_ERROR;

	memset(&currentKeyboard, 0, sizeof(currentKeyboard));

	PIN_CONFIG(COLUMN1_DDR, COLUMN1_PIN, PIN_INPUT);
	PIN_CONFIG(COLUMN2_DDR, COLUMN2_PIN, PIN_INPUT);
	PIN_CONFIG(COLUMN3_DDR, COLUMN3_PIN, PIN_INPUT);
	PIN_CONFIG(COLUMN4_DDR, COLUMN4_PIN, PIN_INPUT);
	PIN_CONFIG(LINE1_DDR,   LINE1_PIN,   PIN_INPUT);
	PIN_CONFIG(LINE2_DDR,   LINE2_PIN,   PIN_INPUT);
	PIN_CONFIG(LINE3_DDR,   LINE3_PIN,   PIN_INPUT);
	//PIN_CONFIG(LINE4_DDR, LINE4_PIN,   PIN_INPUT);


	return ret;
}


Error Kbd_Register (struct Kbd kbd, enum KbdState stateMask) {
	Error ret = NO_ERROR;

	memcpy(&currentKeyboard, &kbd, sizeof(currentKeyboard));

	return ret;
}
