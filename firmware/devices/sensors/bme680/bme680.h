/*
 ** BME680 Sensor Library for AVR Microcontrollers
 ** Version: 	0.1.0
 ** Date: 		September, 2015
 ** License:		LGPL v3
 ** Copyright: 	(C) 2009 - 2012 Radu Motisan, radu.motisan@gmail.com
 ** Description:	The BME680 from Bosch is a temperature and pressure sensor with a I2C interface
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

#pragma  once
//a
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
//#include <util/delay.h>
#include "../../system/i2c/i2c.h"

// Connecting SDO to GND results in slave
//0x76 default I2C address
// address 1110110 (0x76); connection it to VDDIO results in slave address 1110111 (0x77), which
// is the same as BMP280’s I²C address
#define BME680_ADDR (0x77<<1)
#define BME680_CHIP_ID			(0x61)

#define BME680_VALUE_TRUE		(0x01)
#define BME680_VALUE_FALSE		(0x00)

// numeric definitions
#define BME680_TEMP2_XLSB				(1)
#define BME680_CALIB_DATA_LENGTH        (16)
#define BME680_CALIB_DATA_LENGTH_GAS    (25)
#define	BME680_CALIB_REG_DATA			(1)
#define	BME680_NEW_DATA_STAT			(0x01)
#define	BME680_BIT_MASK_H2_DATA			(0x0F)

#define BME680_CALIB_PARAM_SIZE			41

// temperature to Resistance  formulae #defines

//Enable the macros according to heater versions
//#define HEATER_C1_ENABLE
//#define HEATER_V05_ENABLE
#define HEATER_V03_ENABLE

//698
// PAGE_1 REGISTER DEFINITIONS
#define BME680_PAGE1_HEAT_LOOP_CTRL_1_REG		   (0x02)
#define BME680_PAGE1_CURR_LIM_REG				   (0x04)
//782
#define BME680_PAGE1_FIELD_0_PRESSURE_MSB_REG      (0x1F)
#define BME680_PAGE1_FIELD_0_TEMPERATURE1_MSB_REG  (0x22)
#define BME680_PAGE1_FIELD_0_HUMIDITY_MSB_REG      (0x25)
#define BME680_PAGE1_FIELD_0_GAS_RL_MSB_REG        (0x2A)
#define BME680_PAGE1_FIELD_0_GAS_RL_LSB_REG        (0x2B)
#define BME680_PAGE1_RES_HEAT_0_REG                (0x5A)
#define BME680_PAGE1_RES_HEAT_1_REG                (0x5B)
#define BME680_PAGE1_RES_HEAT_2_REG                (0x5C)
#define BME680_PAGE1_RES_HEAT_3_REG                (0x5D)
#define BME680_PAGE1_RES_HEAT_4_REG                (0x5E)
#define BME680_PAGE1_RES_HEAT_5_REG                (0x5F)
#define BME680_PAGE1_RES_HEAT_6_REG                (0x60)
#define BME680_PAGE1_RES_HEAT_7_REG                (0x61)
#define BME680_PAGE1_RES_HEAT_8_REG                (0x62)
#define BME680_PAGE1_RES_HEAT_9_REG                (0x63)
#define BME680_PAGE1_GAS_WAIT_0_REG                (0x64)
#define BME680_PAGE1_CONTROL_GAS1_REG              (0x71)
#define BME680_PAGE1_CONTROL_HUMIDITY_REG          (0x72)
#define BME680_PAGE1_CONTROL_MEASUREMENT_REG       (0x74)
#define BME680_PAGE1_CONFIG_REG                    (0x75)

// CONSTANTS FOR PROFILE INFORMATIONS
#define BME680_MAX_NUMBER_PROFILES		(10)
#define BME680_PROFILE_MIN				(0)
#define BME680_PROFILE_MAX				(10)

#define SUBMEAS_INDEX_MAX_VALUE			((u8)0xFF)
#define MAX_HUMIDITY_VALUE					((double)100.0)
#define MIN_HUMIDITY_VALUE					((double)0.0)

#define GAS_PROFILE_TEMPERATURE_MIN		(200)
#define GAS_PROFILE_TEMPERATURE_MAX		(400)

#define BME680_REG_MAX_VALUE					(0xFF)
#define	BME680_GAS_WAIT_MAX_TIMER_VALUE			(0x3F)
#define	BME680_GAS_WAIT_MIN_TIMER_VALUE			(0x00)
#define	BME680_GAS_WAIT_MAX_WAIT_TIME			(0xFC0)
#define BME680_GAS_MULTIPLICATION_FACTOR_ONE    (4)
#define BME680_GAS_MULTIPLICATION_FACTOR_TWO	(16)
#define BME680_GAS_MULTIPLICATION_FACTOR_THREE	(64)

//821
#define BME680_GAS_RANGE_RL_LENGTH				(16)
#define BME680_SIGN_BIT_MASK					(0x08)

//1069
// BIT MASK, LENGTH AND POSITION DEFINITIONS OF PAGE1
// PAGE1  FIELD0 DEFINITIONS
// BIT MASK, LENGTH AND POSITION DEFINITIONS OF PAGE1
// MEASUREMENT STATUS
// Page1_RES_HEAT_RANGE Description - Reg Addr --> 0x02, Bit --> 4 to 5
#define BME680_PAGE1_RES_HEAT_RANGE__POS	(4)
#define BME680_PAGE1_RES_HEAT_RANGE__LEN	(2)
#define BME680_PAGE1_RES_HEAT_RANGE__MSK	(0x30)
#define BME680_PAGE1_RES_HEAT_RANGE_ADDR	(BME680_PAGE1_HEAT_LOOP_CTRL_1_REG)
// Page1_RANGE_SWITCHING_ERROR Description - Reg Addr --> 0x04, Bit --> 4 to 7
#define BME680_PAGE1_RANGE_SWITCHING_ERROR__POS		(4)
#define BME680_PAGE1_RANGE_SWITCHING_ERROR__LEN		(4)
#define BME680_PAGE1_RANGE_SWITCHING_ERROR__MSK		(0xF0)
#define BME680_PAGE1_RANGE_SWITCHING_ERROR__ADDR	(BME680_PAGE1_CURR_LIM_REG)

//1189
// Page1_FIELD_0_HUMIDITY.15:8 Description - Reg Addr --> 0x25, Bit --> 0 to 7
#define BME680_PAGE1_FIELD_0_HUMIDITY_MSB_REG__POS	(0)
#define BME680_PAGE1_FIELD_0_HUMIDITY_MSB_REG__LEN	(8)
#define BME680_PAGE1_FIELD_0_HUMIDITY_MSB_REG__MSK	(0xFF)
#define BME680_PAGE1_FIELD_0_HUMIDITY_MSB_REG__ADDR	(BME680_PAGE1_FIELD_0_HUMIDITY_MSB_REG)
// BIT MASK, LENGTH AND POSITION DEFINITIONS OF PAGE1
// FIELD0 GAS MEASUREMENT
// Page1_FIELD_0_GAS_RL.9:2 Description - Reg Addr --> 0x2A, Bit --> 0 to 7
#define BME680_PAGE1_FIELD_0_GAS_MSB_REG__POS	(0)
#define BME680_PAGE1_FIELD_0_GAS_MSB_REG__LEN	(8)
#define BME680_PAGE1_FIELD_0_GAS_MSB_REG__MSK	(0xFF)
#define BME680_PAGE1_FIELD_0_GAS_MSB_REG__ADDR	(BME680_PAGE1_FIELD_0_GAS_RL_MSB_REG)
// Page1_FIELD_0_GAS_RANGE Description -
// Reg Addr --> 0x2B, Bit --> 0 to 3
#define BME680_PAGE1_FIELD_0_GAS_RANGE__POS	(0)
#define BME680_PAGE1_FIELD_0_GAS_RANGE__LEN	(4)
#define BME680_PAGE1_FIELD_0_GAS_RANGE__MSK	(0x0F)
#define BME680_PAGE1_FIELD_0_GAS_RANGE__ADDR	(BME680_PAGE1_FIELD_0_GAS_RL_LSB_REG)
//1728
// BIT MASK, LENGTH AND POSITION DEFINITIONS OF PAGE1
// FIELD2 RESISTANCE HEAT MEASUREMENT
// Page1_BME680_RES_HEAT_0 Description - Reg Addr --> 0x5A, Bit --> 0 to 7
#define BME680_PAGE1_RES_HEAT_0__POS	(0)
#define BME680_PAGE1_RES_HEAT_0__LEN	(8)
#define BME680_PAGE1_RES_HEAT_0__MSK	(0xFF)
#define BME680_PAGE1_RES_HEAT_0__ADDR   (BME680_PAGE1_RES_HEAT_0_REG)

// Page1_BME680_RES_HEAT_1 Description - Reg Addr --> 0x5B, Bit --> 0 to 7
#define BME680_PAGE1_RES_HEAT_1__POS	(0)
#define BME680_PAGE1_RES_HEAT_1__LEN	(8)
#define BME680_PAGE1_RES_HEAT_1__MSK	(0xFF)
#define BME680_PAGE1_RES_HEAT_1__ADDR  (BME680_PAGE1_RES_HEAT_1_REG)

// Page1_BME680_RES_HEAT_2 Description - Reg Addr --> 0x5C, Bit --> 0 to 7
#define BME680_PAGE1_RES_HEAT_2__POS	(0)
#define BME680_PAGE1_RES_HEAT_2__LEN	(8)
#define BME680_PAGE1_RES_HEAT_2__MSK	(0xFF)
#define BME680_PAGE1_RES_HEAT_2__ADDR  (BME680_PAGE1_RES_HEAT_2_REG)

// Page1_BME680_RES_HEAT_3 Description - Reg Addr --> 0x5D, Bit --> 0 to 7
#define BME680_PAGE1_RES_HEAT_3__POS	(0)
#define BME680_PAGE1_RES_HEAT_3__LEN	(8)
#define BME680_PAGE1_RES_HEAT_3__MSK	(0xFF)
#define BME680_PAGE1_RES_HEAT_3__ADDR  (BME680_PAGE1_RES_HEAT_3_REG)

// Page1_BME680_RES_HEAT_4 Description - Reg Addr --> 0x5E, Bit --> 0 to 7
#define BME680_PAGE1_RES_HEAT_4__POS	(0)
#define BME680_PAGE1_RES_HEAT_4__LEN	(8)
#define BME680_PAGE1_RES_HEAT_4__MSK	(0xFF)
#define BME680_PAGE1_RES_HEAT_4__ADDR  (BME680_PAGE1_RES_HEAT_4_REG)

// Page1_BME680_RES_HEAT_5 Description - Reg Addr --> 0x5F, Bit --> 0 to 7
#define BME680_PAGE1_RES_HEAT_5__POS	(0)
#define BME680_PAGE1_RES_HEAT_5__LEN	(8)
#define BME680_PAGE1_RES_HEAT_5__MSK	(0xFF)
#define BME680_PAGE1_RES_HEAT_5__ADDR  (BME680_PAGE1_RES_HEAT_5_REG)

// Page1_BME680_RES_HEAT_6 Description - Reg Addr --> 0x60, Bit --> 0 to 7
#define BME680_PAGE1_RES_HEAT_6__POS	(0)
#define BME680_PAGE1_RES_HEAT_6__LEN	(8)
#define BME680_PAGE1_RES_HEAT_6__MSK	(0xFF)
#define BME680_PAGE1_RES_HEAT_6__ADDR  (BME680_PAGE1_RES_HEAT_6_REG)

// Page1_BME680_RES_HEAT_7 Description - Reg Addr --> 0x61, Bit --> 0 to 7
#define BME680_PAGE1_RES_HEAT_7__POS	(0)
#define BME680_PAGE1_RES_HEAT_7__LEN	(8)
#define BME680_PAGE1_RES_HEAT_7__MSK	(0xFF)
#define BME680_PAGE1_RES_HEAT_7__ADDR  (BME680_PAGE1_RES_HEAT_7_REG)

// Page1_BME680_RES_HEAT_8 Description - Reg Addr --> 0x61, Bit --> 0 to 7
#define BME680_PAGE1_RES_HEAT_8__POS	(0)
#define BME680_PAGE1_RES_HEAT_8__LEN	(8)
#define BME680_PAGE1_RES_HEAT_8__MSK	(0xFF)
#define BME680_PAGE1_RES_HEAT_8__ADDR  (BME680_PAGE1_RES_HEAT_8_REG)

// Page1_BME680_RES_HEAT_9 Description - Reg Addr --> 0x62, Bit --> 0 to 7
#define BME680_PAGE1_RES_HEAT_9__POS	(0)
#define BME680_PAGE1_RES_HEAT_9__LEN	(8)
#define BME680_PAGE1_RES_HEAT_9__MSK	(0xFF)
#define BME680_PAGE1_RES_HEAT_9__ADDR  (BME680_PAGE1_RES_HEAT_9_REG)
//1801

// BIT MASK, LENGTH AND POSITION DEFINITIONS OF PAGE1
// FIELD2 GAS WAIT MEASUREMENT

// Page1_BME680_GAS_WAIT_0 Description -
// Reg Addr --> 0x64, Bit --> 0 to 7
#define BME680_PAGE1_GAS_WAIT_0__POS	(6)
#define BME680_PAGE1_GAS_WAIT_0__LEN	(2)
#define BME680_PAGE1_GAS_WAIT_0__MSK	(0xC0)
#define BME680_PAGE1_GAS_WAIT_0__ADDR  (BME680_PAGE1_GAS_WAIT_0_REG)
//1971
// Page1_NB_CONVERT Description - Reg Addr --> 0x71, Bit --> 0 to 3
#define BME680_PAGE1_NUMBER_OF_CONVERSION__POS	(0)
#define BME680_PAGE1_NUMBER_OF_CONVERSION__LEN	(4)
#define BME680_PAGE1_NUMBER_OF_CONVERSION__MSK	(0x0F)
#define BME680_PAGE1_NUMBER_OF_CONVERSION__ADDR  (BME680_PAGE1_CONTROL_GAS1_REG)
// Page1_RUN_GAS Description - Reg Addr --> 0x71, Bit --> 4
#define BME680_PAGE1_RUN_GAS__POS	 (4)
#define BME680_PAGE1_RUN_GAS__LEN	 (1)
#define BME680_PAGE1_RUN_GAS__MSK  (0x10)
#define BME680_PAGE1_RUN_GAS__ADDR (BME680_PAGE1_CONTROL_GAS1_REG)
//2010
// Page1_OVERSAMP_HUMIDITY Description -
// Reg Addr --> 0x72, Bit --> 0 to 2
#define BME680_PAGE1_OVERSAMP_HUMIDITY__POS	(0)
#define BME680_PAGE1_OVERSAMP_HUMIDITY__LEN	(3)
#define BME680_PAGE1_OVERSAMP_HUMIDITY__MSK	(0x07)
#define BME680_PAGE1_OVERSAMP_HUMIDITY__ADDR (BME680_PAGE1_CONTROL_HUMIDITY_REG)

// 2064
// Page1_POWER_MODE Description -
// Reg Addr --> 0x74, Bit --> 0 to 1
#define BME680_PAGE1_POWER_MODE__POS	(0)
#define BME680_PAGE1_POWER_MODE__LEN	(2)
#define BME680_PAGE1_POWER_MODE__MSK	(0x03)
#define BME680_PAGE1_POWER_MODE__ADDR  (BME680_PAGE1_CONTROL_MEASUREMENT_REG)
// Page1_OVERSAMP_PRESSURE Description -Reg Addr --> 0x74, Bit --> 2to4
#define BME680_PAGE1_OVERSAMP_PRESSURE__POS	 (2)
#define BME680_PAGE1_OVERSAMP_PRESSURE__LEN	 (3)
#define BME680_PAGE1_OVERSAMP_PRESSURE__MSK	 (0x1C)
#define BME680_PAGE1_OVERSAMP_PRESSURE__ADDR (BME680_PAGE1_CONTROL_MEASUREMENT_REG)
// Page1_OVERSAMP_TEMPERATURE Description - Reg Addr --> 0x74, Bit --> 5to7
#define BME680_PAGE1_OVERSAMP_TEMPERATURE__POS	(5)
#define BME680_PAGE1_OVERSAMP_TEMPERATURE__LEN	(3)
#define BME680_PAGE1_OVERSAMP_TEMPERATURE__MSK	(0xE0)
#define BME680_PAGE1_OVERSAMP_TEMPERATURE__ADDR (BME680_PAGE1_CONTROL_MEASUREMENT_REG)
// 2119
// Page1_FILTER Description - Reg Addr --> 0x75, Bit --> 2to4
#define BME680_PAGE1_FILTER__POS	(2)
#define BME680_PAGE1_FILTER__LEN	(3)
#define BME680_PAGE1_FILTER__MSK	(0x1C)
#define BME680_PAGE1_FILTER__ADDR  (BME680_PAGE1_CONFIG_REG)

//2577
//***************************************************/
// CONSTANT DEFINITIONS
//**************************************************/
#define BME680_GEN_READ_WRITE_DATA_LENGTH          (1)
#define BME680_GEN_READ_DATA_LENGTH                (1)
#define BME680_GEN_WRITE_DATA_LENGTH               (1)
#define BME680_INIT_VALUE                          (0)

