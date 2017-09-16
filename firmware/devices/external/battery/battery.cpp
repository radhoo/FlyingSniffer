#include "battery.h"

void Battery::init(DigitalPin *poweradc, ADC10b *adc, uint8_t adcPin) {
	m_pwrAdc = poweradc;
	m_adc = adc;
	m_pin = adcPin;
}

double Battery::readVoltage() {
	*m_pwrAdc = ON;
	voltage = m_adc->readDivVoltage(VREF, BATTERY_ADC_R1, BATTERY_ADC_R2, m_pin);
	*m_pwrAdc = OFF;
	return voltage;
}
// compute battery percentage from voltage
// max voltage on battery is 4.2, but to count the drain we consider it 4.1 . Minimum is 3.3
uint8_t Battery::percentage() {
	uint8_t result = (voltage - BATTERY_MIN) * 100 / (BATTERY_MAX - BATTERY_MIN);
	if (result > 100)
		return 100;
	else
		return result;
}

uint8_t Battery::symbol() {
	if (voltage >= 4.1) return 7; // battery full
	else if (voltage > 4) return 8;
	else if (voltage > 3.9) return 9;
	else if (voltage > 3.7) return 10;
	else if (voltage > 3.5) return 11;
	else if (voltage > 3.4) return 12;
	else
		return 13; // battery empty
}


double Battery::getLastVoltage() {
	return voltage;
}
