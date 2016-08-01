/*
 * kbd.h
 *
 *  Created on: 29 lip 2016
 *      Author: Pawe³
 */

#ifndef DRV_KBD_KBD_H_
#define DRV_KBD_KBD_H_

#include "framework/error.h"
#include "framework/typedefs.h"

enum KbdKey {
	KBD_KEY_UNDEFINIED,
	KBD_KEY_MENU,
	KBD_KEY_PUMP,
	KBD_KEY_LEFT,
	KBD_KEY_RIGHT,
	KBD_KEY_UP,
	KBD_KEY_DOWN,
	KBD_KEY_ENTER,
	KBD_KEY_CANCEL,
	KBD_KEY_DAYLIGHT,
	KBD_KEY_NIGHTLIGHT,
	KBD_KEY_FEEDER,
	KBD_KEY_0,
	KBD_KEY_1,
	KBD_KEY_2,
	KBD_KEY_3,
	KBD_KEY_4,
	KBD_KEY_5,
	KBD_KEY_6,
	KBD_KEY_7,
	KBD_KEY_8,
	KBD_KEY_9,
};

enum KbdState {
	KBD_STATE_KEY_DOWN = 1<<0,
	KBD_STATE_KEY_UP = 1<<1,
	KBD_STATE_ERROR = 1<<2,
};

typedef void (*KbdStateCbf) (enum KbdKey key, enum KbdState state);

struct Kbd {
	enum KbdKey matrix[4][4];
	KbdStateCbf callback;
};

Error Kbd_Initialize ();

/**
 * This will  unregister old keyboard interface automatically
 * @param kbd - keyboard map, and callback function,
 * @param stateMask - state which we are want to know, when changed
 *
 */
Error Kbd_Register (struct Kbd kbd, enum KbdState stateMask);


Error Kbd_Scan ();

#endif /* DRV_KBD_KBD_H_ */
