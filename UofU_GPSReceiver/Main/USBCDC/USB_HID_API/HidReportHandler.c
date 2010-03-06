// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (HID Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: HidReportHandler.c, v1.12 2009/05/26                               |
|  Author: Rostyslav Stolyar                                                  |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  R.Stolyar    2009/03/03   born                                             |
+----------------------------------------------------------------------------*/

#include "..\Common\device.h"
#include "..\Common\types.h"              // Basic Type declarations
#include "..\USB_Common\defMSP430USB.h"
#include "..\USB_Common\usb.h"                // USB-specific Data Structures
#include "..\USBmain.h"
#include "..\USB_Common\descriptors.h"
#include "HidReportHandler.h"


extern __no_init tEDB0 tEndPoint0DescriptorBlock;
extern __no_init tEDB tInputEndPointDescriptorBlock[];
extern __no_init tEDB tOutputEndPointDescriptorBlock[];


//----------------------------------------------------------------------------

VOID Handler_InReport(VOID)
{
}

//----------------------------------------------------------------------------

VOID Handler_InFeature(VOID)
{
    switch((BYTE)tSetupPacket.wValue)    // tSetupPacket.wValue is contains HID-Report-ID
    {
        case 1:
            // user's specified code...
            break;

         case 2:
            // user's specified code...
            break;

        default:;
    }
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData);
}

//----------------------------------------------------------------------------

VOID Handler_OutReport(VOID)
{
}

//----------------------------------------------------------------------------

VOID Handler_OutFeature(VOID)
{
    switch((BYTE)tSetupPacket.wValue)    // tSetupPacket.wValue is contains HID-Report-ID
    {
        case 1:
            // user's specified code...
            break;

         case 2:
            // user's specified code...
            break;

        default:;
    }
}

/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
