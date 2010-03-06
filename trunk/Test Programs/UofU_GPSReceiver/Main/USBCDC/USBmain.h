// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: main.h, v1.10 2009/05/15                                           |
|  Author: Rostyslav Stolyar                                                  |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  R.Stolyar    2008/09/03   born                                             |
+----------------------------------------------------------------------------*/

#ifndef _MAIN_H_
#define _MAIN_H_

#ifdef __cplusplus
extern "C"
{
#endif


VOID Init_Ports(VOID);
VOID Init_Clocks(VOID);
BYTE retInString(char*);
VOID Init_TimerA1(VOID);

#ifdef __cplusplus
}
#endif
#endif /* _MAIN_H_ */
/*------------------------ Nothing Below This Line --------------------------*/

