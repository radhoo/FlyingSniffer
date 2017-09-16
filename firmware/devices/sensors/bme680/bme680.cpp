/*
** BME680 Sensor Library for AVR Microcontrollers
** Version: 	0.1.0
** Date: 		September, 2015
** License:		LGPL v3
** Copyright: 	(C) 2009 - 2015 Radu Motisan, radu.motisan@gmail.com
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

#include "BME680.h"


uint8_t BME680::uread8(uint8_t reg) {
	uint8_t buf;
	m_i2c->readmem(BME680_ADDR, reg, &buf, 1);
	return buf;
}

uint16_t BME680::uread16(uint8_t reg) {
	uint8_t buf[2];
	m_i2c->readmem(BME680_ADDR, reg, buf, 2);
	uint16_t ret = (buf[0] << 8) | buf[1];
	return ret;
}


int16_t BME680::read16(uint8_t reg) {
	return (int16_t)uread16(reg);
}

uint16_t BME680::uread16LE(uint8_t reg) {
  uint16_t temp = uread16(reg);
  return (temp >> 8) | (temp << 8);

}

int16_t BME680::read16LE(uint8_t reg) {
  return (int16_t)uread16LE(reg);
}


bool BME680::start(I2C *i2c, uint8_t a) {
	m_i2c = i2c;

	uint8_t id = 0;
	m_i2c->readmem(BME680_ADDR, BME680_CHIP_ID_REG, &id, 1);
	if (id != BME680_CHIP_ID) return false;

	getcalibration();
	//m_i2c->writemem(BME680_ADDR, BME680_REGISTER_CONTROLHUMID, 0x03); // Set before CONTROL (DS 5.4.3)
	//m_i2c->writemem(BME680_ADDR, BME680_REGISTER_CONTROL, 0x3F);

	init_forced_mode();
	return true;
}

void BME680::getcalibration() {
	uint8_t buffer[BME680_CALIB_PARAM_SIZE], v_data = BME680_INIT_VALUE;
	m_i2c->readmem(BME680_ADDR, BME680_DIG_T2_LSB_REG - BME680_CALIB_REG_DATA, buffer, BME680_CALIB_DATA_LENGTH_GAS);
	m_i2c->readmem(BME680_ADDR, BME680_DIG_H2_MSB_REG, buffer + BME680_CALIB_DATA_LENGTH_GAS, BME680_CALIB_DATA_LENGTH);
	par_T1 = (uint16_t)((((uint16_t)(buffer[DIG_T1_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_08_BITS) | buffer[DIG_T1_LSB_REG]);
	par_T2 = (int16_t)(((((uint16_t)buffer[DIG_T2_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_08_BITS) | buffer[DIG_T2_LSB_REG]);
	par_T3 = (int8_t)(buffer[DIG_T3_REG]);

	par_P1 = (uint16_t)((((uint16_t)(buffer[DIG_P1_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_08_BITS) | buffer[DIG_P1_LSB_REG]);
	par_P2 = (int16_t)(((((uint16_t)buffer[DIG_P2_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_08_BITS) | buffer[DIG_P2_LSB_REG]);
	par_P3 = (int8_t)buffer[DIG_P3_REG];
	par_P4 = (int16_t)(((((uint16_t)buffer[DIG_P4_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_08_BITS) | buffer[DIG_P4_LSB_REG]);
	par_P5 = (int16_t)(((((uint16_t)buffer[DIG_P5_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_08_BITS) | buffer[DIG_P5_LSB_REG]);
	par_P6 = (int8_t)(buffer[DIG_P6_REG]);
	par_P7 = (int8_t)(buffer[DIG_P7_REG]);
	par_P8 = (int16_t)(((((uint16_t)buffer[DIG_P8_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_08_BITS) | buffer[DIG_P8_LSB_REG]);
	par_P9 = (int16_t)(((((uint16_t)buffer[DIG_P9_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_08_BITS) | buffer[DIG_P9_LSB_REG]);
	par_P10= (uint8_t)(buffer[DIG_P10_REG]);
	/* read humidity calibration*/
	par_H1 = (uint16_t)(((((uint16_t)buffer[DIG_H1_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_04_BITS) | ((buffer[DIG_H1_LSB_REG]) >> BME680_SHIFT_BIT_POSITION_BY_04_BITS));
	par_H2 = (uint16_t)(((((uint16_t)buffer[DIG_H2_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_04_BITS) | (buffer[DIG_H2_LSB_REG] & BME680_BIT_MASK_H2_DATA));
	par_H3 = (int8_t)buffer[DIG_H3_REG];
	par_H4 = (int8_t) buffer[DIG_H4_REG];
	par_H5 = (int8_t) buffer[DIG_H5_REG];
	par_H6 = (uint8_t)buffer[DIG_H6_REG];
	par_H7 = (int8_t)buffer[DIG_H7_REG];
	/* read gas calibration*/
	par_GH1 = (int8_t)buffer[DIG_GH1_REG];
	par_GH2 = (int16_t)(((((uint16_t)buffer[DIG_GH2_MSB_REG])) << BME680_SHIFT_BIT_POSITION_BY_08_BITS) | buffer[DIG_GH2_LSB_REG]);
	par_GH3 = (int8_t)buffer[DIG_GH3_REG];

	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_HEAT_LOOP_CTRL_1_REG, &v_data, BME680_GEN_READ_DATA_LENGTH);

	res_heat_range = BME680_GET_BITSLICE(v_data, BME680_PAGE1_RES_HEAT_RANGE);

	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_CURR_LIM_REG, &v_data, BME680_GEN_READ_DATA_LENGTH);

	range_switching_error = BME680_GET_BITSLICE(v_data, BME680_PAGE1_RANGE_SWITCHING_ERROR);


	/*uint8_t buffer[26];
	m_i2c->readmem(BME680_ADDR, BME680_REGISTER_T1, buffer, 26);

	par_T1 = buffer[0] | (buffer[1]<<8) ;
	par_T2 = buffer[2] | (buffer[3]<<8) ;
	par_T3 = buffer[4] | (buffer[5]<<8) ;
	par_P1 = buffer[6] | (buffer[7]<<8) ;
	par_P2 = buffer[8] | (buffer[9]<<8) ;
	par_P3 = buffer[10] | (buffer[11]<<8) ;
	par_P4 = buffer[12] | (buffer[13]<<8) ;
	par_P5 = buffer[14] | (buffer[15]<<8) ;
	par_P6 = buffer[16] | (buffer[17]<<8) ;
	par_P7 = buffer[18] | (buffer[19]<<8) ;
	par_P8 = buffer[20] | (buffer[21]<<8) ;
	par_P9 = buffer[22] | (buffer[23]<<8) ;

	par_H1 = buffer[25];

	m_i2c->readmem(BME680_ADDR, BME680_REGISTER_H2, buffer, 7);
	par_H2 = buffer[0] | (buffer[1]<<8) ;
	par_H3 = buffer[2] ;
	par_H4 = (buffer[3]<<4) | (buffer[4]&0x0f) ;
	par_H5 = ((buffer[4]&0xf0) >> 4) | (buffer[5]<<4);
	par_H6 = buffer[6] ;*/
}

void BME680::set_filter(uint8_t filter) {
	uint8_t data = BME680_INIT_VALUE;
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_FILTER__ADDR, &data, BME680_GEN_READ_DATA_LENGTH);
	data = BME680_SET_BITSLICE(data, BME680_PAGE1_FILTER, filter);
	m_i2c->writemem(BME680_ADDR, BME680_PAGE1_FILTER__ADDR, data ); //BME680_GEN_WRITE_DATA_LENGTH
}

void BME680::set_oversamp_humidity(uint8_t oversamp_humidity) {
	uint8_t data = BME680_INIT_VALUE;
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_OVERSAMP_HUMIDITY__ADDR, &data, BME680_GEN_READ_DATA_LENGTH);
	data = BME680_SET_BITSLICE(data, BME680_PAGE1_OVERSAMP_HUMIDITY, oversamp_humidity);
	m_i2c->writemem(BME680_ADDR, BME680_PAGE1_OVERSAMP_HUMIDITY__ADDR, data ); //BME680_GEN_WRITE_DATA_LENGTH
}

void BME680::set_oversamp_pressure(uint8_t oversamp_pressure) {
	uint8_t data = BME680_INIT_VALUE;
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_OVERSAMP_PRESSURE__ADDR, &data, BME680_GEN_READ_DATA_LENGTH);
	data = BME680_SET_BITSLICE(data, BME680_PAGE1_OVERSAMP_PRESSURE, oversamp_pressure);
	m_i2c->writemem(BME680_ADDR, BME680_PAGE1_OVERSAMP_PRESSURE__ADDR, data ); //BME680_GEN_WRITE_DATA_LENGTH
}

