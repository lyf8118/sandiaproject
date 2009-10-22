//*******************************************************************************
//  D169 Demo - Voice Recorder and Playback with 8-bit Compression
//
//  Uses the new DMA and DAC12 modules to perform autonomous speech recording
//  and playback. CPU runs from DCO at the default speed of ~800KHz. Using two
//  DMA channels, the 12-bit data sampled from ADC12 is first converted to 
//  8-bits using DMA0 and the hardware multiplier, then saved to Flash
//  using DMA1. Playback is using the DAC0 in 8-bit Mode.
//
//
//   +--------------------+
//   |                    |
//   |               ADC0 +-------< input from mic pre-amplifier
//   |               DAC0 +-------> output to audio output stage
//   |               P1.0 +-------> LED #1
//   |               P1.1 +-------> LED #2
//   |  MSP430F169   P1.3 +-------> input stage shutdown (0 = shutdown)
//   |               P1.4 +-------> output stage shutdown (1 = shutdown)
//   |               P1.6 +-------< KEY #1
//   |               P1.7 +-------< KEY #2
//   |               P6.2 +-------> charge pump snooze mode (0 = snooze)
//   |                    |
//   +--------------------+
//
//
//  M. Buccini
//  Texas Instruments Inc.
//  August 2003
//  Built with IAR Embedded Workbench Version: 1.26B
//*******************************************************************************
#include <msp430x16x.h>
//------------------------------------------------------------------------------
#define Memstart             0x1400         // Memory range to be filled with
#define Memend               0xfe00         // Sampled data

#define SamplePrd            90             // Record & playback sample period
                                            // SampleR = 800,000 / SamplePrd

