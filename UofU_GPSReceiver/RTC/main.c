#include "../Main/UGPS.h"
//#include "io430.h"
//#include  "msp430x552x.h"
//#include <signal.h>
//#include "time.h"
/*struct RTC_date_t 
{
     unsigned int year;
     unsigned int month;
     unsigned int day;
     unsigned int hour;
     unsigned int min;
     unsigned int sec;
};*/


void main(void) {
  WDTCTL = WDTPW+WDTHOLD; // Stop the dog
 // RTCCTL01 = RTCBCD | 0x20;
  //P1DIR |= 0x01;
 
  RTC_init();
 
  //RTC_date today = {0x20,0x10,0x03,0x22,0x12, 0x00, 0x55};
  //RTC_setDate(&today);
  //RTC_setMinute();
  //RTC_init();
 //P1OUT  |= 0x1;
 for(;;)
 {
   __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, enable ints 
   __no_operation();                   // For debugger
 }
}

