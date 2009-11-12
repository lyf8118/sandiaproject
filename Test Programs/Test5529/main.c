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

#define U_ADCWAIT 400
#define U_ADCSAMPLESNEEDED 10

volatile char ADCSamples = 0;
volatile unsigned int P6_0Results[U_ADCSAMPLESNEEDED];
volatile unsigned int P6_1Results[U_ADCSAMPLESNEEDED];
volatile unsigned int P6_2Results[U_ADCSAMPLESNEEDED];

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
  TA1CCR0 = U_ADCWAIT;
  TA1CTL = TASSEL_1 + MC_1 + TACLR + ID_3;         // AMCLK, contmode, clear TAR
  
  ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
  ADC12CTL1 = ADC12SHP + 0x08;                     // Use sampling timer, select ACLK
  ADC12IE = 0x01;                           // Enable interrupt
  ADC12CTL0 |= ADC12ENC;
  P6SEL |= 0x01;                            // P6.0 ADC option select
  P1DIR |= 0x03;                            // P1.0,1.1 output

  __bis_SR_register(LPM3_bits + GIE);       // Enter LPM0, enable interrupts
  __no_operation();                         // For debugger
}

// Timer1 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
  //P1OUT = 0x01;                            // Toggle P1.0
  TA1CCR0 += U_ADCWAIT;                         // Add Offset to CCR0
  if(ADCSamples == U_ADCSAMPLESNEEDED)
  ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
  switch(__even_in_range(ADC12IV,34))
  {
  case  6:                                  // Vector  6:  ADC12IFG0
   // if (ADC12MEM0 >= 0x7ff)                 // ADC12MEM = A0 > 0.5AVcc?
      P1OUT ^= 0x01; 
      P6_0Results[ADCSamples] = ADC12MEM0;
      P6_1Results[ADCSamples] = ADC12MEM1;
      P6_2Results[ADCSamples] = ADC12MEM2;
      ADCSamples++;
  break;
    //__bic_SR_register_on_exit(LPM3_bits);   // Exit active CPU
  default: break; 
  }
}