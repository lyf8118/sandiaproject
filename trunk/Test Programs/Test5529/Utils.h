//#include "ADC.h"
#ifndef _UTILS_H
#define _UTILS_H

//#define min(a,b)(a < b ? a:b)
//#define max(a,b)(a > b ? a:b)
#define TRUE  1
#define FALSE 0

//operating modes
#define UMODE_NORMAL  0x0
#define UMODE_INIT    0x1
#define UMODE_ADC     0x2
#define UMODE_SIGE    0x4
#define UMODE_DEBUG   0xf

//ADC Macros
#define U_ADCWAIT 0x03ff
#define U_ADCSAMPLESNEEDED 16
#define U_ADCHISTORYNEEDED 10 

//ADC variables
volatile char ADCSamples = 0;
volatile char ADCSampleNum = 0;
volatile char ADCCounter = 0;
volatile char ADCHistory = 0;

//Global variables
volatile char MODE;
volatile char movementDetected = 0; 

unsigned int max(unsigned int a, unsigned int b)
{
  return (a > b ? a:b);
}

unsigned int min(unsigned int a, unsigned int b)
{
  return (a < b ? a:b);
}

int abs(int x)
{
  return (x > 0) ? x : -x; 
}

#endif