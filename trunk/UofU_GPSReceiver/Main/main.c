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


