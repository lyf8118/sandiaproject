
#include  <msp430x16x.h>

#define MAXDELAY 0xffff
void Wait(unsigned int, short);
void MemoryWrite(unsigned int,short);
void DisplayMemory(unsigned int);
void DisplayValue(unsigned int);

int main( void )
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  P1DIR |= 0x0f;                            // Set P1.0 - P1.3 to output direction
  P1OUT = P1OUT & 0xf0;                     //reset P1 LEDs to zero
  
  for (;;)
  {
    volatile unsigned int i;
    i = 0x0208;
    MemoryWrite(i, 0xabcd);
    MemoryWrite(i+4, 0x1234);
    DisplayMemory(i);
    //DisplayValue(0x0123);
    //DisplayValue(0x4567);
    //DisplayValue(0x89ab);
    //DisplayValue(0xcdef);
    P1OUT = P1OUT & 0xf0;                     //reset P1 LEDs to zero
  }
}

void MemoryWrite(unsigned int address, short value)
{
  int* piece;
  piece = (int*)address;
  *piece = value;
}

//displays 2 bytes of the value for the given address
void DisplayMemory(unsigned int address)
{
    unsigned int *i;
    i = (unsigned int*)address;
    DisplayValue(*i);
}

//displays 16 bit val in 4 bit increments starting with high bits
//with short delays so values can be detected
void DisplayValue(unsigned int val)
{
    char high4 = val >> 12;
    char highMiddle = val >> 8;
    char lowMiddle = val >> 4;
    char low4 = val & 0xf;
    P1OUT = high4 & 0xf;
    Wait(MAXDELAY, 2);
    P1OUT = highMiddle & 0xf;
    Wait(MAXDELAY, 2);
    P1OUT = lowMiddle & 0xf;
    Wait(MAXDELAY, 2);
    P1OUT = low4 & 0xf;
    Wait(MAXDELAY, 2);
}

//delay - how long you wish to delay
//loops - used for when you want to wait beyond the MAXDELAY value
//        use 1 for a default value
void Wait(unsigned int delay, short loops)               //a simple delay loop, for debugging purposes
{
 volatile unsigned int i = delay;                              // Delay
 short loopCount = 0;
 while(loopCount < loops){
   while (i != 0)
      i--;   
   loopCount++;
   i = delay;
 }
}