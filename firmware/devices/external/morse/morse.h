/*
 * General purpose code
 * Copyright (C) 2015, Radu Motisan , radu.motisan@gmail.com , All rights reserved.
 * http://www.pocketmagic.net/
 */
 
#pragma once

#include <stdint.h>

class Morse {
private:
	void *m_speaker;
	void binary(uint8_t decimal);
public:
	void init(void *spk);
	void encode(char c);
	void encode(char *s);
};
