/*
 * I2C master library using hardware TWI interface
 * Version: 	0.1.0
 * Date: 		July, 2015
 */

#include <compat/twi.h>

#include "i2c.h"


// Initialization of the I2C bus interface. Need to be called only once
void I2C::init(uint32_t freq) {
  TWSR = 0;                         // no prescaler
  TWBR = ((F_CPU/freq) - 16) / 2;  	// must be > 10 for stable operation
}


// Issues a start condition and sends address and transfer direction.
// return 0 = device accessible, 1= failed to access device
uint8_t I2C::start(uint8_t address) {
    uint8_t   twst;

	// send START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait until transmission completed
	uint16_t timeout = 1002;
	while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; }

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_START) && (twst != TW_REP_START)) return I2C_ERROR;

	// send device address
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wail until transmission completed and ACK/NACK has been received
	timeout = 1002;
	while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; }

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return I2C_ERROR;

	return I2C_SUCCESS;

}


// Issues a start condition and sends address and transfer direction.
// If device is busy, use ack polling to wait until device is ready
// Input:   address and transfer direction of I2C device
// @retval   0  successful
// @retval   1  failed
uint8_t I2C::start_wait(uint8_t address) {
	do {
		//Put Start Condition on TWI Bus
		TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
		//Poll Till Done
		uint16_t timeout = 1002;
		while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; }
		//Check status
		if((TWSR & 0xF8) != 0x08) return 1;
		//Now write SLA+W
		TWDR=address;
		//Initiate Transfer
		TWCR=(1<<TWINT)|(1<<TWEN);
		//Poll Till Done
		timeout = 1002;
		while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; }
	} while((TWSR & 0xF8) != 0x18);

	return 0;
}


// Terminates the data transfer and releases the I2C bus
void I2C::stop(void) {
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); // send stop condition
	// wait until stop condition is executed and bus released
	uint16_t timeout = 1002;
	while( (TWCR & (1<<TWSTO))) { timeout--; if (!timeout) return; };
}



// Send one byte to I2C device
// Input:    byte to be transfered
// Return:   0 write successful, 1 write failed
uint8_t I2C::write( uint8_t data ) {
    uint8_t   twst;
	// send data to the previously addressed device
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait until transmission completed or timeout
	uint16_t timeout = 1002;
	while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; };
	// check value of TWI Status Register. Mask prescaler bits
	twst = TW_STATUS & 0xF8;
	if( twst != TW_MT_DATA_ACK) return I2C_ERROR;
	return I2C_SUCCESS;
}


// Read one byte from the I2C device, request more data from device
// Return:  byte read from I2C device
uint8_t I2C::readAck(void) {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)));
    return TWDR;
}

// Read one byte from the I2C device, read is followed by a stop condition
// Return:  byte read from I2C device
uint8_t I2C::readNak(void) {
	TWCR = (1<<TWINT) | (1<<TWEN);
	uint16_t timeout = 1002;
	while(!(TWCR & (1<<TWINT))) { timeout--; if (!timeout) return I2C_ERROR; };
    return TWDR;
}


uint8_t I2C::writemem(uint8_t address, uint8_t reg, uint8_t value) {
	if (start_wait(address | I2C_WRITE))  return I2C_ERROR;
	if (write(reg)) return I2C_ERROR;
	if (write(value)) return I2C_ERROR;
	stop();
	return I2C_SUCCESS;
}

uint8_t I2C::writemem(uint8_t address, uint8_t reg, uint8_t buff[], uint8_t bytes) {
	if (start_wait(address | I2C_WRITE))  return I2C_ERROR;
	if (write(reg)) return I2C_ERROR;
	for (uint8_t i = 0; i< bytes; i++)
		if (write(buff[i])) {
			stop();
			return I2C_ERROR;
		}
	stop();
	return I2C_SUCCESS;
}

// i2c read
uint8_t I2C::readmem(uint8_t address, uint8_t reg, uint8_t *out) {
	if (start_wait(address | I2C_WRITE)) return I2C_ERROR;
	if (write(reg)) return I2C_ERROR;
	if (start(address | I2C_READ)) return I2C_ERROR;
	*out = readAck();
	readNak();
	stop();
	return I2C_SUCCESS;
}

uint8_t I2C::readmem(uint8_t address, uint8_t reg, uint8_t buff[], uint8_t bytes) {
	if (start_wait(address | I2C_WRITE)) return I2C_ERROR;
	if (write(reg)) return I2C_ERROR;
	if (start(address | I2C_READ)) return I2C_ERROR;
	for(uint8_t i=0; i<bytes; i++)
		buff[i] = readAck();
	readNak();
	stop();
	return I2C_SUCCESS;
}
