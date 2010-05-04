// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
//----------------------------------------------------------------------------+
// LED Control Demo
// This USB demo example is to be used with a PC application (e.g. HyperTerminal)
// This demo application is used to control the operation of the LED at P1.0.
// Typing the following pharses in the HyperTerminal Window does the following
// 1. "LED ON" Turns on the LED and returns "LED is ON" phrase to PC
// 2. "LED OFF" Turns off the LED and returns "LED is OFF" back to HyperTerminal
// 3. "LED TOGGLE - SLOW" Turns on the timer used to toggle LED with a large 
//     period and returns "LED is toggling slowly" phrase back to HyperTerminal
// 4. "LED TOGGLE - FAST" Turns on the timer used to toggle LED with a smaller
//     period and returns "LED is toggling fast" phrase back to HyperTerminal
//
// Please refer to "USB CDC API Demo Examples Guide.pdf" for more details
//----------------------------------------------------------------------------+

/*----------------------------------------------------------------------------+
| Main Routine                                                                |
+----------------------------------------------------------------------------*/
#include "../Main/UGPS.h"

char p[2048];
unsigned char MST_Data,SLV_Data;
volatile unsigned char temp;
unsigned int currentblock;
unsigned char spare;
void SetVcoreUp (unsigned int level);
char pageread (unsigned int row);
void takesample(void);
char blockerase(unsigned int row);
char progexe(unsigned int page);
void erasewholeflash(void);
void unlock(void);
void alignread(void);
void initclk(void){
  // when cpu wakes up from lpm init spi ports and clock
  // code to init clock 
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
// Increase Vcore setting to level3 to support fsystem=25MHz
  // NOTE: Change core voltage one level at a time..
  SetVcoreUp (0x01);
  SetVcoreUp (0x02);  
  SetVcoreUp (0x03);  
  
  UCSCTL3 = SELREF_2;                       // Set DCO FLL reference = REFO
  UCSCTL4 |= SELA_2;                        // Set ACLK = REFO

  __bis_SR_register(SCG0);                  // Disable the FLL control loop
  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
  UCSCTL1 = DCORSEL_7;                      // Select DCO range 50MHz operation
  UCSCTL2 = FLLD_1 + 762;                   // Set DCO Multiplier for 25MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (762 + 1) * 32768 = 25MHz
                                            // Set FLL Div = fDCOCLK/2
  __bic_SR_register(SCG0);                  // Enable the FLL control loop

  // Worst-case settling time for the DCO when the DCO range bits have been
  // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
  // UG for optimization.
  // 32 x 32 x 25 MHz / 32,768 Hz ~ 780k MCLK cycles for DCO to settle
  __delay_cycles(782000);
}