#define BME680_HUMIDITY_DATA_LENGTH			2
#define BME680_GAS_DATA_LENGTH				2
#define BME680_PRESSURE_DATA_LENGTH			3
#define BME680_TEMPERATURE_DATA_LENGTH		3

//	RES HEAT SELECTION DEFINITIONS
#define	BME680_RES_HEAT_0		(0)
#define	BME680_RES_HEAT_1		(1)
#define	BME680_RES_HEAT_2		(2)
#define	BME680_RES_HEAT_3		(3)
#define	BME680_RES_HEAT_4		(4)
#define	BME680_RES_HEAT_5		(5)
#define	BME680_RES_HEAT_6		(6)
#define	BME680_RES_HEAT_7		(7)
#define	BME680_RES_HEAT_8		(8)
#define	BME680_RES_HEAT_9		(9)

//2606
//	ARRAY SIZE DEFINITIONS
#define	BME680_HUMIDITY_DATA_SIZE	2
#define	BME680_HUMIDITY_MSB_DATA	0
#define	BME680_HUMIDITY_LSB_DATA	1
// temperature data definitions
#define	BME680_TEMPERATURE_DATA_SIZE   3
#define	BME680_TEMPERATURE_MSB_DATA	   0
#define	BME680_TEMPERATURE_LSB_DATA	   1
#define	BME680_TEMPERATURE_XLSB_DATA   2
// pressure data definitions
#define	BME680_PRESSURE_DATA_SIZE     3
#define	BME680_PRESSURE_MSB_DATA	  0
#define	BME680_PRESSURE_LSB_DATA	  1
#define	BME680_PRESSURE_XLSB_DATA     2
// gas data definitions
#define	BME680_GAS_DATA_SIZE   2
#define	BME680_GAS_MSB_DATA	   0
#define	BME680_GAS_LSB_DATA	   1

