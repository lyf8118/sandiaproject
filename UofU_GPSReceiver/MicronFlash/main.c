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

unsigned char MST_Data,SLV_Data;
unsigned char temp;
void SetVcoreUp (unsigned int level);
void usb (char status);
void pageread (unsigned int row);
void takesample(void);
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
 // __delay_cycles(782000);
  P1DIR |= BIT1;                            // P1.1 output

  P1DIR |= BIT0;                            // ACLK set out to pins
  //P1SEL |= BIT0;                            
}
void initspi(){
  P1DIR |= BIT1;                            // Set P1.0-2 to output direction
  P3SEL |= BIT3+BIT4;                       // P3.3,4 option select
  P2SEL |= BIT7;                            // P2.7 option select
  
  UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA0CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;    // 3-pin, 8-bit SPI master
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
void resetflash(){
  P1OUT &= ~0x02;                           // CS = 0
  UCA0TXBUF = 0xff;                         // program load
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  P1OUT |= 0x02;                            // cs = 1
  __delay_cycles(24000);                    // wait for reset
}
char getbyte(){
  char byte = 0;
  int i;
  P2OUT &= ~BIT7;                           // clk = 0
  P2DIR |= BIT7;                            // set clk to output
  P2SEL &= ~BIT7;                           // set clk to io
  for(i=7;i>=0;i--){
    byte += !(!(P3IN & BIT5)) << i;       // get input
    P2OUT |= BIT7;
    P2OUT &= ~BIT7;                         // toggle clk
  }
  P2SEL |= BIT7;                            // P2.7 restore clk
  return byte;
  }
// read status reads the status register 
// after calling readstatus you can call
// getbyte() to keep reading the register
// remember to set cs to 1 after 
char readstatus(){
  char status;
  P1OUT &= ~0x02;                           // cs = 0
  UCA0TXBUF = 0x0F;                         // get feature
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0xC0;                         // status register address
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  status = getbyte();                       // read status register
  //P1OUT |= 0x02;                            // CS = 1
  return status;                            // opperation finished
}

void pageread(unsigned int row){
  char status;
  int i;
  P1OUT &= ~0x02;                           // CS = 0
  UCA0TXBUF = 0x13;                         // page read
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = 0x00;                         // dummy
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = (row & 0xFF00);               // row addr hi
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = (row & 0x00FF);               // row addr low
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  __delay_cycles(3);                        // wait 100ns
  P1OUT |= 0x02;                            // CS = 1
  status = readstatus();
  while (status & BIT0)                     // poll the opperation in
    status = getbyte();                     // progress flag until done
  P1OUT |= 0x02;                            // CS = 1
  P1OUT &= ~0x02;                           // CS = 0
  UCA0TXBUF = 0x03;                         // read data
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = 0x00;                         // dummy
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = 0x00;                         // col addr hi
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  UCA0TXBUF = 0x00;                         // col addr low
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  for (i=0;i<2048;i++)
  page[i] = getbyte();                      // fill page with data 
                                            // from flash  
  P1OUT |= 0x02;                            // CS = 1
}

void writeen(void){
  P1OUT &= ~0x02;                           // CS = 0
  UCA0TXBUF = 0x06;                         // Write Enable
  while (!(UCA0IFG&UCTXIFG));               // wait for data to be sent
  P1OUT |= 0x02;                            // CS = 1
  /* uncomment this after flashes are setup
  P1OUT &= ~0x02;                           // CS = 0
  UCA0TXBUF = 0x02;                         // program load
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x00;                         // column address1
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x02;??                         // column address0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  */
}
// program execute sends the flash the execute command and waits for the
// data to be flashed.
char progexe(void){
  unsigned int i;
  
  P1OUT &= ~0x02;                           // cs = 0
  UCA0TXBUF = 0x10;                         // program execute
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x00;                         // dummy bits
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x00;                         // row address1
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0x20;                         // row address0
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  P1OUT |= 0x02;                            // CS = 1 
  
  for(i = 5; i>0; i--);                     // wait
  
  P1OUT &= ~0x02;                           // cs = 0
  UCA0TXBUF = 0x0F;                         // get feature
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  UCA0TXBUF = 0xC0;                         // status register address
  while (!(UCA0IFG&UCTXIFG));               // USCI_A0 TX buffer ready?
  do{
    UCA0TXBUF = 0x00;                         // get status
    while (!(UCA0IFG&UCTXIFG));               
    }
  while (!(UCA0RXBUF & 0x01));              // check oip flag
  P1OUT |= 0x02;                            // CS = 1
                                            // opperation finished
  return (!(!(UCA0RXBUF & 0x08)));             // return 1 if program sucessfull
}

void main(void)
{
  volatile unsigned int i;
  char status;
  P1OUT |= 0x02;
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  initclk();
  initspi();

  for(i=50;i>0;i--);                        // Wait for slave to initialize

  MST_Data = 0x01;                          // Initialize data values
  SLV_Data = 0x00;                          //
  while(1){
    resetflash();
    //usb(readstatus());//1
    //P1OUT |= 0x02; //1                           // CS = 1
    writeen();//2                              // initilize flash
    usb(readstatus());//2
    pageread(500);//3 
    usb(page[0]);//3
   
    for(i=50;i>0;i--){
      while (!(UCA0IFG&UCTXIFG));               // send simulated data 
      UCA0TXBUF = i;
    }
    status = readstatus();
    usb(status);
  }//erase                                       // here we will switch the mux to
  while(1){//erase                                  // capture data from reciver 
    for(i=50;i>0;i--){
      while (!(UCA0IFG&UCTXIFG));               // send simulated data 
      UCA0TXBUF = i;
    }
    while (!(UCA0IFG&UCTXIFG));               // wait for last bit to be sent
    P1OUT |= 0x02;                            // CS = 1
  
  if(progexe())                          // program execute
    P1OUT |= 0x01;                           // light led if program was sucessfull
  }
  __bis_SR_register(LPM0_bits + GIE);       // CPU off, enable interrupts
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
  volatile unsigned int i;

  switch(__even_in_range(UCA0IV,4))
  {
    case 0: break;                          // Vector 0 - no interrupt
    case 2:                                 // Vector 2 - RXIFG
      //while (!(UCA0IFG&UCTXIFG));           // USCI_A0 TX buffer ready?

      if (!(UCA0RXBUF & 0x01))                 // Test for done character RX'd
        if(!(UCA0RXBUF & 0x08))
          P1OUT |= 0x01;                      // If correct, light LED
        else
          P1OUT &= ~0x01;                     // If incorrect, clear LED
      
      //MST_Data++;                           // Increment data
      //SLV_Data++;
      //UCA0TXBUF = MST_Data;                 // Send next value

      //for(i = 20; i>0; i--);                // Add time between transmissions to
                                            // make sure slave can process information
      break;
    case 4: break;                          // Vector 4 - TXIFG
    default: break;
  }
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

VOID usb(char status)
{
    WDTCTL = WDTPW + WDTHOLD;	    // Stop watchdog timer
    Init_Ports();                   // Init ports
    Init_Clocks();
    Init_TimerA1();
 
    USB_init();
    
    // Enable various USB event handling routines
    USB_setEnabledEvents(kUSB_VbusOnEvent+kUSB_VbusOffEvent+kUSB_receiveCompletedEvent
                          +kUSB_dataReceivedEvent+kUSB_UsbSuspendEvent+kUSB_UsbResumeEvent);
    
    // See if we're already attached physically to USB, and if so, connect to it
    // Normally applications don't invoke the event handlers, but this is an exception.  
    if (USB_connectionInfo() & kUSB_vbusPresent)
      USB_handleVbusOnEvent();
    
    while(1)
    {
        BYTE i;
        // Check the USB state and directly main loop accordingly
        switch(USB_connectionState())
        {
           case ST_USB_DISCONNECTED:
                 __bis_SR_register(LPM3_bits + GIE); 	             // Enter LPM3 w/ interrupts enabled
                _NOP();                                              // For Debugger
                break;
                
           case ST_USB_CONNECTED_NO_ENUM:
                break;
                
           case ST_ENUM_ACTIVE:
                __bis_SR_register(LPM0_bits + GIE); 	             // Enter LPM0 (can't do LPM3 when active)
                _NOP();                                              // For Debugger
                
                // Exit LPM on USB receive and perform a receive operation 
                if(bDataReceived_event)                              // Some data is in the buffer; begin receiving a command              
                {
                  char pieceOfString[MAX_STR_LENGTH] = "";           // Holds the new addition to the string
                  char outString[MAX_STR_LENGTH] = "";               // Holds the outgoing string
                  
                  // Add bytes in USB buffer to theCommand
                  receiveDataInBuffer((BYTE*)pieceOfString,MAX_STR_LENGTH,1);                 // Get the next piece of the string
                  strcat(wholeString,pieceOfString);                                          // Add it to the whole
                  sendData_inBackground((BYTE*)pieceOfString,strlen(pieceOfString),1,0);      // Echoes back the characters received (needed for Hyperterm)
                  
                  if(retInString(wholeString))                                                        // Has the user pressed return yet?
                  {
                    if(!(strcmp(wholeString, "LED ON")))                                              // Compare to string #1, and respond
                    {
                      TA1CTL &= ~MC_1;                                                                // Turn off Timer
                      P1OUT |= BIT0;                                                                  // Turn on LED P1.0
                      strcpy(outString,"\r\nLED is ON\r\n\r\n");                                      // Prepare the outgoing string
                      sendData_inBackground((BYTE*)outString,strlen(outString),1,0);                  // Send the response over USB
                    }
                    else if(!(strcmp(wholeString, "LED OFF")))                                        // Compare to string #2, and respond
                    {
                      TA1CTL &= ~MC_1;                                                                // Turn off Timer
                      P1OUT &= ~BIT0;                                                                 // Turn off LED P1.0
                      strcpy(outString,"\r\nLED is OFF\r\n\r\n");                                     // Prepare the outgoing string
                      sendData_inBackground((BYTE*)outString,strlen(outString),1,0);                  // Send the response over USB
                    }
                    else if(!(strcmp(wholeString, "LED TOGGLE - SLOW")))                              // Compare to string #3, and respond
                    {                      
                      TA1CTL &= ~MC_1;                                                                // Turn off Timer                      
                      TA1CCR0 = SlowToggle_Period;                                                    // Set Timer Period for slow LED toggle
                      TA1CTL |= MC_1;                                                                 // Start Timer
                      strcpy(outString,"\r\nLED is toggling slowly\r\n\r\n");                         // Prepare the outgoing string
                      sendData_inBackground((BYTE*)outString,strlen(outString),1,0);                  // Send the response over USB
                    }
                    else if(!(strcmp(wholeString, "LED TOGGLE - FAST")))                              // Compare to string #4, and respond
                    {
                      TA1CTL &= ~MC_1;                                                                // Turn off Timer                     
                      TA1CCR0 = FastToggle_Period;                                                    // Set Timer Period for fast LED toggle
                      TA1CTL |= MC_1;
                      strcpy(outString,"\r\nLED is toggling fast\r\n\r\n");                           // Prepare the outgoing string
                      sendData_inBackground((BYTE*)outString,strlen(outString),1,0);                  // Send the response over USB
                    } 
                    
                    else if(!(strcmp(wholeString, "READSTATUS")))                              // Compare to string #4, and respond
                    { 
                      char st[2] = "";
                      st[0] = ((!(!(status & BIT3)))+48);
                      strcpy(outString,"\r\nReading Status: \r\n");
                      strcat(outString,"\r\nProgram Fail: ");                           // Prepare the outgoing string 
                      strcat(outString, st);
                      st[0] = ((!(!(status & BIT2)))+48);
                      strcat(outString,"\r\nERASE Fail: "); 
                      strcat(outString, st);
                      st[0] = ((!(!(status & BIT0)))+48);
                      strcat(outString,"\r\nO.I.P.: ");
                      strcat(outString, st);
                      st[0] = ((!(!(status & BIT5)))+48);
                      strcat(outString,"\r\nECC Fail: "); 
                      strcat(outString, st);
                      st[0] = ((!(!(status & BIT1)))+48);
                      strcat(outString,"\r\nWrite Enable: "); 
                      strcat(outString, st);
                      strcat(outString, "\r\n");
                      sendData_inBackground((BYTE*)outString,strlen(outString),1,0);                  // Send the response over USB
                     
                    }                    
                    else                                                                               // Handle other
                    {
                      strcpy(outString,"\r\nNo such command!\r\n\r\n");                                // Prepare the outgoing string
                      sendData_inBackground((BYTE*)outString,strlen(outString),1,0);                   // Send the response over USB
                    }                    
                    for(i=0;i<MAX_STR_LENGTH;i++)                                                       // Clear the string in preparation for the next one  
                      wholeString[i] = 0x00;
                  }
                  bDataReceived_event = FALSE;
                }
                break;
                
           case ST_ENUM_SUSPENDED:
                __bis_SR_register(LPM3_bits + GIE); 	        // Enter LPM3 w/ interrupts
                break;
                
           case ST_ENUM_IN_PROGRESS:
                break;
                
           case ST_ERROR:
                _NOP();
                break;
                
           default:;
        } 
    
    }  // while(1) 
} //main()


//----------------------------------------------------------------------------


/*----------------------------------------------------------------------------+
| System Initialization Routines                                              |
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| System Initialization Routines                                              |
+----------------------------------------------------------------------------*/
VOID Init_TimerA1(VOID)
{
  TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
  TA1CTL = TASSEL_1 + TACLR;                // ACLK, clear TAR
}
  

VOID Init_Clocks(VOID)
{
    if (USB_PLL_XT == 2)
    {
        P5SEL |= 0x0C;                                      // Enable the XT2 pins.  Without this, the xtal pins default to being I/O's.  
        
        // Use the REFO oscillator to source the FLL and ACLK
        UCSCTL3 = (UCSCTL3 & ~(SELREF_7)) | (SELREF__REFOCLK);
        UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK);
        
        // MCLK will be driven by the FLL (not by XT2), referenced to the REFO
        Init_FLL(USB_MCLK_FREQ/1000, USB_MCLK_FREQ/32768);  // Start the FLL, at the freq indicated by the config constant USB_MCLK_FREQ
        
        XT2_Start();                                        // Start the "USB crystal"
    }
    else
    {
        P5SEL |= 0x10;                                      // Enable the XT1 pins.  Without this, the xtal pins default to being I/O's.  
        
        // Use the REFO oscillator to source the FLL and ACLK
        UCSCTL3 = SELREF__REFOCLK;             
        UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__REFOCLK); 
        
        // MCLK will be driven by the FLL (not by XT2), referenced to the REFO
        Init_FLL(USB_MCLK_FREQ/1000, USB_MCLK_FREQ/32768); // set FLL (DCOCLK)
        
        XT1_Start();                                       // Start the "USB crystal"
    }
}

