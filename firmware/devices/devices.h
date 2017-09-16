/* uRADMonitor model D
 * (C) 2016 Magnasci SRL, www.magnasci.com
 * All rights reserved.
 */

#pragma once

#include <avr/io.h>
// system
#include "system/adc/adc.h"
#include "system/gpio/DigitalPin.h"
#include "system/i2c/i2c.h"
#include "system/spi/spi.h"
#include "system/time/AsyncRTC128T0.h"
#include "system/time/timecounter.h"
#include "system/uart/uart.h"
// external
#include "external/inverter/inverter.h"
#include "external/nmea/nmea.h"
#include "external/rtc/DS1337.h"
#include "external/battery/battery.h"
#include "external/sdcard/SD.h"
#include "external/speaker/Speaker.h"
#include "external/morse/morse.h"
// sensors
#include "sensors/bme680/bme680.h"
#include "sensors/geiger/geiger.h"
#include "sensors/zh03a/zh03a.h"
// settings
#include "config.h"
// data
#include "data/data.h"

#define OFFLINE_FILE "offline.txt"

class Devices {
public:
	// share buffer
	char 				*pBuf;
	uint16_t 			cBuf;

	// define system
	ADC10b 				adc;													// 10bit ADC converter
	DigitalPin 			sdCS, sdAvailable,										// SDCARD pins
						pwrUnit,												// power unit, OFF
						pwrWLAN,												// power WLAN
						pwrGPS,													// power GPS
						pwrFAN,													// power air FAN
						pwrSPK,													// power speaker pin
						pwrDustLED,												// dust sensor control pin, ON
						pwrBatteryADC,											// control battery reading
						pwrInverter5V;											// control 5V inverter
	I2C					i2c;
	SPI					spi;
	AsyncRTC128T0 		time;													// handles the time interrupts
	TimeCounter			time0;
	UART 				uart0, 													// connected to FT232 for serial communication and firmware upgrade
						uart1;													// connected to ESP8266 for Internet Access

	// define external
	Inverter 			inverter;												// handles the PWM on Timer1 to drive the Geiger tube inverter
	NMEA				nmea;
	DS1337 				rtc;
	SDClass				sd;														// the sdcard
	Battery				battery;												// Battery manager

	// define sensors
	BME680				bme680;													// Bosch BME680
	GEIGER				geiger;													// Geiger LND712 tube
	ZH03A				zh03a;													// Winsensor ZH03A

	Data				data;
	Speaker				speaker;
	Morse				morse;
	File 				logFile;
	char				filename[13];

	volatile bool		rtcSynced;												// sync RTC when GPS first is connected

public:
	Devices();

	void init(char *buf, uint16_t len);
	void start();
	void startWLAN();
	void shutdown();

	bool sdcardCheck();

	void rtcRead();

	void loop(bool tick);

};
