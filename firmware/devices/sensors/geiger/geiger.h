/*
** uRADMonitor radiation detectors
** Version: 	0.1.0
** Date: 		January, 2014
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	Used in the uRADMonitor code to identify various radiation detectors, mostly Geiger tubes
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
#include <avr/pgmspace.h>

// define Radiation DETECTORS
#define 			GEIGER_TUBE_UNKNOWN 	0x0
#define				GEIGER_TUBE_SBM20 		0x1
#define 			GEIGER_TUBE_SI29BG 		0x2
#define 			GEIGER_TUBE_SBM19 		0x3
#define 			GEIGER_TUBE_LND712 		0x4
#define 			GEIGER_TUBE_SBM20M 		0x5
#define 			GEIGER_TUBE_SI22G 		0x6
#define 			GEIGER_TUBE_STS5 		0x7
#define 			GEIGER_TUBE_SI3BG 		0x8
#define 			GEIGER_TUBE_SBM21 		0x9
#define 			GEIGER_TUBE_SBT9 		0xA
#define 			GEIGER_TUBE_SI1G 		0xB

// low, normal, high, danger
#define				GEIGER_LOW				0.10
#define				GEIGER_NORMAL			0.20
#define				GEIGER_HIGH				1.00


class GEIGER {
	uint8_t m_tube;
	uint32_t m_cpm;
	float m_dose;

	float factor();

public:
	GEIGER(uint8_t tube = GEIGER_TUBE_LND712);

	const char *name();

	void setCPM(uint32_t geigerCPM);
	uint32_t getCPM();
	float dose();

	float doseFactored();
	char doseMulSymbol();

	uint16_t cpmFactored();
	char cpmMulSymbol();

	uint8_t checkThreshold();

};

