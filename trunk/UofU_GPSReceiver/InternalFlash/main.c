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
  
  P1DIR = 0x01;
  P1OUT = 1;
  
  char* PageDataArray = (char*)PAGEDATASTART;
  
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
  P1OUT = 0;
  while(1){  }
}