//so far only the ADC12MEM0 is working the way we want it to, I included a code sample in the same
//folder where this file is that we can implement
//and test to get the other two working
//This code already puts the CPU in low power mode 0 and wakes up to look at it

#include  <msp430x16x.h>

#define NUMSAMPLES 10
#define MAXDELAY 0xffff
#define DEFAULT 0xda9//0x7ff
//#define ACTUAL 0xD01

static int numCollected = 0;
static unsigned int pin6_0H[NUMSAMPLES];
static unsigned int pin6_0L[NUMSAMPLES];
void Wait(unsigned int, short);

int main( void )
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  ADC12CTL0 = ADC12ON+SHT0_2;           // Turn on ADC12, extend sampling time
                                            // to avoid overflow of results
  ADC12CTL1 = SHP;//+CONSEQ_3;                 // Use sampling timer, repeated sequence
  //ADC12MCTL0 = INCH_0;                      // ref+=AVcc, channel = A3, end seq.
  ADC12IE = 0x01;                           // Enable ADC12IFG.3
  ADC12CTL0 |= ENC;                         // Enable conversions
  ADC12CTL0 |= ADC12SC;                     // Start conversion
  P6SEL = 0x0F;                             // Enable A/D channel inputs
  P1DIR |= 0x07;
  
  while(1)
  {
    ADC12CTL0 |= ADC12SC;                   // Sampling open
    _BIS_SR(CPUOFF + GIE);                  // LPM0, ADC12_ISR will force exit
    //Wait(MAXDELAY/2,1);
  }
}

//delay - how long you wish to delay
//loops - used for when you want to wait beyond the MAXDELAY value
//        use 1 for a default value
void Wait(unsigned int delay, short loops)               //a simple delay loop, for debugging purposes
{
 volatile unsigned int i = delay;                              // Delay
 short loopCount = 0;
 while(loopCount < loops){
   while (i != 0)
      i--;   
   loopCount++;
   i = delay;
 }
}

// ADC12 interrupt service routine
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR0 (void)
{
  if (ADC12MEM0 < DEFAULT){
    if(numCollected < NUMSAMPLES){
      pin6_0L[numCollected] = ADC12MEM0;
      pin6_0H[numCollected] = 0;  
    }
      P1OUT &= ~0x01;                       // Clear P1.0 LED off      
  }
    else{
      if(numCollected < NUMSAMPLES){
        pin6_0H[numCollected] = ADC12MEM0;
        pin6_0L[numCollected] = 0;
      }
      P1OUT |= 0x01;                        // Set P1.0 LED on      
    }
    numCollected++;
    _BIC_SR_IRQ(CPUOFF);                    // Clear CPUOFF bit from 0(SR)
}



