/* 
 * OGUSB-WIN32 - Open GNSS USB Driver for Windows
 * 
 * Copyright (C) 2006 Marcus Junered (junered@ltu.se)
 * GNSS Laboratory, 
 * Luleå Technical University, 
 * Sweden
 *
 * ----------------------------------------------------------------------
 * 	Further Modifications by :
 *	Michael Steval
 *	SiGe Semiconductor
 *	Bishops Stortford, UK
 *	mis@sige.com
 *	Sept, 2006
  * ---------------------------------------------------------------------
 * 
 * OGUSB-WIN32 - Open GNSS USB Driver for Windows
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


#include <ogusb-lite.h>


void show_help(void)
{
printf( "\n"
	"SE4120 Program Option [file transfer]\n"
	"\n"
	" MENU  MSPS	BITS	DATA	SYNC	\n"
	" ----  ----	----	----    ---- 	\n"
	"  1	8.184	2-bit	I/Q     pulse 	\n"
	"  2	4.092	2-bit	I/Q     pulse 	\n"
	"  3	4.092	4-bit	I/Q     pulse 	\n"
	"  4	5.456	1-bit	real    byte 	\n"
	"  5	5.456	2-bit	real    byte 	\n"
	"  6	5.456	2-bit	I/Q     byte 	\n"
	"  7	4.092	2-bit	I/Q     byte 	\n"	
	"  8	5.456	2-bit	real   parallel \n"	
	"  9	16.368	1-bit	real   parallel \n"	
	" 10	16.368	2-bit	real   parallel \n"	
	" 11	16.368	1-bit	real   unsampled **\n"	
	" 12	5.456	1-bit	real   parallel \n"	
	" 13	16.368	2-bit	real   bypass **\n"	
	" 14	16.368	2-bit	I/Q    parallel \n"	
	" 15	8.184	2-bit	I/Q    parallel \n"	
	" 16	5.456	2-bit	I/Q    parallel \n"	
	" 17	4.092	2-bit	I/Q    parallel \n"	
	" 18	16.368	4-bit	I/Q    parallel \n"	
	" 19	8.184	4-bit	I/Q    parallel \n"	
	" 20	5.456	4-bit	I/Q    parallel \n"	
	" 21	4.092	4-bit	I/Q    parallel \n"	
	"  									 	\n"	
	" For Galileo Filter add 100 to menu code \n"	
	" For File transfer add option switch [1] \n"	
	"  									 	 \n"	
	" e.g. 'ogusb-lite.exe 103 1' is:       \n"
	" 'Galileo, 4.092MSPS, 4-bit I/Q, pulse-sync + file transfer\n"
	"  \n"
	"  ** means Block Convertor bypass modes \n"
	"\n"
	);
}



/*
// start test config code Michael Steval SiGe
int atoz(char *s)
{
int a;
if(!strncasecmp("0x", s, 2)){
	sscanf(s, "%x", &a);
	return a;
	}
return atoi(s);
}

//	fprintf(stderr, "Changing SE41120 Configuration \n");
//	config_4120 (fx2c.udev, 0xbc, 0x18);		// program 4120 in S/W (D0,D1)
//	fprintf(stderr, "4.092 Msps, 2b I/Q, Byte Sync, 16.368MHz \n");

// end test config code Michael Steval SiGe
*/


