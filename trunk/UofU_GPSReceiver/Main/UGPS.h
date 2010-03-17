#pragma once
#ifndef _UGPS_H
#define _UGPS_H

#include  "msp430x552x.h"

//General Purpose
///////////////////////////////////////////////////////////////////////////////

unsigned int UINT_MAX(unsigned int, unsigned int);
unsigned int UINT_MIN(unsigned int, unsigned int);
unsigned int UINT_ArrayMax(unsigned int volatile* Array, int length);
unsigned int UINT_ArrayMax(unsigned int volatile* Array, int length);
unsigned int UINT_ArrayAverage(unsigned int volatile* Array, int length);

//Clear length bytes and return the pointer, starting at address
//address must be in RAM address space, or a null pointer is returned
char* makeArrayBlock(unsigned int address, int length);

///////////////////////////////////////////////////////////////////////////////


//For ADC.c
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


//For InternalFlash.c

//////////////////////////////////////////////////////////////////////////////

#define PAGEDATASTART   0x3400    //address in RAM, using 128 bytes

//TRUE if flash info has already been stored
char FlashInfoStored();             
//TRUE if given page number from flash is bad
char BadPage(int pageNumber, int flashID);
//returns the next good page, looks at currentPage+1 first
int FindNextPage(int currentPage, int flashID); 
//writes 128 bytes to given address block addressed by flashID
void write_Flash(char* pageData, int flashID);  
//helper to build 128 byte array
void insertBit(char val, int position, char* pageData); 
//Main function to do commands and store bad flash page info
void StoreFlashPageInfo();

//////////////////////////////////////////////////////////////////////////////


//For RTC.c

//////////////////////////////////////////////////////////////////////////////
//#include "time.h"
//#include <signal.h>

//void TIME_getDate(TIME_date_tm *const date);
//void TIME_setDate(const TIME_date_tm *date);

//////////////////////////////////////////////////////////////////////////////

//For PulseCounter.c

//////////////////////////////////////////////////////////////////////////////

static unsigned long SiGEPulseCount;
void WakeupPinSetup();
void EnableWakeupPin();
void DisableWakeupPin();

//////////////////////////////////////////////////////////////////////////////


//For USBCDC code

//////////////////////////////////////////////////////////////////////////////

#include "USBCDC\Common\device.h"
#include "USBCDC\Common\types.h"          // Basic Type declarations
#include "USBCDC\Common\hal_UCS.h"

#include "USBCDC\USB_Common\descriptors.h"
#include "USBCDC\USB_Common\usb.h"        // USB-specific Data Structures
#ifdef _CDC_
    #include "USBCDC\USB_CDC_API\UsbCdc.h"
#endif
#ifdef _HID_
    #include "USBCDC\USB_HID_API\UsbHid.h"
#endif

#include <string.h>
#include "USBCDC\USBCDC_constructs.h"
#include "USBCDC\USBmain.h"            
#include "USBCDC\USB_Common\defMSP430USB.h"


#define MAX_STR_LENGTH 64
static volatile BYTE bDataReceived_event = FALSE; // Indicates data has been received without an open rcv operation
static char wholeString[MAX_STR_LENGTH] = "";     // The entire input string from the last 'return'
static unsigned int SlowToggle_Period = 20000-1;
static unsigned int FastToggle_Period = 1000-1;
static char page[2048];                 

//////////////////////////////////////////////////////////////////////////////


#endif