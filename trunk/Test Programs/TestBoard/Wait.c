const unsigned int MAXDELAY = 0xffff;
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