#include "../Main/UGPS.h"

void WakeupPinSetup()
{
 //SiGEPulseCount = 0;
 P2DIR &= ~BIT0;      //set P2.0 as an input
 P2IES &= ~BIT0;      //triggered on rising edge
}

void DisableWakeupPin()
{
  P2IE &= ~BIT0;    //disable 2.0 interrupt
}

void EnableWakeupPin()
{
  P2IE |= BIT0;        //enable 2.0 interrupt 
}

#pragma vector=PORT1_VECTOR 
__interrupt void Port1Interrupt(void)
{
  _DINT();                                 //Disable interrupts
  if(P1IFG & 0x2)
  {
//    SiGEPulseCount++;//use this as the external interrupt to wake up the CPU
    P1IFG &= ~0x02;
    __bic_SR_register_on_exit(LPM3_bits);   // Exit to active CPU
  }
  _EINT();                                //Enable interrupts
}