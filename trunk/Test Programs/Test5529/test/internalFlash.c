#include "UGPS.h"

//------------------------------------------------------------------------------
// Input = value[], holds value to writes to segment B
//------------------------------------------------------------------------------
void write_Flash()
{
  unsigned int i;
  char * Flash_ptr;                         // Initialize Flash pointer
  Flash_ptr = (char *) 0x1900;
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

//val is a TRUE/FALSE value
//position is 0 - 1023
void insertBit(char val, int position)
{
  int charIndex = position/8;
  int bitIndex = position%8;
  val <<= bitIndex;
  pageData[charIndex] |= val;
}

char PageIsBad(int pageNumber)
{
  int charIndex = pageNumber/8;
  int bitIndex = pageNumber%8;
  return pageData[charIndex] & (1<<bitIndex);
}