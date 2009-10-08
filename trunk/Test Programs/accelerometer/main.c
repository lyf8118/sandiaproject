//so far only the ADC12MEM0 is working the way we want it to, I included a code sample in the same
//folder where this file is that we can implement
//and test to get the other two working
//This code already puts the CPU in low power mode 0 and wakes up to look at it

#include  <msp430x16x.h>

#define MAXDELAY 0xffff
#define DEFAULT 0xD09//0x7ff
//#define ACTUAL 0xD01

void Wait(unsigned int, short);

int main( void )
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  ADC12CTL0 = ADC12ON+SHT0_2;           // Turn on ADC12, extend sampling time
                                            // to avoid overflow of results
  ADC12CTL1 = SHP;//+CONSEQ_3;                 // Use sampling timer, repeated sequence
  //ADC12MCTL0 = INCH_0;                      // ref+=AVcc, channel = A3, end seq.
  ADC12IE = 0x01;                           // Enable ADC12IFG.3
  ADC12CTL0 |= ENC;                         // Enable conversions
  ADC12CTL0 |= ADC12SC;                     // Start conversion
  P6SEL = 0x0F;                             // Enable A/D channel inputs
  P1DIR |= 0x07;
  
  for(;;)
  {
    ADC12CTL0 |= ADC12SC;                   // Sampling open
    _BIS_SR(CPUOFF + GIE);                  // LPM0, ADC12_ISR will force exit
  }
}

//delay - how long you wish to delay
//loops - used for when you want to wait beyond the MAXDELAY value
//        use 1 for a default value
void Wait(unsigned int delay, short loops)               //a simple delay loop, for debugging purposes
{
 volatile unsigned int i = delay;                              // Delay
 short loopCount = 0;
 while(loopCount < loops){
   while (i != 0)
      i--;   
   loopCount++;
   i = delay;
 }
}

// ADC12 interrupt service routine
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR0 (void)
{
    if (ADC12MEM0 < DEFAULT)
      P1OUT &= ~0x01;                       // Clear P1.0 LED off
    else
      P1OUT |= 0x01;                        // Set P1.0 LED on
    
    if (ADC12MEM1 < DEFAULT)
      P1OUT &= ~0x02;                       // Clear P1.1 LED off
    else
      P1OUT |= 0x02;                        // Set P1.1 LED on
    
    if (ADC12MEM2 < DEFAULT)
      P1OUT &= ~0x04;                       // Clear P1.2 LED off
    else
      P1OUT |= 0x04;                        // Set P1.2 LED on
    _BIC_SR_IRQ(CPUOFF);                    // Clear CPUOFF bit from 0(SR)
}