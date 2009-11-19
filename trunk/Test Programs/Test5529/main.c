//******************************************************************************

//******************************************************************************

#include  "msp430x552x.h"
#include "Utils.h"
#include "ADC.h"
#include "Timer.h"

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  TimerA_Setup();
  ADCSetup();
  P1DIR |= 0x03;                            // P1.0,1.1 output
  P1OUT = 0;
  MODE = UMODE_INIT;
  while(1){
    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, enable interrupts
    unsigned int i = 0;
    if(MODE == UMODE_SIGE){
      while(i++ < 0xffff){} 
      TA1CCTL0 ^= CCIE;                          // CCR0 interrupt enabled
      i=0;
      while(i++ < 0xffff){} 
      TA1CCTL0 ^= CCIE;
    }
  }
}