/*
 * rtc.c
 *
 *  Created on: 3 sie 2016
 *      Author: Pawe³
 */

#include "rtc.h"
#include "avr/interrupt.h"
#include "drv/port/port.h"
#include "drv/LCD/HD44780.h"

#define _MIN  (60L)
#define _HOUR (60L*60L)
#define _DAY  (24L*60L*60L)

#define _DAYS_IN_YEAR 365
#define _LEAP_DAYS 1 //extra days in leap-year

#define _BASE_YEAR RTC_BASE_YEAR
#define _BASE_YEAR_IS_LEAP false

//seconds from 1.1.2010
volatile uint32_t timestamp = 0 + 23*_HOUR + 59*_MIN + 50; //04-08-2016 05:15:21
bool initialized = false;

uint8_t daysInMonths[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
uint16_t daysPassedTillMonth[] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
uint8_t leapsInMonths[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

struct RTC_init lastInitParams = {0};

ISR(TIMER2_COMP_vect, ISR_NOBLOCK) {
	++timestamp;

	if (lastInitParams.onSecChangedCbf != NULL) {
		lastInitParams.onSecChangedCbf();
	}
}
char dbg[21] = {0};
static void _debug(char *txt, uint8_t line) {
	LCD_GoTo(0, line);
	LCD_WriteText("                    ");
	LCD_GoTo(0, line);
	LCD_WriteText(txt);
}

Error RTC_Initialize (struct RTC_init *initParemeters) {
	Error ret = NO_ERROR;

	TCCR2 = 0; //stop timer
	initialized = false;

	if (NULL != initParemeters) {
		lastInitParams.onSecChangedCbf = initParemeters->onSecChangedCbf;

	} else {
		lastInitParams.onSecChangedCbf = NULL;
	}

	//AS2  = 1   - asynchronous mode.
	ASSR |= 1<<AS2;


	//OCR2 = 128 - 1 sec on timer compare
	OCR2 = 128;

	//OCIE2 = 1 - enable output compare interrupt
	TIMSK |= 1<<OCIE2;

	//WGM21:0 - 10 - CTC (clear on time compare);
	//COM21:0 - 00 - Normal port mode - disconnected from timer;
	//FOC2    - 0  - no force...
	//CS22:0   -110 - Prescaler = f/256

	TCCR2 |= ( 1<<WGM21 ) | ( 1<<CS22 ) | ( 1<<CS21 );

	initialized = true;

	return ret;
}


bool _isLeapYear(uint16_t yyyy) {
	return (yyyy%4 == 0) && ( (yyyy%400 == 0) || (yyyy%100 != 0) );
}

/**
 * @param dayInYear [out] return rest of days in counted year
 */
uint8_t _getYear(uint16_t *daysInYear) {
	uint8_t i = 0;
	uint16_t year = _BASE_YEAR; //not sure if this value is needed;
	uint32_t temp = timestamp/_DAY;
	bool isLeapYear = _BASE_YEAR_IS_LEAP;

	while (temp > (_DAYS_IN_YEAR + (isLeapYear))) {
		temp -= (_DAYS_IN_YEAR + (isLeapYear));
		++i;
		isLeapYear = _isLeapYear(year + i);
	}

	if (daysInYear != NULL) {
		*daysInYear = (uint16_t) temp; //in this point temp shouldn't be greater then 366
	}

	return i;
}

uint8_t _getMonth(uint16_t dayInYear, bool isLeapYear, uint8_t *daysInMonth) {
	uint8_t i = 1;
	uint16_t temp = 0;//daysInMonths[0];

	if (dayInYear > 365 + isLeapYear) return 0;

	while (dayInYear > (temp + daysInMonths[i-1])+ ((isLeapYear) ? leapsInMonths[i-1]: 0)) {
		temp += daysInMonths[i-1];
		++i;
	}

	if (daysInMonth != NULL) {
		*daysInMonth = (uint8_t) (dayInYear - temp); //in this point temp shouldn't be greater then 366
	}

	return i;
}


uint8_t _getExtraDaysTill(struct RTC_Time *time) {
	uint8_t i = 0;
	uint16_t temp = _BASE_YEAR, year = _BASE_YEAR + time->year;

	for (; temp<year; ++temp) {
		if (_isLeapYear(temp)) {
			++i;
		}
	}
	return i;
}


//TODO: add enum to decide if we want to get date, time or both
Error RTC_GetTime (struct RTC_Time *time) {
	Error ret = NO_ERROR;
	uint16_t d;
	uint8_t d2;

	if (time != NULL && initialized) {
		time->sec = timestamp%60;
		time->min = (timestamp/60)%60;
		time->hour= (timestamp/(60*60))%24;

		time->year= _getYear(&d);
		time->month = _getMonth(d, _isLeapYear(_BASE_YEAR+time->year), &d2);
		time->day  = d2;

	} else if (!initialized){
		ret = ERROR_UNINITIALIZED;
	} else {
		//TODO: Add cexceptions to project!
		ret = ERROR_INVALID_PARAMETER;
	}

	return ret;
}

//TODO: add enum to decide if we want to get date, time or both
Error RTC_SetTime (struct RTC_Time *time) {
	Error ret = NO_ERROR;

	snprintf(dbg, 21, "set %04d-%02d-%02d", _BASE_YEAR+time->year, time->month, time->day);
	_debug(dbg, 1);

	if (time != NULL && initialized) {
		timestamp = time->sec;
		timestamp += (uint32_t) time->min * _MIN;
		timestamp += (uint32_t) time->hour * _HOUR;

		timestamp += (uint32_t) (time->day) * _DAY;
		timestamp += (uint32_t) daysPassedTillMonth[(time->month-1)%12] * _DAY;
		timestamp += (uint32_t) time->year * _DAY * 365;
		timestamp += (uint32_t) _getExtraDaysTill(time) * _DAY;
	} else if (!initialized){
		ret = ERROR_UNINITIALIZED;
	} else {
		//TODO: Add cexceptions to project!
		ret = ERROR_INVALID_PARAMETER;
	}

	if (lastInitParams.onSecChangedCbf != NULL) {
		lastInitParams.onSecChangedCbf();
	}

	return ret;
}
