/*
 * Time handling subsystem
 * Copyright (C) July 10, 2013, Radu Motisan , radu.motisan@gmail.com , All rights reserved.
 * http://www.pocketmagic.net/
 */

#include "timecounter.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// init function used to set a callback to be called every new minute
void  TimeCounter::init() {
	TIMSK0 |= (1 << TOIE0); //atmega168
	sei(); // enable interrupts
	TCCR0B |= _BV(CS01) | _BV(CS00); // set prescaler to 64 and start the timer
}
