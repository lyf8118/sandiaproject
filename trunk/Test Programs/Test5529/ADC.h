#ifndef _ADC_H
#define _ADC_H

#define U_ADCWAIT 500
#define U_ADCSAMPLESNEEDED 16

typedef struct{
  volatile char max;       //pin in Results with the min,max value
  volatile char min;
  volatile unsigned int Results[U_ADCSAMPLESNEEDED];
}ADCResults;

#endif