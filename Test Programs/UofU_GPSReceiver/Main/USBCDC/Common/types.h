// (c)2008 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: types.h, v1.12 2009/05/26                                          |
|  Author: Rostyslav Stolyar                                                  |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  R.Stolyar    2008/09/03   born                                             |
+----------------------------------------------------------------------------*/
#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __TI_COMPILER_VERSION__
#define __no_init
#define __ACCESS_20BIT_REG__  __SFR_FARPTR
#endif

/*----------------------------------------------------------------------------+
| Include files                                                               |
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| Function Prototype                                                          |
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| Type Definition & Macro                                                     |
+----------------------------------------------------------------------------*/
typedef char            CHAR;
typedef unsigned char   UCHAR;
typedef int             INT;
typedef unsigned int    UINT;
typedef short           SHORT;
typedef unsigned short  USHORT;
typedef long            LONG;
typedef unsigned long   ULONG;
typedef void            VOID;
typedef unsigned long   HANDLE;
typedef char *          PSTR;
typedef int             BOOL;
typedef double          DOUBLE;
typedef unsigned char   BYTE;
typedef unsigned char*  PBYTE;
typedef unsigned int    WORD;
typedef unsigned long   DWORD;
typedef unsigned long*  PDWORD;
#define VOID void

// DEVICE_REQUEST Structure
typedef struct _tDEVICE_REQUEST
{
    BYTE    bmRequestType;              // See bit definitions below
    BYTE    bRequest;                   // See value definitions below
    WORD    wValue;                     // Meaning varies with request type
    WORD    wIndex;                     // Meaning varies with request type
    WORD    wLength;                    // Number of bytes of data to transfer
} tDEVICE_REQUEST, *ptDEVICE_REQUEST;

typedef struct _tDEVICE_REQUEST_COMPARE
{
    BYTE    bmRequestType;              // See bit definitions below
    BYTE    bRequest;                   // See value definitions below
    BYTE    bValueL;                    // Meaning varies with request type
    BYTE    bValueH;                    // Meaning varies with request type
    BYTE    bIndexL;                    // Meaning varies with request type
    BYTE    bIndexH;                    // Meaning varies with request type
    BYTE    bLengthL;                   // Number of bytes of data to transfer (LSByte)
    BYTE    bLengthH;                   // Number of bytes of data to transfer (MSByte)
    BYTE    bCompareMask;               // MSB is bRequest, if set 1, bRequest should be matched
    VOID    (*pUsbFunction)(VOID);      // function pointer
} tDEVICE_REQUEST_COMPARE, *ptDEVICE_REQUEST_COMPARE;

//----------------------------------------------------------------------------
typedef enum
{
    STATUS_ACTION_NOTHING,
    STATUS_ACTION_DATA_IN,
    STATUS_ACTION_DATA_OUT
} tSTATUS_ACTION_LIST;

typedef enum
{
    DISABLE,
    ENABLE
} tSTATUS_EN_DISABLED;

typedef enum
{
    FALSE,
    TRUE
} tBOOL;

/*----------------------------------------------------------------------------+
| Constant Definition                                                         |
+----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------+
| End of header file                                                          |
+----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif /* _TYPES_H_ */
/*------------------------ Nothing Below This Line --------------------------*/
