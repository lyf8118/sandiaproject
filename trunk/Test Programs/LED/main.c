//*******************************************************************************
//  MSP-FET430P140 Demo - Software Toggle P1.0 - P1.3
//
//  Description: Toggle P1.0 - P1.3 by xor'ing P1.0 - P1.3 inside of a software loop.
//  ACLK= n/a, MCLK= SMCLK= default DCO ~800k
//
//                MSP430F149
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//******************************************************************************

#include  <msp430x16x.h>

void Wait(unsigned int);

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  P1DIR |= 0x0f;                            // Set P1.0 - P1.3 to output direction
  P1OUT = P1OUT & 0xf0;                     //reset P1 LEDs to zero

  for (;;)
  {
    volatile unsigned int i;
    P1OUT ^= 0x0f;                          // Toggle P1.0 - P1.3 using exclusive-OR
    Wait(50000);
  }
}

void Wait(unsigned int delay)               //a simple delay loop, for debugging purposes
{
 volatile unsigned int i = delay;                              // Delay
 do (i--);
 while (i != 0);
}

