/*
 * main.c
 *
 *  Created on: 23 cze 2015
 *      Author: Pawe³
 */

#include "avr/io.h"
#include "util/delay.h"

#include "drv/drv_time.h"
#include "drv/LCD/HD44780.h"
#include "drv/peripheral/peripherials.h"
#include "drv/kbd/kbd.h"
#include "drv/port/port.h"

static void _onKeyCbf (enum KbdKey key, enum KbdState state) {
	char text[11];

	LCD_GoTo(0, 1);
	snprintf(text, 11, "State: %3d", key);
	LCD_WriteText(text);

}

int main (void) {
	//char text[81];
	int i=0;
	struct Kbd kbd = {
		.matrix = {
				{KBD_KEY_0, KBD_KEY_1, KBD_KEY_2, KBD_KEY_UNDEFINIED},
				{KBD_KEY_3, KBD_KEY_4, KBD_KEY_5, KBD_KEY_UNDEFINIED},
				{KBD_KEY_6, KBD_KEY_7, KBD_KEY_8, KBD_KEY_UNDEFINIED},
				{KBD_KEY_9, KBD_KEY_ENTER, KBD_KEY_CANCEL, KBD_KEY_UNDEFINIED},
		},
		.callback = _onKeyCbf,
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

	LCD_Initalize();

	LCD_WriteText(text);
	Peripherials_Initialize();
	//PIN_CONFIG(DDRA, PA6, PIN_INPUT);
	Kbd_Initialize();
	Kbd_Register(kbd, 0);

	_onKeyCbf(KBD_KEY_UNDEFINIED, 0);

	while (1)
	{
		//snprintf(&text[72], 9, "%8d ", i++);
		///text[79] = (i++) + 0x30;
		//i %= 10;

		Kbd_Scan();
		_delay_ms(200);
		//PIN_SET(PORTA, PA6, 0);
		//peripherials_set(PERIPH_PUMP, PERIPH_STATE_ON);
	}

	return 0;
}
