#include "Speaker.h"

void Speaker::init(DigitalPin *spk) {
	m_spk = spk;
	mute = 0;
}

void Speaker::beep() {
	if (mute) return;

	if (!alarm) {
		*m_spk = 1;
		_delay_us(80);
		*m_spk = 0;
		_delay_us(50);
	}
}

void Speaker::beepshort() {
	if (mute) return;

	if (!alarm) {
		*m_spk = 1;
		delay();
		*m_spk = 0;
		delay();
	}
}

void Speaker::beeplong() {
	if (mute) return;

	if (!alarm) {
		*m_spk = 1;
		delay();delay();delay();
		*m_spk = 0;
		delay();
	}
}

void Speaker::delay() {
	_delay_ms(50);
}

void Speaker::secondTick() {
	if (alarm && !mute)
		m_spk->toggle();
}

bool Speaker::getMute() {
	return mute;
}

void Speaker::setMute(bool value) {
	mute = value;
	if (mute) *m_spk = 0;
}

bool Speaker::getAlarm() {
	return alarm;
}

void Speaker::setAlarm(bool value) {
	if (alarm == value) return;
	else {
		if (!mute) *m_spk = value;
		alarm = value;
	}
}

void Speaker::toggleAlarm() {
	setAlarm(!getAlarm());
}