void BME680::set_run_gas(uint8_t run_gas_u8) {
	uint8_t data = BME680_INIT_VALUE;
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RUN_GAS__ADDR, &data, BME680_GEN_READ_DATA_LENGTH);
	data = BME680_SET_BITSLICE(data, BME680_PAGE1_RUN_GAS, run_gas_u8);
	m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RUN_GAS__ADDR, data ); //BME680_GEN_WRITE_DATA_LENGTH
}

void BME680::set_oversamp_temperature(uint8_t oversamp_temperature) {
	uint8_t data = BME680_INIT_VALUE;
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_OVERSAMP_TEMPERATURE__ADDR, &data, BME680_GEN_READ_DATA_LENGTH);
	data = BME680_SET_BITSLICE(data, BME680_PAGE1_OVERSAMP_TEMPERATURE, oversamp_temperature);
	m_i2c->writemem(BME680_ADDR, BME680_PAGE1_OVERSAMP_TEMPERATURE__ADDR, data ); //BME680_GEN_WRITE_DATA_LENGTH
}

void BME680::set_number_of_conversion(uint8_t number_of_conversion) {
	uint8_t data = BME680_INIT_VALUE;
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_NUMBER_OF_CONVERSION__ADDR, &data, BME680_GEN_READ_DATA_LENGTH);
	data = BME680_SET_BITSLICE(data, BME680_PAGE1_NUMBER_OF_CONVERSION, number_of_conversion);
	m_i2c->writemem(BME680_ADDR, BME680_PAGE1_NUMBER_OF_CONVERSION__ADDR, data ); //BME680_GEN_WRITE_DATA_LENGTH
}

