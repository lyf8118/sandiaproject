#include "../Main/UGPS.h"

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  unsigned long i = 0;
  P1DIR = 0x01;
  P1OUT = 1;
  ADCSetup();
  TimerA_Setup();
  
  while(1){
    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, Enable interrupts
    _DINT();
    i=0;
    while(i != 0xfffff){i++;}
    _EINT();
  }
}
