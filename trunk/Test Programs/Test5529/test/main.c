/*#include "UGPS.h"
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
}*/



#include  "msp430x552x.h"

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  P1DIR |= 0x01;                            // P1.0 output
  TA1CTL = TASSEL_2 + MC_2 + TACLR + TAIE;  // SMCLK, contmode, clear TAR
                                            // enable interrupt
  TA1CCTL0 |= CCIE;                          // CCR0 interrupt enabled
  TA1CCR0 = 2000;

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
  __no_operation();                         // For debugger
}

// Timer1_A3 Interrupt Vector (TAIV) handler
#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
{
  switch(__even_in_range(TA1IV,14))
  {
    case  0: 
      TA1CCR0 += 10000; 
      break;                          // No interrupt
    case  2: break;                          // CCR1 not used
    case  4: break;                          // CCR2 not used
    case  6: break;                          // reserved
    case  8: break;                          // reserved
    case 10: break;                          // reserved
    case 12: break;                          // reserved
    case 14: P1OUT ^= 0x01;                  // overflow
             break;
    default: break; 
  }
}