#define BME680_FIELD_DATA_SINGLE_LENGTH     15
#define BME680_FIELD_PRESSURE_TEMPERATUTRE  17
#define BME680_FIELD_HUMIDITY_GAS           34
#define BME680_CALIB_PARAM_SIZE				41
#define BME680_FIELD_DATA_SIZE				49
#define BME680_COMPONENT_FIELD_DATA_SIZE	15

#define	BME680_GAS_BIT_MASK                        (0x00C0)

//2688
//	GAS WAIT SELECTION DEFINITIONS
#define	BME680_GAS_WAIT_0		(0)
#define	BME680_GAS_WAIT_1		(1)
#define	BME680_GAS_WAIT_2		(2)
#define	BME680_GAS_WAIT_3		(3)
#define	BME680_GAS_WAIT_4		(4)
#define	BME680_GAS_WAIT_5		(5)
#define	BME680_GAS_WAIT_6		(6)
#define	BME680_GAS_WAIT_7		(7)
#define	BME680_GAS_WAIT_8		(8)
#define	BME680_GAS_WAIT_9		(9)
//2702
// POWER MODE DEFINITIONS
// Sensor Specific constants
#define BME680_SLEEP_MODE					(0x00)
#define BME680_FORCED_MODE					(0x01)
#define BME680_PARALLEL_MODE				(0x02)
#define BME680_SEQUENTIAL_MODE				(0x03)

