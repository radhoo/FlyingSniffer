/*
** zh03a dust sensor Library for AVR Microcontrollers
** Version: 	0.1.0
** Date: 		September, 2017
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2017 Radu Motisan, radhoo.tech@gmail.com
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


#pragma  once

#include "../../system/adc/adc.h"

#define				PM25_LOW				30
#define				PM25_NORMAL				60
#define				PM25_HIGH				100

class ZH03A {
	ADC10b 		*m_adc;
	uint8_t 	m_pin;


public:
	ZH03A() {};
	// sets the pulser GPIO pin and the ADC converter, both to be used with this sensor
	void init( ADC10b *adc, uint8_t adcPin) {
		m_adc = adc;
		m_pin = adcPin;
	}

	// returns particulate matter > PM0.5u as ug/m^3
	void readPM25(double *pm25);


	uint8_t checkThreshold(double pm25);
};
