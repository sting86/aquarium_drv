/*
 * main.c
 *
 *  Created on: 23 cze 2015
 *      Author: Pawe³
 */

#include "avr/io.h"
#include "avr/interrupt.h"
#include "util/delay.h"
#include "avr/pgmspace.h"
#include <string.h>

#include "framework/typedefs.h"

#include "drv/drv_time.h"
#include "drv/LCD/HD44780.h"
#include "drv/peripheral/peripherials.h"
#include "drv/kbd/kbd.h"
#include "drv/port/port.h"
#include "drv/rtc/rtc.h"
#include "drv/1wire/1wire.h"
#include "drv/uart/uart.h"

#include "framework/db/db.h"

bool updateTime = false;
bool newLine = false;
static void _onEOLRecv (drvUart *drv) {
	newLine = true;
}

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
	__flash const static char text[81]  ="ABCDEFGHIJKLMNOPRSTW"
	              "NIE TA LINIA :(     "
	              "                    "
		          "Maciek mowi:        ";//PRSTUVWXYZabcdefghijklmnoprstuvwxyz1234567890 [];'\\,./!@#$%^&*()`";

	{ // TODO: Cos od LCD, nie pamietam, do ustalenia i przerobienia
		DDRB |= 1<<PB4;
		//turn on LCD
		PORTB &= ~(1<<PB4);
		_delay_ms(500);
		PORTB |= (1<<PB4);
		_delay_ms(500);
		PORTB &= ~(1<<PB4);
		_delay_ms(500);
		PORTB |= (1<<PB4);
		_delay_ms(500);
		PORTB &= ~(1<<PB4);
		_delay_ms(500);
		PORTB |= (1<<PB4);
		_delay_ms(500);
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

	//pgm_read_byte()

	LCD_WriteTextP(text);
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
	UART_Initialize();
	DB_Initialize();

	{
		DBInstance *db = NULL;

		DB_GetDB(&db);

		while (1) {
			Error ret = NO_ERROR;

			struct RTC_Time alarm = {0};
			if (updateTime) {
				char text[21];

				struct RTC_Time time = {0};
				//PGM_P const dow = RTC_GetDayName(RTC_GetDayOfWeek());
				__flash const char* dow = RTC_GetDayName(RTC_GetDayOfWeek());

				RTC_GetTime(&time);

				LCD_GoTo(0, 3);
				snprintf_P(text, 21, PSTR("%S%04d%02d%02d %02d:%02d:%02d "), (dow), (uint16_t)time.year + RTC_BASE_YEAR, time.month, time.day, time.hour, time.min, time.sec);
				LCD_WriteText(text);
				updateTime = false;

				if (time.sec == 10) {
					ret = DB_PutEntry(db, DB_FIELD_ID_LIGHT_ON, (uint8_t*) &time, sizeof(struct RTC_Time));
				}
			}
			uint8_t size = sizeof(struct RTC_Time);
			DB_GetEntry(db, DB_FIELD_ID_LIGHT_ON, (uint8_t*) &alarm, &size);

			if (size != 0) {
				char text[21];
				LCD_GoTo(0, 0);
				snprintf_P(text, 21, PSTR("%04d%02d%02d %02d:%02d:%02d "), (uint16_t)alarm.year + RTC_BASE_YEAR, alarm.month, alarm.day, alarm.hour, alarm.min, alarm.sec);
				LCD_WriteText(text);
				updateTime = false;
			} else {
				char text[21];
				LCD_GoTo(0, 0);
				snprintf_P(text, 21, PSTR("%d %d --:--:-- "), ret, size);
				LCD_WriteText(text);
				updateTime = false;
			}
			_delay_ms(100);
		}
	}

	{
		__flash const static uint8_t customChar[] = {//TODO: move it to flash
			//null
			(__flash const uint8_t) 0x18,
			(__flash const uint8_t) 0x14,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0xa,
			(__flash const uint8_t) 0xe,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x2,
			(__flash const uint8_t) 0x3,
			//anthena
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x15,
			(__flash const uint8_t) 0x15,
			(__flash const uint8_t) 0xe,
			(__flash const uint8_t) 0x4,
			(__flash const uint8_t) 0x4,
			(__flash const uint8_t) 0x4,
			(__flash const uint8_t) 0x20,
			//signal max
			(__flash const uint8_t) 0x1e,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x1c,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x18,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x18,
			(__flash const uint8_t) 0x20,
			//signal NORMAL
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x1c,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x18,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x18,
			(__flash const uint8_t) 0x20,
			//signal low
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x18,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x18,
			(__flash const uint8_t) 0x20,
			//signal none
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x20,
			(__flash const uint8_t) 0x18,
			(__flash const uint8_t) 0x20,

			0x8,
			0x14,
			0x8,
			0x3,
			0x4,
			0x4,
			0x3,
			0x20,

			(__flash const uint8_t) 0x0,
		};

		LCD_WriteCommand(HD44780_CGRAM_SET);
		LCD_WriteTextP((const char*) customChar);
		LCD_WriteCommand(HD44780_DDRAM_SET);

		LCD_GoTo(3, 1);
		LCD_WriteData(0);
		LCD_WriteData(1);
		LCD_WriteData(2);
		LCD_WriteData(3);
		LCD_WriteData(4);
		LCD_WriteData(5);
		LCD_WriteData(6);
	}

	{
		drvUart *usart = NULL;
		struct uartInitParams params = {
				.parity = UART_PARITY_NONE,
				.dataBits = 8,
				.boundRate = 9600,
				.stopBits = UART_SB_ONE,
				.handShake = UART_HFC_NULL,
				.onEndLineReceived = _onEOLRecv,
		};

		UART_Open(&usart, &params);

		UART_ConfigDataBits(usart, 8);
		UART_ConfigParity(usart, UART_PARITY_NONE);
		UART_ConfigBound(usart, 9600);
		UART_ConfigStopBits(usart, UART_SB_ONE);

		UART_Connect(usart);

		while (0)
		{
			if (updateTime) {
				char text[21];

				struct RTC_Time time = {0};
				//PGM_P const dow = RTC_GetDayName(RTC_GetDayOfWeek());
				__flash const char* dow = RTC_GetDayName(RTC_GetDayOfWeek());

				RTC_GetTime(&time);

				LCD_GoTo(0, 3);
				snprintf_P(text, 21, PSTR("%S%04d%02d%02d %02d:%02d:%02d "), (dow), (uint16_t)time.year + RTC_BASE_YEAR, time.month, time.day, time.hour, time.min, time.sec);
				LCD_WriteText(text);
				updateTime = false;
			}

			if (newLine) {
				char text[50];
				char s[51] = {0};
				uint8_t size = 50;
				UART_ReadLine(usart, text, &size);

				//LCD_WriteText(text);

				newLine = false;
				snprintf_P	(s, 51, PSTR("czyt: %s, %u"), text, size);

				LCD_WriteText(s);


			}

			_delay_ms(100);
		}

		UART_Disconnect(usart);
		UART_Close(usart);
	}

	while (1)
	{
		if (updateTime) {
			char text[21];

			struct RTC_Time time = {0};
			//PGM_P const dow = RTC_GetDayName(RTC_GetDayOfWeek());
			__flash const char* dow = RTC_GetDayName(RTC_GetDayOfWeek());

			RTC_GetTime(&time);

			LCD_GoTo(0, 3);
			snprintf_P(text, 21, PSTR("%S%04d%02d%02d %02d:%02d:%02d "), (dow), (uint16_t)time.year + RTC_BASE_YEAR, time.month, time.day, time.hour, time.min, time.sec);
			LCD_WriteText(text);
			updateTime = false;
		}

		{
			struct OW_device list[4];
			int8_t temp;
			uint8_t cnt = 2, rest=100; //rest cannot be higher than 100
			uint16_t tempStamp;

			OW_SearchRom(list, &cnt, OW_Family_NULL);

			if (cnt>0) {
				static uint8_t index = 0;
				++index;
				index = index % cnt;

				OW_DS18x20_StartConversion(&list[index]);
				_delay_ms(800);

				OW_DS18x20_ReadTemp(&list[index], &tempStamp);

				if (list[index].dev.laseredRom.family == OW_Family_DS18B20) {
					OW_DS18b20_ConvertTemp(tempStamp, &temp, &rest);
				} else if (list[index].dev.laseredRom.family == OW_Family_DS1820) {
					OW_DS1820_ConvertTemp(tempStamp, &temp, &rest);
				}

				if (rest < 100) {
					char text[21];
					LCD_GoTo(0, 1);
					snprintf_P(text, 21, PSTR("Temp[%d]: %d,%02d\6     "), index, temp, rest);
					LCD_WriteText(text);
				}
			} else {
				char text[21];
				LCD_GoTo(0, 1);
				snprintf_P(text, 21, PSTR("No device found :(  "));
				LCD_WriteText(text);
			}
		}

		_delay_ms(100);
	}

	while (1)
	{
		if (updateTime) {
			char text[21];

			struct RTC_Time time = {0};
			//PGM_P const dow = RTC_GetDayName(RTC_GetDayOfWeek());
			__flash const char* dow = RTC_GetDayName(RTC_GetDayOfWeek());

			RTC_GetTime(&time);

			LCD_GoTo(0, 3);
			snprintf_P(text, 21, PSTR("%S%04d%02d%02d %02d:%02d:%02d "), (dow), (uint16_t)time.year + RTC_BASE_YEAR, time.month, time.day, time.hour, time.min, time.sec);
			LCD_WriteText(text);
			updateTime = false;
		}
		OW_Magic();


		_delay_ms(100);
	}

	while (1)
	{
		Kbd_Scan();

		//_delay_ms(200);
	}

	return 0;
}
