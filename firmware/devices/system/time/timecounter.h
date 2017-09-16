/*
 * Time handling subsystem
 * Copyright (C) July 10, 2013, Radu Motisan , radu.motisan@gmail.com , All rights reserved.
 * http://www.pocketmagic.net/
 */

#pragma once
#include <stdint.h>

class TimeCounter {
	uint16_t		milis;		// time: counts the intervals of 2.048ms (on 8MHz F_CPU) elapsed
					
	public:

	TimeCounter() {

	}
	void init();
	inline void TimerEvent();

};

// event to be executed every  2.048ms here with 8MHz crystal
// event to be executed every  1.024ms here with 16MHz crystal
void TimeCounter::TimerEvent() {
	milis ++;
	// small timing error here: 488.28125 exact value for 8MHz
	if (milis < 488) return; // 8MHZ
	//if (milis < 976) return; // 16MHZ

	// reset counter
	milis = 0; // 488 x 2.048ms ~= 1000 ms = 1sec for 8MHz , 976 x 1.024ms ~= 1000 ms = 1 sec for 16MHz
}
