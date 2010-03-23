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
  P1DIR |= 0x01;
 
  RTCCTL01 = RTCBCD | RTCHOLD | 0x2040 | RTCTEV_0;
 // Enable, BCD, int every minute
 RTCSEC = 0x01; // Set Seconds
 RTCMIN = 0x00; // Set Minutes
 RTCHOUR = 0x08; // Set Hours
 RTCDOW = 0x02; // Set DOW
 RTCDAY = 0x23; // Set Day
 RTCMON = 0x11; // Set Month
 RTCYEAR = 0x2009; // Set Year
 RTCCTL01 &= ~RTCHOLD; // Enable RTC
 
  //RTC_date today = {0x20,0x10,0x03,0x22,0x12, 0x00, 0x55};
  //RTC_setDate(&today);
  //RTC_setMinute();
  //RTC_init();
 P1OUT  |= 0x1;
 __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, enable ints 
 __no_operation();                   // For debugger
 for(;;)
 {
 }
}

