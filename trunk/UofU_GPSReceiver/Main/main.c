/*
  ports:

      Name        use       pin #       notes         used by
      
      P1.0        ---        21
      P1.1      flash CS     22        0 = active       hr
      P1.2     flash1 hld    23        0 = hld          hr 
      P1.3     flash2 hld    24
      P1.4        mux1       25        0 = mcu          hr
      P1.5        mux2       26        1 = sige         hr
      P1.6        ---        27
      P1.7        ---        28
      P2.0        ---        29
      P2.1        ---        30
      P2.2        ---        31
      P2.3        ---        32
      P2.4        ---        33
      P2.5        ---        34
      P2.6        ---        35
      P2.7   flash/UCA0CLK   36                         hr
      P3.0        ---        37
      P3.1        ---        38
      P3.2        ---        39
      P3.3   flash/UCA0SIMO  40                         hr  
      P3.4        ---        41
      P3.5   flash/UCA0SOMI  42                         hr
      P3.6        ---        43
      P3.7        ---        44










*/

#include "UGPS.h"

void main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer

  if(FlashInfoStored())         //checking if flash chip info has already been stored
  {
    StoreFlashPageInfo();       //storing flash info, function not implemented
  }
  
  ADCSetup();                    //Setup ADC conversion, also does TimerA setup
  ExternalPinWakeupSetup();       //External wakeup signal
  while(1)
  {
    __bis_SR_register(LPM3_bits + GIE);   //Enter LPM3 Mode, wait for ADXL trigger to exit
    //Need to turn ADC and External wakeup pin off
    //setup SiGe pins and micron flash
    //control the switching between the flash chips for a set amount of time
  }
}


