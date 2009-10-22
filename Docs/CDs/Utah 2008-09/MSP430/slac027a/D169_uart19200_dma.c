//******************************************************************************
//  D169 Demo - USART0 UART 19200 DMA Rpt Block, CCR2IFG, 32kHz ACLK + DCO
//
//  Description; DMA0 is used to transfer the string "Hello World" as a block 
//  byte-by-byte to to USART0 TXBUF. Timer_A operates in cont-mode from SMCLK/4 
//  and CCR2_IFG used to trigger DMA0. After each complete block transfer, 
//  source, and DMA size are reset to inital setting automatically using DMA 
//  transfer mode 4. 
//  ACLK= LFXT1/8= 32768/8,  MCLK= SMCLK= UCLK0= DCOCLK = 1048576
//  Baud rate divider with 1048576hz = 1048576Hz/19200 ~ 55 (0036h)
//  //*An external watch crystal on XIN XOUT is required for ACLK*//	  
//  //*MSP430F169 Device Required*//
//
//               MSP430F169
//             -----------------
//         /|\|              XIN|-  
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |             P3.4|------------> "Hello World"
//            |                 | 19200 - 8N1
//
//  M.Buccini
//  Texas Instruments, Inc
//  August 2003
//  Built with IAR Embedded Workbench Version: 1.26B
//******************************************************************************

#include  "msp430x16x.h"

static char string1[] = { "Hello World\r\n" };

void Set_DCO(void);

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog
  Set_DCO();                            // Set DCO

  ME1 |= UTXE0 + URXE0;                 // Enabled USART0 TXD/RXD
  UCTL0 |= CHAR;                        // 8-bit character, SWRST=1
  UTCTL0 |= SSEL1;                      // UCLK = SMCLK
  UBR00 = 0x36;                         // 19200 from 1Mhz
  UBR10 = 0x00;                         //
  UMCTL0 = 0x00;                        // No modulation
  UCTL0 &= ~SWRST;                      // Initialize USART state machine

  // D169 specific initialization
  P1OUT = 0x10;                         // disable audio output stage
  P1DIR = 0x3f;                         // P1.6/P1.7 inp, all other outp
  P2OUT = 0x00;                         // clear P2 output register
  P2DIR = 0xff;                         // unused pins as outp  
  P3SEL = 0x30;                         // P3.4,5 = USART0 TXD/RXD
  P3OUT = 0x00;                         // clear P3 output register
  P3DIR = 0xd5;                         // all but I2C pins to output
  P4OUT = 0x00;                         // clear P4 output register
  P4DIR = 0xff;                         // unused pins as outp  
  P5OUT = 0x00;                         // clear P5 output register
  P5DIR = 0xff;                         // unused pins as outp  
  P6OUT = 0x00;                         // clear P6 output register
  P6DIR = 0xfe;                         // unused pins as output

  DMA0SA = (int)string1;                // Source block address
  DMA0DA = U0TXBUF_;                    // Destination single address
  DMA0SZ = sizeof string1 - 1;          // Block size (discard trailing 0)
  DMACTL0 = DMA0TSEL_1;                 // CCR2 trigger
  DMA0CTL = DMADT_4 + DMASRCINCR_3 + DMASBDB + DMAEN; // Rpt, inc src
  TACTL = TASSEL_2 + ID_2 + MC_2;       // SMCLK/2, cont-mode
       
  _BIS_SR(CPUOFF);                      // CPU off
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
