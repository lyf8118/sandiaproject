//******************************************************************************
//  D169 Demo - DMA0 Repeated Block to DAC0 Sine Output, CCR1
//
//  Description; DMA0 is used to transfer a sine look-up table word-by-word 
//  as a repeating block to DAC12_0. The effect is sine wave output. Timer_A 
//  operates in upmode with CCR1 loading DAC12_0 on rising edge and DAC12_OIFG
//  triggering next DMA0 transfer. P1.0 pulses showing 1M instructions.
//  ACLK = n/a, MCLK = SMCLK = TACLK = default DCO ~ 800kHz
//
//               MSP430F169             
//            -----------------
//        /|\|              XIN|-  
//         | |                 |    
//         --|RST          XOUT|-       
//           |                 |
//           |        DAC0/P6.6|--> sine wave output
//           |             P1.4|--> output stage shutdown (1 = shutdown)
//           |             P1.0|--> LED 1MIP toggle 
//
//
//  M.Buccini
//  Texas Instruments, Inc
//  August 2003
//  Built with IAR Embedded Workbench Version: 1.26B
//******************************************************************************

#include  <msp430x16x.h>

//------------------------------------------------------------------------------ 
// 12-bit Sine Lookup table with 32 steps
//------------------------------------------------------------------------------ 
static int Sin_tab[32] = {
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
  WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog
  ADC12CTL0 = REF2_5V + REFON;          // Internal 2.5V ref 
  DMA0SA = (int) Sin_tab;               // Source block address
  DMA0DA = DAC12_0DAT_;                 // Destination single address
  DMA0SZ = 0x20;                        // Block size
  DMACTL0 = DMA0TSEL_5;                 // DAC12IFG trigger
  DMA0CTL = DMADT_4 + DMASRCINCR_3 + DMAEN;     // Rpt, inc src, word-word
  DAC12_0CTL = DAC12LSEL_2 + DAC12AMP_5 + DAC12IFG + DAC12ENC; // Config
                                        // **force first interrupt**
  P6OUT |= 0x04;                        // Disable charge pump snooze mode
  P6DIR |= 0x04;                        
  P1OUT &= ~0x10;                       // Enable audio output stage
  P1DIR = 0x3f;                         // P1.6/P1.7 inp, all other outp
  CCTL1 = OUTMOD_3;                     // CCR1 set/reset
  CCR1 = 1;                             // CCR1 PWM Duty Cycle	  
//  CCR0 = 25-1;                          // ~1kHz Clock period 
  CCR0 = 64-1;                          // ~ 390Hz Clock period 
  TACTL = TASSEL_2 + MC_1;              // SMCLK, up-mode

  for (;;)                              
  {
    unsigned int i;
    P1OUT ^= 0x01;                      // Toggle P1.0 
    for (i = 0; i < 0xFFFF; i++);       // ~500k cycles

  }
}