// FILTER DEFINITIONS
#define BME680_FILTER_COEFF_OFF				(00)
#define BME680_FILTER_COEFF_1				(01)
#define BME680_FILTER_COEFF_3				(03)
#define BME680_FILTER_COEFF_7				(07)
#define BME680_FILTER_COEFF_15				(15)
#define BME680_FILTER_COEFF_31				(31)
#define BME680_FILTER_COEFF_63				(63)
#define BME680_FILTER_COEFF_127				(127)

// OVER SAMPLING DEFINITIONS
#define BME680_OVERSAMP_SKIPPED          (0x00)
#define BME680_OVERSAMP_1X               (0x01)
#define BME680_OVERSAMP_2X               (0x02)
#define BME680_OVERSAMP_4X               (0x03)
#define BME680_OVERSAMP_8X               (0x04)
#define BME680_OVERSAMP_16X              (0x05)

#define BME680_ENABLE_GAS					(0x01)
#define BME680_DISABLE_GAS					(0x00)

//2770
#define BME680_NB_CONV_SKIPPED			(0x00)
#define BME680_NB_CONV_1						(0x01)
#define BME680_NB_CONV_2						(0x02)
#define BME680_NB_CONV_3						(0x03)
#define BME680_NB_CONV_4						(0x04)
#define BME680_NB_CONV_5						(0x05)
#define BME680_NB_CONV_6						(0x06)
#define BME680_NB_CONV_7						(0x07)
#define BME680_NB_CONV_8						(0x08)
#define BME680_NB_CONV_9						(0x09)