void BME680::set_gas_wait(uint16_t duration, uint8_t profile_num_uint8_t) {
		/* used to return the communication result*/
	uint8_t data_uint8_t = BME680_INIT_VALUE;
	uint8_t gas_sensor_multiplication_factor = BME680_INIT_VALUE;
	uint8_t duration_uint8_t = BME680_INIT_VALUE;
	uint8_t unuse_4, unuse_16, unuse_64;
	uint8_t quotient_one, quotient_two, quotient_three;
	uint8_t remainder_one, remainder_two, remainder_three;

	quotient_one = quotient_two = quotient_three = BME680_INIT_VALUE;
	remainder_one = remainder_two = remainder_three = BME680_INIT_VALUE;
	unuse_4 = unuse_16 = unuse_64 = BME680_INIT_VALUE;
	/* check the gp_bme680 is NULL pointer */

	/* Setting duration in gas_wait_X registers */
	if (((duration > BME680_GAS_WAIT_MIN_TIMER_VALUE) &&
			(duration <= BME680_GAS_WAIT_MAX_TIMER_VALUE)) ||
			(duration == BME680_GAS_WAIT_MIN_TIMER_VALUE)) {
				duration_uint8_t = (uint8_t)duration;
				/* write the duration in gas wait shared*/
				m_i2c->writemem(BME680_ADDR, BME680_PAGE1_GAS_WAIT_0_REG + profile_num_uint8_t, duration_uint8_t);//, BME680_GEN_WRITE_DATA_LENGTH);
	} else if (duration > BME680_GAS_WAIT_MAX_TIMER_VALUE && duration <= BME680_GAS_WAIT_MAX_WAIT_TIME) {
		quotient_one = (duration/BME680_GAS_MULTIPLICATION_FACTOR_ONE);
		if (quotient_one <= BME680_GAS_WAIT_MAX_TIMER_VALUE)
			remainder_one = (duration%BME680_GAS_MULTIPLICATION_FACTOR_ONE);
		else
			unuse_4 = BME680_VALUE_TRUE;
		quotient_two = (duration/BME680_GAS_MULTIPLICATION_FACTOR_TWO);
		if (quotient_two <= BME680_GAS_WAIT_MAX_TIMER_VALUE)
			remainder_two = (duration%BME680_GAS_MULTIPLICATION_FACTOR_TWO);
		 else
			unuse_16 = BME680_VALUE_TRUE;
		quotient_three = (duration/BME680_GAS_MULTIPLICATION_FACTOR_THREE);
		if (quotient_three <= BME680_GAS_WAIT_MAX_TIMER_VALUE)
			remainder_three = (duration%BME680_GAS_MULTIPLICATION_FACTOR_THREE);
		 else
			unuse_64 = BME680_VALUE_TRUE;

		if (unuse_4 == BME680_VALUE_TRUE) {
			if (remainder_two <= remainder_three) {
				if (unuse_16 != BME680_VALUE_TRUE)
					gas_sensor_multiplication_factor = 2;
				else
					gas_sensor_multiplication_factor = 3;
			}
		}
		if (unuse_16 == BME680_VALUE_TRUE) {
			if (remainder_one <= remainder_three) {
				if (unuse_4 != BME680_VALUE_TRUE)
					gas_sensor_multiplication_factor = 1;
				else
					gas_sensor_multiplication_factor = 3;
			}
		}
		if (unuse_64 == BME680_VALUE_TRUE) {
			if (remainder_one <= remainder_two) {
				if (unuse_4 != BME680_VALUE_TRUE)
					gas_sensor_multiplication_factor = 1;
				else
					gas_sensor_multiplication_factor = 2;
			}
		}
		if ((!unuse_4) && (!unuse_16) && (!unuse_64)) {
			if (remainder_one <= remainder_two) {
				if (remainder_one <= remainder_three)
					gas_sensor_multiplication_factor = 1;
				else if (remainder_three <= remainder_one)
					gas_sensor_multiplication_factor = 3;
			}
			else if (remainder_two < remainder_one) {
				if (remainder_two <= remainder_three)
					gas_sensor_multiplication_factor = 2;
				else if (remainder_three < remainder_two)
					gas_sensor_multiplication_factor = 3;
			}
		}

		if (1 == gas_sensor_multiplication_factor && (unuse_4 == BME680_VALUE_FALSE)) {
			/* write the duration in gas wait shared*/
			m_i2c->writemem(BME680_ADDR, (BME680_PAGE1_GAS_WAIT_0_REG + (profile_num_uint8_t)), quotient_one);//, BME680_GEN_WRITE_DATA_LENGTH);
			data_uint8_t = BME680_SET_BITSLICE(quotient_one, BME680_PAGE1_GAS_WAIT_0, gas_sensor_multiplication_factor);
			m_i2c->writemem(BME680_ADDR, (BME680_PAGE1_GAS_WAIT_0_REG+ (profile_num_uint8_t)), data_uint8_t);//, BME680_GEN_WRITE_DATA_LENGTH);
		}
		if (2 == gas_sensor_multiplication_factor && unuse_16 == BME680_VALUE_FALSE) {
			/* write the duration in gas wait shared*/
			m_i2c->writemem(BME680_ADDR, (BME680_PAGE1_GAS_WAIT_0_REG + (profile_num_uint8_t)), quotient_two);//, BME680_GEN_WRITE_DATA_LENGTH);
			data_uint8_t = BME680_SET_BITSLICE(quotient_two, BME680_PAGE1_GAS_WAIT_0, gas_sensor_multiplication_factor);
			m_i2c->writemem(BME680_ADDR, (BME680_PAGE1_GAS_WAIT_0_REG + (profile_num_uint8_t)), data_uint8_t);//, BME680_GEN_WRITE_DATA_LENGTH);
		}
		if (3 == gas_sensor_multiplication_factor && unuse_64 == BME680_VALUE_FALSE) {
			/* write the duration in gas wait shared*/
			m_i2c->writemem(BME680_ADDR, (BME680_PAGE1_GAS_WAIT_0_REG + (profile_num_uint8_t)), quotient_three);//,BME680_GEN_WRITE_DATA_LENGTH);
			data_uint8_t = BME680_SET_BITSLICE(quotient_three, BME680_PAGE1_GAS_WAIT_0, gas_sensor_multiplication_factor);
			m_i2c->writemem(BME680_ADDR, (BME680_PAGE1_GAS_WAIT_0_REG + (profile_num_uint8_t)), data_uint8_t);//, BME680_GEN_WRITE_DATA_LENGTH);
		}
	} else {
		data_uint8_t = BME680_REG_MAX_VALUE;
		m_i2c->writemem(BME680_ADDR, (BME680_PAGE1_GAS_WAIT_0_REG + (profile_num_uint8_t)), data_uint8_t);//BME680_GEN_WRITE_DATA_LENGTH);
	}
}

