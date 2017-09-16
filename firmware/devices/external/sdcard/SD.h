/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
 (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#include "utility/SdFat.h"
#include "utility/SdFatUtil.h"
#include "../../system/gpio/DigitalPin.h"
#include "../../system/spi/spi.h"

#define FILE_READ O_READ
#define FILE_WRITE (O_READ | O_WRITE | O_CREAT)

class File {
private:
	char _name[13]; // our name
	SdFile *_file;  // underlying file pointer

public:
	File(SdFile f, const char *name);     // wraps an underlying SdFile
	File(void);      // 'empty' constructor
	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t *buf, size_t size);
	virtual int read();
	virtual int peek();
	virtual int available();
	virtual void flush();
	int read(void *buf, uint16_t nbyte);
	bool seek(uint32_t pos);
	uint32_t position();
	uint32_t size();
	void close();
	operator bool();
	char * name();

	bool isDirectory(void);
	File openNextFile(uint8_t mode = O_RDONLY);
	void rewindDirectory(void);

};

class SDClass {

private:
	// These are required for initialisation and use of sdfatlib
	Sd2Card card;
	SdVolume volume;
	SdFile root;

	// my quick&dirty iterator, should be replaced
	SdFile getParentDir(const char *filepath, int *indx);
public:
	Sd2Card getCard() { return card; }
	SdVolume getVolume() { return volume; }
	SdFile getRoot() { return root; }
	// This needs to be called to set up the connection to the SD card
	// before other methods are used.
	//int8_t begin(DigitalPin *cs, DigitalPin *available, SPI *spi); //uint8_t csPin = SD_CHIP_SELECT_PIN);
	int8_t begin(DigitalPin *cs, SPI *spi);

	void end();

	// Open the specified file/directory with the supplied mode (e.g. read or
	// write, etc). Returns a File object for interacting with the file.
	// Note that currently only one file can be open at a time.
	File open(const char *filename, uint8_t mode = FILE_READ);

	// Methods to determine if the requested file path exists.
	bool exists(char *filepath);

	// Create the requested directory heirarchy--if intermediate directories
	// do not exist they will be created.
	bool mkdir(char *filepath);

	// Delete the file.
	bool remove(char *filepath);

	bool rmdir(char *filepath);

private:

	// This is used to determine the mode used to open a file
	// it's here because it's the easiest place to pass the
	// information through the directory walking function. But
	// it's probably not the best place for it.
	// It shouldn't be set directly--it is set via the parameters to `open`.
	int fileOpenMode;

	friend class File;
	friend bool callback_openPath(SdFile&, char *, bool, void *);
};

extern SDClass SD;

