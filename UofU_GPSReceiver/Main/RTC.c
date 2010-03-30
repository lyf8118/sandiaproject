#include "../Main/UGPS.h"

void RTC_init()
{
   RTCCTL01 = RTCBCD | RTCHOLD | 0x2040 | RTCTEV_0;
   // Enable, BCD, int every minute
//   RTCSEC = 0x01; // Set Seconds
//   RTCMIN = 0x00; // Set Minutes
//   RTCHOUR = 0x08; // Set Hours
//   RTCDOW = 0x02; // Set DOW
//   RTCDAY = 0x23; // Set Day
//   RTCMON = 0x11; // Set Month
//   RTCYEAR = 0x2009; // Set Year
   RTCCTL01 &= ~RTCHOLD; // Enable RTC
}
void RTC_getDate(RTC_date *date)
{
   while(!(RTCCTL1 & 0x10));   //wait for RTCRDY signal to go high, so read will be valid
   date->yearh = RTCYEARH;
   date->yearl = RTCYEARL;
   date->month = RTCMON;
   date->day = RTCDAY;
   date->hour = RTCHOUR;
   date->min = RTCMIN;
   date->sec = RTCSEC;
}
 
void RTC_setDate(RTC_date *date)
{
     RTCCTL01 |= RTCHOLD;
     RTCYEARH = date->yearh;
     RTCYEARL = date->yearl;
     RTCMON = date->month;
     RTCDAY = date->day;
     RTCHOUR = date->hour;
     RTCMIN = date->min;
     RTCSEC = date->sec;
     RTCCTL01 &= ~RTCHOLD;
}

void RTC_setAlarm(char day,char dayOfWeek,char hour,char minute)
{
  RTCAMIN = minute;
  RTCAHOUR = hour;
  RTCADOW = dayOfWeek;
  RTCADAY = day;
  RTCCTL01 = RTCAIFG;
}

//make functions to set the different interrupts to fire and then put the interrupts below.
void RTC_setMinute()
{
  RTCCTL01 = RTCTEV__MIN | RTCTEVIFG;
}

void RTC_setHour()
{
  RTCCTL01 = RTCTEV__HOUR | RTCTEVIFG;
}

void RTC_setNoon()
{
  RTCCTL01 = RTCTEV__1200 | RTCTEVIFG;
}

void RTC_setMidnight()
{
  RTCCTL01 = RTCTEV__0000 | RTCTEVIFG;
}

#pragma vector=RTC_VECTOR
__interrupt void every_minute(void) {
  _DINT();                                 //Disable interrupts
  RTCIV &= 0x00;
//  P1OUT ^= 0x01;
  _EINT();                                //Enable interrupts
}