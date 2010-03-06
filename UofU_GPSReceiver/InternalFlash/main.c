#include "../Main/UGPS.h"
unsigned long pulseCount;

void InternalFlashMain();
void PulseCounterMain();

void SiGECounterSetup()
{
 P1DIR = 0x01;
 P1IES = 0x00;
 P1IE = 0x02;
 pulseCount = 0;
}

void main( void )
{
   WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
//  InternalFlashMain();
  //PulseCounterMain();
//}

//void PulseCounterMain()
//{
 long i = 0;
 SiGECounterSetup();
 P1OUT=1;
 while( i !=0xffff){i++;}
   i=0;
 P1OUT=0;
 while(1)
 {
  // __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, Enable interrupts
   _DINT();
   if(pulseCount > 100000)
     pulseCount = 0;
   _EINT();
//   _DINT();  
//   while( i !=0xffff){i++;}
//   i=0;
//   _EINT(); 
 }
}

//need to move this to where micron flash code is
#pragma vector=PORT1_VECTOR
__interrupt void Port1Interrupt(void)
{
  _DINT();                                 //Disable interrupts
  if(P1IFG & 0x2){
    pulseCount++;
    P1IFG &= ~0x02;
   // __bic_SR_register_on_exit(LPM3_bits);
  }
  _EINT();
}

void InternalFlashMain()
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  unsigned long i = 0;
  char* PageDataArray = (char*)PAGEDATASTART;
  P1DIR = 0x01;
  P1OUT = 1;
  ADCSetup();
  TimerA_Setup();
  for(int x = 0; x < 1024; x++)
  {
    if(x < 128)
      PageDataArray[x] = 0;
    if(x == 33 || x == 101 || x == 1000)
      insertBit(TRUE, x, PageDataArray);
    else insertBit(FALSE, x, PageDataArray); 
  }
  write_Flash(PageDataArray, 0);   //writing info for flash 1
  for(int x = 0; x < 1024; x++)
  {
    if(x < 128)
      PageDataArray[x] = 0;
    if(x == 32 || x == 100 || x == 999)
      insertBit(TRUE, x, PageDataArray);
    else insertBit(FALSE, x, PageDataArray); 
  }
  write_Flash(PageDataArray, 1);   //writing info for flash 2
  
  while(1){
    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, Enable interrupts
    _DINT();
    i=0;
    while(i != 0xfffff){i++;}
    _EINT();
  }
}