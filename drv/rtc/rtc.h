/*
 * rtc.h
 *
 *  Created on: 3 sie 2016
 *      Author: Pawe³
 */

#ifndef DRV_RTC_RTC_H_
#define DRV_RTC_RTC_H_

#include "framework/error.h"
#include "framework/typedefs.h"

#define RTC_BASE_YEAR 2010

struct RTC_init {
	void (*onSecChangedCbf) ();
};

//struct RTC_Time {
//	uint8_t sec : 6;//max value 60, 6 bits are enough
//	uint8_t min : 6;
//	uint8_t hour: 5;
//	//TODO: for now implemented only for hh:mm:ss
//	uint8_t day:  5;
//	uint8_t month: 4;
//	uint8_t year: 6; //assumption that there year between 2010 - 2073
//};

struct RTC_Time {
	uint8_t sec;//max value 60, 6 bits are enough
	uint8_t min;
	uint8_t hour;
	//TODO: for now implemented only for hh:mm:ss
	uint8_t day;
	uint8_t month;
	uint8_t year; //assumption that there year between 2010 - 2073
};


Error RTC_Initialize (struct RTC_init *initParemeters);
Error RTC_GetTime (struct RTC_Time *time);
Error RTC_SetTime (struct RTC_Time *time);

const char* RTC_GetDayName(uint8_t dayOfWeek);
uint8_t RTC_GetDayOfWeek();

#endif /* DRV_RTC_RTC_H_ */
