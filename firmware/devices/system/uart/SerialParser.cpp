#include "SerialParser.h"

void SerialParser::init(SerialParserCallback c) {
	callback = c;
}

void SerialParser::addChar(uint8_t c) {
	if (c == '\n' || c == '\r' || index == SERIAL_BUFFER_SIZE - 1) {
		callback(buffer);
		memset(buffer, 0, sizeof(buffer));
		index = 0;
	} else {
		buffer[index++] = c;
		buffer[index] = 0;
	}
}
