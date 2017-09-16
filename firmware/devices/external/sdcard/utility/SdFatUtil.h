#pragma once
/**
 * \file
 * Useful utility functions.
 */
#include <avr/pgmspace.h>
/** Store and print a string in flash memory.*/
#define PgmPrint(x) SerialPrint_P(PSTR(x))
/** Store and print a string in flash memory followed by a CR/LF.*/
#define PgmPrintln(x) SerialPrintln_P(PSTR(x))
/** Defined so doxygen works for function definitions. */
#define NOINLINE __attribute__((noinline,unused))
#define UNUSEDOK __attribute__((unused))
//------------------------------------------------------------------------------
/** Return the number of bytes currently free in RAM. */
static UNUSEDOK int FreeRam(void) {
	extern int __bss_end;
	extern int* __brkval;
	int free_memory;
	if (reinterpret_cast<int>(__brkval) == 0) {
		// if no heap use from end of bss section
		free_memory = reinterpret_cast<int>(&free_memory)
				- reinterpret_cast<int>(&__bss_end);
	} else {
		// use from top of stack to heap
		free_memory = reinterpret_cast<int>(&free_memory)
				- reinterpret_cast<int>(__brkval);
	}
	return free_memory;
}