//2793
// FUNCTION FOR GET AND SET BITSLICE
#define BME680_GET_BITSLICE(regvar, bitname) ((regvar & bitname##__MSK) >> bitname##__POS)
#define BME680_SET_BITSLICE(regvar, bitname, val) ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))

// REGISTER ADDRESS DEFINITIONS
#define BME680_CHIP_ID_REG                   (0xD0)//Chip ID Register
#define BME680_RST_REG                       (0xE0)//Softreset Register
#define BME680_STAT_REG                      (0xF3)//Status Register
#define BME680_CTRL_MEAS_REG                 (0xF4)//Ctrl Measure Register
#define BME680_CTRL_HUMIDITY_REG             (0xF2)//Ctrl Humidity Register
#define BME680_CONFIG_REG                    (0xF5)//Configuration Register
#define BME680_PRESSURE_MSB_REG              (0xF7)//Pressure MSB Register
#define BME680_PRESSURE_LSB_REG              (0xF8)//Pressure LSB Register
#define BME680_PRESSURE_XLSB_REG             (0xF9)//Pressure XLSB Register
#define BME680_TEMPERATURE_MSB_REG           (0xFA)//Temperature MSB Reg
#define BME680_TEMPERATURE_LSB_REG           (0xFB)//Temperature LSB Reg
#define BME680_TEMPERATURE_XLSB_REG          (0xFC)//Temperature XLSB Reg
#define BME680_HUMIDITY_MSB_REG              (0xFD)//Humidity MSB Reg
#define BME680_HUMIDITY_LSB_REG              (0xFE)//Humidity LSB Reg
//calibration parameters for I2C
#define BME680_DIG_T1_LSB_REG      (0xE9)
#define BME680_DIG_T1_MSB_REG      (0xEA)
#define BME680_DIG_T2_LSB_REG      (0x8A)
#define BME680_DIG_T2_MSB_REG      (0x8B)
#define BME680_DIG_T3_REG		   (0x8C)
#define BME680_DIG_T4_REG		   (0x8D)
#define BME680_DIG_T5_REG          (0x93)
#define BME680_DIG_P1_LSB_REG      (0x8E)
#define BME680_DIG_P1_MSB_REG      (0x8F)
#define BME680_DIG_P2_LSB_REG      (0x90)
#define BME680_DIG_P2_MSB_REG      (0x91)
#define BME680_DIG_P3_REG		   (0x92)
#define BME680_DIG_P4_LSB_REG      (0x94)
#define BME680_DIG_P4_MSB_REG      (0x95)
#define BME680_DIG_P5_LSB_REG      (0x96)
#define BME680_DIG_P5_MSB_REG      (0x97)
#define BME680_DIG_P7_REG		   (0x98)
#define BME680_DIG_P6_REG		   (0x99)
#define BME680_DIG_P8_LSB_REG      (0x9C)
#define BME680_DIG_P8_MSB_REG      (0x9D)
#define BME680_DIG_P9_LSB_REG      (0x9E)
#define BME680_DIG_P9_MSB_REG      (0x9F)
#define BME680_DIG_P10_REG         (0xA0)
#define BME680_DIG_P11_REG         (0x9A)
#define BME680_DIG_P12_REG         (0x9B)
#define BME680_DIG_P13_REG         (0xA1)
#define BME680_DIG_H1_MSB_REG      (0xE3)
#define BME680_DIG_H1_LSB_REG      (0xE2)
#define BME680_DIG_H2_LSB_REG      (0xE2)
#define BME680_DIG_H2_MSB_REG      (0xE1)
#define BME680_DIG_H3_REG          (0xE4)
#define BME680_DIG_H4_REG          (0xE5)
#define BME680_DIG_H5_REG          (0xE6)
#define BME680_DIG_H6_REG          (0xE7)
#define BME680_DIG_H7_REG          (0xE8)
#define BME680_DIG_GH1_REG         (0xED)
#define BME680_DIG_GH2_LSB_REG     (0xEB)
#define BME680_DIG_GH2_MSB_REG     (0xEC)
#define BME680_DIG_GH3_REG         (0xEE)
//**************************************************/
//*Array Index to Calibration API name mapping **/
//*************************************************/
#define DIG_T2_LSB_REG		(1)
#define DIG_T2_MSB_REG		(2)
#define DIG_T3_REG			(3)
#define DIG_P1_LSB_REG		(5)
#define DIG_P1_MSB_REG		(6)
#define DIG_P2_LSB_REG		(7)
#define DIG_P2_MSB_REG		(8)
#define DIG_P3_REG			(9)
#define DIG_P4_LSB_REG		(11)
#define DIG_P4_MSB_REG		(12)
#define DIG_P5_LSB_REG		(13)
#define DIG_P5_MSB_REG		(14)
#define DIG_P7_REG			(15)
#define DIG_P6_REG			(16)
#define DIG_P8_LSB_REG		(19)
#define DIG_P8_MSB_REG      (20)
#define DIG_P9_LSB_REG      (21)
#define DIG_P9_MSB_REG      (22)
#define DIG_P10_REG         (23)
#define DIG_H2_MSB_REG		(25)
#define DIG_H2_LSB_REG		(26)
#define DIG_H1_LSB_REG		(26)
#define DIG_H1_MSB_REG		(27)
#define DIG_H3_REG          (28)
#define DIG_H4_REG          (29)
#define DIG_H5_REG			(30)
#define DIG_H6_REG          (31)
#define DIG_H7_REG          (32)
#define DIG_T1_LSB_REG      (33)
#define DIG_T1_MSB_REG      (34)
#define DIG_GH2_LSB_REG		(35)
#define DIG_GH2_MSB_REG     (36)
#define DIG_GH1_REG			(37)
#define DIG_GH3_REG         (38)

