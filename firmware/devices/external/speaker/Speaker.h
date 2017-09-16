#pragma once
#include <util/delay.h>
#include "../../system/gpio/DigitalPin.h"

class Speaker {

	bool mute;
	DigitalPin *m_spk;
	bool alarm;

public:
	void init(DigitalPin *spk);

	void secondTick();

	void beep();
	void beepshort();
	void beeplong();
	void delay();

	bool getMute();
	void setMute(bool value);

	bool getAlarm();
	void setAlarm(bool value);
	void toggleAlarm();
};
