#include "UGPS.h"
// need to transfer flash methods to ugps to use.
void StoreFlashPageInfo()
{
  char* flashArray = makeArrayBlock(PAGEDATASTART, 128);
    unsigned int i = 0;
    char result;
    WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
    initclk();
    initspi();
    
    //********** record bad blocks for flash chip 1 ********//
    insertBit(1, 0, flashArray);// dont use block1 otp area
    for(i=1;i<1024;i++){
      resetflash();
      P1OUT &= ~BIT3;//hold2 = 0 flash2 on hold
      pageread((i<<6)+2048); //read spare area in block
      result = (page[0] == 0x00) ? 1 : 0;// 1 bit result from the command
      insertBit(result, i, flashArray);//1=bad
    }
    write_Flash(flashArray, 0);//store bad pages
    
    //********** record bad blocks for flash chip 2 ********//
    flashArray = makeArrayBlock(PAGEDATASTART, 128);    //getting new array
    P1OUT |= BIT3;// hold2 = 1 flash2 off hold
    P1OUT &= ~BIT2;//hold1
    insertBit(1, 0, flashArray);// dont use block1 otp area
    //loop for flash chip 2
    for(i=1;i<1024;i++){
      resetflash();
      //hold1 = 0 flash1 on hold
      pageread((i<<6)+2048); //read spare area in block
      result = (page[0] == 0x00) ? 1 : 0;// 1 bit result from the command
      insertBit(result, i, flashArray);
    }
    write_Flash(flashArray, 1);//store bad pages
}

//------------------------------------------------------------------------------
// Input = value[], holds value to writes to segment B
//------------------------------------------------------------------------------
void write_Flash(char* pageData, int flashID)
{
  unsigned int i;
  char * Flash_ptr;                         // Initialize Flash pointer
  Flash_ptr = (char *) 0x1800 + 0x0100 * flashID;
  FCTL3 = FWKEY;                            // Clear Lock bit
  FCTL1 = FWKEY+ERASE;                      // Set Erase bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash seg
  FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation

  for (i = 0; i < 128; i++)
  {
    *Flash_ptr++ = pageData[i];                   // Write value to flash
  }
  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY+LOCK;                       // Set LOCK bit
}

//checks first four bytes of flash blocks, if it equals 0xffffffff, then it hasn't been initialized
//if the first 32 pages of both blocks are bad (very low probability),
//then this will just have to get the info again and re-store it
char FlashInfoStored()
{
  return (*((unsigned long*)0x1800) == 0xffffffff) && (*((unsigned long*)0x1900) == 0xffffffff);
}

//val is a TRUE/FALSE value
//position is 0 - 1023
void insertBit(char val, int position, char* pageData)
{
  int charIndex = position/8;
  int bitIndex = position%8;
  val <<= bitIndex;
  pageData[charIndex] |= val;
}
//returns1 if page is bad 0 otherwise
char BadPage(int pageNumber, int flashID)
{
  char* pageData = (char*)0x1800 + 0x100 * flashID;
  int charIndex = pageNumber/8;
  int bitIndex = pageNumber%8;
  return ( (pageData[charIndex] & (1<<bitIndex)) > 0 ) ? 1 : 0;
}

// returns next good block #
int FindNextPage(int currentPage, int flashID)
{
  while(BadPage(currentPage + 1, flashID)) 
  {
    currentPage++;
    if(currentPage == 1023)     //reached end of pages, pageData[1023+1] is out of bounds
      return -1;
  }
  return currentPage + 1;
}