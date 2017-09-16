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
#include <util/delay.h>
#include "AsyncRTC128T0.h"

// init function used to set a callback to be called every new minute
void  AsyncRTC128T0::init(TimeCallback callSec, TimeCallback callMin) {
	ptrFuncSec = callSec;
	ptrFuncMin = callMin;
	
	//Wait for external clock crystal to stabilize;
	_delay_ms(20);

	// By setting the AS0 bit in ASSR, Timer/Counter0 is asynchronously clocked from the TOSC1 pin.
	// When AS0 is set, pins TOSC1 and TOSC2 are disconnected from Port C.
	/*ASSR |= (1<<AS01);
	TCCR0 |= (1<<CS02) | (1<<CS00); // prescaler set to 128 32768/128=256 Hz
	TIMSK |= (1<<TOIE0); //TOIE0 bit in the TIMSK (Timer/Counter Interrupt Mask Register) is then set to enable Timer/Counter0 Overflow Interrupt.
	sei();*/

	// Stop timer , prescaler reset
	GTCCR |= (1 << TSM) | (1 << PSRASY);
	// Enables the asynchronous mode with external 32.768 kHz watch crystal .
	// ASSR - Asynchronous Status Register
	// Data Sheet Page 192
	ASSR  = (1<<AS2);
	TCCR2A = (1 << WGM21);                //CTC Modus
	// Sets the divider to 256
	// TCCR2B - Timer / Counter Control Register B
	// Data Sheet Page 190
	TCCR2B |= (1<<CS22)| (1<<CS21); // 256
	// OCR2A – Output Compare Register A
	// Datenblatt Seite 191
	OCR2A = 128 - 1; // half second int
	// Enables interrupt TOV2
	// TIMSK2 - Timer / Counter2 Interrupt Mask Register
	// Data Sheet Page 193
	TIMSK2  = (1<<OCIE2A);
	GTCCR &= ~(1 << TSM);
	sei();


}

void AsyncRTC128T0::TimerEvent() {
	countedSec++;

	if ((countedSec>0) && (countedSec % 60 == 0)) {
		//total_min++;
		if (ptrFuncMin != 0) ptrFuncMin();
	}
	if (ptrFuncSec != 0) ptrFuncSec();
}

uint8_t AsyncRTC128T0::getSec() { return countedSec % 60; }
uint8_t AsyncRTC128T0::getMin() { return (countedSec / 60) % 60; }
uint8_t AsyncRTC128T0::getHour() { return (countedSec / 3600) % 24; }

void AsyncRTC128T0::startInterval() {
	timeKey = countedSec;
}

uint32_t AsyncRTC128T0::getInterval() {
	return countedSec - timeKey;
}

uint32_t dateToEpoch(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
	year -= month <= 2;
    uint16_t era = (year >= 0 ? year : year - 399) / 400;
    uint16_t yoe = (year - era * 400);      // [0, 399]
    uint16_t doy = (153*(month + (month > 2 ? -3 : 9)) + 2)/5 + day - 1;  // [0, 365]
    uint16_t doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
    uint32_t seconds = ((era * 146097 + doe - 719468) * 24 + hour) * 3600 + minute * 60 + second;
    return seconds;
}

void epochToDate(uint32_t timestamp, uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second) {

    *second = timestamp % 60; timestamp /= 60;
    *minute = timestamp % 60; timestamp /= 60;
    *hour   = timestamp % 24; timestamp /= 24;
    timestamp+= 719468;
    uint16_t era = (timestamp >= 0 ? timestamp : timestamp - 146096) / 146097;
    uint16_t doe = (timestamp - era * 146097);          // [0, 146096]
    uint16_t yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
    *year = yoe + era * 400;
    uint16_t doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
    uint16_t mp = (5*doy + 2)/153;                                   // [0, 11]

    *day = doy - (153* mp + 2)/5 + 1;                             // [1, 31]
    *month = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
    *year+= ((*month) <= 2);
}

void AsyncRTC128T0::setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
	epochSec = dateToEpoch(year, month, day, hour, minute, second);
	epochSec -= countedSec;
}

void AsyncRTC128T0::getTime(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *second) {
	epochToDate(epochSec + countedSec, year, month, day, hour, minute, second);
}
