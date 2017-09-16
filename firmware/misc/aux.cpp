/*
** uRADMonitor auxiliary code
** Version: 	0.1.0
** Date: 		January, 2014
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
** Description:	Used in the uRADMonitor code to serve various purposes
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

#include "aux.h"


// Reset the microcontroller
void aux_softwarereset() {
	asm ("JMP 0");
	// http://forum.arduino.cc/index.php?topic=78020.0
}

// Proper reset using the watchdog: untested
void aux_wdreset() {
  wdt_enable(WDTO_15MS);
  while(1);
}



/**
 * Encodes a small float with two digits (-127.XX .. 127.XX) to an 16 bit integer, including sign and two decimals
 */
uint16_t double2int(double val) {
	int8_t sintp = (int8_t)val;				//signed integer part
	int8_t sdecp = (val - sintp ) * 100;	//signed decimal part
	uint8_t udecp = sdecp>0?sdecp:-1*sdecp; //removed decimal sign
	uint8_t uintp = sintp + 127;			//convert to unsigned
	uint16_t res = (udecp << 8) | (uint16_t)uintp;		//pack it together
	return res;
}

uint32_t coordToInt(double latlong, uint32_t multiplier) {
	uint32_t res = fabs(latlong) * multiplier;
	if (latlong < 0) res |= 0x80000000;
	return res;
}

double intToCoord(uint32_t coord, uint32_t multiplier) {
	if (coord & 0x80000000)
		return (coord & 0x0FFFFFFF) / -(double)multiplier;
	else
		return coord / (double)multiplier;
}



/**
 * copyBytes
 * copy specified src_bytes from src to dst at specified offset
 * this function is not safe, as dst is not checked for size. Make sure it is big enough!
 */
uint16_t copyBytes(void *dst, uint16_t dst_offset, void *src, uint8_t src_bytes) {
	if (!dst || !src_bytes) return dst_offset;

	for (int i=0; i< src_bytes;i++)
		((uint8_t *)dst)[dst_offset + i] = !src? 0 : ((uint8_t *)src)[src_bytes - i - 1]; // either 0 if empty source or most significant bytes go first
	return dst_offset + src_bytes;
}



/**
 * jsonKeyFind
 * finds a key and copies its value to the value output pointer
 * {"success":"ok"}
 * +CIFSR:APIP,"192.168.4.1"
 *	+CIFSR:STAIP,"192.168.2.106"
 */
bool jsonKeyFind(char *response, char *key, char *value, uint8_t size, uint8_t offset) {
	memset(value, 0, size);

	char *s1 = strstr(response, key);
	uint8_t len = strlen(key);
	if (s1 && len) {
		char *s2 = strstr(s1 + len + offset, "\"");
		if (s2) {
			strncpy(value, s1 + len + offset, MIN(s2 - s1 - len - offset, size - 1) );
			return true;
		}
	}
	return false;
}

/**
 * getKeyFind
 * finds a key and copies its value to the output pointer
 * example: getKeyFind("/s?u=test&p=123_this%40is.ke+mab123 HTTP/1.1", "u=" .. will return test
 */
bool getKeyFind(char *response, char *key, char *value, uint8_t size) {
	memset(value, 0, size);

	char *s1 = strstr(response, key);
	uint8_t len = strlen(key);
	if (s1 && len) {
		char *s2 = strstr(s1 + len, "&");
		char *s3 = strstr(s1 + len, " ");
		if (s2)
			strncpy(value, s1 + len, MIN(s2 - s1 - len, size - 1) );
		else
			strncpy(value, s1 + len, MIN(s3 - s1 - len, size - 1) );
		return true;

	}
	return false;
}
/*
 * Searches for a substring in the rigth part of a string after a given key
 * The substring should be delimited by two other substrings, startwith and stopwith
 * the result is returned in pointer 'value'
 * True on success, false if the search op didn't find any results
 */
bool find(char *response, char *key, char *startwith, char *stopwith, char *value, uint8_t size) {
	memset(value, 0, size);
	char *s1 = strstr(response, key);
	uint8_t len1 = strlen(key);
	if (s1 && len1) {
		char *s2 = strstr(s1 + len1, startwith);
		uint8_t len2 = strlen(startwith);
		if (s2) {
			char *s3 = strstr(s2 + len2, stopwith);
			if (s3) {
				strncpy(value, s2 + len2, MIN(s3 - s2 - len2, size - 1) );
				return true;
			}
		}
	}
	return false;
}

uint8_t hexdigit(uint8_t digit) {
	if (digit >= '0' && digit <= '9') return digit  - '0';
    else if (digit >= 'a' && digit <='f') return digit - ('a' - 10);
    else if (digit >= 'A' && digit <='F') return digit - ('A' - 10);
    else return 0;
}

/**
 * hex2int
 * take a hex string and convert it to a 32bit number (max 8 hex digits)
 */
uint32_t hex2int(char *hex) {
    uint32_t val = 0;
     while (*hex) {
    	// shift 4 to make space for new digit, and add the 4 bits of the new digit
    	val = (val << 4) | (hexdigit(*hex++) & 0xF);
    }
    return val;
}

void hex2bin(char *buffer, uint16_t len) {
	for (uint16_t i=0;i<len / 2; i++)
		buffer[i] = (hexdigit(buffer[i*2]) << 4) + hexdigit(buffer[i*2 + 1]);
	buffer[len / 2] = 0;
}

// handle hex codes and plus character for encoded urls
void urldecode(char *urlenc) {
	char *dec = urlenc;
	while (*urlenc) {
	    if (*urlenc == '+')
	    	*dec = ' ';
	    else if (*urlenc == '%')
			*dec = (hexdigit(*++urlenc) << 4) | hexdigit(*++urlenc);
		else
			*dec = *urlenc;
		// next character
	    dec++;
	    urlenc++;
	}
	*dec = 0;
}

// original CRC32
// note: if the seed is not "big" enough, first values in buffer will not have any influence
uint32_t crc32(uint8_t *buffer, int length, uint32_t seed) {
	uint32_t crc = seed;
	int i, j;
	for (i=0; i<length; i++) {
        crc = crc ^ *(buffer++);
		for (j=0; j<8; j++) {
			if (crc & 1)
		        crc = (crc>>1) ^ 0xEDB88320UL ;
			else
		        crc = crc >>1 ;
        }
    }
	return crc ^ 0xFFFFFFFFUL;
}


// an uint16_t stores 16 bits, we use first bit for sign, and following 15 bits for number (0..32767)
// result is divided by 100 for a real with 2 decimals, max is 327.00
uint16_t di(double val) {
	uint16_t res = 0;
	uint16_t mask = 1<<15;
	if (val > 327 || val < -327) return res;
	if (val < 0) {
		val *= -1000;
		res = val / 10; // fix double approximation issue
		res |= mask;
	} else {
		val *= 1000;
		res = val / 10; // fix double approximation issue
	}
	return res;
}

// an uint16_t stores 16 bits, we use first bit for sign, and following 15 bits for number (0..32767)
// result is divided by 100 for a real with 2 decimals, max is 327.00
double id(uint16_t val) {
	double res = 0;
	uint16_t mask = 1<<15;
	// check negative number
	if (val & mask) {
		val &= ~mask; // remove sign bit
		res = val;
		res = -res;
	} else {
		res = val;
	}
	res /= 100.0; // restore the 2 decimals
	return res;
}


