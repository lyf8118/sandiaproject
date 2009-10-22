/* 
 * Code developed for use with the SiGe USB module to accompany
 * the textbook: "A Software-Defined GPS and Galileo Receiver: A
 * Single-Frequency Approach" by Kai Borre, Dennis Akos, et.al. by:
 *
 * Marcus Junered, GNSS Research Group
 * Lulea University of Technology
 * Lulea, Sweden 
 * junered@ltu.se
 * ---------------------------------------------------------------------
 * 	Further Modifications by :
 *	Michael Steval
 *	SiGe Semiconductor
 *	Bishops Stortford, UK
 *	mis@sige.com
 *	Sept, 2006
 * ---------------------------------------------------------------------
 * 
 * GN3S - GNSS IF Streamer for Cypress FX2LP (GNSSS)
 * Copyright (C) 2006 Marcus Junered
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "gn3s_se4110.h"

// ***********Should set PA to input and read the port (PA3) to see if S/W programming is allowed. **************


char init_se4120(void) // 4120 inialisation ready for programming on EK3 
// bit A3 defines hardware (1) or software programming (0)
{

  // Port A	: Port A used to software / hardware program the 4120
  A0 = 0;	// 3W_LEN / F2
  A1 = 0;	// 3W_Data / F1
  A2 = 0;	// 3W_CLK  / F0 
  A3 = 1;	// HW_prog = high, (SW_prog = lo) initalise to hardware mode
  A4 = 0;	// n/c
  A5 = 0;	// n/c
  A6 = 0; 	// n/c
  A7 = 0;	// n/c

  OEA = 0xFF; 	// Set Port A[0,1,2,3] as outputs, A [4,5,6,7] as outputs also(unused, n/c), 
  
 /*  MIS - disable this for now - as it overwrites the FIFO initialisation
  // Port B	: Port B is used by NordNav to software program the 4120 (not on the fly)
  B0 = 0;	
  B1 = 0;
  B2 = 0;	// 3W_LEN  (hardwired to port A also)
  B3 = 0;	// 3W_Data  (hardwired to port A also)
  B4 = 0;	// 3W_CLK   (hardwired to port A also)
  B5 = 0;	
  B6 = 0; 	
  B7 = 0;

 // OEB = 0x1C; 	// Set Port B[0,1,5,6,7] as inputs, B[2,3,4] as outputs
  OEB = 0x00; 	// Initialise Port B[0..7] as inputs as port B[2,3,4] are hardwired as common with port A
  */
  
  
  // Port D
  D0 = 0; 	
  D1 = 0; 
  D2 = 0; 	
  D3 = 0; 	
  D4 = 0; 	
  D5 = 0; 
  D6 = 0; 	
  D7 = 0;   

  OED = 0xFF; 	// Set Port D as outputs
 
  return 0;
}


