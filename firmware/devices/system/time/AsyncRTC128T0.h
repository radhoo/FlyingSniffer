/*
** Time counter Library for Atmega128 using an external 32.768kHz crystal
** Version: 	0.1.0
** Date: 		February 03, 2015
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	mega128 features a RTC module that can provide accurate time measurement by using only a 32.768kHz crystal connected to TOSC1 / TOSC2
**
** www.pocketmagic.net
**
** This file is a part of "Portable Environmental Monitor" open source project presented on
** https://hackaday.io/project/4977-portable-environmental-monitor
**
** This project is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published
** by the Free Software Foundation; either version 3 of the License,
** or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>


typedef void (*TimeCallback)();

class AsyncRTC128T0 {
	uint32_t 		countedSec,
					epochSec,
					//total_min,
					timeKey;	// used to save a time coordinate
	TimeCallback	ptrFuncSec,	// called when a full second has elapsed
					ptrFuncMin;	// a full minute has elapsed
	public:
	AsyncRTC128T0() {
		ptrFuncSec = 0; ptrFuncMin = 0;
	}
	void init(TimeCallback callSec, TimeCallback callMin);
	void TimerEvent();
	uint32_t getTotalSec() { return countedSec; };
	//uint32_t getTotalMin() { return total_min; };
	uint8_t getSec();
	uint8_t getMin();
	uint8_t getHour();

	void startInterval();
	uint32_t getInterval();

	void setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
	void getTime(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second);
};


