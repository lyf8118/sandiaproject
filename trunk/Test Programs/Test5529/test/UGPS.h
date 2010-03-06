#pragma once
#ifndef _UGPS_H
#define _UGPS_H

#include  "msp430x552x.h"

//General Purpose
///////////////////////////////////////////////////////////////////////////////
#define   TRUE              1
#define   FALSE             0

unsigned int UINT_MAX(unsigned int, unsigned int);
unsigned int UINT_MIN(unsigned int, unsigned int);
unsigned int UINT_ArrayMax(unsigned int volatile* Array, int length);
unsigned int UINT_ArrayMax(unsigned int volatile* Array, int length);

///////////////////////////////////////////////////////////////////////////////


//For ADC code
///////////////////////////////////////////////////////////////////////////////

#define   U_ADCSAMPLESIZE   10
#define   U_ADCBASELINESIZE 34
#define   U_ADCTHRESHHOLD   0x9f

static volatile unsigned int* ADXL_X = (unsigned int*)0x02c00;//beginning of RAM addresses, ends 0x02c00 + 2*length
static volatile unsigned int* ADXL_Y = (unsigned int*)0x02d00;//ends 0x2d00 + 2*length
static volatile unsigned int* ADXL_Z = (unsigned int*)0x02e00;//ends 0x2e00 + 2*length
/*volatile unsigned int ADXL_X[U_ADCBASELINESIZE];
volatile unsigned int ADXL_Y[U_ADCBASELINESIZE];
volatile unsigned int ADXL_Z[U_ADCBASELINESIZE];
volatile unsigned int maxVX = 0;
volatile unsigned int maxVY = 0;
volatile unsigned int maxVZ = 0;
volatile unsigned long numSamples = 0;*/ //for DEBUG
void ADCSetup();
void TimerA_Setup();

//////////////////////////////////////////////////////////////////////////////


//For internal Flash

//////////////////////////////////////////////////////////////////////////////

#define PAGEDATASTART   0x3400    //address in RAM, using 128 bytes

//TRUE if flash info has already been stored
char FlashInfoStored();             
//TRUE if given page number from flash is bad
char BadPage(int pageNumber, int flashID);
//returns the next good page, looks at currentPage+1 first
int FindNextPage(int currentPage, int flashID); 
//writes 128 bytes to given address block
void write_Flash(char* pageData, int flashID);  
//helper to build 128 byte array
void insertBit(char val, int position, char* pageData); 

//////////////////////////////////////////////////////////////////////////////

#endif