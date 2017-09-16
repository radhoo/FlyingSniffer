/*
 * General purpose code
 * Copyright (C) 2015, Radu Motisan , radu.motisan@gmail.com , All rights reserved.
 * http://www.pocketmagic.net/
 */
 
#include "morse.h"
#include "../speaker/Speaker.h"

void Morse::binary(uint8_t decimal) {
	Speaker *speaker = (Speaker *)m_speaker;
	if (decimal) {
		binary(decimal/2);
		if (decimal != 1) decimal%2 ? speaker->beeplong(): speaker->beepshort();
	}
}

void Morse::encode(char c) {
	Speaker *speaker = (Speaker *)m_speaker;
	if (c == ' ') {
		speaker->delay();
		speaker->delay();
		speaker->delay();
		speaker->delay();
		speaker->delay();
		speaker->delay();
		speaker->delay();
	} else {
		const char *latin = "**ETIANMSURWDKGOHVF*L*PJBXCYZQ**"; //32
		if (c >= 'a' && c <= 'z') c -= 32; // convert to uppercase
		if (c < 'A' || c > 'Z') return;
		uint8_t i = 0;
		while (latin[++i] != c);
		binary(i);
		speaker->delay();
		speaker->delay();
		speaker->delay();
	}
}

void Morse::encode(char *s) {
	char *k = s;
	while (*k) encode(*k++);
}

void Morse::init(void *spk) {
	m_speaker = spk;
}
