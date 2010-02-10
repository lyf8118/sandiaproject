// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (HID Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbHidReq.c, v1.12 2009/05/26                                      |
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
#include "HidReportHandler.h"
#include "..\USB_Common\descriptors.h"


VOID usbClearOEP0ByteCount(VOID);
VOID usbSendDataPacketOnEP0(PBYTE pbBuffer);
VOID usbReceiveDataPacketOnEP0(PBYTE pbBuffer);


VOID usbGetHidDescriptor(VOID)
{
#ifdef _HID_
    usbClearOEP0ByteCount();
    wBytesRemainingOnIEP0 = 9;
    usbSendDataPacketOnEP0((PBYTE)&abromConfigurationDescriptorGroup[START_HID_DESCRIPTOR]);
#endif
}

//----------------------------------------------------------------------------

VOID usbGetReportDescriptor(VOID)
{
    wBytesRemainingOnIEP0 = SIZEOF_REPORT_DESCRIPTOR;
    usbSendDataPacketOnEP0((PBYTE)&abromReportDescriptor);
}

//----------------------------------------------------------------------------

VOID usbSetReport(VOID)
{
    usbReceiveDataPacketOnEP0((PBYTE) &abUsbRequestIncomingData);     // receive data over EP0 from Host
}

//----------------------------------------------------------------------------

VOID usbGetReport(VOID)
{
    switch((BYTE)tSetupPacket.wValue)
    {
       case USB_REQ_HID_FEATURE:
            Handler_InFeature();
            break;
       case USB_REQ_HID_INPUT:
            Handler_InReport();
            break;
       default:;
    }
}

/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
