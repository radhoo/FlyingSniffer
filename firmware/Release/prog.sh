#!/bin/sh
#avrdude -v

#echo "*** writing fuses for 14.7456MHz external crystal"
avrdude -p atmega2561 -c usbasp -U lfuse:w:0xCE:m 	-U hfuse:w:0x9F:m 	-U efuse:w:0xFF:m 

#echo "*** writing fuses for 14.7456MHz external crystal, preserve eeprom"
#avrdude -p atmega2561 -c usbasp -U lfuse:w:0xCE:m 	-U hfuse:w:0x91:m 	-U efuse:w:0xFF:m 

# 7.372800Mhz with preserve eeprom
#avrdude -p atmega2561 -c usbasp -U lfuse:w:0xFV:m 	-U hfuse:w:0x97:m 	-U efuse:w:0xFF:m 

echo "*** writting flash"
avrdude -V -p atmega2561 -c usbasp -U flash:w:uradmonitor-Drone.hex:i

#avrdude -V -p atmega2561 -c avrispmkII -Pusb -U flash:w:uradmonitor-D-105.hex:i

echo "Ready on"
date
