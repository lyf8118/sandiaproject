
#include  <msp430x16x.h>

int main( void )
{
 //   BCSCTL1 += RSEL2+RSEL1+RSEL0;
 // DCOCTL += DCO2+DCO1+DCO0;//code to set max frequency
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD; 
 P1DIR |= 0x03;                            // P1.0 output
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  CCR0 = 2000;//0xfff9;						// number of clock ticks till interrupt
  BCSCTL1 |= 0x30;							//ACLK divider, /8
  // a/d conversion, this slow ACLK might need to be sped up for conversion process
  ADC12CTL0 = SHT0_2 + ADC12ON;             // Set sampling time, turn on ADC12
  ADC12CTL1 = SHP + 0x8;               // Use sampling timer, select ACLK as source clock
  ADC12IE = 0x01;                      // Enable interrupt
  ADC12CTL0 |= ENC;						//Enable conversions
  P6SEL |= 0x01;						//Use A/D for P6.0, will expand to use 
											// P6.1 and P6.2 for y, z axis
  TACTL = TASSEL_1 + MC_1+ID_3;			//For timer A, select ACLK source
										//MC_1 = mode, ID_3 = divide source / 8
  const char SR = LPM3_bits + GIE;
  _BIS_SR(SR);                 // Enter LPM3 w/ interrupt
}

// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  P1OUT ^= 0x01;                            // Toggle P1.0
  CCR0 += 2000;//0xfff9;            // Add Offset to CCR0, 2000 is ~4 seconds
  ADC12CTL0 |= ADC12SC;             //Sampling open after wakeup, this starts conversion
									//process, ADC12 ISR will be triggered when done
}

// ADC12 interrupt service routine
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR (void)
{
    if (ADC12MEM0 < 0x7FF)
      P1OUT &= ~0x02;                       // Clear P1.0 LED off
    else
      P1OUT |= 0x02;                        // Set P1.0 LED on
	//  _BIC_SR_IRQ(LPM3_bits);       //code to clear LPM3, use if movement is detected
}