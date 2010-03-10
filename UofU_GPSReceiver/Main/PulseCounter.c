#include "../Main/UGPS.h"

void ExternalPinWakeupSetup()
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
//    SiGEPulseCount++;//use this as the external interrupt to wake up the CPU
    __bic_SR_register_on_exit(LPM3_bits);   // Exit to active CPU
    P1IFG &= ~0x02;
  }
  _EINT();                                //Enable interrupts
}