//******************************************************************************
//  MSP-FET430P140 Demo - Timer_A, Toggle P1.0, Overflow ISR, 32kHz ACLK
//
//  Description: Toggle P1.0 using software and the Timer_A overflow ISR.
//  In this example an ISR triggers when TA overflows. Inside the ISR P1.0
//  is toggled. Toggle rate is exactly 0.5Hz. Proper use of the TAIV interrupt
//  vector generator is demonstrated.
//  ACLK = TACLK = 32768Hz, MCLK = SMCLK = default DCO ~800kHz
//  //* An external watch crystal on XIN XOUT is required for ACLK *//	
//
//           MSP430F149
//         ---------------
//     /|\|            XIN|-
//      | |               | 32kHz
//      --|RST        XOUT|-
//        |               |
//        |           P1.0|-->LED
//
//  M. Buccini
//  Texas Instruments Inc.
//  Feb 2005
//  Built with CCE Version: 3.2.0 and IAR Embedded Workbench Version: 3.21A
//******************************************************************************

#include  <msp430x14x.h>

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  P1DIR |= 0x01;                            // P1.0 output
  TACTL = TASSEL_1 + MC_2 + TAIE;           // ACLK, contmode, interrupt

  _BIS_SR(LPM3_bits + GIE);                 // Enter LPM3 w/ interrupt
}

// Timer_A3 Interrupt Vector (TAIV) handler
#pragma vector=TIMERA1_VECTOR
__interrupt void Timer_A(void)
{
  switch( TAIV )
  {
    case  2:  break;                        // CCR1 not used
    case  4:  break;                        // CCR2 not used
    case 10:  P1OUT ^= 0x01;                // overflow
              break;
  }
}

