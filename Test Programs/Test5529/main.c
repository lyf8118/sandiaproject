//******************************************************************************
//  MSP430F552x Demo - Timer1_A3, Toggle P1.0, CCR0 Cont. Mode ISR, DCO SMCLK
//
//  Description: Toggle P1.0 using software and TA_1 ISR. Toggles every
//  50000 SMCLK cycles. SMCLK provides clock source for TACLK.
//  During the TA_1 ISR, P1.0 is toggled and 50000 clock cycles are added to
//  CCR0. TA_1 ISR is triggered every 50000 cycles. CPU is normally off and
//  used only during TA_ISR.
//  ACLK = n/a, MCLK = SMCLK = TACLK = default DCO ~1.045MHz
//
//           MSP430F552x
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |               |
//        |           P1.0|-->LED
//
//   Bhargavi Nisarga
//   Texas Instruments Inc.
//   April 2009
//   Built with CCE v3.1 and IAR Embedded Workbench Version: 4.20
//******************************************************************************

#include  "msp430x552x.h"
#include "ADC.h"

#define min(a,b)(a < b ? a:b)
#define max(a,b)(a > b ? a:b)
#define TRUE  1
#define FALSE 0

volatile char light = 0;

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
  ADC12CTL0 = ADC12ON+ADC12MSC+ADC12SHT0_8; // Turn on ADC12, extend sampling time
                                            // to avoid overflow of results
  ADC12CTL1 = ADC12SHP+ADC12CONSEQ_3 + 0x08;       // Use sampling timer, repeated sequence
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
  }
  
  for(int i = 0; i < U_ADCSAMPLESNEEDED; i++)
  {
      ADXL[0].Results[i] = 0;
      ADXL[1].Results[i] = 0;
      ADXL[2].Results[i] = 0;
  }
}

char isMoving()
{
    int sum0 = 0;
    int sum1 = 0;
    int sum2 = 0;
    for(int i = 0; i < U_ADCSAMPLESNEEDED; i++)
    {   //sum up each Results array, ignoring min, max elements
      sum0 += (ADXL[0].min == i || ADXL[0].max == i) ? 0 : ADXL[0].Results[i];
      sum1 += (ADXL[1].min == i || ADXL[1].max == i) ? 0 : ADXL[1].Results[i];
      sum2 += (ADXL[2].min == i || ADXL[2].max == i) ? 0 : ADXL[2].Results[i];
    }
    int avg0 = sum0/U_ADCSAMPLESNEEDED;
    int avg1 = sum1/U_ADCSAMPLESNEEDED;
    int avg2 = sum2/U_ADCSAMPLESNEEDED;
    if(avg0 > 0x7ff)
      return TRUE;
    else if(avg1 > 0x7ff)
      return TRUE;
    else if(avg2 > 0x7ff)
      return TRUE; 
    
    return FALSE; 
}

void TimerA_Setup()
{
  TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
  TA1CCR0 = U_ADCWAIT;
  TA1CTL = TASSEL_1 + MC_1 + TACLR + ID_3;         // AMCLK, contmode, clear TAR
}              

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  TimerA_Setup();
  ADCSetup();
  P1DIR |= 0x03;                            // P1.0,1.1 output
  P1OUT = 0;

  __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, enable interrupts
  __no_operation();                         // For debugger
}

// Timer1 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
  _DINT();
  //P1OUT = 0x01;                            // Toggle P1.0
  TA1CCR0 += U_ADCWAIT;                         // Add Offset to CCR0
unsigned short c = ADC12IV;
  if(ADCCounter > 0x6f){
    ADCOn();
    if(ADCSamples == U_ADCSAMPLESNEEDED){
      ADCOff();
//      P1OUT = 0;//!P1OUT;
      c = isMoving();
      ADCSamples = 0;
      ADCCounter = 0;
    }
    else if(ADCSamples == ADCSampleNum){   //wait till previous sample is finished
        ADCSampleNum = (ADCSampleNum+1 == U_ADCSAMPLESNEEDED) ? 0 : ADCSampleNum + 1;
      P1OUT = 0;//!P1OUT;
        ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    }
  }else ADCCounter++;
  _EINT();
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
  _DINT();
  int minVal;
  int newVal;
   // if (ADC12MEM0 >= 0x7ff)                 // ADC12MEM = A0 > 0.5AVcc?
  if(ADCSamples < U_ADCSAMPLESNEEDED){
    ADXL[0].Results[ADCSamples] = ADC12MEM0;
      ADXL[1].Results[ADCSamples] = ADC12MEM1;
      ADXL[2].Results[ADCSamples] = ADC12MEM2; 
      P1OUT = 1;//!P1OUT;
      //update min,max values
      for(int i = 0; i < 3; i++){
        minVal = ADXL[i].Results[ADXL[i].min];
        newVal = ADXL[i].Results[ADCSamples];
        ADXL[i].min = (min(minVal, newVal) != minVal) ? ADCSamples : ADXL[i].min;
      }   
      ADCSamples++;
  }
    //__bic_SR_register_on_exit(LPM3_bits);   // Exit active CPU
  _EINT();
}