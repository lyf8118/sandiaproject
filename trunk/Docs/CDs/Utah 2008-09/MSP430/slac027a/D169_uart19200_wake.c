//******************************************************************************
//  D169 Demo - USART0 UART 19200 Ultra-low Echo ISR, 32kHz ACLK + DCO
//
//  Description; Echo a received character, USART0 RX ISR at high-speed used 
//  with ultra-low power techniques. Normal operation in LPM3, Set_DCO 
//  subroutine needs to be called to configure DCO that is used for UART baud 
//  generation. On valid RX character, character echoed back. Use start-bit 
//  edge detect - URXSE - to automatically (re)enable DCO and trigger ISR. ISR
//  must make sure DCO clock source remains enabled for the UART to receive 
//  full character. 
//  Software needs to make sure a character has been completely TX'ed, or RX'ed
//  before entering LPM3, which disables DCO required for the USART baud rate 
//  generator. In the example, TX'ing is checked using the TXEPT bit directly. 
//  RX'ing is checked using the SSEL0 clock select bit as a flag. When 
//  SSEL1= SSEL0= 1 SMCLK is selcted with no RX'ing and LPM3 is allowed. When
//  SSEL1=1, SSEL0=0 SMCLK is selected, but RX'ing is active, and the DCO must
//  not be disabled.   
//  ACLK= LFXT1/8= 32768/8,  MCLK= SMCLK= UCLK0= DCOCLK = 1048576
//  Baud rate divider with 1048576hz= 1048576Hz/19200 ~ 55 (0036h)
//  //*An external 32kHz watch crystal on XIN XOUT is required for ACLK*//	  
//
//              
//                MSP430F169   
//             -----------------
//         /|\|              XIN|-  
//          | |                 | 32768Hz 
//          --|RST          XOUT|-
//            |                 |
//            |             P3.4|------------> 
//            |                 | 19200 - 8N1
//            |             P3.5|<------------
//
//       
//  M. Buccini
//  Texas Instruments, Inc
//  August 2003
//  Built with IAR Embedded Workbench Version: 1.26B
//******************************************************************************

#include <msp430x16x.h>

void Set_DCO(void);

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;             // Stop WDT
  Set_DCO();                            // Set DCO

  ME1 |= UTXE0 + URXE0;                 // Enabled USART0 TXD/RXD
  UCTL0 |= CHAR;                        // 8-bit character, SWRST=1
  UTCTL0 |= SSEL1 + SSEL0 + URXSE;      // UCLK= SMCLK, start edge detect
  UBR00 = 0x36;                         // 19200 from 1Mhz
  UBR10 = 0x00;                         
  UMCTL0 = 0x00;                        // No modulation
  UCTL0 &= ~SWRST;                      // Initalize USART state machine
  IE1 |= URXIE0;                        // Enable USART0 RX interrupt

  // D169 specific initialization
  P1OUT = 0x10;                         // Disable audio output stage
  P1DIR = 0x3f;                         // P1.6/P1.7 inp, all other outp
  P2OUT = 0x00;                         // Clear P2 output register
  P2DIR = 0xff;                         // Unused pins as outp  
  P3SEL = 0x30;                         // P3.4,5 = USART0 TXD/RXD
  P3OUT = 0x00;                         // Clear P3 output register
  P3DIR = 0xf5;                         // All but I2C pins to output
  P4OUT = 0x00;                         // Clear P4 output register
  P4DIR = 0xff;                         // Unused pins as outp  
  P5OUT = 0x00;                         // Clear P5 output register
  P5DIR = 0xff;                         // Unused pins as outp  
  P6OUT = 0x00;                         // Clear P6 output register
  P6DIR = 0xfe;                         // Unused pins as output
  
  for (;;)                             
  {
    while (!(UTCTL0 & TXEPT));          // Confirm no TXing before --> LPM3 
    _DINT();                            // Disable interrupts for flag test
    _NOP();
    if (UTCTL0 & SSEL0)
      _BIS_SR(LPM3_bits + GIE);         // Enter LPM3, int's active
    else
      _BIS_SR(LPM0_bits + GIE);         // RX'ing char, enter LPM0, int's active
  }
}

interrupt[UART0RX_VECTOR] void usart0_rx(void)
{
  if (IFG1 & URXIFG0)                   // Test URXIFG0
  {
    while (!(IFG1 & UTXIFG0));          // USART0 TX buffer ready?
    TXBUF0 = RXBUF0;                    // RXBUF0 to TXBUF0
    _BIC_SR_IRQ(LPM3_bits);             // Exit LPM3 on reti
    UTCTL0 |= SSEL0;                    // SSEL0= 1, no RX activity
  }
  else                                  // Start edge
  {
    UTCTL0 &= ~URXSE;                   // Clear URXS signal
    UTCTL0 |= URXSE;                    // Re-enable edge detect
    _BIC_SR_IRQ(SCG1 + SCG0);           // DCO reamins on reti
    UTCTL0 &= ~SSEL0;                   // SSEL0= 0, RX activity
  }
}

//------------------------------------------------------------------------------ 
#define DELTA 256                       // Target DCO = DELTA*(4096) = 1048576  
//------------------------------------------------------------------------------ 
void Set_DCO(void)                      // Set DCO to selected frequency
{
  unsigned int Compare, Oldcapture = 0;

  BCSCTL1 |= DIVA_3;                    // ACLK = LFXT1CLK/8
  CCTL2 = CM_1 + CCIS_1 + CAP;          // CAP, ACLK
  TACTL = TASSEL_2 + MC_2 + TACLR;      // SMCLK, cont-mode, clear

  while (1)
  {
    while (!(CCIFG & CCTL2));           // Wait until capture occured
    CCTL2 &= ~CCIFG;                    // Capture occured, clear flag
    Compare = CCR2;                     // Get current captured SMCLK 
    Compare = Compare - Oldcapture;     // SMCLK difference
    Oldcapture = CCR2;                  // Save current captured SMCLK

    if (DELTA == Compare)
    {
      break;                            // Break loop
    }
    else if (DELTA < Compare)           
    {
      DCOCTL--;
      if (DCOCTL == 0xFF)               // DCO is too fast, slow it down
        if (BCSCTL1 & 0x07)
          BCSCTL1--;                    // Did DCO role under?, Sel lower RSEL
        else
          break;                        // Error condition, break loop
    }
    else
    {                      
      DCOCTL++;                         // DCO is too slow, speed it up
      if (DCOCTL == 0x00) 
        if ((BCSCTL1 & 0x07) != 0x07) 
          BCSCTL1++;                    // Did DCO role over? Sel higher RSEL
        else
          break;                        // Error condition, break loop
    }
  }

  CCTL2 = 0;                            // Stop CCR2
  TACTL = 0;                            // Stop Timer_A
}