/*!
 *	@brief This function is used to convert temperature to resistance
 *	using the compensation formula
 *
 *	@param heater_temp_u16: The value of heater temperature
 *	@param ambient_temp_s16: The value of ambient temperature
 *
 *	@return calculated resistance from temperature
 *
 *
 *
*/
uint8_t BME680::bme680_convert_temperature_to_resistance(uint16_t heater_temp_u16, int16_t ambient_temp_s16) {
	double var1 = BME680_INIT_VALUE;
	double var2 = BME680_INIT_VALUE;
	double var3 = BME680_INIT_VALUE;
	double var4 = BME680_INIT_VALUE;
	double var5 = BME680_INIT_VALUE;
	double res_heat = BME680_INIT_VALUE;

	if ((heater_temp_u16 >= GAS_PROFILE_TEMPERATURE_MIN) && (heater_temp_u16 <= GAS_PROFILE_TEMPERATURE_MAX))

#ifdef	HEATER_C1_ENABLE
		var1 = (((double)par_GH1 /(16.0)) + 49.0);
		var2 = ((((double)par_GH2 /(32768.0)) * (0.0005)) + 0.00235);
#endif
#ifdef	HEATER_V05_ENABLE
		var1 = (((double)par_GH1 / (16.0)) + 53.4);
		var2 = ((((double)par_GH2 /(32768.0)) * (0.0005)) + 0.00268);
#endif
#ifdef HEATER_V03_ENABLE
		var1 = (((double)par_GH1 /(16.0)) + 37.0);
		var2 = ((((double)par_GH2 /(32768.0)) * (0.0005)) + 0.00235);
#endif

		var3 = ((double)par_GH3 /(1024.0));
		var4 = (var1 * (1.0 + (var2 * (double)heater_temp_u16)));
		var5 = (var4 + (var3 * (double)ambient_temp_s16));
		res_heat = (((var5 * (4.0 / (4.0 + (double)res_heat_range))) - 25.0) * 3.4);

	return (uint8_t)res_heat;
}

