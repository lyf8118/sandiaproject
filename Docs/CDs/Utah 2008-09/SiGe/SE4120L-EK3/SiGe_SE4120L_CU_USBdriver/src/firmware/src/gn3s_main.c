/*
 * Code from: USRP - Universal Software Radio Peripheral
 *
 * Initial modifications by:
 *
 * Stephan Esterhuizen, Aerospace Engineering Sciences
 * University of Colorado at Boulder
 * Boulder CO, USA
 * 
 * Further modifications for use with the SiGe USB module to accompany
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


#include "gn3s_main.h"


void main(void) {
  
  init_usrp(); 
  init_gpif();
   
 // init_se4110();  	// MIS -  this is no longer required for EK3, 4110 or 4120 varient
  init_se4120();	//  MIS - initialise SE4120 - default is to Hardware programming mode
 
 
  
// ***********Should first read the port (PA3) to see if S/W programming is allowed. **************


//------------------------------------------- Hardware programming start - just keep separate for now 
// Use portA for Hardware programming, 
// as can't split PortB at bit level when functionality of PortB is set to GPIF mode / FIFO mode
  hard_program_se4120(0x01); 	//    0x1 : F0 = 0, F1 = 1, F2=0  : select the chosen Sampling  /Serial format  
//---------------------------------------------Hardware programming end


//-------------------------------------------Software  programming start - just keep separate for now 
// Just re-program it once, then leave in S/W mode
  soft_program_se4120(0xbc, 0x18); //    Change to required S/W Prog mode - MSByte first
//--------------------------------------------Software programming end

 
/*
 //------------------------------------------------debug code start
 while (1) //DEBUG
 {
	 unsigned short i = 0;
	 unsigned short j = 0;
	
	 for (i = 0x0; i <= 0xff; i += 0x4)
		{
			soft_program_se4120((0xFD & i), 0x18); //    Change to required S/W Prog mode - MSByte first 
			toggle_portD();  
			
			for (j = 0x0; j < 0x08; j++)
				{
				mdelay(0xff); // 4 ms x delay = 0.5 secs	((clock is still at 12MHz not 48MHz)
				} 
		}
	
	  //  hold it here ! MIS - else init_usrp will reset FIFO / portb settings.
 } 
 //---------------------------------------------debug code end 
*/


  EA = 0;	// disable all interrupts

  setup_autovectors();
  usb_install_handlers();
    
  EIEX4 = 1;   	      	// disable INT4 FIXME
  EA = 1;           	// global interrupt enable

  fx2_renumerate();	// simulates disconnect / reconnect

 // enable_se4110();         // MIS -  this is no longer required for EK3, 4110 or 4120 varient - leave for EK2 compatibility ?

  main_loop();
}



static void get_ep0_data(void)
{
    EP0BCL = 0;	// arm EP0 for OUT xfer.  This sets the
                // busy bit

    // wait for busy to clear
    while (EP0CS & bmEPBUSY);
}



/*
 * Handle our "Vendor Extension" commands on endpoint 0.
 * If we handle this one, return non-zero.
 */
unsigned char app_vendor_cmd(void)
{
  if (bRequestType == VRT_VENDOR_IN) 
  {

	// ///////////////////////////////
	// handle the IN requests
	// ///////////////////////////////

	switch (bRequest) 
	{

	case VRQ_GET_STATUS:
	  switch (wIndexL) 
	  {
	  case GS_RX_OVERRUN:
		EP0BUF[0] = GPIFIDLECS>>7;
		EP0BCH = 0;
		EP0BCL = 1;
		break;
		
	  case GS_TX_UNDERRUN:
		EP0BUF[0] = GPIFIDLECS>>7;
		EP0BCH = 0;
		EP0BCL = 1;
		break;
		
	  default:
		return 0;
	  }
      break;
 
	default:
	  return 0;
	  
	} 
  } //(bRequestType == VRT_VENDOR_IN) 

  else if (bRequestType == VRT_VENDOR_OUT) 
  {

	// ///////////////////////////////
	// handle the OUT requests
	// ///////////////////////////////

	switch (bRequest) 
	{

	  /* Start/Stop transfer */
	case VRQ_XFER:

	  /* start transfer */
	  if (wValueL) 
	  {
        setup_flowstate_common();
        SYNCDELAY;
        GPIFABORT =0xff; SYNCDELAY;

        GPIFTRIG = 0; SYNCDELAY;

        setup_flowstate_read(); SYNCDELAY;
        /* Stop transfer */
        /* Zero out FIFO */
        FIFORESET = bmNAKALL;SYNCDELAY;
        FIFORESET = 2;SYNCDELAY;
        FIFORESET = 6;SYNCDELAY;
        FIFORESET = 0;SYNCDELAY;

		/* Start transfer */
    	GPIFTRIG = bmGPIF_EP6_START | bmGPIF_READ; SYNCDELAY; 
	  }

	   
	  /* stop transfer */
	  else 
	  {
		GPIFTRIG = 0; SYNCDELAY;
		GPIFABORT =0xff; SYNCDELAY;

		/* Clear fifo */
		FIFORESET = bmNAKALL;SYNCDELAY;
		FIFORESET = 2;SYNCDELAY;
		FIFORESET = 6;SYNCDELAY;
		FIFORESET = 0;SYNCDELAY;
	  }

	  break;
	  
	   
  // Michael Steval Program 4120
  
	case VRQ_CONFIG_SE4120:
 
 //		soft_program_se4120(wValueH,wValueL); //    Change to required S/W Prog mode - MSByte first

 //		Don't re-initialise HW / SW mode.  - Use test_program for on the fly changes
 //		A little intermittent so repeat the command to be sure !
		test_program_se4120(wValueH,wValueL); //    Change to required S/W Prog mode - MSByte first
		test_program_se4120(wValueH,wValueL); //    Change to required S/W Prog mode - MSByte first
		break;
	  	
	  
	
	default:
	  return 0;
	}
  } //else if (bRequestType == VRT_VENDOR_OUT) 
  
 
	  
  else
	return 0;	// invalid bRequestType

  return 1;
}



static void main_loop(void) 
{
  setup_flowstate_common();
  SYNCDELAY;

#if 0
  /* Stop transfer */
  GPIFTRIG = 0; SYNCDELAY;
  GPIFABORT =0xff; SYNCDELAY;
  setup_flowstate_write(); SYNCDELAY;
  /* Start transfer */
  GPIFTRIG = bmGPIF_EP2_START | bmGPIF_WRITE; 
  SYNCDELAY;
#endif

#if 0
  GPIFTRIG 		= 0; 		SYNCDELAY;
  GPIFABORT 	= 0xff; 	SYNCDELAY;
  FIFORESET 	= bmNAKALL;	SYNCDELAY;
  FIFORESET 	= 2;		SYNCDELAY;
  FIFORESET 	= 6;		SYNCDELAY;
  FIFORESET 	= 0;		SYNCDELAY;
#endif

  while (1)
    {
      // We don't do much, GPIF is running on autopilot
	}
}
