#pragma once
#ifndef _UPS_H
#define _UPS_H

#include  "msp430x552x.h"

//General Purpose
#define   TRUE              1
#define   FALSE             0

unsigned int UINT_MAX(unsigned int, unsigned int);
unsigned int UINT_MIN(unsigned int, unsigned int);
unsigned int UINT_ArrayMax(unsigned int volatile* Array, int length);
unsigned int UINT_ArrayMax(unsigned int volatile* Array, int length);

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

static char* pageData = (char*)0x03400;  //128 bytes long, ends 0x2f7f
void write_Flash();
void insertBit(char val, int position);

//////////////////////////////////////////////////////////////////////////////

#endif