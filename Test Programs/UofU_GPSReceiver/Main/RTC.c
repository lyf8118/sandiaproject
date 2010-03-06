#include "../Main/UGPS.h"
/*
void TIME_getDate(TIME_date_t *const date)
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
 
void TIME_setDate(const TIME_date_t *date)
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


#pragma vector=RTC_VECTOR
__interrupt void basic_timer(void) {
//  P5OUT ^= 0x02; // Toggle P5.1 every minute
  P1OUT = ~P1OUT;
}