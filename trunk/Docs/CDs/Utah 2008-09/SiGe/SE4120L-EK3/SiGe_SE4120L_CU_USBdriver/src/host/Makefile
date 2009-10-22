INCLUDES=-I. -I/usr/local/include
CPP=g++ -g -O2 -Wall
FX2P=../../bin/
FW_PATH=../../firmware/
VID=0x04b4
PID=0x8613
VID_SIGE=0x16C0
PID_SIGE=0x072F

all:	ogusb install

ogusb: 	ogusb-lite.cc ogusb-lite.h fusb_win32.cc fusb_win32.h fusb.cc fusb.h fusb_sysconfig_win32.cc
	$(CPP) $(INCLUDES) -c ogusb-lite.cc -o ogusb-lite.o
	$(CPP) $(INCLUDES) -c fusb_win32.cc -o fusb_win32.o
	$(CPP) $(INCLUDES) -c fusb.cc -o fusb.o
	$(CPP) $(INCLUDES) -c fusb_sysconfig_win32.cc -o fusb_sysconfig_win32.o
	$(CPP) $(INCLUDES) -o ogusb-lite ogusb-lite.o fusb_win32.o fusb.o fusb_sysconfig_win32.o -lusb

install:
	@mv ogusb-lite.exe ../../bin

scan: 
	$(FX2P)fx2-programmer any any dump_busses

program:
	$(FX2P)fx2-programmer $(VID_SIGE) $(PID_SIGE) set 0xE600 1
	$(FX2P)fx2-programmer $(VID_SIGE) $(PID_SIGE) program $(FW_PATH)gn3s_firmware.ihx
	$(FX2P)fx2-programmer $(VID_SIGE) $(PID_SIGE) set 0xE600 0

loader:	
	$(FX2P)fx2-programmer $(VID) $(PID) set 0xE600 1
	$(FX2P)fx2-programmer $(VID) $(PID) program $(FW_PATH)Vend_Ax.ihx
	$(FX2P)fx2-programmer $(VID) $(PID) set 0xE600 0

vendor: loader vendor1

vendor1:
	$(FX2P)fx2-programmer $(VID) $(PID) program_vendor $(VID_SIGE) $(PID_SIGE)

read_old:
	$(FX2P)fx2-programmer $(VID) $(PID) read_vendor

read_new:
	$(FX2P)fx2-programmer $(VID_SIGE) $(PID_SIGE) read_vendor

clean: 
	rm -f *.o
