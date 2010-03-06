#include "UGPS.h"
void main2(void);
volatile char c = 0x55;
void main(void)
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
  //main2();
  while(1){
    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, Enable interrupts
    _DINT();
    i=0;
    while(i != 0xfffff){i++;}
    _EINT();
  }
}


