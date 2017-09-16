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

#include "geiger.h"

GEIGER::GEIGER(uint8_t tube) {
	m_tube = tube;
}

// not more than 10 chars!
const char* GEIGER::name() {
	switch (m_tube) {
		case GEIGER_TUBE_SBM20: return PSTR("SBM20");
		case GEIGER_TUBE_SI29BG: return PSTR("SI29BG");
		case GEIGER_TUBE_SBM19: return PSTR("SBM19");
		case GEIGER_TUBE_LND712: return PSTR("LND712");
		case GEIGER_TUBE_SBM20M: return PSTR("SBM20M");
		case GEIGER_TUBE_SI22G: return PSTR("SI22G");
		case GEIGER_TUBE_STS5: return PSTR("STS5");
		case GEIGER_TUBE_SI3BG: return PSTR("SI3BG");
		case GEIGER_TUBE_SBM21: return PSTR("SBM21");
		case GEIGER_TUBE_SBT9: return PSTR("SBT9");
		case GEIGER_TUBE_SI1G: return PSTR("SI1G");
		default: return PSTR("unknown");
	}
}

float GEIGER::factor() {
	switch (m_tube) {
		case GEIGER_TUBE_SBM20: 	return 0.006315; // CPM 19
		case GEIGER_TUBE_SI29BG: 	return 0.010000; // CPM 12
		case GEIGER_TUBE_SBM19: 	return 0.001500; // CPM 80
		case GEIGER_TUBE_LND712: 	return 0.005940; // CPM 20.20
		case GEIGER_TUBE_SBM20M:	return 0.013333; // CPM 9
		case GEIGER_TUBE_SI22G: 	return 0.001714; // CPM 70
		case GEIGER_TUBE_STS5: 		return 0.006666; // CPM 18
		case GEIGER_TUBE_SI3BG: 	return 0.631578; // CPM 0.19
		case GEIGER_TUBE_SBM21: 	return 0.048000; // CPM 2.5
		case GEIGER_TUBE_SBT9: 		return 0.010900; // CPM 11
		case GEIGER_TUBE_SI1G:		return 0.006000; // CPM 20
	}
	return 0;
}

void GEIGER::setCPM(uint32_t geigerCPM) {
	m_cpm = geigerCPM;
	m_dose = factor() * m_cpm;
}

uint32_t GEIGER::getCPM() {
	return m_cpm;
}


// dose equivalent linear approximative conversion from CPM
float GEIGER::dose() {
	return m_dose;
}

// to save display space, we use multipliers
float GEIGER::doseFactored() {
	if (m_dose < 10) return m_dose; // show in micros
	else if (m_dose < 10000) return m_dose / 1000.0; // show in milis
	else return m_dose / 1000000.0; // show in units
}

char GEIGER::doseMulSymbol() {
	 if (m_dose < 10) return 'u'; // show in micros
	 else if (m_dose < 10000) return 'm'; // show in milis
	 else return ' '; // show in units
}

// apply multiplication factor to CPM
uint16_t GEIGER::cpmFactored() {
	 if (m_cpm < 1000) return (uint16_t)m_cpm;
	 else if (m_cpm < 1000000) return m_cpm / 1000;
	 else return m_cpm / 1000000UL;
}

// get multiplication factor symbol for CPM
char GEIGER::cpmMulSymbol() {
	 if (m_cpm < 1000) return ' ';
	 else if (m_cpm < 1000000) return 'K';
	 else return 'M';
}

uint8_t GEIGER::checkThreshold() {
	if (m_dose < GEIGER_LOW) return 0;
	else if (m_dose < GEIGER_NORMAL) return 1;
	else if (m_dose < GEIGER_HIGH) return 2;
	else return 3;
}
