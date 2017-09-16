#include "devices.h"
#include "../misc/aux.h"
#include "../app/app.h"

Devices::Devices() {
	data.sdInitialized = 0;
	pBuf = 0;
	cBuf = 0;
}

void Devices::init(char *buf, uint16_t len) {
	pBuf = buf;
	cBuf = len;
	rtcSynced = 0;

	sdCS.init(&PORTE, PE3);
	pwrUnit.init(&PORTA, PA6);
	pwrWLAN.init(&PORTE, PE2);
	pwrGPS.init(&PORTG, PG0);
	pwrFAN.init(&PORTD, PD6);
	pwrSPK.init(&PORTA, PA7);
	sdAvailable.init(&PORTE, PE5, DigitalPin::INPUT);
	pwrBatteryADC.init(&PORTG, PG2);
	pwrInverter5V.init(&PORTC, PC7);
	speaker.init(&pwrSPK);
	morse.init(&speaker);

	//eeprom.init(&i2c);
	rtc.init(&i2c);
}


void Devices::start() {
	//_delay_ms(STARTUP_DELAY);
	// keep power pin High unless we want to shutdown
	pwrUnit = ON;
	_delay_ms(500);
	pwrWLAN = ON;
	_delay_ms(500);
	pwrGPS = ON;
	_delay_ms(500);
	pwrInverter5V = ON;
	_delay_ms(500);
	pwrSPK = ON; _delay_ms(10); pwrSPK = OFF;

	// system config
	MCUCR |= (1 << JTD); MCUCR |= (1 << JTD); // disable JTAG so we can use PF4 (UNUSED),PF5 (BAT ADC),PF6 (TOUCH XN ADC),PF7(TOUCH YP ADC) for ADC and GPIO
	//spi.startslow(); // start SPI
	adc.start(); // start ADC
	uart0.start(0, 9600, 1); // connected to the FTDI/microUSB and GPS
	uart1.start(1, 9600, 0); // needed by esp8266 . 9600 | 115200 // AT+UART_DEF=9600,8,1,0,0
	EICRB |= (1 << ISC00) | (1 << ISC01); // Configure INT4 to trigger on RISING EDGE
	EIMSK |= (1 << INT4); // Configure INT4 to fire interrupts
	i2c.init();
	rtc.start(); rtc.readTime();
	time.init(callback_timeSecond, callback_timeMinute);
	//time0.init();
	spi.startfast();

	// external config
	battery.init(&pwrBatteryADC, &adc, BATTERY_ADC); // init battery manager
	inverter.start(&adc, INVERTER_ADC); // create Timer T1 PWM to drive inverter for regulated Geiger tube voltage
	bme680.start(&i2c); // init Bosch BME680 sensor
	zh03a.init(&adc, PMSENSOR_ADC); // init PM2.5 Sharp GP2Y1010AU0f sensor

	sdcardCheck();

	sei();

	// init settings from internal EEPROM
	speaker.setMute(0);

}

void Devices::startWLAN() {
	pwrWLAN = ON;
	_delay_ms(200);
	//	pwrWLAN = OFF;
}

void Devices::shutdown() {
	speaker.beep();
	speaker.beep();
	pwrUnit = OFF;
}


bool Devices::sdcardCheck() {
	// check the physical toggle switch to know if there is an sdcard
	if (!sdAvailable.read()) {
		// if a card is inserted, and we're not yet initialized, try to initialize
		if (!data.sdInitialized) {
			spi.startslow();
			if (sd.begin(&sdCS, &spi) == 0) {
				cid_t cid;
				sd.getCard().readCID(&cid);
				// open log file here?

				sprintf_P(filename, PSTR("%02d-%02d-%02d.csv"), rtc.getYears() - 2000, rtc.getMonths(), rtc.getDays());
				logFile = sd.open(filename, FILE_WRITE);
				strcpy_P(pBuf, PSTR("   RTCTIME, LOCALTIME,  LATITUDE, LONGITUDE,  ALTITUDE,     SPEED,   BEARING,SATS,TEMPERATURE,PRESSURE,HUMIDITY,     VOC,   PM25,BATTERY,     CPM,VOLTAGE,DUTY,GPS,WIFI,HTTP\r\n"));
				logFile.write((uint8_t *)pBuf, strlen(pBuf));
				logFile.close();
				// mark as ready to use
				data.sdInitialized = 1;
			} else {
				logFile.close();
				data.sdInitialized = 0;
				sd.end();
			}
			spi.startfast();
		}
	} else {
		logFile.close();
		data.sdInitialized = 0;
		sd.end();
	}

	return data.sdInitialized;
}


void Devices::loop(bool tick) {
	// read fast
	inverter.adjustDutyCycle(); // do nothing on failure, we can't reset


	// read slow
	if (tick) {
		// sync time with GPS once per every reboot, when GPS becomes available
		if (!rtcSynced && nmea.isdataready()) {
			rtc.setYears(DS1337_BASE_YR + nmea.getYear());
			rtc.setMonths(nmea.getMonth());
			rtc.setDays(nmea.getDay());
			rtc.setHours(nmea.getHour());
			rtc.setMinutes(nmea.getMinute());
			rtc.setSeconds(nmea.getSecond());
			rtc.writeTime();
			rtcSynced = 1;
		}
		// read time
		rtc.readTime();
		// read battery
		battery.readVoltage();

		// check sdcard
		sdcardCheck();
		// speaker tick
		speaker.secondTick();

		// read first time and every 5 seconds
		if (data.sensorPressure == 0 || time.getSec() % PARTIAL_INTERVAL == 0) {
			// read BME680
			bme680.readSensors(&data.sensorTemperature, &data.sensorPressure, &data.sensorHumidity, &data.sensorVOC);

			// shoot the IR light beam and measure scattered light via ADC
			// convert output voltage to dust density in micrograms/m3
			zh03a.readPM25(&data.sensorPM25);


			// turn off 5V inverter to conserve power
			//pwrInverter5V = OFF;

			sprintf_P(pBuf, PSTR("%10lu,%10lu,%10.06f,%10.06f,%10.02f,%10.02f,%10.02f,%4u,%11.02f,%8lu,%8.02f,%8lu,%7.02f,%7.02f,%8lu,%7u,%4u\r\n"),
				rtc.date_to_epoch_seconds(),
				time.getTotalSec(),
				nmea.getLatitude(), nmea.getLongitude(),nmea.getAltitude(), nmea.getSpeed(),nmea.getBearing(), nmea.getSatellites(),
				data.sensorTemperature, (uint32_t)data.sensorPressure, data.sensorHumidity, (uint32_t)data.sensorVOC,
				data.sensorPM25, battery.getLastVoltage(),
				geiger.getCPM(),
				inverter.getMeasured(),
				inverter.getDuty()
			);
			//timertc,localtime,latitude,longitude,altitude,speed,temperature,pressure,humidity,voc,pm25,battery,cpm,voltage,duty
			// write log file here?
			if (data.sdInitialized) {
				sprintf_P(filename, PSTR("%02d-%02d-%02d.csv"), rtc.getYears() - 2000, rtc.getMonths(), rtc.getDays());
				logFile = sd.open(filename, FILE_WRITE);
				logFile.write((uint8_t *)pBuf, strlen(pBuf));
				logFile.close();
			}
			// send over radio
			uart1.send(pBuf, -1);
		}


		// sound alarm if thresholds are reached
		//speaker.setAlarm( (geiger.checkThreshold() == 3) || (zh03a.checkThreshold(data.sensorPM25) == 3));

	}
}



