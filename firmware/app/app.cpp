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
#include "app.h"

/************************************************************************************************************************************************/
/* Global Objects                                                       																		*/
/************************************************************************************************************************************************/
#define				MAX_BUFFER 100
char 				buffer[MAX_BUFFER];


Devices 			devices;

// rework:
bool 				secondTick = false,
					cmdBeep = false;
uint32_t			countedSec = 0;
/************************************************************************************************************************************************/
/* Callbacks                                                                                                                                    */
/************************************************************************************************************************************************/

// callback function called from the timecounter object when a full minute has elapsed
void callback_timeMinute() {
	devices.geiger.setCPM(devices.data.geigerPulses);
	devices.data.geigerPulses = 0;
}

// callback function called from the timecounter object when a full second has elapsed
void callback_timeSecond() {
	if (devices.time.getSec() > 0 && devices.time.getSec() % PARTIAL_INTERVAL == 0) {
		devices.geiger.setCPM((60 * devices.data.geigerPulses) / devices.time.getSec());
	}
}

// Async AS2, this ISR will be called precisely each second
ISR(TIMER2_COMPA_vect) {
	devices.time.TimerEvent();
}

// int4 interrupt handler, triggered each time a rising edge is detected on INT4, used to count Geiger pulses
ISR (INT4_vect) {
	devices.data.geigerPulses++;
	cmdBeep = 1;
}

// receiving data on the FT232/USB and GPS channel
ISR(USART0_RX_vect) {
	volatile uint8_t c = UDR0;	 //Read the value out of the UART buffer
	devices.nmea.fusedata(c);
}

// receiving data on the WLAN channel
ISR(USART1_RX_vect) {
	volatile uint8_t c = UDR1; //Read the value out of the UART buffer
}




// main entry point, go ahead, have fun!
int main(void) {
	//while(1);
	// prepare all devices
	devices.init(buffer, MAX_BUFFER);

	// start devices, and powerup
	devices.start();


	// go for main loop
	while (1) {
		// check why we need this
		if (countedSec != devices.time.getTotalSec()) {
			secondTick = 1;
			countedSec = devices.time.getTotalSec();
		}

		// read sensors and modules
		devices.loop(secondTick);

		if (cmdBeep) {
			devices.speaker.beep();
			cmdBeep = 0;
		}

		// check battery
		if (devices.battery.getLastVoltage() != 0 && devices.battery.getLastVoltage() < BATTERY_LEVEL_SHUTDOWN) {
			_delay_ms(2000);
			devices.shutdown();
		}

		secondTick = 0;

	}

	return 0;
}


