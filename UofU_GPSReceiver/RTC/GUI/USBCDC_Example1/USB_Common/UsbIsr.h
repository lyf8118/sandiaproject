// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbIsr.h, v1.12 2009/05/26                                         |
|  Author: Rostyslav Stolyar                                                  |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  R.Stolyar    2008/09/03   born                                             |
|  R.Stolyar    2008/12/23   enhancements of CDC API                          |
+----------------------------------------------------------------------------*/

#ifndef _ISR_H_
#define _ISR_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
Handle incoming setup packet.
*/
VOID SetupPacketInterruptHandler(VOID);

/**
Handle VBuss on signal.
*/
VOID PWRVBUSonHandler(VOID);

/**
Handle VBuss off signal.
*/
VOID PWRVBUSoffHandler(VOID);

/**
Handle In-requests from control pipe.
*/
VOID IEP0InterruptHandler(VOID);

/**
Handle Out-requests from control pipe.
*/
VOID OEP0InterruptHandler(VOID);

/*----------------------------------------------------------------------------+
| End of header file                                                          |
+----------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif /* _ISR_H_ */

/*------------------------ Nothing Below This Line --------------------------*/
