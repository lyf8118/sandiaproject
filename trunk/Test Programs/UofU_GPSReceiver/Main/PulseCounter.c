#include "../Main/UGPS.h"

void SiGECounterSetup()
{
 SiGEPulseCount = 0;
 P1DIR = 0x01;
 P1IES = 0x00;
 P1IE = 0x02;
}

#pragma vector=PORT1_VECTOR 
__interrupt void Port1Interrupt(void)
{
  _DINT();                                 //Disable interrupts
  if(P1IFG & 0x2)
  {
    SiGEPulseCount++;
    P1IFG &= ~0x02;
  }
  _EINT();
}