void BME680::set_res_heat(uint8_t res_heat_u8, uint8_t select_res_u8)
{
	uint8_t data_u8 = BME680_INIT_VALUE;
	/* used to return the communication result*/
	switch (select_res_u8) {
		case BME680_RES_HEAT_0:
			m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RES_HEAT_0__ADDR,&data_u8,BME680_GEN_READ_DATA_LENGTH);
			data_u8 = BME680_SET_BITSLICE(data_u8,BME680_PAGE1_RES_HEAT_0,res_heat_u8);
			m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RES_HEAT_0__ADDR, data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
		break;
		case BME680_RES_HEAT_1:
			m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RES_HEAT_1__ADDR, &data_u8, BME680_GEN_READ_DATA_LENGTH);
			data_u8 = BME680_SET_BITSLICE(data_u8,BME680_PAGE1_RES_HEAT_1,res_heat_u8);
			m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RES_HEAT_1__ADDR,data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
		break;
		case BME680_RES_HEAT_2:
			m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RES_HEAT_2__ADDR,&data_u8,BME680_GEN_READ_DATA_LENGTH);
			data_u8 = BME680_SET_BITSLICE(data_u8,BME680_PAGE1_RES_HEAT_2,res_heat_u8);
			m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RES_HEAT_2__ADDR,data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
		break;
		case BME680_RES_HEAT_3:
			m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RES_HEAT_3__ADDR,&data_u8,BME680_GEN_READ_DATA_LENGTH);
			data_u8 = BME680_SET_BITSLICE(data_u8,BME680_PAGE1_RES_HEAT_3,res_heat_u8);
			m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RES_HEAT_3__ADDR,data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
		break;
		case BME680_RES_HEAT_4:
			m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RES_HEAT_4__ADDR,&data_u8,BME680_GEN_READ_DATA_LENGTH);
			data_u8 = BME680_SET_BITSLICE(data_u8,BME680_PAGE1_RES_HEAT_4,res_heat_u8);
			m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RES_HEAT_4__ADDR,data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
		break;
		case BME680_RES_HEAT_5:
			m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RES_HEAT_5__ADDR,&data_u8,BME680_GEN_READ_DATA_LENGTH);
			data_u8 = BME680_SET_BITSLICE(data_u8,BME680_PAGE1_RES_HEAT_5,res_heat_u8);
			m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RES_HEAT_5__ADDR,data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
		break;
		case BME680_RES_HEAT_6:
			m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RES_HEAT_6__ADDR,&data_u8,BME680_GEN_READ_DATA_LENGTH);
			data_u8 = BME680_SET_BITSLICE(data_u8, BME680_PAGE1_RES_HEAT_6, res_heat_u8);
			m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RES_HEAT_6__ADDR,data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
		break;
		case BME680_RES_HEAT_7:
			m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RES_HEAT_7__ADDR, &data_u8,BME680_GEN_READ_DATA_LENGTH);
			data_u8 = BME680_SET_BITSLICE(data_u8,BME680_PAGE1_RES_HEAT_7,res_heat_u8);
			m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RES_HEAT_7__ADDR,data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
		break;
		case BME680_RES_HEAT_8:
			m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RES_HEAT_8__ADDR,&data_u8,BME680_GEN_READ_DATA_LENGTH);
			data_u8 = BME680_SET_BITSLICE(data_u8,BME680_PAGE1_RES_HEAT_8,res_heat_u8);
			m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RES_HEAT_8__ADDR,data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
		break;
		case BME680_RES_HEAT_9:
			m_i2c->readmem(BME680_ADDR, BME680_PAGE1_RES_HEAT_9__ADDR, &data_u8, BME680_GEN_READ_DATA_LENGTH);
			data_u8 = BME680_SET_BITSLICE(data_u8,BME680_PAGE1_RES_HEAT_9,res_heat_u8);
			m_i2c->writemem(BME680_ADDR, BME680_PAGE1_RES_HEAT_9__ADDR,data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
		break;
		default: break;
	}
}

