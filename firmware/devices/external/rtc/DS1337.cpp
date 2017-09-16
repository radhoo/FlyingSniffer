/*
 Yet-Another DS1337 RTC Library
 By Tim Gipson (drmn4ea at google's mail)
 
 Based loosely on mattt and xSmurf's RTC library at (http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1191209057/0)
 epoch_seconds_to_date() function based on this Dallas/Maxim application note: http://www.maxim-ic.com/app-notes/index.mvp/id/517
 
 This library is written for use with the Mosquino (http://code.google.com/p/mosquino/),
 but should be adaptable to other boards with no/minimal changes 
 (mainly just changing the RTC_INT_NUMBER in DS1337.h if different, or modifying the alarm 
 functions to work without interrupts if needed).
 
*/
#include "DS1337.h"

extern "C" {

  #include <avr/power.h>
  #include <avr/sleep.h>

  // Dummy "interrupt handler" for sleep to wake up to on alarm interrupt
  void _dummy_int_handler(void)
  {

  }
  
}

// NOTE: To keep the math from getting even more lengthy/annoying than it already is, the following constraints are imposed:
//   1) All times are in 24-hour format (military time)
//   2) DayOfWeek field is not used internally or checked for validity. Alarm functions may optionally set alarms repeating on DayOfWeek, but this feature has not been tested yet.
//   3) This library's buffer stores all times in raw BCD format, just as it is sent from the RTC.
//      It is not converted to/from 'real' (binary) values until needed via get...() and set...() functions.
//      In other words, don't go hacking around and reading from the rtc_bcd[] buffer directly, unless you want the raw BCD results.


