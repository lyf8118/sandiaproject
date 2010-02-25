#ifndef _ADC_H
#define _ADC_H

typedef struct{
  volatile unsigned int PinAverage;
  volatile unsigned int max;       //value in Results as the min,max value
  volatile unsigned int min;
  volatile unsigned int Results[U_ADCSAMPLESNEEDED];
  volatile unsigned int Averages[U_ADCHISTORYNEEDED];
}ADCResults;

volatile ADCResults ADXL[3];

void ADCSetup()
{/*
  P6SEL |= 0x07;                            // P6.0 ADC option select
  ADC12CTL0 = ADC12ON+ADC12SHT01;//ADC12MSC;             // 00 for Sampling time, ADC12 on
  ADC12CTL1 = ADC12SHP + 0x08 +ADC12CONSEQ0;    // sampling timer, ACLK, sequence
//  ADC12IE = 0x01;                           // Enable interrupt
  ADC12MCTL0 = ADC12INCH_0;                      // ref+=AVcc, channel = A0
  ADC12MCTL1 = ADC12INCH_1;                      // ref+=AVcc, channel = A1
  ADC12MCTL2 = ADC12INCH_2+ADC12EOS;                  // ref+=AVcc, channel = A2, end seq.
  ADC12IE = 0x04;                           // Enable ADC12IFG.2
  ADC12CTL0 |= ADC12ENC;
  */
  //sample code
  
  P6SEL = 0x07;                             // Enable A/D channel inputs
  ADC12CTL0 = ADC12ON+ADC12MSC+ADC12SHT0_7; // Turn on ADC12, extend sampling time
                                            // to avoid overflow of results
//  ADC12CTL1 = ADC12SHP+ADC12CONSEQ_3 + 0x08;       // Use sampling timer, repeated sequence
  ADC12CTL1 = ADC12CONSEQ_3 + 0x08;       // Use sampling timer, repeated sequence
  ADC12CTL2 |= 0x180;                        //predivide clock source and turn off temp sensor
  ADC12MCTL0 = ADC12INCH_0;                 // ref+=AVcc, channel = A0
  ADC12MCTL1 = ADC12INCH_1;                 // ref+=AVcc, channel = A1
  ADC12MCTL2 = ADC12INCH_2+ADC12EOS;        // ref+=AVcc, channel = A3, end seq.
  ADC12IE = 0x04;                           // Enable ADC12IFG.2
  ADC12CTL0 |= ADC12ENC;                    // Enable conversions
  ADCCounter = 0x70;
  
                              //initializing ADXL array
  for(int i = 0; i < 3; i++)
  {
      ADXL[i].min = 0x0; 
      ADXL[i].max = 0x0;
      ADXL[i].PinAverage = 0x0;
  }
  
  for(int i = 0; i < U_ADCSAMPLESNEEDED; i++)
  {
      ADXL[0].Results[i] = 0;
      ADXL[1].Results[i] = 0;
      ADXL[2].Results[i] = 0;
  }
}

void EnableADCInterrupt()
{
  ADC12IE |= 0x04;
}

void DisableADCInterrupt()
{
  ADC12IE &= ~0x04;
}
void ADCOff(){
  if((ADC12CTL0 & ADC12ON) == ADC12ON)
    ADC12CTL0 ^= ADC12ON;
}

void ADCOn(){
  if((ADC12CTL0 & ADC12ON) != ADC12ON)
    ADC12CTL0 ^= ADC12ON;
}

char ADCBusy()
{
 return (ADC12CTL0 & ADC12BUSY_L) && (ADC12IE & 0x04);
}

void AverageHistory()
{
   unsigned int sum0=0;
   unsigned int sum1=0;
   unsigned int sum2=0;
   for(int i = 0; i < U_ADCHISTORYNEEDED; i++){
     sum0 += ADXL[0].Averages[i];
     sum1 += ADXL[1].Averages[i];
     sum2 += ADXL[2].Averages[i];
   }
   ADXL[0].PinAverage = sum0/U_ADCHISTORYNEEDED;;
   ADXL[1].PinAverage = sum1/U_ADCHISTORYNEEDED;;
   ADXL[2].PinAverage = sum2/U_ADCHISTORYNEEDED;;
}

char isMoving()
{
    unsigned int sum0 = 0;
    unsigned int sum1 = 0;
    unsigned int sum2 = 0;
    
    for(int i = 0; i < U_ADCSAMPLESNEEDED; i++)
    {   //sum up each Results array, ignoring min, max elements
      sum0 += ADXL[0].Results[i];
      sum1 += ADXL[1].Results[i];
      sum2 += ADXL[2].Results[i];
    }
    sum0 -= ADXL[0].max+ADXL[0].min;
    sum1 -= ADXL[1].max+ADXL[1].min;
    sum2 -= ADXL[2].max+ADXL[2].min;
    
    unsigned int avg0 = sum0/(U_ADCSAMPLESNEEDED-2);
    unsigned int avg1 = sum1/(U_ADCSAMPLESNEEDED-2);
    unsigned int avg2 = sum2/(U_ADCSAMPLESNEEDED-2);
    if(ADCHistory < U_ADCHISTORYNEEDED){
     ADXL[0].Averages[ADCHistory] = avg0;
     ADXL[1].Averages[ADCHistory] = avg1;
     ADXL[2].Averages[ADCHistory] = avg2;
     ADCHistory++;  
     if(ADCHistory == U_ADCHISTORYNEEDED)
       AverageHistory();
    }
    else{
      if(abs(ADXL[0].PinAverage-avg0) > 0x7f)
        return TRUE;
      else if(abs(ADXL[1].PinAverage-avg1) > 0x7f)
        return TRUE;
      else if(abs(ADXL[2].PinAverage-avg2) > 0x7f)
        return TRUE; 
    }
    
    return FALSE; 
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
  _DINT();
  if(__even_in_range(ADC12IV,34) == 10)
  {
    int minVal;
    int maxVal;
    int newVal;
     // if (ADC12MEM0 >= 0x7ff)                 // ADC12MEM = A0 > 0.5AVcc?
    if(ADCSamples < U_ADCSAMPLESNEEDED){
      ADXL[0].Results[ADCSamples] = ADC12MEM0;
        ADXL[1].Results[ADCSamples] = ADC12MEM1;
        ADXL[2].Results[ADCSamples] = ADC12MEM2; 
        //P1OUT = !P1OUT;
        //update min,max values
        for(int i = 0; i < 3; i++){
          minVal = ADXL[i].Results[ADXL[i].min];
          maxVal = ADXL[i].Results[ADXL[i].max];
          newVal = ADXL[i].Results[ADCSamples];
          ADXL[i].min = (min(minVal, newVal) < minVal) ? newVal : ADXL[i].min;
          ADXL[i].max = (max(maxVal, newVal) > maxVal) ? newVal : ADXL[i].max;
        }   
        ADCSamples++;
        
        if(ADCSamples == U_ADCSAMPLESNEEDED)
          DisableADCInterrupt();
    }
  }
  ADC12IFG = 0;             //clear interrupt flag
  _EINT();
}
#endif