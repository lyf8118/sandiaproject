// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (HID Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbHidReq.h, v1.12 2009/05/26                                      |
|  Author: Rostyslav Stolyar                                                  |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  R.Stolyar    2009/03/03   born                                             |
+----------------------------------------------------------------------------*/

#ifndef _UsbHidReq_H_
#define _UsbHidReq_H_

#ifdef __cplusplus
extern "C"
{
#endif


/**
Return Hid descriptor to host over control endpoint
*/
VOID usbGetHidDescriptor(VOID);

/**
Return HID report descriptor to host over control endpoint
*/
VOID usbGetReportDescriptor(VOID);

/**
Receive Out-report from host
*/
VOID usbSetReport(VOID);

/**
Return In-report or In-feature-report to host over interrupt endpoint
*/
VOID usbGetReport(VOID);

#ifdef __cplusplus
}
#endif
#endif //_UsbHidReq_H_
