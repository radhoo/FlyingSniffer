#pragma once

#include "../../system/gpio/DigitalPin.h"
#include "../../system/adc/adc.h"


#define				BATTERY_MIN				3.3					// shutdown if voltage on battery is lower
#define				BATTERY_MAX				4.1
#define				BATTERY_ADC_R1			470					// 470K voltage divider R1 for battery voltmeter
#define				BATTERY_ADC_R2			470					// 470K voltage divider R2 for battery voltmeter

class Battery {
	DigitalPin 	*m_pwrAdc;
	ADC10b 		*m_adc;
	uint8_t 	m_pin;
	double		voltage;
public:
	Battery() {};
	void init(DigitalPin *poweradc, ADC10b *adc, uint8_t adcPin);

	// read voltage on battery via divider
	double readVoltage();

	uint8_t percentage();
	uint8_t symbol();

	double getLastVoltage();
};