// DEFINITION FOR BIT SHIFT OPERATIONS

#define BME680_SHIFT_BIT_POSITION_BY_01_BIT	   (1)
#define BME680_SHIFT_BIT_POSITION_BY_02_BITS   (2)
#define BME680_SHIFT_BIT_POSITION_BY_03_BITS   (3)
#define BME680_SHIFT_BIT_POSITION_BY_04_BITS   (4)
#define BME680_SHIFT_BIT_POSITION_BY_05_BITS   (5)
#define BME680_SHIFT_BIT_POSITION_BY_06_BITS   (6)
#define BME680_SHIFT_BIT_POSITION_BY_07_BITS   (7)
#define BME680_SHIFT_BIT_POSITION_BY_08_BITS   (8)
#define BME680_SHIFT_BIT_POSITION_BY_09_BITS   (9)
#define BME680_SHIFT_BIT_POSITION_BY_12_BITS   (12)
#define BME680_SHIFT_BIT_POSITION_BY_14_BITS   (14)
#define BME680_SHIFT_BIT_POSITION_BY_16_BITS   (16)

class BME680 {
public:
	bool start(I2C *i2c, uint8_t addr = BME680_ADDR);
	void readSensors(double *temperature, double *pressure, double *humidity, double *gas);
private:
	I2C *m_i2c;
	// compensation temperature
	uint16_t par_T1;
	int16_t par_T2;
	int8_t par_T3;
	// compensation pressure
	uint16_t par_P1;
	int16_t par_P2;
	int8_t par_P3;
	int16_t par_P4;
	int16_t par_P5;
	int8_t par_P6;
	int8_t par_P7;
	int16_t par_P8;
	int16_t par_P9;
	uint8_t par_P10;
	// compensation humidity
	uint16_t par_H1;
	uint16_t par_H2;
	int8_t par_H3;
	int8_t par_H4;
	int8_t par_H5;
	uint8_t par_H6;
	int8_t par_H7;
	// compensation gas
	int8_t par_GH1;
	int16_t par_GH2;
	int8_t par_GH3;

