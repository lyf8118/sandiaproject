//******************************************************************************
//  MSP-FET430P140 Demo - USART0 SPI Interface to TLV5516 DAC
//
//  Description: Demonstrate USART0 in SPI mode interface to TLV5516 DAC. 
//  USART0 is used to transmit data to DAC, software generated frame sync 
//  pulse, DAC is updated inside CCR0 ISR operating in continuos mode. 
//  ACLK= n/a, MCLK= SMCLK= UCLK= default DCOCLK
//  //** SWRST** please see MSP430x1xx Users Guide for description **//
//  //** USART1 control bits are in different SFR's from other MSP430's **// 
//
//               MSP430F149             
//            -----------------
//        /|\|              XIN|-  
//         | |                 |           TLV5616
//         --|RST          XOUT|-       -------------
//           |             P3.0|------>|FS        OUT|--> ~ sine wave
//           |       SIMO0/P3.1|------>|DIN          |
//           |       UCLK0/P3.3|------>|SCLK       CS|-|
//           |                 |       |             | v
//
//
//  M.Buccini
//  Texas Instruments, Inc
//  August 2003
//  Built with IAR Embedded Workbench Version: 1.26B
//******************************************************************************

#include  <msp430x14x.h>

unsigned int pointer;                    // 16-bit value to write 

//------------------------------------------------------------------------------ 
// 12-bit Sine Lookup table with 32 steps
//------------------------------------------------------------------------------ 
static unsigned int Sin_tab[32] = {
        2048,
        2447,
        2831,
        3185,
        3495,
        3750,
        3939,
        4056,
        4095,
        4056,
        3939,
        3750,
        3495,
        3185,
        2831,
        2447,
        2048,
        1648,
        1264,
        910,
        600,
        345,
        156,
        39,
        0,
        39,
        156,
        345,
        600,
        910,
        1264,
        1648
};

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;             // Stop WDT
  P3SEL |= 0xA;                         // P3.1,3 SPI option select
  P3DIR |= 0xB;                         // P3.0,1,3 output direction
  P3OUT &= ~0x1;                        // FS reset
  P1DIR |= 0x1;                         // P1.0 output direction
  ME1 |= USPIE0;                        // Enable USART0 SPI
  UCTL0 |= CHAR + SYNC + MM;            // 8-bit SPI Master **SWRST**
  UTCTL0 = CKPH + CKPL + SSEL1 + STC;   // Inv. delayed, SMCLK, 3-pin
  UBR00 = 0x2;                          // ACLK/2 for baud rate
  UBR10 = 0x0;                          // ACLK/2 for baud rate
  UMCTL0 = 0x0;                         // Clear modulation
  UCTL0 &= ~SWRST;                      // Initalise USART state machine
  pointer = 0;                          // Clear pointer
  CCTL0 = CCIE;                         // CCR0 interrupt enabled
//  CCR0 = 25-1;                          // ~1kHz Clock period 
  CCR0 = 64-1;                          // ~ 390Hz Clock period 
  TACTL = TASSEL_2 + MC_1;              // SMCLK, Up-mode
  _EINT();                              // Enable interrupts

  for (;;)                              
  {
    unsigned int i;
    P1OUT ^= 0x01;                      // Toggle P1.0 using exclusive-OR
    for (i = 0; i < 0xFFFF; i++);       // ~500k cycles
  }
}

// Timer A0 interrupt service routine
interrupt[TIMERA0_VECTOR] void Timer_A(void)
{
  P3OUT |= 0x1;                         // FS set
  P3OUT &= ~0x1;                        // FS reset
  TXBUF0 = Sin_tab[pointer] >> 8;
  TXBUF0 = Sin_tab[pointer];
  pointer++;
  pointer &= 0x1F;                  
}
