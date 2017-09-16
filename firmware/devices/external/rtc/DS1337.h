/*
  DS1337.h - library for DS1337 rtc
*/

// ensure this library description is only included once
#pragma once

#include "DS1337.h"
#include "../../system/i2c/i2c.h"


// Mosquino has the RTC interrupt pin tied to INT2 for alarm/snooze functions
#define RTC_INT_NUMBER 2
#define RTC_INT_PIN	PIN_INT2

// indices within the rtc_bcd[] buffer
#define DS1337_SEC	0
#define DS1337_MIN	1
#define DS1337_HR	2
#define DS1337_DOW	3
#define DS1337_DATE     4
#define DS1337_MTH	5
#define DS1337_YR	6

#define DS1337_BASE_YR		2000

#define DS1337_ADDR			(0x68 << 1)

// Define register bit masks
#define DS1337_CLOCKHALT	0x80

#define DS1337_LO_BCD		0xF
#define DS1337_HI_BCD		0xF0

#define DS1337_HI_SEC		0x00
#define DS1337_HI_MIN		0x01
#define DS1337_HI_HR		0x02
#define DS1337_LO_DOW		0x03
#define DS1337_HI_DATE		0x04
#define DS1337_HI_MTH		0x05
#define DS1337_HI_YR		0x06

#define DS1337_ARLM1		0x07
#define DS1337_ARLM1_LO_SEC	0xF
#define DS1337_ARLM1_HI_SEC	0x70
#define DS1337_ARLM1_LO_MIN	0x70
#define DS1337_ARLM1_HI_MIN	0xF

#define DS1337_SP			0x0E
#define	DS1337_SP_EOSC		0x80
#define	DS1337_SP_RS2		0x10
#define	DS1337_SP_RS1		0x8
#define	DS1337_SP_INTCN		0x4
#define	DS1337_SP_A2IE		0x2
#define	DS1337_SP_A1IE		0x1

#define DS1337_STATUS		0x0F
#define DS1337_STATUS_OSF	0x80
#define DS1337_STATUS_A2F	0x2
#define DS1337_STATUS_A1F	0x1

/* Definitions for alarm repeat */
/* The private variable alarm_repeat holds the user's alarm repeat preference. However, the DS1337 encodes these in the topmost bit(s) of the 4 alarm registers. */
/* Splattering these bits across the alarm regs is handled in the writeAlarm() function. */
/* If DY/DT is set, the day field is interpreted as a DayOfWeek (1 ~ 7), else it is interpreted as a DayOfMonth.*/

/* user alarm_repeat bit mask:
       7   6   5    4       3      2       1     0
      [x   x   x   A1M4   DY/DT   A1M3   A1M2   A1M1]
*/

#define EVERY_SECOND       0x17
#define EVERY_MINUTE       0x16
#define EVERY_HOUR         0x14
#define EVERY_DAY          0x10
#define EVERY_WEEK         0x8
#define EVERY_MONTH        0x0



// library interface description
class DS1337 {
	I2C *m_i2c;
	// user-accessible "public" interface
	public:
		void init(I2C *i2c);

		uint8_t time_is_set();
		uint8_t alarm_is_set();
		//uint8_t time_is_valid();

		void enable_interrupt();
		void disable_interrupt();
		void clear_interrupt();
		void readTime();
		void readAlarm();
		void writeTime();
		void writeTime(uint32_t);
		void writeAlarm();
		void writeAlarm(uint32_t);
		void setAlarmRepeat(uint8_t repeat);
		uint8_t getSeconds();
		uint8_t getMinutes();
		uint8_t getHours();
		uint8_t getDays();
		uint8_t getDayOfWeek();
		uint8_t getMonths();
		uint16_t getYears();
		uint32_t date_to_epoch_seconds(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
		uint32_t date_to_epoch_seconds();
		void epoch_seconds_to_date(uint32_t);
		void snooze(uint32_t secondsToSnooze);
		void custom_snooze(uint32_t secondsToSnooze);

		void setSeconds(uint8_t);
		void setMinutes(uint8_t);
		void setHours(uint8_t);
		void setDays(uint8_t);
		void setDayOfWeek(uint8_t);
		void setMonths(uint8_t);
		void setYears(uint16_t);

		void start(void);
		void stop(void);
		uint8_t getRegister(uint8_t registerNumber);
		void setRegister(uint8_t registerNumber, uint8_t registerValue);
	private:
		uint8_t time_set;
		uint8_t alarm_repeat;
		uint8_t	rtc_bcd[7]; // used prior to read/set DS1337 registers;
		void	read(void);
		void	save(void);
		uint8_t bcd2bin(uint8_t);
		uint8_t bin2bcd(uint8_t);
};