	int32_t t_fine;
	uint8_t res_heat_range; //resistance calculation*/
	uint8_t range_switching_error; //range switching error*/
	//-----

	//
	uint16_t heater_temp_u16;
	int16_t bme680_comp_temp;

	void getcalibration();
	void set_filter(uint8_t filter);
	void set_oversamp_humidity(uint8_t oversamp_humidity);
	void set_oversamp_pressure(uint8_t oversamp_pressure);
	void set_oversamp_temperature(uint8_t oversamp_temperature);
	void set_run_gas(uint8_t run_gas_u8);
	void set_number_of_conversion(uint8_t number_of_conversion);
	void set_gas_wait(uint16_t duration, uint8_t profile_num);

	uint8_t bme680_convert_temperature_to_resistance(uint16_t heater_temp_u16,
			int16_t ambient_temp_s16);
	void set_res_heat(uint8_t res_heat_u8, uint8_t select_res_u8);
	void init_forced_mode();

	uint8_t uread8(uint8_t reg);
	uint16_t uread16(uint8_t reg);
	int16_t read16(uint8_t reg);
	uint16_t uread16LE(uint8_t reg);
	int16_t read16LE(uint8_t reg);

	void set_power_mode(uint8_t power_mode_u8);
	double compensate_temperature_double(uint32_t uncom_temperature_u32);
	double compensate_humidity_double(uint16_t uncom_humidity_u16,
			double comp_temperature);
	long double compensate_pressure_double(uint32_t uncom_pressure_u32);
	double calculate_gas_double(uint16_t gas_adc_u16, uint8_t gas_range_u8);
};
