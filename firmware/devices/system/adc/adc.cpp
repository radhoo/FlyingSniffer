/*
** ADC Library for AVR Microcontrollers
** Version: 	0.1.0
** Date: 		July, 2015
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description: Analog to digital 10bit class
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
#include "adc.h"

void ADC10b::start() {
	// enable ADC, select ADC clock = F_CPU / 128 (i.e. 125 kHz)
	ADCSRA =  //1<<ADEN | 1<<ADPS2 | 1<<ADPS1;
			(1<<ADEN | 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0 ); // F_CPU / 128 = 125 kHz
			//(1<<ADEN | 1<<ADPS2  ); // F_CPU / 16 ~= 1M
			//(1<<ADEN | 1<<ADPS2 | 1<<ADPS1  ); // F_CPU / 64
	running = true;
}

// Reads the ADC port specified by i
uint16_t ADC10b::read(uint8_t i) {
	if (!running) {
		start();
		_delay_ms(2);
	}
	// no REFS0 and no REFS1: AREF, Internal Vref turned off
	// REFS0: AVCC with external capacitor at AREF pin
	// REFS0 + REFS1: Internal 2.56V Voltage Reference with external capacitor at AREF pin
	//ADMUX = (1<<REFS0) | (1<<REFS1) | i;
	ADMUX = (1<<REFS0)  | i;
	//ADMUX =  i; //not good on test board
	// ADSC: start one conversion
	ADCSRA |= 1 << ADSC;
	// wait for conversion
	while (ADCSRA & (1 << ADSC));
	// return 16bit result
	return ADCL | (ADCH << 8);
}

// return ADC port voltage computed against given VRef, with resistive divider
float ADC10b::readDivVoltage(float vref, uint16_t divider_R1, uint16_t divider_R2, uint8_t i) {
	return  (read(i) / 1024.0 * (divider_R1 + divider_R2) / divider_R2 * vref);
}
