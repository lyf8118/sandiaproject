//******************************************************************************
//  MSP430F552x Demo - ADC12, Repeated Sequence of Conversions
//
//  Description: This example shows how to perform a repeated sequence of
//  conversions using "repeat sequence-of-channels" mode.  AVcc is used for the
//  reference and repeated sequence of conversions is performed on Channels A0,
//  A1, A2, and A3. Each conversion result is stored in ADC12MEM0, ADC12MEM1,
//  ADC12MEM2, and ADC12MEM3 respectively. After each sequence, the 4 conversion
//  results are moved to A0results[], A1results[], A2results[], and A3results[].
//  Test by applying voltages to channels A0 - A3. Open a watch window in
//  debugger and view the results. Set Breakpoint1 in the index increment line
//  to see the array values change sequentially and Breakpoint2 to see the entire
//  array of conversion results in A0results[], A1results[], A2results[], and
//  A3results[]for the specified Num_of_Results.
//
//  Note that a sequence has no restrictions on which channels are converted.
//  For example, a valid sequence could be A0, A3, A2, A4, A2, A1, A0, and A7.
//  See the MSP430x5xx User's Guide for instructions on using the ADC12.
//
//               MSP430F552x
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//    Vin0 -->|P6.0/CB0/A0      |
//    Vin1 -->|P6.1/CB1/A1      |
//    Vin2 -->|P6.2/CB2/A2      |
//            |                 |
//            |                 |
//
//   Bhargavi Nisarga
//   Texas Instruments Inc.
//   April 2009
//   Built with CCE v3.1 and IAR Embedded Workbench Version: 4.20
//******************************************************************************

#include "UGPS.h"

volatile char HAVEBASELINE = FALSE;
volatile char ADCStarted = FALSE;
volatile char ADCDone = FALSE;
volatile unsigned long ADXL_XBASE;
volatile unsigned long ADXL_YBASE;
volatile unsigned long ADXL_ZBASE;

unsigned int UINT_MAX(unsigned int a, unsigned int b)
{
  return a > b ? a : b;
}

unsigned int UINT_MIN(unsigned int a, unsigned int b)
{
  return a < b ? a : b;
}

unsigned int UINT_ArrayMax(unsigned int volatile* Array, int length)
{
 unsigned int max = 0;
 for(int i = 0; i < length; i++)
     max = UINT_MAX(Array[i], max);
 return max;
}

unsigned int UINT_ArrayMin(unsigned int volatile* Array, int length)
{
 unsigned int min = 0xFFFF;
 for(int i = 0; i < length; i++)
     min = UINT_MIN(Array[i], min);
 return min;
}

//Averages values in an array, eliminating max min values of the array
unsigned int ArrayAverage(unsigned int volatile* Array, int length)
{
  unsigned long sum = 0;
 for(int i = 0; i < length; i++) 
   sum += Array[i];
 sum -= UINT_ArrayMax(Array, length);
 sum -= UINT_ArrayMin(Array, length);
 return sum/(length-2);
}

void TimerA_Setup()
{
  TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
  TA1CCR0 = 2000;
  TA1CTL = TASSEL_1 + MC_1 + TACLR + ID_3;         // ACLK, upmode, clear TAR
}

void ADCSetup()
{
  P6SEL = 0x07;                             // Enable A/D channel inputs
  ADC12CTL0 = ADC12ON+ADC12MSC+ADC12SHT0_8; // Turn on ADC12, extend sampling time
                                            // to avoid overflow of results
  ADC12CTL1 = ADC12SHP+ADC12CONSEQ_3+0x8;   // Use sampling timer, repeated sequence
  ADC12MCTL0 = ADC12INCH_0;                 // ref+=AVcc, channel = A0
  ADC12MCTL1 = ADC12INCH_1;                 // ref+=AVcc, channel = A1
  ADC12MCTL2 = ADC12INCH_2+ADC12EOS;        // ref+=AVcc, channel = A2, end seq.
  ADC12CTL2 |= 0x80;                        //turn ADC temp sensor off
  ADC12IE = 0x04;                           // Enable ADC12IFG.2
  ADC12CTL0 |= ADC12ENC;                    // Enable conversions
}