/*---------------------------------------------------------------
Configuration of 3_wire bus
	Bit 0 = loadEnable	
	Bit 1 = Data
	bit 2 = CLK
	bit 3 = H/W mode / S/W mode
------------------------------------------------------------------*/
 /*-------------------------------------------------------------------------------------------
Configuration of first  data Byte D0 		: default state
	bit 0 = FRPlan<0> 				: x	FsSEL0 : Used to change Hardware programability  ?
	Bit 1 = FRPlan<1>				: x	FsSEL1 : Used to change Hardware programability  ?
	Bit 2 = BW, 				: 0	0= GPS, 1 = Galileo 
	bit 3 = OPBuf<0>				: 1	Current drive 2.6mA
	bit 4 = OPBuf<1>				: 1	Current drive 2.6mA
	bit 5 = AGCCP_I_Hi			: 0	? no current control ?
	bit 6 = PLLCP_I_Hi			: 0	? SW controlled ?
	bit 7 = Testmode				: 0	Security on always zero
-----------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------
Configuration of 2nd  data Byte D1  
	bit 0 = CLK_Inv				: 0    	0 = data changes on -Ve edge of OCLK		
	Bit 1 = ADC_Ext_Ref			: 0     	0=TCX0=internal ADC sample clock
	Bit 2 = CLK_Div<0>			: 1      See table below. 
	bit 3 = CLK_Div<1>			: 0     See table below. 
	bit 4 = SerMode<0>			: 1	See table below. 
	bit 5 = SerMode<1>			: 1	See table below.  
	bit 6 = OutMode<0>			: 1	See table below.
	bit 7 = OutMode<1>			: 0	See table below.
------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------
Configuration of D2 .. D10  data Bytes
	D2			
	D3	
	D4
	D5
	D6
	D7	=  Test Mode only							: 0xxx	
	D8
	D9	=  B< 7> = 1 to allow Software ovewrite for OSC modes	: 0x10		
	D10
------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------------

Configuration of OutMode / SerMode / CLK_DIV
																H/W mode
Menu	OutMode	SerMode	CLKDiv	Msps		bits	      Mode	          OClk MHz	Hex	 Fs_sel<1:0>
-------	----------------------------------------------------------------------------------------------------------	-----	---------------
   1	   10	    	     10	             01		8.184	       2b I/Q	      Pulse Sync	16.368	a4	         	00
   2	   10	    	     10	             11		4.092       2b I/Q	      Pulse Sync	   8.184	ac		 -
   3	   11	    	     10	             11		4.092       4b I/Q	      Pulse Sync  	16.368	ec		11
  ----	  ----------------------------------------------------------------------------------------------------------	----- 
   4	   00	    	     11	             10		5.456       1b real	      Byte Sync   	   8.184 	38		 -
   5	   01	    	     11	             10		5.456       2b real	      Byte Sync   	 16.368 	78		 -
   6	   10	    	     11	             10		5.456       2b I/Q	      Byte Sync   	 16.368 	B8		01
   7	   10	    	     11	             11		4.092       2b I/Q	      Byte Sync   	 16.368 	BC		10
  ----	----------------------------------------------------------------------------------------------------------	----- 
  12	   00	    	     01	             10		5.456       1b real	      parallel  	   5.456 	 18	  
    8	   01	    	     01	             10		5.456       2b real	      parallel  	   5.456         58
    9	   00	    	     01	             00		16.368      1b real	      parallel  	 16.368 	 10	
  10	   01	    	     01	             00		16.368      2b real	      parallel  	 16.368 	 50
  ----     ----------------------------------------------------------------------------------------------------------	----- 
   14	   10	    	     01	             00		16.368      2b I/Q	      parallel  	 16.368 	 90
   15	   10	    	     01	             01		  8.184      2b I/Q	      parallel  	    8.184	 94	
   16	   10	    	     01	             10		  5.456      2b I/Q	      parallel  	   5.456 	 98
   17	   10	    	     01	             11		  4.092      2b I/Q	      parallel  	   4.092 	 9c
  ----	 ----------------------------------------------------------------------------------------------------------	-----  
  18	   11	    	     01	             00		16.368      4b I/Q	      parallel  	 16.368 	 d0
  19	   11	    	     01	             01		  8.184      4b I/Q	      parallel  	    8.184	 d4	
  20	   11	    	     01	             10		  5.456      4b I/Q	      parallel  	   5.456 	 d8
  21	   11	    	     01	             11		  4.092      4b I/Q	      parallel  	   4.092 	 dc
  ----	  ----------------------------------------------------------------------------------------------------------	 ----
  11	   00	    	     00	             xx		16.368      1b real	     Unsampled	16.368 	 08		// Block Convertor bypassed,Unsampled mode
  13	   01,10,11    	     00	             10		16.368      2b real	     Sampled	 	16.368 	 48		// Block Convertor bypassed, Sampled mode
 -----------------------------------------------------------------------------------------------------------------------------*/	 

// Use Once only on startup - leave in S/W mode
char soft_program_se4120(unsigned char Byte1, unsigned char Byte2) // 4120 Software  programming on EK3 
{
  OEA = 0xFF; 		// re-set Port A[0,1,2,3] as outputs, A [4,5,6,7] as outputs also(unused, n/c), 
  udelay(1); //1us	// clock is still at 12MHz not 48MHz 
  
  IOA = IOA & bm3W_HW_MASK; // turn off  bit 3 (HW mode) Leave  bits 0,1,2
  udelay(1); //1uS	// clock is still at 12MHz not 48MHz 

  IOA = IOA & bm3W_LEN_MASK;	// turn off  bit 0 (load Enable) Leave  bits 1,2,3
  udelay(1); //1us	// clock is still at 12MHz not 48MHz 
  
  clock_data_out(Byte1); // byte 1
  clock_data_out(Byte2); // byte 2
   
  udelay(1); //1uS	// clock is still at 12MHz not 48MHz 

  IOA = IOA | bm3W_LEN;	// turn on bit 0 load enable - latch data
  udelay(1); //1us	// clock is still at 12MHz not 48MHz 
  
  OEA = 0xf0 | bm3W_HW | bm3W_LEN; 	// Reset-Port A[0,3] as outputs, A[1,2]  as inputs, A [4,5,6,7] as outputs (unused, n/c), 
  
  return 0;
}

// Following routine just used to change mode on the fly for test purposes
// Does not re-initilise the H/W mode

char test_program_se4120(unsigned char Byte1, unsigned char Byte2) // 4120 Software  programming on EK3 
{
  
  OEA = 0xFF; 		// re-set Port A[0,1,2,3] as outputs, A [4,5,6,7] as outputs also(unused, n/c), 
  IOA = IOA & bm3W_HW_MASK; // turn off  bit 3 (HW mode) Leave  bits 0,1,2

 
 /*      // Toggle HW as a marker
  IOA = IOA | 0x08;	// turn on  bit 3 (HW mode) Leave  bits 0,1,2
  udelay(1); //1uS	// clock is still at 12MHz not 48MHz 
  IOA = IOA & 0xF7;	// turn off  bit 3 (HW mode) Leave  bits 0,1,2
  */    // Toggle HW as a marker

 
  IOA = IOA & bm3W_LEN_MASK;	// turn off  bit 0 (load Enable) Leave  bits 1,2,3

  clock_data_out(Byte1); // byte 1
  clock_data_out(Byte2); // byte 2
   
  IOA = IOA | bm3W_LEN;	// turn on bit 0 load enable - latch data

  OEA = 0xf0 | bm3W_HW | bm3W_LEN; 	// Reset-Port A[0,3] as outputs, A[1,2]  as inputs, A [4,5,6,7] as outputs (unused, n/c), 
 
  return 0;
}



