/*
 * I2C master library using hardware TWI interface
 * Version: 	0.1.0
 * Date: 		July, 2015
 */

#pragma once

#include <avr/io.h>


#define I2C_READ    	1				// defines the data direction (reading from I2C device) in start(),rep_start()
#define I2C_WRITE   	0				// defines the data direction (writing to I2C device) in start(),rep_start()
#define SCL_CLOCK  		400000UL		// I2C clock in Hz
#define I2C_ERROR 		1				// error code
#define I2C_SUCCESS 	0				// error code

class I2C {
public:
	/**
	@brief initialize the I2C master interace. Need to be called only once
	@param  void
	@return none
	*/
	void init(uint32_t freq = SCL_CLOCK);

	/**
	@brief Terminates the data transfer and releases the I2C bus
	@param void
	@return none
	*/
	void stop(void);

	/**
	@brief Issues a start condition and sends address and transfer direction
	@param    addr address and transfer direction of I2C device
	@retval   0   device accessible
	@retval   1   failed to access device
	*/
	uint8_t start(uint8_t addr);

	/**
	@brief Issues a start condition and sends address and transfer direction
	If device is busy, use ack polling to wait until device ready
	@param    addr address and transfer direction of I2C device
	@retval   0  successful
	@retval   1  failed
	*/
	uint8_t start_wait(uint8_t addr);

	/**
	@brief Send one byte to I2C device
	@param    data  byte to be transfered
	@retval   0 write successful
	@retval   1 write failed
	*/
	uint8_t write(uint8_t data);

	/**
	@brief    read one byte from the I2C device, request more data from device
	@return   byte read from I2C device
	*/
	uint8_t readAck(void);

	/**
	@brief    read one byte from the I2C device, read is followed by a stop condition
	@return   byte read from I2C device
	*/
	uint8_t readNak(void);

	/**
	@brief    read one byte from the I2C device
	Implemented as a macro, which calls either readAck or readNak
	@param    ack 1 send ack, request more data from device<br>
			   0 send nak, read is followed by a stop condition
	@return   byte read from I2C device
	*/
	//#define read(ack)  (ack) ? readAck() : readNak();

	uint8_t writemem(uint8_t address, uint8_t reg, uint8_t value);

	uint8_t writemem(uint8_t address, uint8_t reg, uint8_t buff[], uint8_t bytes);

	uint8_t readmem(uint8_t address, uint8_t reg, uint8_t *out);

	uint8_t readmem(uint8_t address, uint8_t reg, uint8_t buff[], uint8_t bytes);
};