//sequence of channels for multiple
/*
//******************************************************************************
//  MSP-FET430P140 Demo - ADC12, Sequence of Conversions (non-repeated)
//
//  Description: This example shows how to perform A/D conversions on a sequence
//  of channels. A single sequence of conversions is performed - one conversion
//  each on channels A0, A1, A2, and A3. Each conversion uses AVcc and AVss for
//  the references. The conversion results are stored in ADC12MEM0, ADC12MEM1,
//  ADC12MEM2, and ADC12MEM3 respectively and are moved to 'results[]' upon
//  completion of the sequence. Test by applying voltages to pins A0, A1, A2,
//  and A3, then setting and running to a break point at the "_BIC..."
//  instruction in the ISR. To view the conversion results, open a watch window
//  in debugger and view 'results' or view ADC12MEM0, ADC12MEM1, ADC12MEM2, and
//  ADC12MEM3 in an ADC12 SFR window.
//  This can run even in LPM4 mode as ADC has its own clock
//  Note that a sequence has no restrictions on which channels are converted.
//  For example, a valid sequence could be A0, A3, A2, A4, A2, A1, A0, and A7.
//  See the MSP430x1xx User's Guide for instructions on using the ADC12.
//
//
//                MSP430F149
//            -----------------
//           |                 |
//   Vin0 -->|P6.0/A0          |
//   Vin1 -->|P6.1/A1          |
//   Vin2 -->|P6.2/A2          |
//   Vin3 -->|P6.3/A3          |
//           |                 |
//
//
//  M. Mitchell
//  Texas Instruments Inc.
//  Feb 2005
//  Built with CCE Version: 3.2.0 and IAR Embedded Workbench Version: 3.21A
//******************************************************************************

#include  <msp430x14x.h>
#define MAXDELAY 0xffff
#define DEFAULT 0x0ff//0x7ff
#define NUMSAMPLES 10
//#define ACTUAL 0xD01

static unsigned int results[4];             // Needs to be global in this example
                                            // Otherwise, the compiler removes it
                                            // because it is not used for anything.
static unsigned int pin1[NUMSAMPLES];
static unsigned int pin2[NUMSAMPLES];
static unsigned int pin3[NUMSAMPLES];
static unsigned int pin4[NUMSAMPLES];

//delay - how long you wish to delay
//loops - used for when you want to wait beyond the MAXDELAY value
//        use 1 for a default value
void Wait(unsigned int delay, short loops)               //a simple delay loop, for debugging purposes
{
 volatile unsigned int i = delay;                              // Delay
 short loopCount = 0;
 while(loopCount < loops){
   while (i != 0)
      i--;   
   loopCount++;
   i = delay;
 }
}

void main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  P6SEL = 0x0F;                             // Enable A/D channel inputs
  ADC12CTL0 = ADC12ON+MSC+SHT0_2;           // Turn on ADC12, set sampling time
  ADC12CTL1 = SHP+CONSEQ_1;                 // Use sampling timer, single sequence
  ADC12MCTL0 = INCH_0;                      // ref+=AVcc, channel = A0
  ADC12MCTL1 = INCH_1+EOS;                      // ref+=AVcc, channel = A1
//  ADC12MCTL2 = INCH_2+EOS;                      // ref+=AVcc, channel = A2
//  ADC12MCTL3 = INCH_3;+EOS;                  // ref+=AVcc, channel = A3, end seq.
  ADC12IE = 0x02;//8;                           // Enable ADC12IFG.3
  ADC12CTL0 |= ENC;                         // Enable conversions
  P1DIR = 0x0f;
  P1OUT = 0x0;
  int collected = 0;

  while(collected < NUMSAMPLES)
  {
  ADC12CTL0 |= ADC12SC;                     // Start conversion
  _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0, Enable interrupts
  if (results[0] > INCH_0 + DEFAULT)
      P1OUT &= ~0x01;                       // Clear P1.0 LED off
    else
      P1OUT |= 0x01;                        // Set P1.0 LED on
    
    if (results[1]  > INCH_1 + DEFAULT)
      P1OUT &= ~0x02;                       // Clear P1.1 LED off
    else
      P1OUT |= 0x02;                        // Set P1.1 LED on
    
    if (results[2]  > INCH_2 + DEFAULT)
      P1OUT &= ~0x04;                       // Clear P1.2 LED off
    else
      P1OUT |= 0x04;                        // Set P1.2 LED on
    
    pin1[collected] = results[0];
    pin2[collected] = results[1];
    pin3[collected] = results[2];
    pin4[collected] = results[3];
    Wait(MAXDELAY, 1);
    
    P1OUT = 0;
    Wait(MAXDELAY/2, 1);
    collected++;
  }
  while(1){}      //set breakpoint here and view samples in debugger
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
  results[0] = ADC12MEM0;                   // Move results, IFG is cleared
  results[1] = ADC12MEM1;                   // Move results, IFG is cleared
  results[2] = ADC12MEM2;                   // Move results, IFG is cleared
  results[3] = ADC12MEM3;                   // Move results, IFG is cleared
  _BIC_SR_IRQ(LPM0_bits);                   // Clear LPM0, SET BREAKPOINT HERE
}*/