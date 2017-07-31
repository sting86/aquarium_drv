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
#include "drv/1wire/1wire.h"

bool updateTime = false;

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

			case KBD_KEY_LEFT: {
				struct RTC_Time time = {0};
				RTC_GetTime(&time);
				if (time.min>=5) time.min = time.min - 5; else time.min = 64 - time.min - 5;
				RTC_SetTime(&time);
				break;
			}

			case KBD_KEY_UP: {
				struct RTC_Time time = {0};
				RTC_GetTime(&time);
				time.min = (time.min + 5)%60;
				RTC_SetTime(&time);
				break;
			}

			case KBD_KEY_RIGHT: {
				struct RTC_Time time = {0};
				RTC_GetTime(&time);
				if (time.hour>=1) --time.hour; else time.hour = 23;
				RTC_SetTime(&time);
				break;
			}

			case KBD_KEY_DOWN: {
				struct RTC_Time time = {0};
				RTC_GetTime(&time);
				time.hour = (time.hour + 1)%24;
				RTC_SetTime(&time);
				break;
			}

			case KBD_KEY_ENTER: {
				struct RTC_Time time = {0};
				RTC_GetTime(&time);
				if (time.day>=2) --time.day; else time.day = 31;//TODO: should be max
				RTC_SetTime(&time);
				break;
			}

			case KBD_KEY_CANCEL: {
				struct RTC_Time time = {0};
				RTC_GetTime(&time);
				time.day = (time.day)%31 + 1;
				RTC_SetTime(&time);
				break;
			}

			case KBD_KEY_MENU: {
				struct RTC_Time time = {0};
				RTC_GetTime(&time);
				time.month = (time.month)%12 + 1;
				RTC_SetTime(&time);
				break;
			}

			case KBD_KEY_DAYLIGHT: {
				struct RTC_Time time = {0};
				RTC_GetTime(&time);
				++time.year;
				RTC_SetTime(&time);
				break;
			}

			default:
				break;
		}
	}
}

void _onTimeCbf() {
	updateTime = true;
	char text[21];
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

	//TODO: Refactor LCD driver: add correction to write, change of mode (with/without cursor etc.)
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

	_delay_ms(500);
	LCD_Initalize();

	sei();


	LCD_WriteText(text);
	Peripherials_Initialize();
	Kbd_Initialize();
	RTC_Initialize(&rtcInitParams);
	Kbd_Register(kbd, 0);

//	PIN_CONFIG(DDRB, PB2, PIN_OUTPUT); //DS1820
//	while (1)
//	{
//		PIN_SET(PORTB, PB2, OUT_HI);
//		_delay_us(400);
//		PIN_SET(PORTB, PB2, OUT_LO);
//		_delay_us(400);
//	}
	OW_Initialize();

	while (1)
	{
		if (updateTime) {
			struct RTC_Time time = {0};
			const char* dow = RTC_GetDayName(RTC_GetDayOfWeek());

			RTC_GetTime(&time);

			LCD_GoTo(0, 2);
			snprintf(text, 21, "%s%04d%02d%02d %02d:%02d:%02d ", dow, (uint16_t)time.year + RTC_BASE_YEAR, time.month, time.day, time.hour, time.min, time.sec);
			LCD_WriteText(text);
		}
		OW_Magic();


		_delay_ms(1000);
	}

	while (1)
	{
		Kbd_Scan();

		//_delay_ms(200);
	}

	return 0;
}
