/*
** Portable Environmental Monitor
** Copyright (C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** A battery powered handheld device that measures radiation (alpha,beta,gamma), and air parameters:
** temperature, pressure, humidity, content of dust particles, CO2 levels, tVOCs levels
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

#include <stdio.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include "../../devices/external/nmea/nmea.h"
#include "../../misc/aux.h"



#include <avr/pgmspace.h>

class Data {

public:
	Data();

	uint32_t			geigerPulses;
	double				sensorTemperature,				// bme680: temperature
						sensorHumidity,					// bme680: humidity
						sensorPM25,						// zh03a: pm2.5
						sensorPressure,					// bme680: barometric pressure
						sensorVOC;						// bme680: VOC
	bool				sdInitialized;

};
