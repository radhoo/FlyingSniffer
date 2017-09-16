/*
** input/output GPIO class
** Version: 	0.1.0
** Date: 		January, 2015
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:
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
#include <avr/io.h>

#define				ON						1
#define				OFF						0

class DigitalPin  {
public:
    enum Type {
    	INPUT = 0,
		OUTPUT = 1
    };
private:
    volatile uint8_t 	*m_pport;
    uint8_t				m_dq;
    uint8_t				mask1, mask0;
    Type				m_type;
    bool 				m_state;

    // Get the other registers from the PORT
    #define PORT2DDR(x) (x - 1)
	#define PORT2PIN(x) (x - 2)
public:
    DigitalPin(){};
    // Create a DigitalOut connected to the specified pin
    DigitalPin(volatile  uint8_t  *port, uint8_t  dq, Type type = OUTPUT, bool state = false) {
        	// save globals
        	m_dq = dq;
        	m_pport = port;
        	mask1 = (1<<dq);
        	mask0 = ~(1<<dq);
        	m_type = type;
        	// set input or output
        	config(type);
        	// set default state if needed
        	if (state && type == OUTPUT) {
        		write(state);
        	}
        	m_state = state;
        }

    void init(volatile  uint8_t  *port, uint8_t  dq, Type type = OUTPUT, bool state = false) {
    	// save globals
    	m_dq = dq;
    	m_pport = port;
    	mask1 = (1<<dq);
    	mask0 = ~(1<<dq);
    	m_type = type;
    	// set input or output
    	config(type);
    	// set default state if needed
    	if (state && type == OUTPUT) {
    		write(state);
    	}
    	m_state = state;
    }

    void config(Type type);

    uint8_t getPin() {
    	return m_dq;
    }

    // Set the output, specified as 0 or 1 (int)
    void write(bool state);

    // Return the output setting, represented as 0 or 1 (int)
    bool read();

    // negate state
    bool toggle();

    DigitalPin& operator= (int value) {
        write(value);
        return *this;
    }

    DigitalPin& operator= (DigitalPin& rhs) {
        write(rhs.read());
        return *this;
    }
    
    /*DigitalOut& operator ! () const {
    	return *this;
    }*/

    operator int() {
        return read();
    }

};
