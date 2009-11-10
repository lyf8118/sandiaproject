
#include  <msp430x16x.h>

int main( void )
{
 //   BCSCTL1 += RSEL2+RSEL1+RSEL0;
 // DCOCTL += DCO2+DCO1+DCO0;//code to set max frequency
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD; 
 P1DIR |= 0x03;                            // P1.0 output
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  CCR0 = 2000;//0xfff9;
  BCSCTL1 |= 0x30;
  // a/d conversion
  ADC12CTL0 = SHT0_2 + ADC12ON;             // Set sampling time, turn on ADC12
  ADC12CTL1 = SHP + 0x8;                          // Use sampling timer
  ADC12IE = 0x01;                           // Enable interrupt
  ADC12CTL0 |= ENC;
  P6SEL |= 0x01;
  
  TACTL = TASSEL_1 + MC_1+ID_3;  
  const char sr = LPM3_bits + GIE;
 // for(;;){
    _BIS_SR(sr);                 // Enter LPM3 w/ interrupt
    
//  }
}

// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  P1OUT ^= 0x01;                            // Toggle P1.0
  CCR0 += 2000;//0xfff9;                            // Add Offset to CCR0
//  _BIC_SR_IRQ(LPM3_bits);                    // Clear LPM3
  ADC12CTL0 |= ADC12SC;                   // Sampling open after wakeup
}

// ADC12 interrupt service routine
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR (void)
{
    if (ADC12MEM0 < 0x7FF)
      P1OUT &= ~0x02;                       // Clear P1.0 LED off
    else
      P1OUT |= 0x02;                        // Set P1.0 LED on
}