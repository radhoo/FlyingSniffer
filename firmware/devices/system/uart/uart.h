/*
** UART Library for AVR microcontrollers
** Copyright (C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** UART interrupt driven serial communication class file for atmega128
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
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//volatile static voidFuncPtru08 UartRxFunc;
//typedef void (*tUARTReceiverFunc)(unsigned char);

class UART {
	
	private:
		int m_index; //0 for UART0, 1 for UART1 and so on
		uint32_t m_nBaudRate,
				 m_nBaudRateRegister;
	//static tUARTReceiverFunc m_UARTReceiverFunc;
	public:
	
	/*
	 * Initiates UART serial communication
	 * uartindex: the UART index, for microcontrollers that have more than one (eg. atmega128 has too)
	 * baud: an integer representing the baud rate
	 * use_interrupt: receive data as interrupt, in ISR(USART0_RX_vect)/ISR(USART1_RX_vect) or by polling, using RecvPoll
	 */
	void start(int uartindex, uint32_t baud, bool use_interrupt );
	
	/*
	 * send one byte, b
	 */
	void send(uint8_t b);
	
	/*
	 * send a byte array (data) of given size (len)
	 */
	void send(char *data, int len);
	
	/*
	 * send a formated string
	 */
	void send(char *buffer, uint16_t len, char *szFormat, ...);
	void send_P(char *buffer, uint16_t len, const char *szFormat, ...);
	
	/*
	 * wait until one byte is received, and return: blocking function
	 */
	uint8_t recv();

	bool isdata();

	bool recv(uint8_t *b);

	void recvline(char *buf, uint16_t lenmax);
};
