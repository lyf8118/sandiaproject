//*****************************************************************************
//  D169 Demo - ADC12 / Integrated Temperature Sensor
//
//  Use ADC12 module and the integrated temperature sensor to detect
//  temperature gradients. The temperature sensor output voltage is sampled
//  ~ every 80ms and compared with the defined delta values using an ISR.
//  Timer_A will trigger the conversions, and the ADC12IFG.0 will trigger
//  the ISR call.
//
//  A. Dannenberg / MEB
//  Texas Instruments Inc.
//  August 2003
//*****************************************************************************
#include <msp430x16x.h>
//-----------------------------------------------------------------------------
#define ADCDeltaOn       12                       // 2 Deg C delta for LED on
#define ADCDeltaOff      6                        // 1 Deg C delta for LED off
//-----------------------------------------------------------------------------
static unsigned int FirstADCVal;                      // holds 1st ADC result
//-----------------------------------------------------------------------------
void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                           // Stop watchdog
  P1OUT = 0x00;                                       // Clear P1
  P1DIR = 0x01;                                       // P1.0 as output
  ADC12CTL1 = SHS_1 + SHP + CONSEQ_2;                 // TA trig., rpt conv.
  ADC12MCTL0 = SREF_1 + INCH_10;                      // Channel A10, Vref+
  ADC12IE = 0x01;                                     // Enable ADC12IFG.0          
  ADC12CTL0 = SHT0_8 + REF2_5V + REFON + ADC12ON + ENC;  // config ADC12
  TACCTL1 = OUTMOD_4;                                 // Toggle on EQU1 (TAR = 0)
  TACTL = TASSEL_2 + MC_2;                            // SMCLK, cont-mode
  while ((0x01 & ADC12IFG) == 0);                     // First conversion?
  FirstADCVal = ADC12MEM0;                            // Read out 1st ADC value

  _BIS_SR(LPM0_bits + GIE);                           // Enter LPM0 w/ interrupt
}

interrupt[ADC_VECTOR] void ADC12ISR(void)
{
  if (P1OUT & 0x01) {                          	      // LED on?
    if (ADC12MEM0 <= FirstADCVal + ADCDeltaOff)
      P1OUT &= ~0x01; }                        	      // LED off
  else {
    if (ADC12MEM0 >= FirstADCVal + ADCDeltaOn)
      P1OUT |= 0x01; }                         	      // LED on
}
