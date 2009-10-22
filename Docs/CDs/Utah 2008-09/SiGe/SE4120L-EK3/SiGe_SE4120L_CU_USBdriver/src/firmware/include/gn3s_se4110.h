/*
 * Copyright (C) 2006 Marcus Junered (junered@ltu.se)
 * GNSS Laboratory, 
 * Luleå Technical University, 
 * Sweden
 * 
 * ---------------------------------------------------------------------
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



#ifndef _GN3S_SE4110_
#define _GN3S_SE4110_

#include "fx2regs.h"
#include "delay.h"

/* Prototypes */
char init_se4120(void);	// MIS - use for 4120 on EK3
char hard_program_se4120(unsigned char); 	// MIS - use for 4120 on EK3
char soft_program_se4120(unsigned char, unsigned char); 
char test_program_se4120(unsigned char, unsigned char); 
char clock_data_out(unsigned char); //transmit 1 byte

char toggle_portD(void); 	// MIS debug code


char init_se4110(void);		// obselete on EK3
char enable_se4110(void);	// obselete on EK3
char disable_se4110(void);  // obselete on EK3
char reset_se4110(void);    // obselete on EK3

#endif
