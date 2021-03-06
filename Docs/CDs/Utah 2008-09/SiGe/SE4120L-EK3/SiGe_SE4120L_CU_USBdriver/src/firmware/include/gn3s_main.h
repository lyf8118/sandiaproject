/* 
 * GN3S - GNSS IF Streamer for Cypress FX2LP (GNSSS)
 * 
 * Copyright (C) 2006 Marcus Junered (junered@ltu.se)
 *
 * GNSS Laboratory, 
 * Lule� Technical University, 
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



#ifndef _GN3S_MAIN_
#define _GN3S_MAIN_

#include "usrp_common.h"
#include "fx2regs.h"
#include "gn3s_regs.h"
#include "gpif_inline.h"
#include "timer.h"
#include "isr.h"
#include "usb_common.h"
#include "fx2utils.h"
#include "gn3s_se4110.h"
#include <string.h>


// ----------------------------------------------------------------
//          Vendor bmRequestType's
// ----------------------------------------------------------------
#define VRT_VENDOR_IN	0xC0
#define VRT_VENDOR_OUT	0x40

// IN commands
#define VRQ_GET_STATUS  0x80

#define GS_TX_UNDERRUN  0   // wIndexL  // returns 1 byte
#define GS_RX_OVERRUN   1   // wIndexL  // returns 1 byte

// OUT commands
#define VRQ_XFER	  	0x01 
#define VRQ_XFER_TX		0x02 

// M I Steval created for SE4120 configuration
#define	VRQ_CONFIG_SE4120	0x03	// wValueL,H:  {byte_D0, byte_D1}

#define	bRequestType	SETUPDAT[0]
#define	bRequest		SETUPDAT[1]
#define	wValueL			SETUPDAT[2]
#define	wValueH			SETUPDAT[3]
#define	wIndexL			SETUPDAT[4]
#define	wIndexH			SETUPDAT[5]
#define	wLengthL		SETUPDAT[6]
#define	wLengthH		SETUPDAT[7]


#undef wordwide

// Prototypes
static void get_ep0_data(void);
unsigned char app_vendor_cmd(void);
static void main_loop(void);

#endif