void ADCOff()
{
  ADC12CTL0 &= ~ADC12ON;
}

void ADCOn()
{
  ADC12CTL0 |= ADC12ON;
}

void ADCStart()
{ 
  ADC12CTL0 |= ADC12SC;                     // Start convn - software trigger
}

char MovementDetected()
{
  char flag = FALSE;
  unsigned int XAverage = ArrayAverage(ADXL_X, U_ADCSAMPLESIZE);
  unsigned int YAverage = ArrayAverage(ADXL_Y, U_ADCSAMPLESIZE);
  unsigned int ZAverage = ArrayAverage(ADXL_Z, U_ADCSAMPLESIZE); 
  unsigned int vx = UINT_MAX(XAverage, ADXL_XBASE) - UINT_MIN(XAverage, ADXL_XBASE);
  unsigned int vy = UINT_MAX(YAverage, ADXL_YBASE) - UINT_MIN(YAverage, ADXL_YBASE);
  unsigned int vz = UINT_MAX(ZAverage, ADXL_ZBASE) - UINT_MIN(ZAverage, ADXL_ZBASE);
    //want to be careful with unsigned math here
  /*if(vx > maxVX)      //for DEBUG
    maxVX = vx;
  if(vy > maxVY)
    maxVY = vy;
  if(vz > maxVZ)
    maxVZ = vz;
  numSamples++;*/
  if(vx > U_ADCTHRESHHOLD)
    flag = TRUE;
  if(vy > U_ADCTHRESHHOLD)
    flag = TRUE;
  if(vz > U_ADCTHRESHHOLD)
    flag = TRUE;
  return flag;
}


#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
  _DINT();
  static unsigned int index = 0;

  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6: break;                           // Vector  6:  ADC12IFG0
  case  8: break;                           // Vector  8:  ADC12IFG1
  case 10:                                  // Vector 10:  ADC12IFG2
      ADXL_X[index] = ADC12MEM0;           // Move A0 results, IFG is cleared
      ADXL_Y[index] = ADC12MEM1;           // Move A1 results, IFG is cleared
      ADXL_Z[index] = ADC12MEM2;           // Move A2 results, IFG is cleared
      index++;                                // Increment results index, modulo; Set Breakpoint1 here
      
      if(HAVEBASELINE == TRUE)
      {
        if (index == U_ADCSAMPLESIZE)
        {
          index = 0;
          ADCDone = TRUE;
        }   //instead of exiting to active, just set flag to start averages 
      }
      else
      {
        if (index == U_ADCBASELINESIZE)
        {
          index = 0;
          ADCDone = TRUE;
        }   //instead of exiting to active, just set flag to start averages
      }
  case 12: break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break; 
  }
_EINT();  
}

// TA1CCR0 += 2000 is approximately .5 seconds, .488 exactly

// Timer1 interrupt service routine, executes once per second
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
  _DINT();                                 //Disable interrupts
  char movementFlag;
  TA1CCR0 += 4000;                         // Add Offset to CCR0
  if(ADCStarted)
  {
    if(ADCDone)
    {
      ADCStarted = FALSE;
      ADCDone = FALSE;
      if(HAVEBASELINE)
        {      
          movementFlag = MovementDetected();        
          P1OUT = movementFlag;
          if(movementFlag == TRUE){
            __bic_SR_register_on_exit(LPM3_bits);   // Exit to active CPU
          }
        }
        else
        {
          P1OUT = 0;
          ADXL_XBASE = ArrayAverage(ADXL_X, U_ADCBASELINESIZE);//U_ADCBASELINESIZE);
          ADXL_YBASE = ArrayAverage(ADXL_Y, U_ADCBASELINESIZE);//U_ADCBASELINESIZE);
          ADXL_ZBASE = ArrayAverage(ADXL_Z, U_ADCBASELINESIZE);//U_ADCBASELINESIZE);
          HAVEBASELINE = TRUE;
        }
    }
  } 
  else
  {
    ADCStarted = TRUE;
    ADCStart();    
  }
  //Also check external signal tied to P1.7, the push button on our test board
  if(P1IN&0x2)
  {
    P1OUT = 1;
    __bic_SR_register_on_exit(LPM3_bits);
  }
  _EINT();                                //Enable interrupts
}