void Init_Sys(void);                        // Function prototypes
void Record(void); 
void Playback(void);
void Erase(void);
//-----------------------------------------------------------------------------
void main(void)
{
  Init_Sys();                               // Initialize system
  
  while (1)                                 // Repeat forever
  {
    // wait for key-press event, hold CPU in low-power mode
    P1IFG &= ~0xc0;                         // Clear all button interrupt flags
    P1IE |= 0xc0;                           // Enable int for buttons
    _BIS_SR(LPM3_bits + GIE);               // Enter LPM3 w/ interrupts
    _DINT();                                // Disable interrupts
    P1IE &= ~0xc0;                          // Disable interrupts for buttons

    // process key-press event
    if (!(P1IN & 0x40))                     // Record button pressed?
      Record();                             
    else                                    // No, -> must be playback button
      Playback();                           
  }
}
//-----------------------------------------------------------------------------
// Record audio data and store in Flash using the ADC12, DMA and MPY
//-----------------------------------------------------------------------------
void Record(void)
{
  // power-up external hardware
  P1OUT |= 0x09;                            // LED#1 & audio input stage on
  P6OUT |= 0x04;                            // Disable charge pump snooze mode
                  
  // setup modules
  ADC12IFG = 0x00;                          // 
  ADC12CTL1 = SHS_3 + CONSEQ_2;             // S&H TB.OUT1, rep. single chan
  ADC12CTL0 = ADC12ON + ENC;                // ADC12 on, enabled

  TBCCR0 = SamplePrd;                       // Initialize TBCCR0 w/ sample prd
  TBCCR1 = SamplePrd - 20;                  // Trigger for ADC12 SC
  TBCCR2 = SamplePrd - 5;                   // Trigger for DMA1
  TBCCTL1 = OUTMOD_7;                       // Reset OUT1 on EQU1, set on EQU0

  DMA0SA = ADC12MEM0_;                      // Src address = ADC12 module
  DMA0DA = OP2_;                            // Dst address = multiplier
  DMA0SZ = 1;                               // Size in words
  DMACTL0 = DMA1TSEL_2 + DMA0TSEL_6;        // DMA1=TBCCR2_IFG, DMA0=ADC12IFGx
  DMA0CTL = DMADT_4 + DMAEN;                // Sng rpt, config

  DMA1SA = RESHI_;                          // Src address = multiplier
  DMA1DA = Memstart;                        // Dst address = Flash memory
  DMA1SZ = (Memend - Memstart);             // Size in bytes
  DMA1CTL = DMADSTINCR_3 + DMASBDB + DMAIE + DMAEN;   // Sng, config

  MPY = 0x0FFF;                             // MPY first operand
  
  // unlock and erase Flash memory
  FCTL3 = FWKEY;                            // Unlock Flash memory for write
  Erase();                                  // Call Flash erase subroutine
  FCTL1 = FWKEY + WRT;                      // Enable Flash write for recording
           
  // start recording and enter LPM
  P1OUT |= 0x01;                            // LED#1 on
  TBCTL = TBSSEL_2+ MC_1 + TBCLR ;          // SMCLK, clear TBR, up mode
  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrups
  _DINT();                                  // Disable interrupts
  
  // deactivate Flash memory write access
  FCTL1 = FWKEY;                            // Disable Flash write
  FCTL3 = FWKEY + LOCK;                     // Lock Flash memory     
           
  // power-down MSP430 modules
  DMA0CTL &= ~DMAEN;                        // Disable
  ADC12CTL1 &= ~CONSEQ_2;                   // Stop conversion immidiately
  ADC12CTL0 &= ~ENC;                        // Disable ADC12 conversion
  ADC12CTL0 = 0;                            // Switch off ADC12 & ref voltage
  TBCTL = 0;                                // Disable Timer_B
 
  // power-down external hardware
  P1OUT &= ~0x09;                           // Disable LED#1 & audio input stage
  P6OUT &= ~0x04;                           // Enable charge pump snooze mode
}
//-----------------------------------------------------------------------------           
// Playback audio data stored in Flash memory using the DMA and DAC12 module
//-----------------------------------------------------------------------------
void Playback(void)
{
  // power-up external hardware
  P1OUT = 0x02;                             // LED#2 on, enable audio outp
  P6OUT |= 0x04;                            // Disable charge pump snooze mode

  // setup modules
  ADC12CTL0 =  REF2_5V + REFON;              // ADC12 ref needed for DAC12
  DAC12_0CTL = DAC12RES + DAC12LSEL_3 + DAC12IR + DAC12AMP_7 + DAC12ENC; // config

  TBR = SamplePrd + 2;                      // Force delay of 64k for Vref  
  TBCCR0 = SamplePrd;                       // Initialize TBCCR0 w/ sample prd
  TBCCR2 = SamplePrd >> 1;                  // EQU2 will trigger DMA
  TBCCTL2 = OUTMOD_7;                       // Reset OUT2 on EQU2, set on EQU0

  DMA1SA = Memstart;                        // Src address = Flash memory
  DMA1DA = DAC12_0DAT_;                     // Dst address = DAC12 module
  DMA0SZ = (Memend - Memstart);             // DMA block size in bytes
  DMACTL0 = DMA1TSEL_2;                     // Timer_B.CCIFG2 triggers DMA1
  DMA1CTL = DMASRCINCR_3 + DMASBDB + DMAIE + DMAEN;   // Config

  // start playback and enter LPM
  TBCTL = TBSSEL_2 + MC0;                   // SMCLK, up mode
  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0 w/ interrups
  _DINT();                                  // Disable interrupts
                                          
  // power-down MSP430 modules
  TBCTL = 0;                                // Disable Timer_B
  ADC12CTL0 = 0;                            // Switch off ADC12 ref voltage
  DAC12_0CTL &= ~DAC12ENC;                  // Disable DAC12 conversion
  DAC12_0CTL = 0;                           // Switch off DAC12
           
  // power-down external hardware
  P1OUT = 0x10;                             // LED#2 off, disable audio outp
  P6OUT &= ~0x04;                           // Enable charge pump snooze mode
}
//-----------------------------------------------------------------------------
// Setup system and peripherals
//-----------------------------------------------------------------------------
void Init_Sys(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog

  P1OUT = 0x10;                             // LEDx off, disable audio outp/ inp
  P1DIR = 0x3f;                             // P1.6/P1.7 inp, all other outp
  P1IES = 0xc0;                             // H->L edge detect for buttons
  P2OUT = 0x00;                             // Clear P2 output register
  P2DIR = 0xff;                             // Unused pins as outp  
  P3OUT = 0x00;                             // Clear P3 output register
  P3DIR = 0xf5;                             // All but I2C pins to output
  P4OUT = 0x00;                             // Clear P4 output register
  P4DIR = 0xff;                             // Unused pins as outp  
  P5OUT = 0x00;                             // Clear P5 output register
  P5DIR = 0xff;                             // Unused pins as outp  
  P6OUT = 0x00;                             // Clear P6 output register
  P6SEL = 0x01;                             // Select ADC12 A0
  P6DIR = 0xff;                             // Unused pins as output
  
  FCTL2 = FWKEY + FSSEL_2 + FN0;            // Clk src = SMCLK / 2 (~440KHz)
}
//-----------------------------------------------------------------------------
// Erase Flash memory for new recording
//-----------------------------------------------------------------------------
void Erase(void)
{
  unsigned int *pMemory = (unsigned int *)Memstart;
                                            // Start of record memory array
  do
  {
    if ((unsigned int)pMemory & 0x1000)     // Use bit 12 to toggle LED#1
      P1OUT |= 0x01;
    else
      P1OUT &= ~0x01;    
    
    FCTL1 = FWKEY + ERASE;
    *pMemory = 0x00;                        // Dummy write to activate
                                            // Segment erase
    pMemory += 0x0100;                      // Point to next segment
  } while (pMemory < (unsigned int *)Memend);
}  
//----------------------------------------------------------------------------
// PORT1 interrupt handler
//-----------------------------------------------------------------------------
interrupt[PORT1_VECTOR] void PORT1ISR(void)
{
  P1IFG = 0;                                // Clear all P1.x interrupt flags
  _BIC_SR_IRQ(LPM3_bits);                   // Exit LPM3 on reti
}
//-----------------------------------------------------------------------------
// DAC12, DMA interrupt handler
//-----------------------------------------------------------------------------
interrupt[DACDMA_VECTOR] void DACDMAISR(void)
{
  DMA1CTL &= ~DMAIFG;                       // Clear DMA1 interrupt flag
  _BIC_SR_IRQ(LPM0_bits);                   // Exit LPM0 on reti
}
