#!/bin/sh

avrdude -V -p atmega2561 -c avrispmkII -Pusb -U flash:w:uradmonitor-D-107.hex:i

#/usr/local/AVRMacPack-20071126/bin/avrdude  -V -p atmega2561 -c avrispmkII -Pusb -U flash:w:uradmonitor-D-105.hex:i