int main(int argc, char *argv[])	// Pass menu parameters to the function
{
  //char a;
  int ret;
  struct usb_device *fx2 = 0;
  bool request = 0;
  
  if(argc<1)
  {
	show_help();
	return -1; 
  }

  
  fx2 = usb_fx2_find(VID, PID, debug);

  // Open and configure FX2 device if found...
  if (!fx2) 
  {
    fprintf(stderr, "Could not find a FX2 device \n");
    return -1;
  }
  
  else 
  {
    fx2_config fx2c;
    ret = usb_fx2_configure(fx2, &fx2c);

    if (ret) 
	{
      fprintf(stderr, "Could not obtain a handle to the FX2 device \n");
      exit(-1);
    }	
    
	else 
	
	{
	  if(argc<2)		// no menu option selected, so list help page
	  {
		show_help();
		return -1; 
	  }

   //  handle the menu options	
   
   // First GPS modes codes 1 to 21
    if(!strcasecmp(argv[1], "1"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xa4, 0x18);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 8.184 Msps, 2b I/Q, Pulse Sync, 16.368MHz \n");
 	}
		
	else if(!strcasecmp(argv[1], "2"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xac, 0x18);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 4.092 Msps, 2b I/Q, Pulse Sync, 8.184MHz \n");
	}
		
	else if(!strcasecmp(argv[1], "3"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xec, 0x18);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 4.092 Msps, 4b I/Q, Pulse Sync, 16.368MHz \n");
	}
	
	else if(!strcasecmp(argv[1], "4"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x38, 0x18);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 5.456 Msps, 1b real, Byte Sync, 8.184MHz \n");
	}
  
	else if(!strcasecmp(argv[1], "5"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x78, 0x18);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 5.456 Msps, 2b real, Byte Sync, 16.368MHz \n");
	}
  
  	else if(!strcasecmp(argv[1], "6"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xb8, 0x18);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 5.456 Msps, 2b I/Q, Byte Sync, 16.368MHz \n");
	}
  
  	else if(!strcasecmp(argv[1], "7"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xbc, 0x18);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 4.092 Msps, 2b I/Q, Byte Sync, 16.368MHz \n");
	}
  	
	else if(!strcasecmp(argv[1], "8"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x58, 0x18);		//  4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 5.456 Msps, 2b real, Parallel, 5.456MHz \n");
	}
	
	else if(!strcasecmp(argv[1], "9"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x10, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 16.368 Msps, 1b real, Parallel, 16.368MHz \n");
	}
	
	else if(!strcasecmp(argv[1], "10"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x50, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 16.368 Msps, 2b real, Parallel, 16.368MHz \n");
	}
	
	else if(!strcasecmp(argv[1], "11")) 			//  Block Convertor  bypassed
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x08, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 16.368 Msps, 1b real, Unsampled Mode, 16.368MHz**\n"); // ??
	}
	
/*	else if(!strcasecmp(argv[1], "xx"))				// Unsupported Mode don't use, 
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x20, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 16.368 Msps, 1b real, Unsupported Mode, 16.368MHz* \n"); 
	}
*/	
		else if(!strcasecmp(argv[1], "12"))				 
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x18, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 5.456 Msps, 1b real, Parallel, 5.456MHz \n"); 
	}
	
/*	
	else if(!strcasecmp(argv[1], "xx"))				//  Don't use this a sync  is active, Use 1-bit parallel mode instead
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x28, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 5.456 Msps, 1b real, Pulse Sync, 5.456MHz \n"); 
	}
*/
	
	else if(!strcasecmp(argv[1], "13")) 					//  
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x48, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 16.368 Msps, 2b real, Block Convertor Bypassed, 16.368MHz**\n"); // ??
	}
	
	
	else if(!strcasecmp(argv[1], "14"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x90, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 16.368 Msps, 2b I/Q, Parallel, 16.368MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "15"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x94, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 8.184 Msps, 2b I/Q, Parallel, 8.184MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "16"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x98, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 5.456 Msps, 2b I/Q, Parallel, 5.456MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "17"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x9c, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 4.092 Msps, 2b I/Q, Parallel, 4.092MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "18"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xd0, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 16.368 Msps, 4b I/Q, Parallel, 16.368MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "19"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xd4, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 8.184 Msps, 4b I/Q, Parallel, 8.184MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "20"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xd8, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 5.456 Msps, 4b I/Q, Parallel, 5.456MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "21"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xdc, 0x18);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "GPS, 4.092 Msps, 4b I/Q, Parallel, 4.092MHz \n"); 
	}
	
	
	// Now Galileo modes - Codes 101 to 121 simple menu method but confusion proof ! 
	
	 //  handle the menu options	
   else if(!strcasecmp(argv[1], "101"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xa4, 0x1c);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 8.184 Msps, 2b I/Q, Pulse Sync, 16.368MHz \n");
 	}
		
	else if(!strcasecmp(argv[1], "102"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xac, 0x1c);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 4.092 Msps, 2b I/Q, Pulse Sync, 8.184MHz \n");
	}
		
	else if(!strcasecmp(argv[1], "103"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xec, 0x1c);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 4.092 Msps, 4b I/Q, Pulse Sync, 16.368MHz \n");
	}
	
	else if(!strcasecmp(argv[1], "104"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x38, 0x1c);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 5.456 Msps, 1b real, Byte Sync, 8.184MHz \n");
	}
  
	else if(!strcasecmp(argv[1], "105"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x78, 0x1c);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 5.456 Msps, 2b real, Byte Sync, 16.368MHz \n");
	}
  
  	else if(!strcasecmp(argv[1], "106"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xb8, 0x1c);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 5.456 Msps, 2b I/Q, Byte Sync, 16.368MHz \n");
	}
  
  	else if(!strcasecmp(argv[1], "107"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xbc, 0x1c);		// program 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 4.092 Msps, 2b I/Q, Byte Sync, 16.368MHz \n");
	}
  	
	else if(!strcasecmp(argv[1], "108"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x58, 0x1c);		//  4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 5.456 Msps, 2b real, Parallel, 5.456MHz \n");
	}
	
	else if(!strcasecmp(argv[1], "109"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x10, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 16.368 Msps, 1b real, Parallel, 16.368MHz \n");
	}
	
	else if(!strcasecmp(argv[1], "110"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x50, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 16.368 Msps, 2b real, Parallel, 16.368MHz \n");
	}
	
	else if(!strcasecmp(argv[1], "111")) 			//  Block Convertor  bypassed
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x08, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 16.368 Msps, 1b real, Unsampled Mode, 16.368MHz**\n"); // ??
	}
	
/*	else if(!strcasecmp(argv[1], "xx"))				// Unsupported Mode don't use, 
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x20, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "16.368 Msps, 1b real, Unsupported Mode, 16.368MHz* \n"); 
	}
*/	
		else if(!strcasecmp(argv[1], "112"))				 
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x18, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 5.456 Msps, 1b real, Parallel, 5.456MHz \n"); 
	}
	
/*	
	else if(!strcasecmp(argv[1], "xx"))				//  Don't use this a sync  is active, Use 1-bit parallel mode instead
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x28, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 5.456 Msps, 1b real, Pulse Sync, 5.456MHz \n"); 
	}
*/
	
	else if(!strcasecmp(argv[1], "113")) 					//  
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x48, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 16.368 Msps, 2b real, Block Convertor Bypassed, 16.368MHz**\n"); // ??
	}
	
	
	else if(!strcasecmp(argv[1], "114"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x90, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 16.368 Msps, 2b I/Q, Parallel, 16.368MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "115"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x94, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 8.184 Msps, 2b I/Q, Parallel, 8.184MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "116"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x98, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 5.456 Msps, 2b I/Q, Parallel, 5.456MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "117"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0x9c, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 4.092 Msps, 2b I/Q, Parallel, 4.092MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "118"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xd0, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 16.368 Msps, 4b I/Q, Parallel, 16.368MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "119"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xd4, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 8.184 Msps, 4b I/Q, Parallel, 8.184MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "120"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xd8, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 5.456 Msps, 4b I/Q, Parallel, 5.456MHz \n"); 
	}
	
	else if(!strcasecmp(argv[1], "121"))
	{
		fprintf(stderr, "Changing SE41120 Configuration \n");
		config_4120 (fx2c.udev, 0xdc, 0x1c);		// 4120 in S/W (D0,D1)
		fprintf(stderr, "Galileo, 4.092 Msps, 4b I/Q, Parallel, 4.092MHz \n"); 
	}
		
	
	else		// undefined option
	{
	  fprintf(stderr,"Incorrect program command syntax\n");
	  show_help();
	  return -1;
    }
	
    if(argc<3)		// no file transfer option selected, so just quit
    {
	  //show_help();
	  return 0; 
    }

	if(!strcasecmp(argv[2], "1"))
	
	// File transfer required
	{	
		char mem = 0; 
		char *filename = "gnss.bin";
		
	//	unsigned int loops = 32 * 64;	// 32 MB (x * 64 * 16 kB)
		unsigned int loops = 1 * 64;	// 1 MB (x * 64 * 16 kB) only need 1MB.

		unsigned int i=0, j=0;
		unsigned int bufsize = BUFSIZE;
		char buf[BUFSIZE];	  
		char *bigbuf = NULL;
		unsigned long long numbytes = 0;
		bool overrun = false;
		unsigned long long nwritten = 0;


		
		// char LUT[4] = {1,-1,3,-3}; //2 bits
		
// MICHAEL STEVAL -  4 bits converted to signed char is a bit tricky ! 		
//		0000 = 1
//		0001 = -1
//		0010 = 3
//		0011 = -3
//		---------
//		0100 = 5
//		0101 = -5
//		0110 = 7
//		0111 = -7
//		---------
//		1000 = 9
//		1001 = -9
//		1010 = 11
//		1011 = -11
//		---------
//		1100 = 13
//		1101 = -13
//		1110 = 15
//		1111 = -15
		
		char LUT[16] = {1,-1,3,-3,5,-5,7,-7,9,-9,11,-11,13,-13,15,-15}; //4 bits

		bigbuf = (char*) malloc(loops * BUFSIZE);
	  
	  // Open output file
		FILE *fid = fopen(filename, "wb");
		if (fid == NULL) 
		{
			fprintf(stderr, "Could not open output file \n");
			perror("fopen");
		}	 	  

	    // Start Waveform
	    usrp_xfer (fx2c.udev, 1);

	    clock_t start_cpu_time = clock();

	    // Loop until we have enough data...
	    for (i=0; i<loops; i++) 
		{
			if (debug >= 3)
			  printf("Loop: %d \n", i);

			// Check for buffer overrun
			check_rx_overrun(fx2c.udev, &overrun);
			if (overrun) 
			{
			  fprintf(stderr, "Buffer overrun... \n");
			  break;
			} 
			
			else if (debug >= 3) 
			{
				printf("No overrun detected \n");
			}

			// FUSB Read...
			ret = fx2c.d_ephandle->read (buf, bufsize);

			if (ret != (int) bufsize) 
			{
				fprintf (stderr, "fusb_read: ret = %d (%d) \n", ret, bufsize);
				fprintf (stderr, "%s\n", usb_strerror());
			}
			else if (debug >=3) 
			{
			  printf("Received %d bytes of data \n", ret);
			}



			// Store IF data as 8bit signed values
			for (j=0; j<bufsize; j++) 
			{
			  
			  // Use LUT to rearrange and interpret bits

//MICHAEL STEVAL HERE		
    	//	     buf[j] = LUT[buf[j] & 0x3]; // 2 bits
			  buf[j] = LUT[buf[j] & 0xf]; // 4 bits
			}
			
			// Copy to buffer
			memcpy(bigbuf+numbytes, buf, bufsize);	
			numbytes += bufsize;	       
		}

	      clock_t stop_cpu_time = clock();

	      // Stop waveform
	      usrp_xfer (fx2c.udev, 0);
		  
	      // Compute data rate and time
	      double delta_cpu = ((double) (stop_cpu_time - start_cpu_time))  
		/ CLOCKS_PER_SEC;
	      printf ("xfered %.3g MB in %.3g seconds.  %.4g MB/s. \n",
		      (double) numbytes/1048576, delta_cpu, 
		      (numbytes/1048576) / delta_cpu);

	      // Dump buffer to disk
	      nwritten = fwrite (bigbuf, sizeof(char), numbytes, fid);
	      if (nwritten != numbytes) 
		  {
			perror ("fwrite");
			fprintf(stderr, "Could only write %lld of %lld IF samples \n",
			nwritten, numbytes);
	      }	
	    
	      // Clean up
	      delete fx2c.d_ephandle;
	      delete fx2c.d_devhandle;

	      fclose(fid);

	      if (debug >= 1)
			printf("Deleted EP handles \n");
		  
	      if (mem)
			free(bigbuf);

	      usb_release_interface(fx2c.udev, fx2c.interface);
	      usb_close(fx2c.udev);
		  
	      if (debug >= 1)
			printf("Closed device \n");
			
	  }
	}
  }

  return 0;
}