/*---------------------------------------------------------------
Configuration of 3_wire bus on PortA
	Bit 0 = loadEnable
	Bit 1 = Data
	bit 2 = CLK
------------------------------------------------------------------*/
char clock_data_out(unsigned char Txbyte) // 4120 Hardware programming on EK3 
{
    unsigned short i = 0;
 	unsigned short Txmask = 0x80;		// MSB first per byte
//	unsigned short Txmask = 0x01;		// LSB first per byte
	
	unsigned short Transmit = 0;
	unsigned short tic = 1;		// tic = 1/2 of clock pulse width
	
	for (i = 0x0; i < 0x8; i++)
	{
		Transmit = Txbyte & Txmask;	// extract the bit to send
		Txmask >>= 1;				// shift mask for next Txbit (MSB ....LSB)
//		Txmask <<= 1;				// shift mask for next Txbit (LSB ....MSB)
		
		if (Transmit > 0)	// Tx one not zero
		{
			IOA = IOA | bm3W_DAT;	// assert data 1
		}
			
		udelay(tic);			// wait for 1 tic
		IOA = IOA | bm3W_CLK ;	// assert clock at midpoint
		udelay(tic);			// wait for 1 tic
		IOA = IOA & bm3W_CLK_MASK & bm3W_DAT_MASK;	// de-assert clock & data
	} 
 
	udelay(tic);		// wait for 1 tic udelay(1);  
 
  return 0;
}


// Program in HW mode, then leave in H/W mode
char hard_program_se4120(unsigned char bmFSEL) // 4120 Hardware programming on EK3 
{
 
  IOA = (IOA & bm3W_CFIG) | bm3W_HW;	// turn off  bits 0,1,2 Leave bit 3 (HW) on,
  
  // mdelay(1); // 1 ms   debug
 
  IOA = IOA | bmFSEL;	// turn on the required bits
       
 // mdelay(1); // 1 ms debug
 
  return 0;
}

/* was portB / leave in case debug required

char program_se4120(unsigned char bmFSEL) // 4120 programming on EK3 
{
  unsigned short i = 0;
  unsigned short bm = 0;

  IOB = IOB & 0x03;	// turn off  bits b2,b3,b4
  
  mdelay(1); // 1 ms   debug
 // udelay(1); //1uS	// clock is still at 12MHz not 48MHz 

  IOB = IOB | bmFSEL;	// turn on the required bits
      
  mdelay(1); // 1 ms debug
 // udelay(1); //1uS	// clock is still at 12MHz not 48MHz 
 
  return 0;
}
*/

char init_se4110(void) // MIS -  Obselete - this is no longer available for EK3 
{
  D0 = 0; 	
  D2 = 0; 
  D3 = 1; 	// P3V3_EN 
  D4 = 0; 	// EXT_REF_CTRL 
  /* D1,D5-D7 are inputs */

  OED = 0xFF; 	// Set Port D as outputs

  /* Set all "config" ports */
  A0 = 1;
  A1 = 0;
  A2 = 0;
  A3 = 1;
  A4 = 0;	// RX_EN
  A5 = 0;	// OSC_EN
  A6 = 0; 	// AGC_EN (active low) 
  A7 = 0;
  
  OEA = 0xFF; 	// Set Port A as output

  return 0;
}


char toggle_portD(void) // DEbug code only
{
  unsigned short i = 0;

  D0 = 0; 	
  D1 = 0; 
  D2 = 0; 	
  D3 = 0; 	
  D4 = 0; 	
  D5 = 0; 
  D6 = 0; 	
  D7 = 0;   
  
  /* Delay */
  mdelay(1); // 1 ms

  D0 = 1; 	
  D1 = 1; 
  D2 = 1; 	
  D3 = 1; 	
  D4 = 1; 	
  D5 = 1; 
  D6 = 1; 	
  D7 = 1;   
  
   /* Delay */
  mdelay(1); // 1 ms

  return 0;
}


char enable_se4110(void) // MIS -  Obselete - this is no longer available for EK3 
{
  /* Enable chip */
  A4 = 1; 	// RX_EN 
  A5 = 1; 	// OSC_EN
  return 0;
}

char disable_se4110(void) // MIS -  Obselete - this is no longer available for EK3 
{
  /* Enable chip */
  A4 = 0; 	// RX_EN 
  A5 = 0; 	// OSC_EN 

  return 0;
}

char reset_se4110(void) // MIS -  Obselete - this is no longer available for EK3 
{
  unsigned short i = 0;

  /* Disable chip */
  A4 = 0;
  A5 = 0;

  /* Delay */
  mdelay(1); // 1 ms

  /* Enable chip */
  A4 = 1;
  A5 = 1;

  return 0;
}