//----------------------------------------------------------------------------

VOID Init_Ports(VOID)
{
    // Drive all I/O's as output-low, making sure there's no shoot-through current.  There 
    // should be no floating I/Os, to prevent unnecessary current draw during USB suspend.  
    PAOUT   =   0x0000;
    PASEL   =   0x0000;
    PADIR   =   0xFFFF;

    PBOUT   =   0x0000;
    PBSEL   =   0x0000;
    PBDIR   =   0xFFFF;

    PCOUT   =   0x0000;
    PCSEL   =   0x0000;
    PCDIR   =   0xFFFF;

    PDOUT   =   0x0000;
    PDSEL   =   0x0000;
    PDDIR   =   0xFFFF;

    PJDIR   =   0xFFFF;
    PJOUT   =   0x0000;
   
}

// To be robust, an application should handle fault events that invoke the NMI vector.  
#pragma vector = UNMI_VECTOR
__interrupt VOID UNMI_ISR(VOID)
{
    switch (__even_in_range(SYSUNIV, SYSUNIV_SYSBUSIV))
    {
    case SYSUNIV_NONE:
      __no_operation();
      break;
    case SYSUNIV_NMIIFG:
      __no_operation();
      break;
    case SYSUNIV_OFIFG:
      UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT1HFOFFG+XT2OFFG); // Clear OSC fault source flags 
      SFRIFG1 &= ~OFIFG;                                // Clear OFIFG flag
      break;
    case SYSUNIV_ACCVIFG:
      __no_operation();
      break;
    case SYSUNIV_SYSBUSIV:
      
      // In the rare event of an internal system bus error - must clear the flag and re-initialize USB. 
      SYSBERRIV = 0;            // Clear flag
      USB_disable();            // Disable USB
      if (USB_connectionInfo() & kUSB_vbusPresent)
        USB_handleVbusOnEvent();
    }
}


