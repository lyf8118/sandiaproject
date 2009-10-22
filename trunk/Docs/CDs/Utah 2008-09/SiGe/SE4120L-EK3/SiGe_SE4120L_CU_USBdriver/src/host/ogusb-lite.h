/*
 * OGUSB-WIN32 - Open GNSS USB Driver for Windows
 *
 * Copyright (C) 2006 Marcus Junered (junered@ltu.se)
 * GNSS Laboratory, 
 * Luleå Technical University, 
 * Sweden
 *
 * ---------------------------------------------------------------------
 * 	Further Modifications by :
 *	Michael Steval
 *	SiGe Semiconductor
 *	Bishops Stortford, UK
 *	mis@sige.com
 *	Sept, 2006
  * ---------------------------------------------------------------------
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


#ifndef _OGUSB_LITE_
#define _OGUSB_LITE_

#include <stdio.h>
#include <usb.h>	// needed for usb functions
#include <errno.h>
#include <getopt.h>
#include <time.h>
#include "fusb.h"
#include "fusb_win32.h"
#include "usrp_bytesex.h"
#include "usrp_prims.h"

#define DEBUG 0

#define BUFSIZE 32 * 512

#define VID 0x16C0
#define PID 0x072F

#define RX_ENDPOINT 0x86

#define VRT_VENDOR_IN 	0xC0
#define VRT_VENDOR_OUT 	0x40

#define RX_INTERFACE 2
#define RX_ALTINTERFACE 0

// IN commands
#define VRQ_GET_STATUS 	0x80


#define GS_RX_OVERRUN 	1   // wIndexL  // returns 1 byte

// OUT commands
#define VRQ_XFER    0x01

// M I Steval created for SE4120 configuration
#define	VRQ_CONFIG_SE4120	0x03		// wValueL,windexL:  {0, byte_D0, 0, byte_D1}

typedef struct {
  int interface;
  int altinterface;
  usb_dev_handle *udev;
  fusb_ephandle *d_ephandle;
  fusb_devhandle *d_devhandle;
} fx2_config;

void print_endpoint(struct usb_endpoint_descriptor *endpoint);
void print_altsetting(struct usb_interface_descriptor *interface);
void print_interface(struct usb_interface *interface);
void print_configuration(struct usb_config_descriptor *config);

static int write_cmd (struct usb_dev_handle *udh, int request, int value, 
		      int index, unsigned char *bytes, int len);

// M I Steval
bool config_4120 (struct usb_dev_handle *udh, char byteD0, char byteD1);
			  
			  
bool _get_status (struct usb_dev_handle *udh, int which, bool *trouble);
bool check_rx_overrun (struct usb_dev_handle *udh, bool *overrun_p);
bool usrp_xfer (struct usb_dev_handle *udh, bool start);
fusb_devhandle *make_devhandle (usb_dev_handle *udh);
struct usb_device *usb_fx2_find(int vid, int pid, char info);
bool usb_fx2_configure(struct usb_device *fx2, fx2_config *fx2c);

// Global variables
static const int FUSB_BUFFER_SIZE = 16 * (1L << 20); // 8 MB
static const int FUSB_BLOCK_SIZE  = 16 * (1L << 10); // 16KB is hard limit
static const int FUSB_NBLOCKS     = FUSB_BUFFER_SIZE / FUSB_BLOCK_SIZE;
static const int debug = DEBUG;

#endif