void print_endpoint(struct usb_endpoint_descriptor *endpoint) {
  printf(" bEndpointAddress: %02xh\n", endpoint->bEndpointAddress);
  printf(" bmAttributes: %02xh\n", endpoint->bmAttributes);
  printf(" wMaxPacketSize: %d\n", endpoint->wMaxPacketSize);
  printf(" bInterval: %d\n", endpoint->bInterval);
  printf(" bRefresh: %d\n", endpoint->bRefresh);
  printf(" bSynchAddress: %d\n", endpoint->bSynchAddress);
}


void print_altsetting(struct usb_interface_descriptor *interface) {
  int i;

  printf(" bInterfaceNumber: %d\n", interface->bInterfaceNumber);
  printf(" bAlternateSetting: %d\n", interface->bAlternateSetting);
  printf(" bNumEndpoints: %d\n", interface->bNumEndpoints);
  printf(" bInterfaceClass: %d\n", interface->bInterfaceClass);
  printf(" bInterfaceSubClass: %d\n", interface->bInterfaceSubClass);
  printf(" bInterfaceProtocol: %d\n", interface->bInterfaceProtocol);
  printf(" iInterface: %d\n", interface->iInterface);

  for (i = 0; i < interface->bNumEndpoints; i++)
	print_endpoint(&interface->endpoint[i]);
}