// This function returns true if there's an 0x0D character in the string; and if so, 
// it trims the 0x0D and anything that had followed it.  
BYTE retInString(char* string)
{
  BYTE retPos=0,i,len;
  char tempStr[MAX_STR_LENGTH] = "";
  
  strncpy(tempStr,string,strlen(string));     // Make a copy of the string
  len = strlen(tempStr);    
  while((tempStr[retPos] != 0x0D) && (retPos++ < len)); // Find 0x0D; if not found, retPos ends up at len
  
  if(retPos<len)                              // If 0x0D was actually found...
  {
    for(i=0;i<MAX_STR_LENGTH;i++)             // Empty the buffer
      string[i] = 0x00;
    strncpy(string,tempStr,retPos);           // ...trim the input string to just before 0x0D
    return TRUE;                              // ...and tell the calling function that we did so
  }
  
  return FALSE;                               // Otherwise, it wasn't found
}

//This interrupt is already defined elsewhere
/*
// Timer1 A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
  P1OUT ^= BIT0;                            // Toggle LED P1.0
}
*/

//thinking about changing to takesample (unsigned int ms)
// un-optimized cpu active whole time
void takesample (void){
  unsigned int count = 0;
  int i;
  P1OUT &= ~BIT4;//mux1 = mcu
  P1OUT &= ~BIT5;//mux2 = mcu
  P1OUT |= BIT4;// hold1 = 1
  P1OUT |= BIT4;// hold2 = 1
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  initclk();
  initspi();
  reset flash();
  writeen();
  
  // ********** write RTC time to flash1 here ***************
  
  // need to test this but i dont know how
  // this is to make shure we start recording at the begining of a word
  if(P2IN&BIT0)//if sync is high
    while(P2IN&BIT0);//wait for it to go low
  while(!(P2IN&BIT0));//wait for it to go high
  P1OUT |= BIT4;//mux1 = sige
  __delay_cycles(49902-25);//record first page
  //   ***************  record 15 sec    ******************
  // pages to program = 16.368mhz / 2 * numofsec / pagesize(2042*8)
  for(i=0;i<7515;i++){   //i=number of pages to program
   
    // need to make shure same bit is not split between 2 pages max25 cycles?
    if(P2IN&BIT0)//if sync is high
      while(P2IN&BIT0);//wait for it to go low
    while(!(P2IN&BIT0));//wait for it to go high
    P1OUT ^= 0x30;     //mux1 ^ mcu mux2 ^ sige   
    //start timer
    //add stop byte 0x00 end of page marker
    UCA0TXBUF = 0x00;                         // stop byte
    while (!(UCA0IFG&UCTXIFG));               // wait for stop to be sent
    P1OUT |= 0x02;                            // CS = 1
    progexe(blocknum); //program flash
      
                     // takes 400us-typ 900us max  
    writeen();         // ?? cycles (profile this)
    // wait for data to be flashed
    //16.368/2mhzsigclk: max delay 1.996ms 49902.2 cycles=2042bytes
    
    //program 2042 bytes onto flash
    __delay_cycles(49902);//may have to lower this syncronization issues
                       //add time to loop
    // instead of delay wait on timer
  }
  // done
}