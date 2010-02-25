#include "UGPS.h"
void main2(void);
volatile char c = 0x55;
void main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  unsigned long i = 0;
  P1DIR = 0x01;
  P1OUT = 1;
  ADCSetup();
  TimerA_Setup();
  for(int x = 0; x < 1024; x++)
  {
    if(x < 128)
      pageData[x] = 0;
    if(x == 33 || x == 101 || x == 1000)
      insertBit(TRUE, x);
    else insertBit(FALSE, x); 
  }
  write_Flash();
  //main2();
  while(1){
    __bis_SR_register(LPM3_bits + GIE);       // Enter LPM3, Enable interrupts
    _DINT();
    i=0;
    while(i != 0xfffff){i++;}
    _EINT();
  }
}

char value;                                 // 8-bit value to write to seg C

// Function prototypes
void write_SegC(char value);
void copy_C2D(void);

void main2(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
  value = 0x55;                                // initialize value

  while(1)
  {
    write_SegC(value++);                    // Write segment C, increment value
//    copy_C2D();                             // Copy segment C to D
    __no_operation();                       // Loop forever, SET BREAKPOINT HERE
  }
}

//------------------------------------------------------------------------------
// Input = value, holds value to write to Seg C
//------------------------------------------------------------------------------
void write_SegC(char value)
{
  unsigned int i;
  char * Flash_ptr;                         // Initialize Flash pointer
  Flash_ptr = (char *) 0x1880;
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash seg
  FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation

  for (i = 0; i < 128; i++)
  {
    *Flash_ptr++ = value;                   // Write value to flash
  }
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
}

//------------------------------------------------------------------------------
// Copy Seg C to Seg D
//------------------------------------------------------------------------------
void copy_C2D(void)
{
  unsigned int i;
  char *Flash_ptrC;
  char *Flash_ptrD;

  Flash_ptrC = (char *) 0x1880;             // Initialize Flash segment C ptr
  Flash_ptrD = (char *) 0x1800;             // Initialize Flash segment D ptr

  __disable_interrupt();                    // 5xx Workaround: Disable global
                                            // interrupt while erasing. Re-Enable
                                            // GIE if needed
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
  *Flash_ptrD = 0;                          // Dummy write to erase Flash seg D
  FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation

  for (i = 0; i < 128; i++)
  {
    *Flash_ptrD++ = *Flash_ptrC++;          // copy value segment C to seg D
  }

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
}