void BME680::init_forced_mode() {
	set_filter(BME680_FILTER_COEFF_OFF);//IIR filter is off
	set_oversamp_humidity(BME680_OVERSAMP_1X); //Humidity sensor oversampling setting is 1
	set_oversamp_pressure(BME680_OVERSAMP_2X); //Pressure sensor oversampling is set to 2
	set_oversamp_temperature(BME680_OVERSAMP_4X); //Temperature sensor oversampling is set to 4
	set_run_gas(BME680_ENABLE_GAS); // Enable gas module
	set_number_of_conversion(BME680_NB_CONV_SKIPPED); // Selected heater profile 0
	set_gas_wait(100, BME680_GAS_WAIT_0);	//Set Heater ON for 100ms
	heater_temp_u16=360; //Set heater temperature to 360 degrees
	bme680_comp_temp = 30; /*Ambient temp is set to 30 degrees*/
	//bme680_comp_temp = bme680_compensate_temperature_double(bme680_uncomp_temp); /*In order to make set heater temperature more accurate,ambient temperature read from sensor could be used*/
	uint8_t res_heat = bme680_convert_temperature_to_resistance( heater_temp_u16, bme680_comp_temp);//Convert target heater temperature to bme680 register format
	set_res_heat(res_heat, BME680_RES_HEAT_0);	//Set Heater resistance as per set target temp
}


/*!
 *	@brief This API used to set the
 *	Operational Mode from the sensor in the register 0x74 bit 0 and 1
 *
 *	@param power_mode_u8 : The value of power mode
 *  value           |    mode
 * -----------------|------------------
 *	0x00            | BME680_SLEEP_MODE
 *	0x01			| BME680_FORCED_MODE
 *  0x02			| BME680_PARALLEL_MODE
 *	0x03            | BME680_SEQUENTIAL_MODE
 *
 *	@return results of bus communication function
 *	@retval 0 -> Success
 *	@retval -1 -> Error
 *
 *
*/
void BME680::set_power_mode(uint8_t power_mode_u8) {
	uint8_t data_u8 = BME680_INIT_VALUE;
	/* used to return the communication result*/
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_POWER_MODE__ADDR, &data_u8, BME680_GEN_READ_DATA_LENGTH);
	data_u8 = BME680_SET_BITSLICE(data_u8,BME680_PAGE1_POWER_MODE,power_mode_u8);
	m_i2c->writemem(BME680_ADDR, BME680_PAGE1_POWER_MODE__ADDR, data_u8);//BME680_GEN_WRITE_DATA_LENGTH);
}
/*!
 * @brief Reads actual temperature from uncompensated temperature
 * @note returns the value in Degree centigrade
 * @note Output value of "51.23" equals 51.23 DegC.
 *
 *  @param uncom_temperature_u32 : value of uncompensated temperature
 *
 *  @return  Return the actual temperature in floating point
 *
*/
double BME680::compensate_temperature_double(uint32_t uncom_temperature_u32) {
	double data1_d = BME680_INIT_VALUE;
	double data2_d = BME680_INIT_VALUE;
	double temperature = BME680_INIT_VALUE;
	/* calculate x1 data */
	data1_d  = ((((double)uncom_temperature_u32 / 16384.0) - ((double)par_T1 / 1024.0)) * ((double)par_T2));
	/* calculate x2 data */
	data2_d  = (((((double)uncom_temperature_u32 / 131072.0) - ((double)par_T1 / 8192.0)) * (((double)uncom_temperature_u32 / 131072.0) -
			((double)par_T1 / 8192.0))) * ((double)par_T3 * 16.0));
	/* t fine value*/
	t_fine = (int32_t)(data1_d + data2_d);
	/* compensated temperature data*/
	temperature  = ((data1_d + data2_d) / 5120.0);

	return temperature;
}

/*!
 * @brief Reads actual humidity from uncompensated humidity
 * @note returns the value in relative humidity (%rH)
 * @note Output value of "42.12" equals 42.12 %rH
 *
 *  @param uncom_humidity_u16 : value of uncompensated humidity
 *  @param comp_temperature   : value of compensated temperature
 *
 *
 *
 *  @return Return the actual humidity in floating point
 *
*/
double BME680::compensate_humidity_double(uint16_t uncom_humidity_u16, double comp_temperature) {
	double humidity_comp = BME680_INIT_VALUE;
	double var1, var2, var3, var4;

	var1 = var2 = var3 = var4 = BME680_INIT_VALUE;

	var1 = ((double)uncom_humidity_u16) - (((double)par_H1 * 16.0) + (((double)par_H3 / 2.0) * comp_temperature));

	var2 = var1 * (((double)par_H2 / 262144.0) *(1.0 + (((double)par_H4 / 16384.0) * comp_temperature)
			+ (((double)par_H5 / 1048576.0) * comp_temperature * comp_temperature)));
	var3 = (double)par_H6 / 16384.0;
	var4 = (double)par_H7 / 2097152.0;

	humidity_comp = var2 + ((var3 + (var4 * comp_temperature)) * var2 * var2);
	if (humidity_comp > MAX_HUMIDITY_VALUE)
		humidity_comp = MAX_HUMIDITY_VALUE;
	else if (humidity_comp < MIN_HUMIDITY_VALUE)
		humidity_comp = MIN_HUMIDITY_VALUE;
	return humidity_comp;
}

