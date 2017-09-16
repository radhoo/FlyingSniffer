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


#include <util/delay.h>
#include "zh03a.h"

// ZH03A output is 0..2V corresponding to 0..1000ug/m^3 of PM2.5
// returns particulate matter  as ug/m^3
void ZH03A::readPM25(double *pm25) {
	double adcVoltage = ((double)m_adc->read(m_pin)) * (3.3 / 1024.0); // x2 because of resistive divider on sensor output.
	if (adcVoltage > 2) adcVoltage = 2;
	*pm25 = adcVoltage * 500.0;
}

uint8_t ZH03A::checkThreshold(double pm25) {
	if (pm25 < PM25_LOW) return 0;
	else if (pm25 < PM25_NORMAL) return 1;
	else if (pm25 < PM25_HIGH) return 2;
	else return 3;
}
