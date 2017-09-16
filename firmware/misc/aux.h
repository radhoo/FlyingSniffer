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

#pragma once

#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#define MIN(x,y) (x<y)?x:y


// Reset the microcontroller
void aux_softwarereset();

// Proper reset using the watchdog: untested
void aux_wdreset() ;

// not more than 10 chars!
char *aux_detectorName(uint8_t param);

/**
 * Encodes a small float (-127 .. 127) to an 16 bit integer, including sign and two decimals
 */
uint16_t double2int(double val);

uint32_t coordToInt(double latlong, uint32_t multiplier);

double intToCoord(uint32_t coord, uint32_t multiplier);

/**
 * copyBytes
 * copy specified src_bytes from src to dst at specified offset
 * this function is not safe, as dst is not checked for size. Make sure it is big enough!
 */
uint16_t copyBytes(void *dst, uint16_t dst_offset, void *src, uint8_t src_bytes);


/**
 * jsonKeyFind
 * finds a key and copies its value to the value output pointer
 */
bool jsonKeyFind(char *response, char *key, char *value, uint8_t size, uint8_t offset = 3);

/**
 * getKeyFind
 * finds a key and copies its value to the output pointer
 * example: getKeyFind("/s?u=test&p=123_this%40is.ke+mab123 HTTP/1.1", "u=" .. will return test
 */
bool getKeyFind(char *response, char *key, char *value, uint8_t size);

/*
 * Searches for a substring in the rigth part of a string after a given key
 * The substring should be delimited by two other substrings, startwith and stopwith
 * the result is returned in pointer 'value'
 * True on success, false if the search op didn't find any results
 */
bool find(char *response, char *key, char *startwith, char *stopwith, char *value, uint8_t size);

/**
 * hex2int
 * take a hex string and convert it to a 32bit number (max 8 hex digits)
 */
uint32_t hex2int(char *hex);


/**
 * getKeyFind
 * finds a key and copies its value to the output pointer
 * example: getKeyFind("/s?u=test&p=123_this%40is.ke+mab123 HTTP/1.1", "u=" .. will return test
 */
bool getKeyFind(char *response, char *key, char *value, uint8_t size);

uint8_t hexdigit(uint8_t digit);


void hex2bin(char *buffer, uint16_t len);

// handle hex codes and plus character for encoded urls
void urldecode(char *urlenc) ;

/*double recAvg(double new_value, double current_avg, uint16_t current_count) ;
uint16_t recAvg(uint16_t new_value, uint16_t current_avg, uint16_t current_count) ;
uint32_t recAvg(uint32_t new_value, uint32_t current_avg, uint16_t current_count) ;*/
//#define recAvg(new_value, current_avg, current_count) ((new_value + current_avg * current_count) / (current_count + 1))
#define recAvg(new_value, current_avg, current_count) (double)(new_value / (double)(current_count +1) + current_avg * ((double)current_count / (double) (current_count + 1)))

// original CRC32
// note: if the seed is not "big" enough, first values in buffer will not have any influence
uint32_t crc32(uint8_t *buffer, int length, uint32_t seed = 0xFFFFFFFFUL);



// an uint16_t stores 16 bits, we use first bit for sign, and following 15 bits for number (0..32767)
// result is divided by 100 for a real with 2 decimals, max is 327.00
uint16_t di(double val);

// an uint16_t stores 16 bits, we use first bit for sign, and following 15 bits for number (0..32767)
// result is divided by 100 for a real with 2 decimals, max is 327.00
double id(uint16_t val);