/*!
 * @brief Reads actual pressure from uncompensated pressure
 * @note Returns pressure in Pa as double.
 * @note Output value of "96386.2"
 * equals 96386.2 Pa = 963.862 hPa.
 *
 *
 *  @param uncom_pressure_u32 : value of uncompensated pressure
 *
 *
 *  @return  Return the actual pressure in floating point
 *
*/
long double BME680::compensate_pressure_double(uint32_t uncom_pressure_u32) {
	double data1_d = BME680_INIT_VALUE;
	double data2_d = BME680_INIT_VALUE;
	double data3_d = BME680_INIT_VALUE;
	double pressure_comp = BME680_INIT_VALUE;

	data1_d = (((double)t_fine / 2.0) - 64000.0);
	data2_d = data1_d * data1_d * (((double)par_P6) / (4.0 * 32768.0));
	data2_d = data2_d + (data1_d * ((double)par_P5) * 2.0);
	data2_d = (data2_d / 4.0) + (((double)par_P4) * 65536.0);
	data1_d = (((((double)par_P3 * data1_d * data1_d * 32.0) / 524288.0) + ((double)par_P2 * data1_d)) / 524288.0);
	data1_d = ((1.0 + (data1_d / 32768.0)) * ((double)par_P1));
	pressure_comp = (1048576.0 - ((double)uncom_pressure_u32));
	/* Avoid exception caused by division by zero */
	if (data1_d != BME680_INIT_VALUE) {
		pressure_comp = (((pressure_comp - (data2_d / 4096.0)) * 6250.0) / data1_d);
		data1_d = (((double)par_P9) * pressure_comp * pressure_comp) /  2147483648.0;
		data2_d = pressure_comp * (((double)par_P8) / 32768.0);
		data3_d = ((pressure_comp / 256.0) * (pressure_comp / 256.0) * (pressure_comp / 256.0) * (par_P10 / 131072.0));
		pressure_comp = (pressure_comp + (data1_d + data2_d + data3_d + ((double)par_P7 * 128.0)) / 16.0);
		return pressure_comp;
	} else {
		return BME680_INIT_VALUE;
	}
}

/*!
 *	@brief This function is used to convert uncompensated gas data to
 *	compensated gas data using compensation formula
 *
 *	@param gas_adc_u16: The value of gas resistance calculated using temperature
 *	@param gas_range_u8: The value of gas range form register value
 *
 *	@return calculated compensated gas from compensation formula
 *	@retval compensated gas
 *
 *
*/
double BME680::calculate_gas_double(uint16_t gas_adc_u16, uint8_t gas_range_u8)
{
	double gas_res_d = BME680_INIT_VALUE;

#ifdef RANGE_SWITCH_CORRECTION_ENABLE

	double lookup_k1_range[BME680_GAS_RANGE_RL_LENGTH] = {
	0, 0, 0, 0, 0, -1, 0, -0.8, 0, 0, -0.2, -0.5, 0, -1, 0, 0};
	double lookup_k2_range[BME680_GAS_RANGE_RL_LENGTH] = {
	0, 0, 0, 0, 0.1, 0.7, 0, -0.8, -0.1, 0, 0, 0, 0, 0, 0, 0};
	int8_t range_switching_error_val = BME680_INIT_VALUE;
	double k1_dev = BME680_INIT_VALUE;
	double k1 = BME680_INIT_VALUE;
	double k2 = BME680_INIT_VALUE;


	/* check if sign bit is set */
	if ((range_switching_error & BME680_SIGN_BIT_MASK)) {
		range_switching_error_val = range_switching_error - 16;
	} else {
		range_switching_error_val = range_switching_error;
	}

	k1_dev = (1340.0 + (5.0 * range_switching_error_val));
	k1 = (k1_dev) * ((100.00 + (lookup_k1_range[gas_range_u8])) / 100.00);
	k2 = ((100.00 + (lookup_k2_range[gas_range_u8])) / 100.00);

	gas_res_d = 1.0 / (double)(k2 * ((0.000000125) * (double)(1 << gas_range_u8) * (((((double)gas_adc_u16) - 512.00) + k1) / k1)));

#else

	gas_res_d = 1.0 / ((0.000000125) * (double)(1 << gas_range_u8) * (((double)(gas_adc_u16)-512.00)+(1024.0 / 0.75)) / (1024.0/0.75));

#endif

	return gas_res_d;
}


