#include "UGPS.h"

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

//Averages values in an UINT array, eliminating max and min values of the array
unsigned int UINT_ArrayAverage(unsigned int volatile* Array, int length)
{
  unsigned long sum = 0;
 for(int i = 0; i < length; i++) 
   sum += Array[i];
 sum -= UINT_ArrayMax(Array, length) + UINT_ArrayMin(Array, length);
 return sum/(length-2);
}