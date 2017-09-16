/*
** uRADMonitor High Voltage Inverter Library
** Version: 	0.1.0
** Date: 		January, 2014
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	Used in uRADMonitor for handling the ferrite core inverter that generates High Voltage
** for the Geiger tubes. It was designed to allow configuration of Frequency and Duty cycle, and also provides
** a regulated voltage output mechanism.
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


#include "inverter.h"

// CREATE Timer T1 PWM to drive inverter for regulated Geiger tube voltage 
void Inverter::start(ADC10b *adc, uint8_t adcPin) {
	m_adc = adc;
	m_pin = adcPin;

	freq = INVERTER_FREQUENCY;
	duty = INVERTER_DEFAULT_DUTY;
	dutymin = INVERTER_DUTY_MIN;
	dutymax = INVERTER_DUTY_MAX;
	target = INVERTER_THRESHOLD;
	tolerance = INVERTER_TOLERANCE;
	highestVoltage = 0;

	TCCR1A = 0;     // disable all PWM on Timer1 whilst we set it up
	DDRB |= _BV(PB5); // Set PB5 as output (PB5 is OC1A)
	ICR1 = F_CPU / freq; // set the frequency FCPU/(ICR1 * PRESCALLING) Hz . Prescalling set to 1X
	setDuty(duty);
	TCCR1B = (1 << WGM13) | (1<<WGM12) | (1 << CS10); //Fast PWM mode via ICR1, with no prescaling (CS11 = 8x, CS10 = 1x)
	TCCR1A |= (1 << COM1A1) | (1<< CS11); // set none-inverting mode and start PWM
}

// read voltage on Geiger tube via divider
uint16_t Inverter::readVoltage() {
	return (uint16_t)m_adc->readDivVoltage(VREF, INVERTER_ADC_R1, INVERTER_ADC_R2, m_pin);
}
// check tube voltage and adjust duty cycle to match output given threshold level
// return false on critical error (when we hit the duty max), true if all ok
bool Inverter::adjustDutyCycle() {
	measuredVoltage = readVoltage();

	if (measuredVoltage > highestVoltage) {
		highestVoltage = measuredVoltage;
		highestVoltageDuty = duty;
	}

	if ( (measuredVoltage >= target + tolerance) && (duty > dutymin)) {
		setDuty(duty - 1); // we need to decrease duty cycle to decrease voltage
	}
	if ( (measuredVoltage <= target - tolerance) && (duty < dutymax)) {
		setDuty(duty + 1); // we need to increase duty cycle to increase voltage
	}

	if (duty == INVERTER_DUTY_MAX) {
		setDuty(highestVoltageDuty);
		return false; // error: no place to increase, yet we have not reached the target voltage
	}

	else return true;
}

// sets the duty cycle for the PWM signal
void Inverter::setDuty(uint16_t d) {
	duty = d;
	OCR1A = (int)( (float)ICR1 * (float) duty  / 1000.0);
}

// returns current duty cycle value
uint16_t Inverter::getDuty() {
	return duty;
}

uint16_t Inverter::getDutyMin() {
	return dutymin;
}
uint16_t Inverter::getDutyMax() {
	return dutymax;
}

uint32_t Inverter::getFrequency() {
	return freq;
}

void Inverter::setFrequency(uint32_t  f) {
	freq = f;
	ICR1 = F_CPU / freq; // set the frequency FCPU/(ICR1 * PRESCALLING) Hz . Prescalling set to 1X
}