// Cumulative number of days elapsed at the start of each month, assuming a normal (non-leap) year.
const uint16_t monthdays[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

void DS1337::init(I2C *i2c) {
	m_i2c = i2c;
	time_set = 0;
	alarm_repeat = 0;
  //pinMode(RTC_INT_PIN, INPUT);
  //digitalWrite(RTC_INT_PIN, HIGH);  // enable software pullup resistor on RTC interrupt pin
}

// Aquire data from the RTC chip in BCD format
// refresh the buffer
void DS1337::readTime(void) {
	m_i2c->readmem(DS1337_ADDR, 0, rtc_bcd, 7);
}

// Read the current alarm value. Note that the repeat flags and DY/DT are removed from the result.
void DS1337::readAlarm(void) {
	// request the *4* uint8_ts of data (secs, min, hr, dow/date). Note the format is nearly identical, except for the choice of dayOfWeek vs. date,
	// and that the topmost bit of each helps determine if/how the alarm repeats.
	m_i2c->readmem(DS1337_ADDR, DS1337_ARLM1, rtc_bcd, 4);
	// 4th uint8_t read may contain either a date or DayOfWeek, depending on the value of the DY/DT flag.
	// For laziness sake we read it into the DayOfWeek field regardless (rtc_bcd[3]). Correct as needed...
	if(rtc_bcd[3] & 0x40) { // DY/DT set: DayOfWeek
	   rtc_bcd[3] &= 0xBF; // clear DY/DT flag
	   rtc_bcd[4] = 0; // alarm *date* undefined
	} else {
		rtc_bcd[4] = rtc_bcd[3];
		rtc_bcd[3] = 0; // alarm dayOfWeek undefined
	}
}

// update the data on the IC from the bcd formatted data in the buffer
void DS1337::writeTime(void) {
	m_i2c->writemem(DS1337_ADDR, 0, rtc_bcd, 7);
	// clear the Oscillator Stop Flag
	setRegister(DS1337_STATUS, getRegister(DS1337_STATUS) & !DS1337_STATUS_OSF);
}

void DS1337::writeTime(uint32_t sse) {
	epoch_seconds_to_date(sse);
	writeTime();
}

// FIXME: automatically set alarm interrupt after writing new alarm? Nah...

// Write the BCD alarm value in the buffer to the alarm registers.
// If an alarm repeat mode has been specified, poke those uint8_ts into the buffer before sending.
void DS1337::writeAlarm(void) {
	uint8_t buf[4] = {0};

	buf[0] = rtc_bcd[DS1337_SEC] | ((alarm_repeat & 0x1 ) << 7); // A1M1
	buf[1] = rtc_bcd[DS1337_MIN] | ((alarm_repeat & 0x2 ) << 6); // A1M2
	buf[2] = rtc_bcd[DS1337_HR] | ((alarm_repeat & 0x4 ) << 5); // A1M3

	// Check if we are using date or DayOfWeek and send the appropriate value
	if(alarm_repeat & 0x8) {// DayOfWeek
		// send DOW as 4th alarm reg uint8_t
		buf[3] = rtc_bcd[DS1337_DOW] | ((alarm_repeat & 0x18 ) << 3); // A1M4 and DY/DT
	} else {// date
		// send date as 4th alarm reg uint8_t
		buf[3] = rtc_bcd[DS1337_DATE] | ((alarm_repeat & 0x18 ) << 3); // A1M4 and DY/DT
	}
	m_i2c->writemem(DS1337_ADDR, DS1337_ARLM1, buf, 4);
}


void DS1337::writeAlarm(uint32_t sse) {
	epoch_seconds_to_date(sse);
	writeAlarm();
}

void DS1337::setAlarmRepeat(uint8_t repeat) {
	alarm_repeat = repeat;
}


uint8_t DS1337::getRegister(uint8_t registerNumber) {
	uint8_t b = 0;
	m_i2c->readmem(DS1337_ADDR, registerNumber, &b);
	return b;
}

void DS1337::setRegister(uint8_t registerNumber, uint8_t value) {
	m_i2c->writemem(DS1337_ADDR, registerNumber, value);
}

uint8_t DS1337::time_is_set() {
	// Return TRUE if Oscillator Stop Flag is clear (osc. not stopped since last time setting), FALSE otherwise
	return ((getRegister(DS1337_STATUS) & DS1337_STATUS_OSF) == 0);
}

uint8_t DS1337::alarm_is_set() {
	// Return TRUE if the alarm interrupt flag is enabled.
	return (getRegister(DS1337_SP) & DS1337_SP_A1IE);
}

void DS1337::enable_interrupt() {
	clear_interrupt();
	setRegister(DS1337_SP, getRegister(DS1337_SP) | DS1337_SP_INTCN | DS1337_SP_A1IE); // map alarm interrupt to INT1 and enable interrupt
}

void DS1337::disable_interrupt() {
	setRegister(DS1337_SP, getRegister(DS1337_SP) & !DS1337_SP_A1IE);
}

void DS1337::clear_interrupt() {
	setRegister(DS1337_STATUS, getRegister(DS1337_STATUS) & !DS1337_STATUS_A1F);
}

uint8_t DS1337::getSeconds() {
	return bcd2bin(rtc_bcd[DS1337_SEC]);
}

uint8_t DS1337::getMinutes() {
	return bcd2bin(rtc_bcd[DS1337_MIN]);
}

uint8_t DS1337::getHours() {
	return bcd2bin(rtc_bcd[DS1337_HR]);
}

uint8_t DS1337::getDays() {
	return bcd2bin(rtc_bcd[DS1337_DATE]);
}

uint8_t DS1337::getDayOfWeek() {
	return bcd2bin(rtc_bcd[DS1337_DOW]);
}

uint8_t DS1337::getMonths() {
	return bcd2bin(rtc_bcd[DS1337_MTH]);
}


uint16_t DS1337::getYears() {
    return 2000 + bcd2bin(rtc_bcd[DS1337_YR]);
}


uint32_t DS1337::date_to_epoch_seconds(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
  //gracefully handle 2- and 4-digit year formats
  if (year > 1999) {
     year -= 2000;
  }


// Between year 2000 and 2100, a leap year occurs in every year divisible by 4.

//   sse_y = (((uint32_t)year)*365*24*60*60);
//   sse_ly = ((((uint32_t)year+3)>>2) + ((uint32_t)year%4==0 && (uint32_t)month>2))*24*60*60;
//   sse_d = ((uint32_t)monthdays[month-1] + (uint32_t)day-1) *24*60*60;
//   sse_h = ((uint32_t)hour*60*60);
//   sse_m = ((uint32_t)minute*60);
//   sse_s = (uint32_t)second;
//
//   sse = sse_y + sse_ly + sse_d + sse_h + sse_m + sse_s;



// NB: The multiplication-by-constants below is intentionally left expanded for readability; GCC is smart and will optimize them to single constants during compilation.


  //         Whole year seconds                      Cumulative total of seconds contributed by elapsed leap year days
  uint32_t sse = (((uint32_t)year)*365*24*60*60)   +   ((((uint32_t)year+3)>>2) + ((uint32_t)year%4==0 && (uint32_t)month>2))*24*60*60   +   \
         ((uint32_t)monthdays[month-1] + (uint32_t)day-1) *24*60*60   +   ((uint32_t)hour*60*60)   +   ((uint32_t)minute*60)   + (uint32_t)second;
         // Seconds in days since start of year                      hours                      minutes           sec
  sse += 946684800; // correct for difference between DS1337 epoch and UNIX epoch
  return sse;
}


uint32_t DS1337::date_to_epoch_seconds()
{
     uint32_t asdf = date_to_epoch_seconds(int(bcd2bin(rtc_bcd[DS1337_YR])), bcd2bin(rtc_bcd[DS1337_MTH]), bcd2bin(rtc_bcd[DS1337_DATE]), bcd2bin(rtc_bcd[DS1337_HR]), bcd2bin(rtc_bcd[DS1337_MIN]), bcd2bin(rtc_bcd[DS1337_SEC]));
     return asdf;
}



void DS1337::epoch_seconds_to_date(uint32_t seconds_left)
{
   // This routine taken from Dallas/Maxim application note 517
   // http://www.maxim-ic.com/app-notes/index.mvp/id/517
   // Arn't the fastest thing, but it produces correct results.

   // NOTE: The earliest date that can be represented by the DS1337 is 1/1/2000 (946684800 in Unix epoch seconds).
   // Passing an earlier Unix time stamp will fail quietly here (produce a date of 0/0/00), 
   // which will probably make your application angry.

   // ALSO NOTE: This has been optimized some to minimize redundant variables, with the side-effect
   // of making it much harder to understand. Please refer to the original appnote above
   // if you are trying to learn from it :-)


   //uint32_t hour;
   //uint32_t day;
   //uint32_t minute;
   //uint32_t second;
   uint32_t month;
   //uint32_t year;

  uint32_t seconds_left_2;
   //uint32_t whole_minutes;
   //uint32_t whole_hours;
   //uint32_t whole_days;
   //uint32_t whole_days_since_1968;
   uint32_t leap_year_periods;
   uint32_t days_since_current_lyear;
   //uint32_t whole_years;
   uint32_t days_since_first_of_year;
   uint32_t days_to_month;
   //uint32_t day_of_week;

   if(seconds_left >= 946684800)
   {
     seconds_left -= 946684800; // correct for difference between DS1337 and UNIX epochs.

     seconds_left_2 = seconds_left / 60; // seconds_left_2 = "whole_minutes"
     rtc_bcd[DS1337_SEC] = bin2bcd(seconds_left - (60 * seconds_left_2));                 // leftover seconds

     seconds_left = seconds_left_2 / 60; // seconds_left = "whole_hours"
     rtc_bcd[DS1337_MIN] = bin2bcd(seconds_left_2 - (60 * seconds_left));            // leftover minutes

     seconds_left_2 = seconds_left / 24; //seconds_left_2 = "whole_days"
     rtc_bcd[DS1337_HR] = bin2bcd(seconds_left - (24 * seconds_left_2));         // leftover hours

     //whole_days_since_1968 = whole_days;// + 365 + 366; // seconds_left_2 = "whole_days" = "whole_days_since_1968"
     leap_year_periods = seconds_left_2 / ((4 * 365) + 1);

     days_since_current_lyear = seconds_left_2 % ((4 * 365) + 1);

     // if days are after a current leap year then add a leap year period
     if ((days_since_current_lyear >= (31 + 29))) {
      leap_year_periods++;
     }
     seconds_left = (seconds_left_2 - leap_year_periods) / 365; // seconds_left = "whole_years"
     days_since_first_of_year = seconds_left_2 - (seconds_left * 365) - leap_year_periods;

     if ((days_since_current_lyear <= 365) && (days_since_current_lyear >= 60)) {
      days_since_first_of_year++;
     }
     //year = seconds_left; // + 68;


    // seconds_left = "year"
    //seconds_left_2 = "month"
     // walk across monthdays[] to find what month it is based on how many days have passed
     //   within the current year
     month = 13;
     days_to_month = 366;
     while (days_since_first_of_year < days_to_month) {
       month--;
       days_to_month = monthdays[month-1];
       if ((month > 2) && ((seconds_left % 4) == 0)) {
         days_to_month++;
      }
     }
     
     rtc_bcd[DS1337_DATE] = bin2bcd( days_since_first_of_year - days_to_month + 1);

     rtc_bcd[DS1337_DOW] = bin2bcd((seconds_left_2  + 4) % 7);


     //rtc_bcd[DS1337_SEC] = bin2bcd(second);
     //rtc_bcd[DS1337_MIN] = bin2bcd(minute);
     //rtc_bcd[DS1337_HR] = bin2bcd(hour);
     //rtc_bcd[DS1337_DATE] = bin2bcd(day);
     //rtc_bcd[DS1337_DOW] = bin2bcd(day_of_week);
     rtc_bcd[DS1337_MTH] = bin2bcd(month);
     rtc_bcd[DS1337_YR] = bin2bcd(seconds_left);
   }
  else
  {
  // else: "invalid" (< year 2000) epoch format.
  // 'Best' way to handle this is to zero out the returned date. 
  
     rtc_bcd[DS1337_SEC] = 0; //0x00 binary = 0x00 BCD
     rtc_bcd[DS1337_MIN] = 0;
     rtc_bcd[DS1337_HR] = 0;
     rtc_bcd[DS1337_DATE] = 0;
     rtc_bcd[DS1337_DOW] = 0;
     rtc_bcd[DS1337_MTH] = 0;
     rtc_bcd[DS1337_YR] = 0;
  }

}





void DS1337::snooze(uint32_t secondsToSnooze)
{ 
  // Given a value in secondsToSnooze, set an alarm for that many seconds into the future and go to sleep.
  // The alarm can be set for a maximum of 28-31 days into the future - it doesn't have settings for months or years.
  
  uint8_t sleep_reg_temp;
  
  readTime(); // update RTC library's buffers to contain the current time.
                  // Remember most functions (including epoch seconds stuff) work on what's in the buffer, not what's in the chip.

  
  setAlarmRepeat(EVERY_MONTH); // There is no DS1337 setting for 'alarm once' - once in a month is the most restrictive it gets.

  writeAlarm(date_to_epoch_seconds() + secondsToSnooze);
 
  //attachInterrupt(RTC_INT_NUMBER, _dummy_int_handler, FALLING);
  //enable_interrupt();
  
  // the default snooze behavior is to put the CPU all the way to sleep. In case the user has previously set a different sleep mode,
  // save the entry sleep mode and restore it after sleeping. NOTE, set_sleep_mode() in avr/sleep.h is actually a giant device-specific mess
  // (making trying to implement a 'get_sleep_mode'-type function that works for all devices an equally nasty mess), but this should cover MOST
  // of the ones likely to be used with Arduino. For those others, user will have to (re)set the desired sleep mode by hand.
  
  #if defined(_SLEEP_CONTROL_REG)
  sleep_reg_temp = _SLEEP_CONTROL_REG;
  #endif
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  // enable deep sleeping
  sleep_enable();
  
  sleep_cpu(); // sleep. Will we waked by next alarm interrupt
 
  #if defined(_SLEEP_CONTROL_REG)
  _SLEEP_CONTROL_REG = sleep_reg_temp;
  #endif
  
  clear_interrupt(); // tell RTC to clear its interrupt flag and drop the INT line
  disable_interrupt(); // ensure we stop receiving interrupts
  //detachInterrupt(RTC_INT_NUMBER); // disconnect INT2 from the current interrupt handler.
  
}

void DS1337::custom_snooze(uint32_t secondsToSnooze)
{ 
  // Same as snooze(), but do not change the current sleep mode. Use to sleep at a custom sleep mode other than ...PWR_DOWN.
  // Intentional use of a lighter sleep mode means the user is probably expecting/handling other interrupts - note of course that
  // most interrupts will wake the CPU from sleep mode, so the snooze may be shorter than specified in this case.
   
  readTime(); // update RTC library's buffers to contain the current time.
                  // Remember most functions (including epoch seconds stuff) work on what's in the buffer, not what's in the chip.

  
  setAlarmRepeat(EVERY_MONTH); // There is no DS1337 setting for 'alarm once' - once in a month is the most restrictive it gets.

  writeAlarm(date_to_epoch_seconds() + secondsToSnooze);
 
  //attachInterrupt(RTC_INT_NUMBER, _dummy_int_handler, FALLING);
  //enable_interrupt();
  
  // enable deep sleeping
  sleep_enable();
  
  sleep_cpu(); // sleep. Will we waked by next alarm interrupt
 
  clear_interrupt(); // tell RTC to clear its interrupt flag and drop the INT line
  disable_interrupt(); // ensure we stop receiving interrupts
  //detachInterrupt(RTC_INT_NUMBER); // disconnect INT2 from the current interrupt handler.
  
}

void DS1337::setSeconds(uint8_t v)
{
    rtc_bcd[DS1337_SEC] = bin2bcd(v);

}
void DS1337::setMinutes(uint8_t v)
{
    rtc_bcd[DS1337_MIN] = bin2bcd(v);

}
void DS1337::setHours(uint8_t v)
{
    rtc_bcd[DS1337_HR] = bin2bcd(v);

}
void DS1337::setDays(uint8_t v)
{
    rtc_bcd[DS1337_DATE] = bin2bcd(v);

}
void DS1337::setDayOfWeek(uint8_t v)
{
    rtc_bcd[DS1337_DOW] = bin2bcd(v);

}
void DS1337::setMonths(uint8_t v)
{
    rtc_bcd[DS1337_MTH] = bin2bcd(v);

}
void DS1337::setYears(uint16_t v)
{
    if (v>1999)
    {
        v -= 2000;
    }
    rtc_bcd[DS1337_YR] = bin2bcd(v);

}

uint8_t DS1337::bcd2bin(uint8_t v)
{
   return (v&0x0F) + ((v>>4)*10);
}

uint8_t DS1337::bin2bcd(uint8_t v)
{
   return ((v / 10)<<4) + (v % 10);
}

void DS1337::stop(void)
{
  setRegister(DS1337_SP, getRegister(DS1337_SP) | DS1337_SP_EOSC);
}

void DS1337::start(void)
{
  setRegister(DS1337_SP, getRegister(DS1337_SP) & !DS1337_SP_EOSC);
}
