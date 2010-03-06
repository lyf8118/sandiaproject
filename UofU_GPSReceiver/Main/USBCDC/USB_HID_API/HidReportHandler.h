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

#ifndef _HidReportHandler_H_
#define _HidReportHandler_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
Parses setup packet for report id and call function which belongs to report id
  Called function will generate InReport.
*/
VOID Handler_InReport(VOID);

/**
Parses setup packet for report id and call function which belongs to report id
Called function will generate InFeatureReport.
*/
VOID Handler_InFeature(VOID);

/**
Parses setup packet for report id and call function which belongs to report id.
*/
VOID Handler_OutReport(VOID);

/**
Parses setup packet for report id and call function which belongs to report id.
*/
VOID Handler_OutFeature(VOID);

#ifdef __cplusplus
}
#endif
#endif //_HidReportHandler_H_
