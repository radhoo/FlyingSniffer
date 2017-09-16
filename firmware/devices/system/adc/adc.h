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

#pragma once

#include <avr/io.h>

#define 			VREF 					3.3					// 3.3 regulated Voltage ref thanks to AMS1117 3.3V


class ADC10b {
	bool running;
public:
	ADC10b() {
		running = 0;
	}

	void start();
	// Reads the ADC port specified by i
	uint16_t read(uint8_t i);
	// return ADC port voltage computed against given VRef, with resistive divider
	float readDivVoltage(float vref, uint16_t divider_R1, uint16_t divider_R2, uint8_t i);
};


