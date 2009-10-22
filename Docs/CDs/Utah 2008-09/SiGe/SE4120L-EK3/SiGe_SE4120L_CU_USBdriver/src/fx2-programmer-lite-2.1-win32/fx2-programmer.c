/*
 * File originates from: FX2 Programmer 2.0.2
 *
 * Modifications for use with the SiGe USB module to accompany
 * the textbook: "A Software-Defined GPS and Galileo Receiver: A
 * Single-Frequency Approach" by Kai Borre, Dennis Akos, et.al. by:
 *
 * Marcus Junered, GNSS Research Group
 * Lulea University of Technology
 * Lulea, Sweden 
 * junered@ltu.se
 *
 * ---------------------------------------------------------------------
 *
 * FX2 Programmer Win32 - Cypress FX2LP Firmware programmer for Windows
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
/* needed for stat() */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/* needed for usb functions */
#include <usb.h>
#include <windows.h>

struct usb_device *current_device;
usb_dev_handle *current_handle;


int atoz(char *s)
{
int a;
if(!strncasecmp("0x", s, 2)){
	sscanf(s, "%x", &a);
	return a;
	}
return atoi(s);
}


void dump_busses(void)
{
  struct usb_bus *p;
  struct usb_device *q;
  p=usb_busses;
  printf("Dump of USB subsystem:\n");
  
  while(p!=NULL) {
	q=p->devices;
	while(q!=NULL) {
	  printf(" bus %s device %s vendor id=0x%04x product id=0x%04x %s\n",
			 p->dirname, q->filename, q->descriptor.idVendor, 
			 q->descriptor.idProduct,
			 (q->descriptor.idVendor==0x4b4) && (q->descriptor.idProduct==0x8613)?
			 "(UNCONFIGURED FX2)":"");
	  q=q->next;
	}
	p=p->next;
  }
  fflush(stdout);
}


void upload_ram(unsigned char *buf, int start, int len)
{
  int i;
  int tlen;
  int quanta=16;
  int a;
  
  for(i=start;i<start+len;i+=quanta) {
    tlen=len+start-i;
    
    if(tlen>quanta)
      tlen=quanta;

    a=usb_control_msg(current_handle, 0x40, 0xa0, 
		      i, 0, buf+(i-start), tlen, 1000);
    
    if(a<0) {
      fprintf(stderr,"Request to upload ram contents failed: %s\n",
	      usb_strerror());
      return;
    }
  }
}


struct usb_device *find_device(char *busname, char *devicename)
{
struct usb_bus *p;
struct usb_device *q;
p=usb_busses;
while(p!=NULL){
	q=p->devices;
	if(strcmp(p->dirname, busname)){
		p=p->next;
		continue;
		}
	while(q!=NULL){
		if(!strcmp(q->filename, devicename))return q;
		q=q->next;
		}
	p=p->next;
	}
return NULL;
}


void program_fx2(char *filename, char mem)
{
  FILE *f;
  unsigned char s[1024];
  int length;
  int addr;
  int type;
  unsigned char data[256];
  unsigned char checksum,a;
  unsigned int b;
  int i;

  f=fopen(filename, "r");

  if(f==NULL){
    fprintf(stderr,"Cannot open file \"%s\" for reading:", filename);
    perror("");
    return;
  }

  printf("Using file \"%s\"\n", filename);

  while(!feof(f)) {
    fgets(s, 1024, f); /* we should not use more than 263 bytes normally */

    if(s[0]!=':'){
      fprintf(stderr,"%s: invalid string: \"%s\"\n", filename, s);
      continue;
    }

    sscanf(s+1, "%02x", &length);
    sscanf(s+3, "%04x", &addr);
    sscanf(s+7, "%02x", &type);

    if(type==0){
      printf("Programming %3d byte%s starting at 0x%04x", 
	     length, length==1?" ":"s", addr);
      a=length+(addr &0xff)+(addr>>8)+type;

      for(i=0;i<length;i++){
	sscanf(s+9+i*2,"%02x", &b);
	data[i]=b;
	a=a+data[i];
      }

      sscanf(s+9+length*2,"%02x", &b);
      checksum=b;

      if(((a+checksum)&0xff)!=0x00) {
	printf("  ** Checksum failed: got 0x%02x versus 0x%02x\n", 
	       (-a)&0xff, checksum);
	continue;
      } else {
	printf(", checksum ok\n");
      }

      upload_ram(data, addr, length);

    } else {
      if(type==0x01) {
	printf("End of file\n");
	fclose(f);

	return;
      } else {
	if(type==0x02) {
	  printf("Extended address: whatever I do with it ?\n");
	  continue;
	}
      }
    }
  }

  fclose(f);
}