void print_interface(struct usb_interface *interface) {
  int i;

  for (i = 0; i < interface->num_altsetting; i++)
	print_altsetting(&interface->altsetting[i]);
}


void print_configuration(struct usb_config_descriptor *config) {
  int i;

  printf(" wTotalLength: %d\n", config->wTotalLength);
  printf(" bNumInterfaces: %d\n", config->bNumInterfaces);
  printf(" bConfigurationValue: %d\n", config->bConfigurationValue);
  printf(" iConfiguration: %d\n", config->iConfiguration);
  printf(" bmAttributes: %02xh\n", config->bmAttributes);
  printf(" MaxPower: %d\n", config->MaxPower);

  for (i = 0; i < config->bNumInterfaces; i++)
	print_interface(&config->interface[i]);
}


static int write_cmd (struct usb_dev_handle *udh, int request, int value, 
		      int index, unsigned char *bytes, int len)
{
  // int r = write_cmd (udh, VRQ_XFER, start, 0, 0, 0);
  int requesttype = (request & 0x80) ? VRT_VENDOR_IN : VRT_VENDOR_OUT; 
  int r = usb_control_msg (udh, requesttype, request, value, index, 
						   (char *) bytes, len, 1000);
  if (r < 0){
	// we get EPIPE if the firmware stalls the endpoint.
	if (errno != EPIPE)
	  fprintf (stderr, "usb_control_msg failed: %s\n", usb_strerror ());
  }
  return r;
}


