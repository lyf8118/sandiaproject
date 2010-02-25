#ifndef _TIMER_H
#define _TIMER_H

void TimerA_Setup()
{
  TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
  TA1CCR0 = U_ADCWAIT;
  TA1CTL = TASSEL_1 + MC_1 + TACLR + ID_3;         // ACLK, contmode, clear TAR
}

void SetNextInterval()
{
  switch(MODE){
  case UMODE_NORMAL:
    TA1CCR0 += (50000 - U_ADCWAIT);
    break;
  case UMODE_INIT:
    TA1CCR0 += (2000 - U_ADCWAIT);
    break;
  case UMODE_ADC:
    TA1CCR0 += (20000 - U_ADCWAIT);
    break;
  case UMODE_DEBUG:
    TA1CCR0 += (2000 - U_ADCWAIT);
    break; 
  }
}

// Timer1 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
  _DINT();
  if(MODE == UMODE_SIGE)
  P1OUT = !P1OUT;
  TA1CCR0 += U_ADCWAIT;                         // Add Offset to CCR0
   if(!ADCBusy()){
      if(ADCSamples == U_ADCSAMPLESNEEDED){
        ADCSamples = 0;
        ADCCounter = 0;
        movementDetected = isMoving();
      if(ADCHistory == U_ADCHISTORYNEEDED && MODE == UMODE_INIT)
        MODE = UMODE_ADC;
      else if(MODE == UMODE_SIGE)
        MODE = UMODE_ADC;
      if(movementDetected == TRUE){
        MODE = UMODE_SIGE;
        /*
          Any conversion mode may be stopped immediately by setting the CONSEQx = 0 and resetting the
          ADC12ENC bit. Conversion data are unreliable.
        */
        __bic_SR_register_on_exit(LPM3_bits);   // Exit to active CPU
      }
      SetNextInterval();     
   }
   else{ //if(ADCSamples == ADCSampleNum){   //wait till previous sample is finished
        //ADCSampleNum = (ADCSampleNum+1 == U_ADCSAMPLESNEEDED) ? 0 : ADCSampleNum + 1;
      EnableADCInterrupt();
      ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion
    }
}
  _EINT();
}

#endif