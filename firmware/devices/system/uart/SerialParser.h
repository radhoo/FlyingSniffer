#pragma once

#include <stdint.h>
#include <string.h>

#define SERIAL_BUFFER_SIZE 256

typedef void (*SerialParserCallback)(uint8_t *result);

class SerialParser {
	uint8_t buffer[SERIAL_BUFFER_SIZE];
	uint8_t index;
	SerialParserCallback callback;
public:
	void init(SerialParserCallback c);

	void addChar(uint8_t c);
};