bool _get_status (struct usb_dev_handle *udh, int which, bool *trouble) {
  unsigned char status;
  *trouble = true;

  if (write_cmd (udh, VRQ_GET_STATUS, 0, which,
		 &status, sizeof (status)) != sizeof (status))
    return false;

  *trouble = status;
  return true;
}


bool check_rx_overrun (struct usb_dev_handle *udh, bool *overrun_p) {
	  return _get_status (udh, GS_RX_OVERRUN, overrun_p);
}


// M I Steval
bool config_4120 (struct usb_dev_handle *udh, char byteD0, char byteD1)
{
	// VRQ_CONFIG_SE4120	0x03		// wValueL,H:  { byte_D0, byte_D1}
	
	// Somthing adrift here - as bRequest gets recieved in instead of = valueL,H
	// Just use it as is - it works !    Michael Steval
	
	//	int r = write_cmd (udh, VRQ_CONFIG_SE4120, 0xbc18, 0, 0, 0); //debug code
	int r = write_cmd (udh, VRQ_CONFIG_SE4120, ((byteD0 << 8) | byteD1), 0, 0, 0);  // Use me

	return r == 0;
}



bool usrp_xfer (struct usb_dev_handle *udh, bool start) {   
	int r = write_cmd (udh, VRQ_XFER, start, 0, 0, 0);
	return r == 0;
}


fusb_devhandle *make_devhandle (usb_dev_handle *udh) {
  return new fusb_devhandle_win32 (udh);
}


struct usb_device *usb_fx2_find(int vid, int pid, char info) {

  struct usb_bus *bus;
  struct usb_device *dev;
  struct usb_device *fx2 = NULL;
  usb_dev_handle *udev;

  int ret;
  char string[256];

  usb_init();
  usb_find_busses();
  usb_find_devices();