// setup spi 
void initspi(){
  P1DIR |= BIT1;                            // Set P1.0-2 to output direction
  P3SEL |= BIT3+BIT4;                       // P3.3,4 option select
  P2SEL |= BIT7;                            // P2.7 option select
  
  UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  //UCA0CTL0 = 0x29;
  UCA0CTL0 |= UCMST+UCSYNC+UCMSB+UCCKPL+UCCKPH;    // 3-pin, 8-bit SPI master if dosnt work try pl&ph=11
                                            // Clock polarity high, MSB
  
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 0x01;                           // 
  UCA0BR1 = 0;                              //
  UCA0MCTL = 0;                             // No modulation
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  P3DIR &= ~BIT5;                           // 3.5 = input
  P3SEL &= ~BIT5;                           // 3.5 = i/o  
 // UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

// reset command 
// reset time is 5us for read, 10us for prog,
// and 500us for erase this delay should be
// added after method returns
void resetflash(){
  P1OUT &= ~0x02;                           // CS = 0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0xff;                         // program load
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  P1OUT |= 0x02;                            // cs = 1
 // __delay_cycles(25000);                    // wait for reset
                                            // 5us=125 rd
                                            // 10us=250 pgm
                                            // 500us=12500 ers
                                            // 1ms=25000 pwr on
}

// allows the flash to be written
void unlock(void){
  P1OUT &= ~0x02;                           // CS = 0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x1F;                         // set features
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0xA0;                         // feature address
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x00;                         // unlock all bits
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  P1OUT |= 0x02;                            // cs = 1
}

// gets a byte from flash memory
// to be used when flash is already outputing data
//set cs high and restore clk after using
char getbyte(){
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x00;                         // dummy byte
  while (!(UCA0IFG&UCTXIFG));               // wait for recive
  //__delay_cycles(3);                        // wait
  return UCA0RXBUF;                         // return recived byte
  }

// toggles clk once to allign data
void alignread(void){
  P2DIR |= BIT7;                            // set clk to output
  P2SEL &= ~BIT7;                           // set clk to io
  P2OUT &= ~BIT7;                         // toggle clk
  P2SEL |= BIT7; //restoreclk
}

// read status reads the status register 
// after calling readstatus you can call
// getbyte() to keep reading the register
// remember to set cs to 1 after 
char readstatus(){
  char status;
  P1OUT &= ~0x02;                           // cs = 0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x0F;                         // get feature
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0xC0;                         // status register address
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  alignread();
  status = getbyte();                       // read status register
  //P1OUT |= 0x02;        // CS = 1 disabled to allow mult calls w/o interruption
  return status;                            // opperation finished
}

// read a page from flash page is stored in char[] page
char pageread(unsigned int row){
  int i=0;
  char status;
  P1OUT &= ~0x02;                           // CS = 0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x13;                         // page read
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = 0x00;                         // dummy
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = (row & 0xFF00)>>8;            // row addr hi
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = (row & 0x00FF);               // row addr low
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  __delay_cycles(3);                        // wait tcs 100ns=3cycles 100us=2500??                                          
  P1OUT |= 0x02;                            // CS = 1
  __delay_cycles(3);                        // wait tcs 100ns=3cycles
  readstatus();
  while (getbyte() & BIT0);                 // poll the opperation in
                                            // progress flag until done
  status = getbyte();                       // save status reg contents
  
  P1OUT |= 0x02;                            // CS = 1
  __delay_cycles(3);                        // wait tcs 100ns=3cycles
                                            // do we need to reset and wait trst rd 5us here?
  if(status&BIT5)                           // if data is bad
    return status;                          // return 
  P1OUT &= ~0x02;                           // CS = 0
  while (!(UCA0IFG&UCTXIFG));               // wait for buffer to be ready
  UCA0TXBUF = 0x03;                         // read data try 0b if dosn't work
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = 0x00;                         // dummy
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = 0x00;                         // col addr hi
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = 0x00;                         // col addr low
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  alignread();
  for (i=0;i<2048;i++)
  page[i] = getbyte();                         // fill page with data 
                                            // from flash  
  spare = getbyte();
  P1OUT |= 0x02;                            // CS = 1
  return status;
}

// preforms write enable and prog load cs is left low
// all spi commands will recorded until cs is set high
void writeen(void){
  P1OUT &= ~0x02;                           // CS = 0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x06;                         // Write Enable
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  P1OUT |= 0x02;                            // CS = 1
  // uncomment this after flashes are setup
  __delay_cycles(3);                        // wait tcs 100ns=3cycles
  P1OUT &= ~0x02;                           // CS = 0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x02;                         // program load
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x00;                         // column address1
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x00;                         // column address0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
}

// erase a block
char blockerase(unsigned int row){
  char status;
  P1OUT &= ~0x02;                           // CS = 0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x06;                         // Write Enable
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  P1OUT |= 0x02;                            // CS = 1
  __delay_cycles(3);                        // wait tcs 100ns=3cycles
  P1OUT &= ~0x02;                           // cs = 0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0xD8;                         // block erase
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x00;                         // dummy
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = (row & 0xFF00)>>8;            // row addr hi
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = (row & 0x00FF);               // row addr low
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  P1OUT |= 0x02;                            // cs=1 
  readstatus();
  while(getbyte() & BIT0);                  // poll status reg til done 
  status = getbyte();
  P1OUT |= 0x02;                            // CS = 1
  return status;
}

// program execute sends the flash the execute command and waits for the
// data to be flashed.
char progexe(unsigned int page){
  char status;
  P1OUT &= ~0x02;                           // cs = 0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x10;                         // program execute
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x00;                         // dummy bits
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = (page&0xFF00)>>8;             // row address1
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = (page&0x00FF);                // row address0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  P1OUT |= 0x02;                            // CS = 1 
  
  readstatus();
  while (getbyte() & BIT0);                 // wait for oip flag to go low
                                            // poll status reg til done 
  status = getbyte();
  P1OUT |= 0x02;                            // CS = 1
  return status;
}

void erasewholeflash(void){
  int i,j;
  for (i=0;i<1024;i++){
    j=blockerase(i);//erase block
    if (j&BIT2);//if erase fail
      //update bad blocks list
  }
}

// main is used to test all methods
void main(void)
{
  unsigned int i,j;
  char mid, did;
  char status;
  unsigned int result,ecc0,ecc1,error;
  P1OUT |= 0x02;                            //cs=1
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  initclk();
  initspi();
 for(i=50;i>0;i--); 
  
  /*        status reg        meaning
    bit5        ecc1          00= no errors,  01= single bit error detected and corrected
    bit4        ecc0          10= multiple errors not corrected.  set on read cleared on reset, read
    bit3        p_fail        1= program fail or locked reigon. cleared on block erase or reset
    bit2        e_fail        1= erase fail (or) locked region. cleared on block erase or reset
    bit1        WEL           1 = ready to program execute or block erase
    bit0        OIP           1=opperation in prog
  */
 /*
    resetflash();                              // initial reset
    __delay_cycles(25000);                     // power on reset 1ms
    status = readstatus(); 
    P1OUT |= BIT1;                             // cs = 1
    __delay_cycles(125);                       // rd reset 5us
    UCA0TXBUF = status;//for debug  
 */   
    ecc0 = ecc1 = 0;
    result = 0;
    error=0;
     P1DIR |= BIT0;//enled
     P1SEL &= ~BIT0;
    P1OUT &= ~0x01;//ledoff 
    
    //read device ID
    while(!((mid==0x2C)&&(did==0x12))){
    P1OUT |= 0x01;//ledon  
    resetflash();
    __delay_cycles(25000);                  // power on reset 1ms ?
    P1OUT &= ~BIT1;//cs=0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x9F;                         // get feature
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x00;                         // status register address
  alignread();

  mid = getbyte();                          // get manufacture id
  did = getbyte();                          // get device id
  P1OUT |= BIT1;                            // cs=1
}
// test read

for(i=0;i<20;i++){
      resetflash();                              // initial reset
      __delay_cycles(125);                       // rd reset 5us?
      status = pageread(i);//read page
      if(status&BIT5)// check ecc
        result++;
      for(j=0;j<2048;j++)
        if(page[j] != 0xFF)//check data
          error++;//should end up 0   
    }
    UCA0TXBUF = result;//for debug
    UCA0TXBUF = error;//for debug
    
    // to test bad block code
    for(i=0;i<1024;i++){
      resetflash();
      __delay_cycles(125);                       // rd reset 5us?
      pageread(i<<6); //read spare area in block
      result += (spare == 0x00)? 1:0;// 1 bit result from the command
      status = readstatus();
      P1OUT |= BIT1;                           // cs = 1

      ecc0 += !(!(status & BIT4));
      ecc1 += !(!(status & BIT5));
    }
UCA0TXBUF = result;//for debug
UCA0TXBUF = ecc0;//for debug
UCA0TXBUF = ecc1;//for debug
    
//test write and erase

     resetflash();
     __delay_cycles(25000);//power on reset
      unlock();
      __delay_cycles(25000);//power on reset
     writeen();
      for(j=0;j<100;j++){
        while (!(UCA0IFG&UCTXIFG));
        UCA0TXBUF = (j&0xff);//fill buffer with data
      }
      while (!(UCA0IFG&UCTXIFG));
       P1OUT |= 0x02;  
        //P2SEL |= BIT7;//restore clk// CS = 1     
      status = progexe(500);//program page
     UCA0TXBUF = status;
     
     status = pageread(500);//read page
     __delay_cycles(25000);//power on reset
      for(j=0;j<100;j++)
        if(page[j] != (j&0xFF))//check data
          error++;
    
    UCA0TXBUF = error;//for debug

     resetflash();
     __delay_cycles(25000);//power on reset
    
     status = blockerase(500);
     UCA0TXBUF = status; //fordebug
     resetflash();                              // initial reset
      __delay_cycles(12500);                       // rd reset 5us?
      status = pageread(500);//read page
      if(status&BIT5)// check ecc
        result++;
      for(j=0;j<2048;j++)
        if(page[j] != 0xFF)//check data
          error++;//should end up 0   
    
    UCA0TXBUF = result;//for debug
    UCA0TXBUF = error;//for debug
 
  __bis_SR_register(LPM0_bits + GIE);       // CPU off, enable interrupts
}

void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;              
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}

