OGUSB-WIN32 - Open GNSS USB Driver for Windows

Original Source: Copyright (C) 2006 Marcus Junered (junered@ltu.se)
GNSS Laboratory, 
Luleå Technical University, 
Sweden

Additional modifications: Copyright (C) 2006 Michael Steval (mis@sige.com)
SiGe Semiconductor (Europe) Ltd., 
Bishops Stortford, 
United Kingdom

---------------------------------------------------------------------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to:

The Free Software Foundation, Inc., 
59 Temple Place, 
Suite 330, 
Boston, 
MA 02111-1307  
USA

---------------------------------------------------------------------

INSTALLATION

1. Libusb

	Libusb does not have to be installed on the system for the driver to 		
	work. However it is needed to create .inf files for new devices.

	Simply uncompress the libusb-win32-device-bin-20060518.rar file and run:

		/bin/inf-wizard.exe

	to create a new inf file for the unconfigured (no firmware) device and 	
	store it in the install directory.

2. Install the drivers

	The following .inf files are available:

		sige_nofw	VID: 0x04b4, PID: 0x8613
		sige_fw		VID: 0x16c0, PID: 0x072f


3. Batch file operation

	2.1 	Edit the program.bat file so that the VID and PID matches the device.

	2.2 	Run program.bat and if it succeds the output should look something like 
			the example in:	doc\programming_output.txt

	2.3		Run scan.bat and confirm that the new VID and PID are: 
	
				VID: 0x16c0, PID: 0x072f

	2.4 	Run run.bat to collect 32 MB to gnss.bin (stored as 8bit signed values)

	2.5 	The binary file may then be analysed using a suitable software tool e.g. Matlab.
	