// pressure(1F) XXX temperature (22) XXX humidity (25) XX
void BME680::readSensors(double *temperature, double *pressure, double *humidity, double *gas) {
	// +++ read temperature
	uint32_t	uncomp_temperature_u32 = 0;
	// MSB, LSB, LSB
	uint8_t temp_data_u8[BME680_TEMPERATURE_DATA_SIZE] = { BME680_INIT_VALUE, BME680_INIT_VALUE, BME680_INIT_VALUE};
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_FIELD_0_TEMPERATURE1_MSB_REG,temp_data_u8,BME680_TEMPERATURE_DATA_LENGTH);

	uncomp_temperature_u32 = (uint32_t)(((((uint32_t)temp_data_u8[BME680_TEMPERATURE_MSB_DATA])) << BME680_SHIFT_BIT_POSITION_BY_12_BITS) |
			((((uint32_t)temp_data_u8[BME680_TEMPERATURE_LSB_DATA])) << BME680_SHIFT_BIT_POSITION_BY_04_BITS) |
			(((uint32_t)temp_data_u8[BME680_TEMPERATURE_XLSB_DATA]) >> BME680_SHIFT_BIT_POSITION_BY_04_BITS));

	*temperature = compensate_temperature_double(uncomp_temperature_u32);

	// +++ read humidity
	uint16_t uncomp_humidity_u16 = 0;
	// MSB, LSB
	uint8_t humi_data_u8[BME680_HUMIDITY_DATA_SIZE] = { BME680_INIT_VALUE, BME680_INIT_VALUE};
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_FIELD_0_HUMIDITY_MSB_REG,humi_data_u8, BME680_HUMIDITY_DATA_LENGTH);
	uncomp_humidity_u16 = (uint16_t)(((((uint16_t)humi_data_u8[BME680_HUMIDITY_MSB_DATA])) << BME680_SHIFT_BIT_POSITION_BY_08_BITS) |
			((humi_data_u8[BME680_HUMIDITY_LSB_DATA])));

	*humidity = compensate_humidity_double(uncomp_humidity_u16, *temperature);
	// +++ read pressure
	uint32_t uncomp_pressure_u32 = 0;
	// MSB, LSB, LSB
	uint8_t pressure_data_u8[BME680_PRESSURE_DATA_SIZE] = { BME680_INIT_VALUE, BME680_INIT_VALUE, BME680_INIT_VALUE};
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_FIELD_0_PRESSURE_MSB_REG, pressure_data_u8, BME680_PRESSURE_DATA_LENGTH);
	uncomp_pressure_u32 = (uint32_t)((
				(((uint32_t)pressure_data_u8[BME680_PRESSURE_MSB_DATA])) << BME680_SHIFT_BIT_POSITION_BY_12_BITS) |
				((((uint32_t)pressure_data_u8[BME680_PRESSURE_LSB_DATA])) << BME680_SHIFT_BIT_POSITION_BY_04_BITS) |
				(((uint32_t)pressure_data_u8[BME680_PRESSURE_XLSB_DATA]) >> BME680_SHIFT_BIT_POSITION_BY_04_BITS));
	*pressure = compensate_pressure_double(uncomp_pressure_u32);

	// +++ gas
	uint16_t	uncomp_bme680_gas_adc = 0;
	// MSB, LSB
	uint8_t gas_data_u8[BME680_GAS_DATA_SIZE] = { BME680_INIT_VALUE, BME680_INIT_VALUE};
	//Read raw adc gas resistance
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_FIELD_0_GAS_MSB_REG__ADDR, gas_data_u8, BME680_GAS_DATA_LENGTH);
	uncomp_bme680_gas_adc = (uint16_t)(((((uint16_t)gas_data_u8[BME680_GAS_MSB_DATA])) << BME680_SHIFT_BIT_POSITION_BY_02_BITS) |
					((((uint16_t)gas_data_u8[BME680_GAS_LSB_DATA]) & BME680_GAS_BIT_MASK) >> BME680_SHIFT_BIT_POSITION_BY_06_BITS));
	uint8_t 	bme680_gas_range = 0;
	uint8_t range_data_u8 = BME680_INIT_VALUE;
	//Read adc range of measured gas resistance
	m_i2c->readmem(BME680_ADDR, BME680_PAGE1_FIELD_0_GAS_RANGE__ADDR, &range_data_u8, BME680_GEN_READ_DATA_LENGTH);
	bme680_gas_range = BME680_GET_BITSLICE(range_data_u8, BME680_PAGE1_FIELD_0_GAS_RANGE);
	*gas = calculate_gas_double(uncomp_bme680_gas_adc, bme680_gas_range);// Calculate gas from adc value and range


	set_power_mode(BME680_FORCED_MODE);			//Trigger forced mode


}