//thinking about changing to takesample (unsigned int ms)
// un-optimized cpu active whole time we want to delay for max time
// to give enough time for page to fill. end of page will be 
// followed by 00FFFFFFFFF....
// reset times:
// 5us=125 rd
// 10us=250 pgm
// 500us=12500 ers
// 1ms=25000 pwr on
void takesample (void){
  int i;
  int pagenum=0;//change to FindNextPage();
  P1OUT &= ~BIT4;//mux1 = mcu
  P1OUT &= ~BIT5;//mux2 = mcu
  P1OUT |= BIT4;// hold1 = 1
  P1OUT |= BIT4;// hold2 = 1
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  initclk();
  initspi();
  resetflash();
  __delay_cycles(25000);//power on reset
  unlock();
  resetflash();
  writeen();
  
  // ********** write RTC time to flash1 here ***************
  
  // this is to make shure we start recording at the begining of a word
  while(P2IN&BIT0);//wait for sync to go low
  while(!(P2IN&BIT0));//wait for sync to go high
  P1OUT |= BIT4;//mux1 = sige
  __delay_cycles(49902-25);//record first page
  //   ***************  record 15 sec    ******************
  // pages to program = 16.368mhz / 2 * numofsec / pagesize(2042*8)
  for(i=0;i<7515;i++){   //i=number of pages to program
   
    // need to make shure same bit is not split between 2 pages max25 cycles?
    while(P2IN&BIT0);//wait for sync to go low
    while(!(P2IN&BIT0));//wait for sync to go high
    P1OUT ^= 0x30;     //mux1 ^ mcu mux2 ^ sige   
    //add stop byte 0x00 end of page marker
    while (!(UCA0IFG&UCTXIFG));               // wait for txbuf to be ready
    UCA0TXBUF = 0x00;                         // stop byte
    while (!(UCA0IFG&UCTXIFG));               // wait for stop to be sent
    P1OUT |= 0x02;                            // CS = 1
    progexe(pagenum+i);// change to FindNextPage() //program flash
                                              // takes 400us-typ 900us max  
    resetflash();
    __delay_cycles(250);                      // wait for pgm reset
    writeen();                                // get flash ready for next record
    //16.368/2mhzsigclk: max delay 1.996ms 49902.2 cycles=2042bytes
    //program 2042 bytes onto flash
    __delay_cycles(49902);//may have to lower this syncronization issues
                          //add time to loop
  }
  // done
}