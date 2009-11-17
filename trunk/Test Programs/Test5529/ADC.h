#ifndef _ADC_H
#define _ADC_H

#define U_ADCWAIT 0x7fff
#define U_ADCSAMPLESNEEDED 16

typedef struct{
  volatile char max;       //pin in Results with the min,max value
  volatile char min;
  volatile unsigned int Results[U_ADCSAMPLESNEEDED];
}ADCResults;

volatile ADCResults ADXL[3];
volatile char ADCSamples = 0;
volatile char ADCSampleNum = 0;
volatile char ADCCounter = 0;

void EnableADCInterrupt(){}
void DisableADCInterrupt(){}
void ADCOff(){
  if((ADC12CTL0 & ADC12ON) == ADC12ON)
    ADC12CTL0 ^= ADC12ON;
}

void ADCOn(){
  if((ADC12CTL0 & ADC12ON) != ADC12ON)
    ADC12CTL0 ^= ADC12ON;
}

#endif