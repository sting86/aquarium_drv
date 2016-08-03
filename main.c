/*
 * main.c
 *
 *  Created on: 23 cze 2015
 *      Author: Pawe³
 */

#include "avr/io.h"
#include "avr/interrupt.h"
#include "util/delay.h"

#include "drv/drv_time.h"
#include "drv/LCD/HD44780.h"
#include "drv/peripheral/peripherials.h"
#include "drv/kbd/kbd.h"
#include "drv/port/port.h"
#include "drv/rtc/rtc.h"

static void _onKeyCbf (enum KbdKey key, enum KbdState state) {
//	char text[21];

//	LCD_GoTo(0, 1);
//	snprintf(text, 21, "Key: %2d, State: %d", key, state);
//	LCD_WriteText(text);

	static uint8_t i = 0;

	if (state == KBD_STATE_KEY_DOWN) {
		switch (key) {
			case KBD_KEY_PUMP:
				Peripherials_Set(PERIPH_PUMP, i);
				i = !i;
				break;

			case KBD_KEY_FEEDER:
				Peripherials_Set(PERIPH_FEEDER, i);
				i = !i;
				break;

			case KBD_KEY_NIGHTLIGHT:
				Peripherials_Set(PERIPH_NIGHTLIGHT, i);
				i = !i;
				break;

			default:
				break;
		}
	}
}

void _onTimeCbf() {
	char text[21];
	static uint16_t i = 0;
	struct RTC_Time time = {0};

	RTC_GetTime(&time);

	LCD_GoTo(0, 2);
	snprintf(text, 21, "t[%04d] = %02d:%02d:%02d     ", ++i, time.hour, time.min, time.sec);
	LCD_WriteText(text);
}

int main (void) {
	//char text[81];
//	int i=0;
	static struct Kbd kbd = {
		.matrix = {
				{KBD_KEY_UNDEFINIED, KBD_KEY_MENU,   KBD_KEY_PUMP,       KBD_KEY_UNDEFINIED},
				{KBD_KEY_LEFT,       KBD_KEY_UP,     KBD_KEY_DAYLIGHT,   KBD_KEY_UNDEFINIED},
				{KBD_KEY_RIGHT,      KBD_KEY_DOWN,   KBD_KEY_FEEDER,     KBD_KEY_UNDEFINIED},
				{KBD_KEY_ENTER,      KBD_KEY_CANCEL, KBD_KEY_NIGHTLIGHT, KBD_KEY_UNDEFINIED},
		},
		.callback = _onKeyCbf,
	};

	static struct RTC_init rtcInitParams = {
		.onSecChangedCbf = _onTimeCbf,
	};

	char text[81]="ABCDEFGHIJKLMNOPRSTW"
	              "NIE TA LINIA :(     "
	              "                    "
		          "Maciek mowi:        ";//PRSTUVWXYZabcdefghijklmnoprstuvwxyz1234567890 [];'\\,./!@#$%^&*()`";

	{ // TODO: Cos od LCD, nie pamietam, do ustalenia i przerobienia
		DDRB |= 1<<PB4;
		//turn on LCD
		PORTB &= ~(1<<PB4);

		//piezzo?
		DDRB |= 1<<PB2;
		PORTB &= ~(1<<PB2);
		//PORTB |= (1<<PB2);


		DDRD |= 1<<PD5;
		//turn on light
		PORTD &= ~(1<<PD5);

		//turn off bg_light
		//PORTD |= (1<<PD5);


		_delay_us(500);

	}

	sei();

	LCD_Initalize();

	LCD_WriteText(text);
	Peripherials_Initialize();
	Kbd_Initialize();
	RTC_Initialize(&rtcInitParams);
	Kbd_Register(kbd, 0);

	while (1)
	{
		Kbd_Scan();

		_delay_ms(200);
	}

	return 0;
}
