#!/bin/sh

#edit source code for ID
cd Release
# 64000001 .. 6400003D
for i in {1..61}; do 

ID=$( printf "6400%04X" ${i} )

#make sure you include $(CFLAGS) in AVR C++ Compiler->Miscellaneous and in AVR C Compiler->Miscellaneous
echo Compiling $ID for $i
make clean
make CFLAGS=-DFORCE_DEV_NUMBER=${i}

mv uradmonitor-D-105.hex ../bin/uRADMonitor-$ID.hex
done


