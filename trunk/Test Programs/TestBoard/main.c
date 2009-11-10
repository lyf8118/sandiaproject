
#include  <msp430x16x.h>


extern Wait(unsigned int, short);
extern const unsigned int MAXDELAY;

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  P1DIR = 0xff;
  P1OUT = 0;
  //BCSCTL1 = RSEL1+RSEL2+RSEL0; //RSEL2 = 800 kHz
  P5SEL = 0x70;
  P5OUT = 0x70;
  //LFXT1;
  char myP1=0;
  while(1){
    myP1++;
    if(myP1 == 0){
      P1OUT = 0xff;
      Wait(MAXDELAY, 3);
      P1OUT = 0x0;
      Wait(MAXDELAY,3);
    }
    Wait(1000,1);    
  }
}
