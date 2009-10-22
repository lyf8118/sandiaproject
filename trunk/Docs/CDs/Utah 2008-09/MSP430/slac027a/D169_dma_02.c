//*****************************************************************************
//  D169 Demo - DMA0 Repeated Block To P1OUT, CCR2IFG Trigger
//
//  Description; DMA0 is used to transfer a string byte-by-byte as a repeating
//  block to P1OUT. Timer_A operates continuosly at ~800Hz with CCR2IFG 
//  triggering DMA0. The effect is P1.0/1.1 toggling at different frequencies. 
//  ACLK= n/a, MCLK= SMCLK=  TACLK= default DCO ~ 800k
//
//               MSP430F169
//            -----------------
//        /|\|              XIN|-  
//         | |                 |
//         --|RST          XOUT|-
//           |                 |
//           |             P1.0|--> LED 
//           |             P1.1|--> LED
//
//  M.Buccini
//  Texas Instruments, Inc
//  August 2003
//  Built with IAR Embedded Workbench Version: 1.26B
//*****************************************************************************

#include  <msp430x16x.h>

const unsigned char testconst[] = { 0x00, 0x03, 0x02, 0x03, 0x00, 0x01 };

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                           // Stop watchdog
  P1DIR |= 0x03;                                      // P1.0/1.1 output 
  DMACTL0 = DMA0TSEL_1;                               // CCR2IFG trigger
  DMA0SA = (unsigned int)testconst;                   // Source block address
  DMA0DA = P1OUT_;                                    // Dest single address
  DMA0SZ = sizeof testconst;                          // Block size
  DMA0CTL = DMADT_4 + DMASRCINCR_3 + DMASBDB + DMAEN; // Rpt, inc src, enable
  TACTL = TASSEL_2 + MC_2;                            // SMCLK, cont-mode
       
  _BIS_SR(CPUOFF);                                    // CPU off
}


