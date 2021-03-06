
//#include "io430.h"
#include  "msp430x552x.h"
#include <signal.h>
#include "time.h"



void main(void) {
  WDTCTL = WDTPW+WDTHOLD; // Stop the dog
 // RTCCTL01 = RTCBCD | 0x20;
 RTCCTL01 = RTCBCD | RTCHOLD | 0x20 | RTCTEV_0;
 // Enable, BCD, int every minute
 RTCSEC = 0x00; // Set Seconds
 RTCMIN = 0x00; // Set Minutes
 RTCHOUR = 0x08; // Set Hours
 RTCDOW = 0x02; // Set DOW
 RTCDAY = 0x23; // Set Day
 RTCMON = 0x11; // Set Month
 RTCYEAR = 0x2009; // Set Year
 RTCCTL01 = ~RTCHOLD; // Enable RTC
 __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, enable ints 
 for(;;)
 {
 }
}

__interrupt void basic_timer(void) {
  P5OUT ^= 0x02; // Toggle P5.1 every minute
}

/*void TIME_getDate (TIME_date_t *const date)
{
     //RTCCTL |= RTCHOLD;
     date->year = RTCYEARH << 8 | RTCYEARL;
     date->month = RTCMON;
     date->day = RTCDAY;
     date->hour = RTCHOUR;
     date->min = RTCMIN;
     date->sec = RTCSEC;
     //RTCCTL &= ~RTCHOLD;
}
 
void TIME_setDate (const TIME_date_t *date)
{
     RTCCTL |= RTCHOLD;
     RTCYEARH = date->year >> 8;
     RTCYEARL = date->year & 0xFF;
     RTCMON = date->month;
     RTCDAY = date->day;
     RTCHOUR = date->hour;
     RTCMIN = date->min;
     RTCSEC = date->sec;
     RTCCTL &= ~RTCHOLD;
}*/