void program_vendor(int vid, int pid) {
  int ret, i;

  unsigned char buf[8] = {0xC0, 0xB4, 0x04, 0x13, 0x86, 0x01, 0x00, 0x01};

  printf("VID: 0x%x, PID: 0x%x \n", vid, pid);

  buf[1] = vid & 0xFF;
  buf[2] = (vid >> 8) & 0xFF;
  buf[3] = pid & 0xFF;
  buf[4] = (pid >> 8) & 0xFF;
 
  printf("Writing 8 bytes to eeprom 0x0 \n");

  for (i=0; i<8; i++) {
    printf("buf[%d] = 0x%x \n", i, buf[i]);
  }

  ret = usb_control_msg(current_handle, 0x40, 0xA9, 0x0, 0x0, buf, 8, 1000);

  if(ret < 0) {
    fprintf(stderr,"Request to upload eeprom contents failed: %s\n",
	    usb_strerror());
    return;
  }
}

void read_vendor() {
  int ret, i;

  unsigned char buf[8];

  printf("Reading 8 bytes from eeprom 0x0 \n");

  ret = usb_control_msg(current_handle, 0xC0, 0xA9, 0x0, 0x0, buf, 8, 1000);

  if(ret < 0) {
    fprintf(stderr,"Request to upload eeprom contents failed: %s\n",
	    usb_strerror());
    return;
  }

  for (i=0; i<8; i++) {
    printf("buf[%d] = 0x%x \n", i, buf[i]);
  }
}

void show_help(void)
{
printf( "\n"
	"fx2_programmer VID PID function [parameters]\n"
	"\n"
	"   Function         Parameters     Description\n"
	"   dump_busses                     show all available devices\n"
	"   set              address byte   changes values of a single byte\n"
	"   program          file.ihx       programs fx2 using Intel hex format file\n"
	"   program_vendor   VID PID        program VID/PID to EEPROM (C0 boot)\n"
	"   read_vendor                     read first 8 bytes of EEPROM \n"
	"                                   (boot, VID, PID, DID and conf)"
	"\n"
	);
}

int main(int argc, char *argv[])
{
  char a;
  struct usb_bus *bus;
  struct usb_device *dev;

  int vid, pid;

  if(argc<4){
	show_help();
	return -1;
  }

  usb_init();
  usb_find_busses();
  usb_find_devices();

  if(!strcasecmp(argv[3], "dump_busses")){
	dump_busses();
	return 0;
  }

  vid = atoz(argv[1]);
  pid = atoz(argv[2]);

  for (bus = usb_busses; bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      printf("%s/%s %04X/%04X\n", bus->dirname, dev->filename,
	     dev->descriptor.idVendor, dev->descriptor.idProduct);

      if ( (dev->descriptor.idVendor == vid) && 
	   (dev->descriptor.idProduct == pid) ) {
	current_device = dev;
      }  
    }
  }

  if(current_device==NULL){
	fprintf(stderr,"Cannot find vid 0x%x pid 0x%x \n", vid, pid);
	return -1;
  }
  printf("Using device vendor id 0x%04x product id 0x%04x\n",
	 current_device->descriptor.idVendor, 
	 current_device->descriptor.idProduct);

  current_handle=usb_open(current_device);

  if(!strcasecmp(argv[3], "set")){
	if(argc<6){
	  fprintf(stderr,"Incorrect set command syntax\n");
	  return -1;
	}
	a=atoz(argv[5]);
	upload_ram(&a, atoz(argv[4]), 1);
	return 0;
  }
  else if(!strcasecmp(argv[3], "program")){
	if(argc<5){
	  fprintf(stderr,"Incorrect program command syntax\n");
	  return -1;
	}
	program_fx2(argv[4], 1);
	return 0;
  }
  else if(!strcasecmp(argv[3], "program_vendor")){
	if(argc<6){
	  fprintf(stderr,"Incorrect program command syntax\n");
	  return -1;
	}
	program_vendor(atoz(argv[4]), atoz(argv[5]));
	return 0;
  }
  else if(!strcasecmp(argv[3], "read_vendor")){
	if(argc<4){
	  fprintf(stderr,"Incorrect program command syntax\n");
	  return -1;
	}

	read_vendor();
	return 0;
  }

  usb_close(current_handle);
  return 0;
}