  if (debug >= 2)
	printf("bus/device idVendor/idProduct\n"); 

  // Scan busses for a FX2 device
  for (bus = usb_busses; bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
	  
      if (debug >= 2)
	printf("%s/%s %04X/%04X\n", bus->dirname, dev->filename,
	       dev->descriptor.idVendor, dev->descriptor.idProduct);
      
      if (dev->descriptor.idVendor == vid && 
        dev->descriptor.idProduct == pid) {       
      	fx2 = dev;
      }
	  
      // Print detailed device information
      if (info) {

	udev = usb_open(dev);
	if (udev) {

	  // Manufacturer
	  if (dev->descriptor.iManufacturer) {
	    ret = usb_get_string_simple(udev,
					dev->descriptor.iManufacturer,
					string, 
					sizeof(string));

	    if (ret > 0)
	      printf("- Manufacturer : %s\n", string);
	    else
	      printf("- Unable to fetch manufacturer string\n");
	  }

	  // Product
	  if (dev->descriptor.iProduct) {
	    ret = usb_get_string_simple(udev,
					dev->descriptor.iProduct, 
					string, 
					sizeof(string));

	    if (ret > 0)
	      printf("- Product : %s\n", string);
	    else
	      printf("- Unable to fetch product string\n");
	  }

	  // Serial Number
	  if (dev->descriptor.iSerialNumber) {
	    ret = usb_get_string_simple(udev,
					dev->descriptor.iSerialNumber,
					string,
					sizeof(string));
	    
	    if (ret > 0)
	      printf("- Serial Number: %s\n", string);
	    else
	      printf("- Unable to fetch serial number string\n");
	  }
	  
	  usb_close (udev);
	}
	
	if (!dev->config) {
	  printf(" Couldn't retrieve descriptors\n");
	  continue;
	}
	
	for (int i = 0; i < dev->descriptor.bNumConfigurations; i++) {
	  print_configuration(&dev->config[i]);
	}
      }
    }
  }	
  
  return fx2;
}


bool usb_fx2_configure(struct usb_device *fx2, fx2_config *fx2c) { 
  
  char status = 0;
  int interface = RX_INTERFACE;
  int altinterface = RX_ALTINTERFACE;
  usb_dev_handle *udev;
  fusb_ephandle *d_ephandle;
  fusb_devhandle *d_devhandle;

  // Try to get a handle for the fx2 device
  udev = usb_open(fx2);
  
  if (!udev) {
    fprintf(stderr, "Could not obtain a handle to GNSS Front-End device \n");
    return -1;
  }
  else {

    if (debug >= 1)
      printf("Received handle for GNSS Front-End device \n");

    // Configure
    if (usb_set_configuration (udev, 1) < 0) {
      fprintf (stderr, "error in %s, \n%s \n", __FUNCTION__, usb_strerror());
      usb_close (udev);
      status = -1;
    }

    // Claim interface
    if (usb_claim_interface (udev, interface) < 0) {
      fprintf (stderr, "error in %s, \n%s \n", __FUNCTION__, usb_strerror());
      fprintf (stderr, "\nDevice not programmed? \n");
      usb_close (udev);
      status = -1;
      exit(0);
    }

    // Set alternative interface
    if (usb_set_altinterface (udev, altinterface) < 0) {
      fprintf (stderr, "error in %s, \n%s \n", __FUNCTION__, usb_strerror());
      usb_release_interface (udev, interface);
      usb_close (udev);
      status = -1;
    }

    // Create FUSB handles
    d_devhandle=make_devhandle(udev);
    d_ephandle = d_devhandle->make_ephandle (RX_ENDPOINT, 
					     true,
					     FUSB_BLOCK_SIZE, 
					     FUSB_NBLOCKS);

    // Enable End-Point
    if (!d_ephandle->start ()){
      fprintf (stderr, "usrp0_rx: failed to start end point streaming");
      usb_strerror ();
      status = -1;
    }

    // Return configuration 
    if (status == 0) {
      fx2c->interface = interface;
      fx2c->altinterface = altinterface;
      fx2c->udev = udev;
      fx2c->d_devhandle = d_devhandle;
      fx2c->d_ephandle = d_ephandle;
      
      return 0;
    }
    else {
      return -1;
    }
  }
}
