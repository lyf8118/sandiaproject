//*******************************************************************************
//  D169 Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P0.1 inside of a software loop.
//  ACLK= n/a, MCLK= SMCLK= default DCO ~ 800k
//
//                MSP430F169
//             -----------------
//         /|\|              XIN|-  
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  M. Buccini
//  Texas Instruments, Inc
//  August 2003
//  Built with IAR Embedded Workbench Version: 1.25A
//******************************************************************************

#include  <msp430x16x.h>

void main(void)
{ 
  WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog
  P1DIR |= 0x01;                        // Set P1.0 to output direction

  for (;;)                              
  {
    unsigned int i;

    P1OUT ^= 0x01;                      // Toggle P1.0 using exclusive-OR

    i = 50000;                          // Delay
    do i--; while (i);
  }
}
