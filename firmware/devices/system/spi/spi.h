/*
 ** Basic SPI interface for the Atmega128
 ** Version: 	0.1.0
 ** Date: 		August, 2015
 ** License:		LGPL v3
 ** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
 ** Description:	spi interface
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
#include <util/delay.h>
#include <avr/io.h>

class SPI {
public:
	// initialize SPI with lowest frequency; max. 400kHz. Useful during SDCARD identification mode, bad for LCD display
	inline void startslow(void) {
		// SPI ATMega2561 configuration
		// enable outputs for MOSI, SCK, SS, input for MISO
		DDRB |= (1 << DDB2); //mosi
		DDRB |= (1 << DDB1); //sck
		DDRB |= (1 << DDB0); //ss
		DDRB &= ~(1 << DDB3); //miso
		// initialize SPI with lowest frequency; max. 400kHz. Useful during SDCARD identification mode, bad for LCD display
		SPCR = (0 << SPIE) | // SPI Interrupt Enable
			   (1 << SPE)  | // SPI Enable
			   (0 << DORD) | // Data Order: MSB first
			   (1 << MSTR) | // Master mode
			   (0 << CPOL) | // Clock Polarity: SCK low when idle
			   (0 << CPHA) | // Clock Phase: sample on rising SCK edge
			   (1 << SPR1) | // Clock Frequency: f_OSC / 128
			   (1 << SPR0);
		// SPR1 SPR0 	SPEED
		// 1	1		/128
		// 1	0		/64
		// 0	1		/16
		SPSR &= ~(1 << SPI2X); // No doubled clock frequency
	}

	// set spi speed and settings
	inline void startfast(void) {
		// SPI ATMega2561 configuration
		// enable outputs for MOSI, SCK, SS, input for MISO
		DDRB |= (1 << DDB2); //mosi
		DDRB |= (1 << DDB1); //sck
		DDRB |= (1 << DDB0); //ss
		DDRB &= ~(1 << DDB3); //miso
		// init SPI at max speed
		SPCR = (1 << SPE) |		// spi enabled
				(1 << MSTR);	// spi master mode
		SPSR |= (1 << SPI2X);	// doubled clock freq
	}

	// send spi data
	// safe but slow, wait until transmission is finished: blocks any gain offered by SPI2x
	inline void send(uint8_t data) {
		SPDR = data;			//move data into spdr
	    // wait for byte to be shifted out
		while(!(SPSR & (1<<SPIF)));//wait till the transmission is finished: this blocks any speed gain offered by SPI2X, so better avoid!
		SPSR &= ~(1 << SPIF);
	}

	// send spi data
	// fastest: write SPI data (18 cycles until next write)
	inline void sendfast(uint8_t data) {
		SPDR = data;			//move data into spdr
		//var2: just a dump delay : 2us works only with SPI2X, else use greater 4us
		//_delay_us(2);
		//var3: fastest: write SPI data (18 cycles until next write)
		__asm__ __volatile__ (
		 // +4 (call to get here)
		 "	adiw	r24,0\n"	// +2 (2-cycle NOP)
		 "	adiw	r24,0\n"	// +2 (2-cycle NOP)
		 "	adiw	r24,0\n"	// +2 (2-cycle NOP)
		 "	adiw	r24,0\n"	// +2 (2-cycle NOP)
		 "	nop\n"				// +1 (2-cycle NOP)
		 "	ret\n"				// +4 (or +5 on >64KB AVR with RAMPD reg)
		 // = 17 cycles
		 );
		SPSR &= ~(1 << SPIF);
	}

	inline uint8_t read(void) {
		SPDR = 0xFF;
		while (!(SPSR & (1 << SPIF)));
		return SPDR;
	}

	inline void send(const uint8_t* buf , uint16_t n) {
	  if (n == 0) {
		return;
	  }
	  SPDR = buf[0];
	  if (n > 1) {
		uint8_t b = buf[1];
		uint16_t i = 2;
		while (1) {
		  while (!(SPSR & (1 << SPIF))) {}
		  SPDR = b;
		  if (i == n) {
			break;
		  }
		  b = buf[i++];
		}
	  }
	  while (!(SPSR & (1 << SPIF))) {}
	}

	inline uint8_t read(uint8_t* buf, uint16_t n) {
	  if (n-- == 0) {
		return 0;
	  }
	  SPDR = 0XFF;
	  for (uint16_t i = 0; i < n; i++) {
		while (!(SPSR & (1 << SPIF))) {}
		uint8_t b = SPDR;
		SPDR = 0XFF;
		buf[i] = b;
	  }
	  while (!(SPSR & (1 << SPIF))) {}
	  buf[n] = SPDR;
	  return 0;
	}
};
