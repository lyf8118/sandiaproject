// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: usb.c, v1.12 2009/05/26                                            |
|  Author: Rostyslav Stolyar                                                  |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  R.Stolyar    2008/09/03   born                                             |
|  R.Stolyar    2008/12/23   enhancements of CDC API                          |
|  R.Stolyar    2009/01/12   enhancements for USB serial number               |
|  R.Stolyar    2009/05/15   added USB_connectionState()                      |
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| Include files                                                               |
+----------------------------------------------------------------------------*/

#include "..\Common\device.h"
#include "..\Common\types.h"              // Basic Type declarations
#include "..\USB_Common\defMSP430USB.h"
#include "..\USB_Common\descriptors.h"
#include "..\USB_Common\usb.h"            // USB-specific Data Structures
#ifdef _CDC_
	#include "..\USB_CDC_API\UsbCdc.h"
#else
#ifdef _HID_
	#include "..\USB_HID_API\UsbHidReq.h"
# endif
# endif
#include "..\Common\hal_UCS.h"
#include "..\Common\TLV_descriptors.h"

/*----------------------------------------------------------------------------+
| Internal Constant Definition                                                |
+----------------------------------------------------------------------------*/
#define NO_MORE_DATA 0xFFFF

/*----------------------------------------------------------------------------+
| Internal Variables                                                          |
+----------------------------------------------------------------------------*/

static BYTE bConfigurationNumber;      // Set to 1 when USB device has been
                                // configured, set to 0 when unconfigured

static BYTE bInterfaceNumber;   // interface number

WORD wBytesRemainingOnIEP0;     // For endpoint zero transmitter only
                                // Holds count of bytes remaining to be
                                // transmitted by endpoint 0.  A value
                                // of 0 means that a 0-length data packet
                                // A value of 0xFFFF means that transfer
                                // is complete.

WORD wBytesRemainingOnOEP0;     // For endpoint zero transmitter only
                                // Holds count of bytes remaining to be
                                // received by endpoint 0.  A value
                                // of 0 means that a 0-length data packet
                                // A value of 0xFFFF means that transfer
                                // is complete.

static PBYTE pbIEP0Buffer;      // A buffer pointer to input end point 0
                                // Data sent back to host is copied from
                                // this pointed memory location

static PBYTE pbOEP0Buffer;      // A buffer pointer to output end point 0
                                // Data sent from host is copied to
                                // this pointed memory location


static BYTE bHostAskMoreDataThanAvailable=0;

BYTE abUsbRequestReturnData[USB_RETURN_DATA_LENGTH];
BYTE abUsbRequestIncomingData[USB_RETURN_DATA_LENGTH];


__no_init BYTE abramSerialStringDescriptor[34];

BYTE bStatusAction;

BYTE bFunctionSuspended=FALSE;  // TRUE if function is suspended
BYTE bEnumerationStatus = 0;    //is 0 if not enumerated

static BYTE bRemoteWakeup;

WORD wUsbEventMask;             //used by USB_getEnabledEvents() and USB_setEnabledEvents()


/*----------------------------------------------------------------------------+
| Global Variables                                                            |
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| Hardware Related Structure Definition                                       |
+----------------------------------------------------------------------------*/

#ifdef __IAR_SYSTEMS_ICC__

#pragma location = 0x2380
__no_init tDEVICE_REQUEST tSetupPacket;

#pragma location = 0x0920
__no_init tEDB0 tEndPoint0DescriptorBlock;

#pragma location = 0x23C8
__no_init tEDB tInputEndPointDescriptorBlock[7];

#pragma location = 0x2388
__no_init tEDB tOutputEndPointDescriptorBlock[7];

#pragma location = 0x2378
__no_init BYTE abIEP0Buffer[EP0_MAX_PACKET_SIZE];

#pragma location = 0x2370
__no_init BYTE abOEP0Buffer[EP0_MAX_PACKET_SIZE];

#pragma location = OEP1_X_BUFFER_ADDRESS
 __no_init BYTE pbXBufferAddressEp1[EP_MAX_PACKET_SIZE];

#pragma location = OEP1_Y_BUFFER_ADDRESS
 __no_init BYTE pbYBufferAddressEp1[EP_MAX_PACKET_SIZE];

#pragma location = IEP1_X_BUFFER_ADDRESS
 __no_init BYTE pbXBufferAddressEp81[EP_MAX_PACKET_SIZE];

#pragma location = IEP1_Y_BUFFER_ADDRESS
 __no_init BYTE pbYBufferAddressEp81[EP_MAX_PACKET_SIZE];

#pragma location = OEP2_X_BUFFER_ADDRESS
 __no_init BYTE pbXBufferAddressEp2[EP_MAX_PACKET_SIZE];

#pragma location = OEP2_Y_BUFFER_ADDRESS
 __no_init BYTE pbYBufferAddressEp2[EP_MAX_PACKET_SIZE];

#pragma location = IEP2_X_BUFFER_ADDRESS
 __no_init BYTE pbXBufferAddressEp82[EP_MAX_PACKET_SIZE];

#pragma location = IEP2_Y_BUFFER_ADDRESS
 __no_init BYTE pbYBufferAddressEp82[EP_MAX_PACKET_SIZE];

#pragma location = OEP3_X_BUFFER_ADDRESS
__no_init BYTE pbXBufferAddressEp3[EP_MAX_PACKET_SIZE];


#endif


#ifdef __TI_COMPILER_VERSION__
extern __no_init tDEVICE_REQUEST tSetupPacket;
extern __no_init tEDB0 tEndPoint0DescriptorBlock;
extern __no_init tEDB tInputEndPointDescriptorBlock[7];
extern __no_init tEDB tOutputEndPointDescriptorBlock[7];
extern __no_init BYTE abIEP0Buffer[EP0_MAX_PACKET_SIZE];
extern __no_init BYTE abOEP0Buffer[EP0_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp1[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp1[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp81[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp81[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp2[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp2[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp82[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbYBufferAddressEp82[EP_MAX_PACKET_SIZE];
extern __no_init BYTE pbXBufferAddressEp3[EP_MAX_PACKET_SIZE];
#endif

VOID CdcResetData();
VOID HidResetData();
VOID USB_InitSerialStringDescriptor(VOID);
VOID USB_initMemcpy(VOID);

//----------------------------------------------------------------------------


BYTE USB_init(VOID)
{
    __disable_interrupt();               // Disable global interrupts

    // configuration of USB module
    USBKEYPID   =     0x9628;            // set KEY and PID to 0x9628 -> access to configuration registers enabled

    USBPHYCTL   =     PUSEL;             // use DP and DM as USB terminals (not needed because an external PHY is connected to port 9)
    USBPWRCTL   =     VUSBEN + SLDOEN + SLDOAON; // enable primary and secondary LDO (3.3 and 1.8 V)

    {
    	volatile unsigned int i;
    	for (i =0; i < 1000; i++);       // wait some time for LDOs
    }

    USBPWRCTL   =   VUSBEN + SLDOEN + SLDOAON + VBONIE;  // enable interrupt VBUSon
    USBKEYPID   =    0x9600;            // access to configuration registers disabled

    //reset events mask
    wUsbEventMask = 0;

    //init Serial Number
#if (USB_STR_INDEX_SERNUM != 0)
    USB_InitSerialStringDescriptor();
#endif

    // init memcpy() function: DMA or non-DMA
    USB_initMemcpy();

    __enable_interrupt();                // enable global interrupts
    return kUSB_succeed;
}

//----------------------------------------------------------------------------
// This function will be compiled only if
#if (USB_STR_INDEX_SERNUM != 0)
VOID USB_InitSerialStringDescriptor(VOID)
{
    BYTE i,j,hexValue;
    PBYTE pbSerNum;
    BYTE bBytes;

    j=0;
    abramSerialStringDescriptor[j++] = 34;
    abramSerialStringDescriptor[j++] = DESC_TYPE_STRING;

    // TLV access Function Call
    Get_TLV_info(TLV_DIERECORD, &bBytes, (UINT*)&pbSerNum); //The die record used for serial number
    if (bBytes == 0)    // no serial number available
    {
        // use 00 as serial number = no serial number available
        abramSerialStringDescriptor[0] = 4;      //length
        abramSerialStringDescriptor[j++] = 0;    // no serial number available
        abramSerialStringDescriptor[j++] = 0;    // no serial number available
    }
    else
    {
        for(i=0; (i<bBytes)&&(i<8); i++,pbSerNum++)
        {
            hexValue = (*pbSerNum & 0xF0)>> 4;
            if(hexValue < 10 ) abramSerialStringDescriptor[j++] = (hexValue + '0');
            else abramSerialStringDescriptor[j++] = (hexValue + 55);
            abramSerialStringDescriptor[j++] = 0x00;  // needed for UNI-Code

            hexValue = (*pbSerNum & 0x0F);
            if(hexValue < 10 ) abramSerialStringDescriptor[j++] = (hexValue + '0');
            else abramSerialStringDescriptor[j++] = (hexValue + 55);
            abramSerialStringDescriptor[j++] = 0x00;    // needed for UNI-Code
        }
    }
}
#endif

//----------------------------------------------------------------------------

BYTE USB_enable()
{
    volatile unsigned int i;
    volatile unsigned int j = 0;

    if (!(USBPWRCTL & USBBGVBV))            // check USB Bandgap and VBUS valid
    {
        return kUSB_generalError;
    }

    USBKEYPID = 0x9628;                     // set KEY and PID to 0x9628 -> access to configuration registers enabled

    XT2_Start();
    USBPLLDIVB = USB_XT_FREQ;                      // Settings desired frequency

    if (USB_PLL_XT == 2)
    {
        USBPLLCTL = UPCS0 + UPFDEN + UPLLEN;// Select XT2 as Ref / Select PLL for USB / Discrim. on, enable PLL
    }
    else
    {
        USBPLLCTL = UPFDEN + UPLLEN;        // Select XT1 as Ref / Select PLL for USB / Discrim. on, enable PLL
    }

    //Wait some time till PLL is settled
    do {
        USBPLLIR    =     0x0000;           // make sure no interrupts can occur on PLL-module

        //wait 2 ms till enable USB
        for (i =0; i < USB_MCLK_FREQ/1000*2/10 /*1000*/; i++);
        if (j++ > 10)
        {
            USBKEYPID   =    0x9600;        // access to configuration registers disabled
            return kUSB_generalError;
        }
    }while (USBPLLIR != 0);

    USBCNF     |=    USB_EN;                // enable USB module
    USBKEYPID   =    0x9600;                // access to configuration registers disabled
    return kUSB_succeed;
}

/*
Disables the USB module and PLL.
*/
BYTE USB_disable(VOID)
{
    USBKEYPID = 0x9628;        // set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBCNF    = 0;             // disable USB module
    USBPLLCTL = 0;             // disable PLL
    USBKEYPID = 0x9600;        // access to configuration registers disabled
    bEnumerationStatus = 0x00; // device is not enumerated
    bFunctionSuspended = FALSE;// device is not suspended
    return kUSB_succeed;
}

/*
Enables/disables various USB events.
*/
BYTE USB_setEnabledEvents(WORD events)
{
    wUsbEventMask = events;
    return kUSB_succeed;
}

/*
Returns which events are enabled and which are disabled.
*/
WORD USB_getEnabledEvents()
{
    return wUsbEventMask;
}

/*
Reset USB-SIE and global variables.
*/
BYTE USB_reset()
{
    USBKEYPID = 0x9628;                   // set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBCTL = FRSTE;                       // Function Reset Connection Enable

    bEnumerationStatus = 0x00;            // Device not enumerated yet
    bFunctionSuspended = FALSE;           // Device is not in suspend mode

    bRemoteWakeup = DISABLE;

    bConfigurationNumber    = 0x00;       // device unconfigured
    bInterfaceNumber        = 0x00;

    USBCTL = 0;                           // Function Reset Connection disable (FRSTE)

    wBytesRemainingOnIEP0   = NO_MORE_DATA;
    wBytesRemainingOnOEP0   = NO_MORE_DATA;
    bStatusAction           = STATUS_ACTION_NOTHING;

    USBFUNADR   =     0x00;               // reset address of USB device (unconfigured)

    /* Set settings for EP0 */
    // NAK both 0 endpoints and enable endpoint 0 interrupt
    tEndPoint0DescriptorBlock.bIEPBCNT = EPBCNT_NAK;
    tEndPoint0DescriptorBlock.bOEPBCNT = EPBCNT_NAK;
    tEndPoint0DescriptorBlock.bIEPCNFG = EPCNF_USBIE | EPCNF_UBME | EPCNF_STALL;    // 8 byte data packet
    tEndPoint0DescriptorBlock.bOEPCNFG = EPCNF_USBIE | EPCNF_UBME | EPCNF_STALL;    // 8 byte data packet

    USBOEPIE = BIT0 | BIT1 | BIT3;                // enable EP0, and EP3 output IRQ
    USBIEPIE = BIT0 | BIT1 | BIT2 | BIT3;         // enable EP0, EP1, EP2 and EP3 input IRQ


  #ifdef _HID_
    /* Set settings for IEP1 */
    // enable endpoint 1 interrupt, input
    tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPCNF   = EPCNF_USBIE | EPCNF_UBME | EPCNF_DBUF; //double buffering
    tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPBBAX  = (BYTE)(((IEP1_X_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3) & 0x00ff);
    tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPBBAY  = (BYTE)(((IEP1_Y_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3) & 0x00ff);
    tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPBCTX  = EPBCNT_NAK;
    tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPBCTY  = EPBCNT_NAK;
    tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPSIZXY = MAX_PACKET_SIZE;

    /* Set settings for OEP1 */
    // enable endpoint 1 interrupt, output
    tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPCNF   = EPCNF_USBIE | EPCNF_UBME | EPCNF_DBUF ; //double buffering
    tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBBAX  = (BYTE)(((OEP1_X_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3) & 0x00ff);
    tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBBAY  = (BYTE)(((OEP1_Y_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3) & 0x00ff);
    tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX  = 0x00;
    tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY  = 0x00; // EPBCT_NAK for one buffer (no double)
    tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPSIZXY = MAX_PACKET_SIZE;

    HidResetData();                     // reset CDC specific data structures
  #endif // _HID_

  #ifdef _CDC_
    /* Set settings for IEP2 */
    // enable endpoint 2 interrupt, input
    tInputEndPointDescriptorBlock[EDB(CDC_INTEP_ADDR)].bEPCNF   = EPCNF_USBIE       // use Interrupt for this EP
                                                | EPCNF_UBME      // enable this EP
                                                | EPCNF_DBUF;     // use double buffering (X and Y buffers)
    tInputEndPointDescriptorBlock[EDB(CDC_INTEP_ADDR)].bEPBBAX  = (BYTE)(((IEP2_X_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3) & 0x00ff);
    tInputEndPointDescriptorBlock[EDB(CDC_INTEP_ADDR)].bEPBBAY  = (BYTE)(((IEP2_Y_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3) & 0x00ff);
    tInputEndPointDescriptorBlock[EDB(CDC_INTEP_ADDR)].bEPBCTX  = EPBCNT_NAK;       // NAK: the buffer has no valid data to send to host
    tInputEndPointDescriptorBlock[EDB(CDC_INTEP_ADDR)].bEPBCTY  = EPBCNT_NAK;       // NAK: the buffer has no valid data to send to host
    tInputEndPointDescriptorBlock[EDB(CDC_INTEP_ADDR)].bEPSIZXY = MAX_PACKET_SIZE;

    /* Set settings for IEP3 */
    // enable endpoint 3 interrupt, input
    tInputEndPointDescriptorBlock[EDB(CDC_INEP_ADDR)].bEPCNF   = EPCNF_USBIE       // use Interrupt for this EP
                                                | EPCNF_UBME      // enable this EP
                                                | EPCNF_DBUF;     // use double buffering (X and Y buffers)
    tInputEndPointDescriptorBlock[EDB(CDC_INEP_ADDR)].bEPBBAX  = (BYTE)(((IEP3_X_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3) & 0x00ff);
    tInputEndPointDescriptorBlock[EDB(CDC_INEP_ADDR)].bEPBBAY  = (BYTE)(((IEP3_Y_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3) & 0x00ff);
    tInputEndPointDescriptorBlock[EDB(CDC_INEP_ADDR)].bEPBCTX  = EPBCNT_NAK;       // NAK: the buffer has no valid data to send to host
    tInputEndPointDescriptorBlock[EDB(CDC_INEP_ADDR)].bEPBCTY  = EPBCNT_NAK;       // NAK: the buffer has no valid data to send to host
    tInputEndPointDescriptorBlock[EDB(CDC_INEP_ADDR)].bEPSIZXY = MAX_PACKET_SIZE;

    /* Set settings for OEP3 */
    // enable endpoint 3 interrupt, output
    tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPCNF   = EPCNF_USBIE      // use Interrupt for this EP
                                                 | EPCNF_UBME     // enable this EP
                                                 | EPCNF_DBUF;    // use double buffering (X and Y buffers)
    tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBBAX  = (BYTE)(((OEP3_X_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3) & 0x00ff);
    tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBBAY  = (BYTE)(((OEP3_Y_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3) & 0x00ff);
    tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX  = 0x00;
    tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY  = 0x00;
    tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPSIZXY = MAX_PACKET_SIZE;

    CdcResetData();                     // reset CDC specific data structures

  #endif // _CDC_

    USBCTL = FEN;                       // enable function
    USBIFG = 0;                         // make sure no interrupts are pending

    USBIE = SETUPIE | RSTRIE | SUSRIE;  // enable USB specific interrupts (setup, reset, suspend)
    USBKEYPID = 0x9600;                 // access to configuration registers disabled
    return kUSB_succeed;
}

/*
Instruct USB module to make itself available to the PC for connection, by pulling PUR high.
*/
BYTE USB_connect()
{
    USBKEYPID = 0x9628;   // set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBCNF |= PUR_EN;     // generate rising edge on DP -> the host enumerates our device as full speed device
    USBPWRCTL |= VBOFFIE; // enable interrupt VUSBoff
    USBKEYPID = 0x9600;   // access to configuration registers disabled

    // after this the enumeration may take place
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();
    __no_operation();

    return kUSB_succeed;
}

/*
Force a disconnect from the PC by pulling PUR low.
*/
BYTE USB_disconnect()
{
    USBKEYPID = 0x9628;     // set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBCNF &= ~PUR_EN;      // disconnect pull up resistor - logical disconnect from HOST
    //USBCNF = 0;            // disable USB module
    USBPWRCTL &= ~VBOFFIE;  // disable interrupt VUSBoff
    USBKEYPID = 0x9600;     // access to configuration registers disabled
    bEnumerationStatus = 0; // not enumerated
    bFunctionSuspended = FALSE;     // device is not suspended
    return kUSB_succeed;
}

/*
Force a remote wakeup of the USB host.
*/
BYTE USB_forceRemoteWakeup()
{
    if (bFunctionSuspended == FALSE) // device is not suspended
    {
        return kUSB_NotSuspended;
    }
    if(bRemoteWakeup == ENABLE)
    {
    	volatile unsigned int i;
        USBCTL |= RWUP;             // USB - Device Remote Wakeup Request

        //RWUP will be not cleared automatically: wait and clear
        //USB2.0: The remote wakeup device must hold the resume signaling for at
        // least 1 ms but for no more than 15 ms (TDRSMUP).
        for (i =0; i < USB_MCLK_FREQ/1000*2 ; i++)
        {
            __no_operation();
        }

        USBCTL &= ~RWUP;            // clear RWUP
        return kUSB_succeed;
    }
    return kUSB_generalError;
}

/*
Returns the status of the USB connection.
*/
BYTE USB_connectionInfo()
{
    BYTE retVal = 0;
    if (USBPWRCTL & USBBGVBV)
    {
        retVal |= kUSB_vbusPresent;
    }

    if (bEnumerationStatus == ENUMERATION_COMPLETE)
    {
        retVal |= kUSB_Enumerated;
    }
    /*
    if () //sync frame are received
    {
        kUSB_busActive;
    } */

    if (USBCNF & PUR_EN)
    {
        retVal |= kUSB_purHigh;
    }

    if (bFunctionSuspended == TRUE)
    {
        retVal |= kUSB_suspended;
    }
    else
    {
        retVal |= kUSB_NotSuspended;
    }
    return retVal;
}

/*
Returns the state of the USB connection.
*/
BYTE USB_connectionState()
{
    // If no VBUS present
    if (!(USBPWRCTL & USBBGVBV))
    {
        return ST_USB_DISCONNECTED;
    }

    // If VBUS present, but PUR is low
    if ((USBPWRCTL & USBBGVBV)&&(!(USBCNF & PUR_EN)))
    {
        return ST_USB_CONNECTED_NO_ENUM;
    }

    // If VBUS present, PUR is high, but no enumeration yet
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (!(bEnumerationStatus == ENUMERATION_COMPLETE)))
    {
        return ST_ENUM_IN_PROGRESS;
    }

    // If VBUS present, PUR is high, and enumeration is complete, and not suspended
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (bEnumerationStatus == ENUMERATION_COMPLETE)
        && (!(bFunctionSuspended == TRUE)))
    {
        return ST_ENUM_ACTIVE;
    }

    // If VBUS present, PUR is high, and enumeration is complete, and  suspended
    if ((USBPWRCTL & USBBGVBV) && (USBCNF & PUR_EN)
        && (bEnumerationStatus == ENUMERATION_COMPLETE)
        && (bFunctionSuspended == TRUE))
    {
        return ST_ENUM_SUSPENDED;
    }

    return ST_ERROR;
}

//----------------------------------------------------------------------------

BYTE USB_suspend(VOID)
{
    bFunctionSuspended  = TRUE;
    USBKEYPID = 0x9628;         // set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBIFG &= ~SUSRIFG;         // clear interrupt flag

    if(USB_DISABLE_XT_SUSPEND)
    {
        if (USB_PLL_XT == 2)
        {
            USBPLLCTL = 0;      // disable PLL
            UCSCTL6 |= XT2OFF;
        }
        else
        {
            USBPLLCTL = 0;      // disable PLL
            UCSCTL6 |= XT1OFF;
        }
    }
    USBIE = RESRIE;             // disable USB specific interrupts (setup, reset, suspend), enable resume
    USBKEYPID = 0x9600;         // access to configuration registers disabled
    return kUSB_succeed;
}

//----------------------------------------------------------------------------

BYTE USB_resume(VOID)
{
    USB_enable();               // enable PLL

    USBIFG &= ~(RESRIFG | SUSRIFG);     // clear interrupt flags
    USBIE = SETUPIE | RSTRIE | SUSRIE;  // enable USB specific interrupts (setup, reset, suspend)

    bFunctionSuspended  = FALSE;
    return kUSB_succeed;
}

//----------------------------------------------------------------------------

VOID usbStallEndpoint0(VOID)
{
    tEndPoint0DescriptorBlock.bIEPCNFG |= EPCNF_STALL;
    tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;
}

//----------------------------------------------------------------------------

VOID usbClearOEP0ByteCount(VOID)
{
    tEndPoint0DescriptorBlock.bOEPBCNT = 0x00;
}

//----------------------------------------------------------------------------

VOID usbStallOEP0(VOID)
{
    // in standard USB request, there is not control write request with data stage
    // control write, stall output endpoint 0
    // wLength should be 0 in all cases
    tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;
}

//----------------------------------------------------------------------------

VOID usbSendNextPacketOnIEP0(VOID)
{
    BYTE bPacketSize,bIndex;

    // First check if there are bytes remaining to be transferred
    if(wBytesRemainingOnIEP0 != NO_MORE_DATA){
        if(wBytesRemainingOnIEP0 > EP0_PACKET_SIZE){
            // More bytes are remaining than will fit in one packet
            // there will be More IN Stage
            bPacketSize = EP0_PACKET_SIZE;
            wBytesRemainingOnIEP0 -= EP0_PACKET_SIZE;
            bStatusAction = STATUS_ACTION_DATA_IN;

        }else if (wBytesRemainingOnIEP0 < EP0_PACKET_SIZE){
            // The remaining data will fit in one packet.
            // This case will properly handle wBytesRemainingOnIEP0 == 0
            bPacketSize = (BYTE)wBytesRemainingOnIEP0;
            wBytesRemainingOnIEP0 = NO_MORE_DATA;        // No more data need to be Txed
            bStatusAction = STATUS_ACTION_NOTHING;

        }else{
            bPacketSize = EP0_PACKET_SIZE;
            if(bHostAskMoreDataThanAvailable == TRUE){
                wBytesRemainingOnIEP0 = 0;
                bStatusAction = STATUS_ACTION_DATA_IN;
            }else{
                wBytesRemainingOnIEP0 = NO_MORE_DATA;
                bStatusAction = STATUS_ACTION_NOTHING;
            }
        }

        for(bIndex=0; bIndex<bPacketSize; bIndex++)
        {
            abIEP0Buffer[bIndex] = *pbIEP0Buffer;
            pbIEP0Buffer++;
        }
        tEndPoint0DescriptorBlock.bIEPBCNT = bPacketSize;

    }else bStatusAction = STATUS_ACTION_NOTHING;

}

//----------------------------------------------------------------------------

VOID usbSendDataPacketOnEP0(PBYTE pbBuffer)
{
    WORD wTemp;

    pbIEP0Buffer = pbBuffer;
    wTemp = tSetupPacket.wLength;

    // Limit transfer size to wLength if needed
    // this prevent USB device sending 'more than require' data back to host
    if(wBytesRemainingOnIEP0 >= wTemp){
        wBytesRemainingOnIEP0 = wTemp;
        bHostAskMoreDataThanAvailable = FALSE;
    }else bHostAskMoreDataThanAvailable = TRUE;

    usbSendNextPacketOnIEP0();
}

//----------------------------------------------------------------------------
VOID usbReceiveNextPacketOnOEP0(VOID)
{
    BYTE bIndex,bByte;

    bByte = tEndPoint0DescriptorBlock.bOEPBCNT & EPBCNT_BYTECNT_MASK;

    if(wBytesRemainingOnOEP0 >= (WORD)bByte)
    {
        for(bIndex=0;bIndex<bByte;bIndex++)
        {
            *pbOEP0Buffer = abOEP0Buffer[bIndex];
            pbOEP0Buffer++;
        }
        wBytesRemainingOnOEP0 -= (WORD)bByte;

        // clear the NAK bit for next packet
        if(wBytesRemainingOnOEP0 > 0)
        {
            usbClearOEP0ByteCount();
            bStatusAction = STATUS_ACTION_DATA_OUT;
        }
        else
        {
            usbStallOEP0();
            bStatusAction = STATUS_ACTION_NOTHING;
        }
    }
    else
    {
        usbStallOEP0();
        bStatusAction = STATUS_ACTION_NOTHING;
    }
}

//----------------------------------------------------------------------------

VOID usbReceiveDataPacketOnEP0(PBYTE pbBuffer)
{

    pbOEP0Buffer = pbBuffer;

    wBytesRemainingOnOEP0 = tSetupPacket.wLength;
    bStatusAction = STATUS_ACTION_DATA_OUT;

    usbClearOEP0ByteCount();
}

//----------------------------------------------------------------------------

VOID usbSendZeroLengthPacketOnIEP0(VOID)
{
    wBytesRemainingOnIEP0 = NO_MORE_DATA;
    bStatusAction = STATUS_ACTION_NOTHING;
    tEndPoint0DescriptorBlock.bIEPBCNT = 0x00;
}

//----------------------------------------------------------------------------

VOID usbClearEndpointFeature(VOID)
{
    BYTE bEndpointNumber;

    // EP is from EP1 to EP7 while C language start from 0
    bEndpointNumber = (tSetupPacket.wIndex & EP_DESC_ADDR_EP_NUM);
    if(bEndpointNumber == 0x00) usbSendZeroLengthPacketOnIEP0();
    else{
        bEndpointNumber--;
        if(bEndpointNumber < MAX_ENDPOINT_NUMBER){
            if((tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN) == EP_DESC_ADDR_DIR_IN){
                // input endpoint
                tInputEndPointDescriptorBlock[bEndpointNumber].bEPCNF &= ~(EPCNF_STALL | EPCNF_TOGGLE);
            }else{
                // output endpoint
                tOutputEndPointDescriptorBlock[bEndpointNumber].bEPCNF &= ~(EPCNF_STALL | EPCNF_TOGGLE);
            }
            usbSendZeroLengthPacketOnIEP0();
        }
    }
}

//----------------------------------------------------------------------------

VOID usbGetConfiguration(VOID)
{
    usbClearOEP0ByteCount();                    // for status stage

    wBytesRemainingOnIEP0 = 1;
    usbSendDataPacketOnEP0((PBYTE)&bConfigurationNumber);
}

//----------------------------------------------------------------------------

VOID usbGetDeviceDescriptor(VOID)
{
    usbClearOEP0ByteCount();
    wBytesRemainingOnIEP0 = SIZEOF_DEVICE_DESCRIPTOR;
    usbSendDataPacketOnEP0((PBYTE) &abromDeviceDescriptor);
}

//----------------------------------------------------------------------------

VOID usbGetConfigurationDescriptor(VOID)
{
    usbClearOEP0ByteCount();
    wBytesRemainingOnIEP0 = sizeof(abromConfigurationDescriptorGroup);
    usbSendDataPacketOnEP0((PBYTE)&abromConfigurationDescriptorGroup);
}

//----------------------------------------------------------------------------

VOID usbGetStringDescriptor(VOID)
{
    WORD bIndex;
    BYTE bVal = (BYTE)tSetupPacket.wValue;

    usbClearOEP0ByteCount();                    // for status stage
#if (USB_STR_INDEX_SERNUM != 0)

    if(bVal == 0x03)
    {
        wBytesRemainingOnIEP0 = abramSerialStringDescriptor[0];
        usbSendDataPacketOnEP0((PBYTE)&abramSerialStringDescriptor);
    }
    else
#endif
    {
        bIndex = 0x00;
        while(bVal-- >  0x00) bIndex += abromStringDescriptor[bIndex];
        wBytesRemainingOnIEP0 = abromStringDescriptor[bIndex];
        usbSendDataPacketOnEP0((PBYTE)&abromStringDescriptor[bIndex]);
    }
}

//----------------------------------------------------------------------------

VOID usbGetInterface(VOID)
{

    // not fully supported, return one byte, zero
    usbClearOEP0ByteCount();                    // for status stage
    wBytesRemainingOnIEP0 = 0x02;
    abUsbRequestReturnData[0] = 0x00;           // changed to report alternative setting byte
    abUsbRequestReturnData[1] = bInterfaceNumber;
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
}

//----------------------------------------------------------------------------

VOID usbGetDeviceStatus(VOID)
{

    if((abromConfigurationDescriptorGroup[OFFSET_CONFIG_DESCRIPTOR_POWER] &
        CFG_DESC_ATTR_SELF_POWERED) == CFG_DESC_ATTR_SELF_POWERED)
        abUsbRequestReturnData[0] = DEVICE_STATUS_SELF_POWER;

    if(bRemoteWakeup == ENABLE)
        abUsbRequestReturnData[0] |= DEVICE_STATUS_REMOTE_WAKEUP;

    usbClearOEP0ByteCount();                    // for status stage

    // Return self power status and remote wakeup status
    wBytesRemainingOnIEP0 = 2;
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
}

//----------------------------------------------------------------------------

VOID usbGetInterfaceStatus(VOID)
{

    // check bIndexL for index number (not supported)
    usbClearOEP0ByteCount();                    // for status stage

    // Return two zero bytes
    wBytesRemainingOnIEP0 = 2;
    abUsbRequestReturnData[0] = 0x00;           // changed to support multiple interfaces
    abUsbRequestReturnData[1] = bInterfaceNumber;
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
}

//----------------------------------------------------------------------------

VOID usbGetEndpointStatus(VOID)
{
    BYTE bEndpointNumber;

    // Endpoint number is bIndexL
    bEndpointNumber = tSetupPacket.wIndex & EP_DESC_ADDR_EP_NUM;
    if(bEndpointNumber == 0x00){
        if((tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN) == EP_DESC_ADDR_DIR_IN){
            // input endpoint 0
            abUsbRequestReturnData[0] = (BYTE)(tEndPoint0DescriptorBlock.bIEPCNFG & EPCNF_STALL);
        }else{
            // output endpoint 0
            abUsbRequestReturnData[0] = (BYTE)(tEndPoint0DescriptorBlock.bOEPCNFG & EPCNF_STALL);
        }
        abUsbRequestReturnData[0] = abUsbRequestReturnData[0] >> 3; // STALL is on bit 3
        usbClearOEP0ByteCount();                    // for status stage
        wBytesRemainingOnIEP0 = 0x02;
        usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
    }else{
        bEndpointNumber--;
        // EP is from EP1 to EP7 while C language start from 0
        // Firmware should NOT response if specified endpoint is not supported. (charpter 8)
        if(bEndpointNumber < MAX_ENDPOINT_NUMBER){
            if(tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN){
                // input endpoint
                abUsbRequestReturnData[0] = (BYTE)(tInputEndPointDescriptorBlock[bEndpointNumber].bEPCNF & EPCNF_STALL);
            }else{
                // output endpoint
                abUsbRequestReturnData[0] = (BYTE)(tOutputEndPointDescriptorBlock[bEndpointNumber].bEPCNF & EPCNF_STALL);
            }
        }   // no response if endpoint is not supported.
        abUsbRequestReturnData[0] = abUsbRequestReturnData[0] >> 3; // STALL is on bit 3
        usbClearOEP0ByteCount();
        wBytesRemainingOnIEP0 = 0x02;
        usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);
    }
}

//----------------------------------------------------------------------------
VOID usbSetAddress(VOID)
{
    usbStallOEP0();                             // control write without data stage

    // bValueL contains device address
    if(tSetupPacket.wValue < 128){
        // hardware will update the address after status stage
        // therefore, firmware can set the address now.
        USBFUNADR = tSetupPacket.wValue;
        usbSendZeroLengthPacketOnIEP0();
    }else usbStallEndpoint0();
}

//----------------------------------------------------------------------------

VOID usbSetConfiguration(VOID)
{
    usbStallOEP0();                             // control write without data stage

    // configuration number is in bValueL
    // change the code if more than one configuration is supported
    bConfigurationNumber = tSetupPacket.wValue;
    usbSendZeroLengthPacketOnIEP0();

    if (bConfigurationNumber == 1)
    {
        bEnumerationStatus = ENUMERATION_COMPLETE;      // set device as enumerated
    }
    else
    {
        bEnumerationStatus = 0; //device is not configured == config # is zero
    }
}

//----------------------------------------------------------------------------

VOID usbClearDeviceFeature(VOID)
{
    // bValueL contains feature selector
    if(tSetupPacket.wValue == FEATURE_REMOTE_WAKEUP){
        bRemoteWakeup = DISABLE;
        usbSendZeroLengthPacketOnIEP0();
    }else usbStallEndpoint0();
}

//----------------------------------------------------------------------------

VOID usbSetDeviceFeature(VOID)
{
    // bValueL contains feature selector
    if(tSetupPacket.wValue == FEATURE_REMOTE_WAKEUP){
        bRemoteWakeup = ENABLE;
        usbSendZeroLengthPacketOnIEP0();
    }else usbStallEndpoint0();
}

//----------------------------------------------------------------------------

VOID usbSetEndpointFeature(VOID)
{
    BYTE bEndpointNumber;

    // wValue contains feature selector
    // bIndexL contains endpoint number
    // Endpoint number is in low byte of wIndex
    if(tSetupPacket.wValue == FEATURE_ENDPOINT_STALL){
        bEndpointNumber = tSetupPacket.wIndex & EP_DESC_ADDR_EP_NUM;
        if(bEndpointNumber == 0x00) usbSendZeroLengthPacketOnIEP0();  // do nothing for endpoint 0
        else{
            bEndpointNumber--;
            // Firmware should NOT response if specified endpoint is not supported. (charpter 8)
            if(bEndpointNumber < MAX_ENDPOINT_NUMBER){
                if(tSetupPacket.wIndex & EP_DESC_ADDR_DIR_IN){
                    // input endpoint
                    tInputEndPointDescriptorBlock[bEndpointNumber].bEPCNF |= EPCNF_STALL;
                }else{
                    // output endpoint
                    tOutputEndPointDescriptorBlock[bEndpointNumber].bEPCNF |= EPCNF_STALL;
                }
                usbSendZeroLengthPacketOnIEP0();
            } // no response if endpoint is not supported.
        }
    }else usbStallEndpoint0();
}

//----------------------------------------------------------------------------

VOID usbSetInterface(VOID)
{
    // bValueL contains alternative setting
    // bIndexL contains interface number
    // change code if more than one interface is supported
    usbStallOEP0();                             // control write without data stage
    bInterfaceNumber = tSetupPacket.wIndex;
    usbSendZeroLengthPacketOnIEP0();
}

//----------------------------------------------------------------------------

VOID usbInvalidRequest(VOID)
{
    // check if setup overwrite is set
    // if set, do nothing since we might decode it wrong
    // setup packet buffer could be modified by hardware if another setup packet
    // was sent while we are deocding setup packet
    if((USBIFG & STPOWIFG) == 0x00){
      usbStallEndpoint0();
    }
}

//----------------------------------------------------------------------------

static const tDEVICE_REQUEST_COMPARE tUsbRequestList[] =
{
  #ifdef _CDC_
    //---- CDC Class Requests -----//
    // GET LINE CODING
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
    USB_CDC_GET_LINE_CODING,
    0x00,0x00,                          // always zero
    INTERFACE_NUMBER_CDC,0x00,          // CDC interface is 0
    0x07,0x00,                          // Size of Structure (data length)
    0xff,&usbGetLineCoding,

    // SET LINE CODING
    USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
    USB_CDC_SET_LINE_CODING,
    0x00,0x00,                          // always zero
    INTERFACE_NUMBER_CDC,0x00,          // CDC interface is 0
    0x07,0x00,                          // Size of Structure (data length)
    0xff,&usbSetLineCoding,

    // SET CONTROL LINE STATE
    USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
    USB_CDC_SET_CONTROL_LINE_STATE,
    0xff,0xff,                          // Contains data
    INTERFACE_NUMBER_CDC,0x00,          // CDC interface is 0
    0x00,0x00,                          // No further data
    0xcf,&usbSetControlLineState,
  #endif // _CDC

  #ifdef _HID_
    //---- HID Class Requests -----//
    // GET REPORT
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
    USB_REQ_GET_REPORT,
    0xff,0xff,
    INTERFACE_NUMBER_HID,0x00,
    0xff,0xff,
    0xcc,&usbGetReport,

    // SET REPORT
    USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
    USB_REQ_SET_REPORT,
    0xff,0xFF,                          // bValueL is index and bValueH is type
    INTERFACE_NUMBER_HID,0x00,
    0xff,0xff,
    0xcc,&usbSetReport,

    // GET REPORT DESCRIPTOR
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
    USB_REQ_GET_DESCRIPTOR,
    0xff,DESC_TYPE_REPORT,              // bValueL is index and bValueH is type
    INTERFACE_NUMBER_HID,0x00,
    0xff,0xff,
    0xdc,&usbGetReportDescriptor,

    // GET HID DESCRIPTOR
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
    USB_REQ_GET_DESCRIPTOR,
    0xff,DESC_TYPE_HID,                 // bValueL is index and bValueH is type
    INTERFACE_NUMBER_HID,0x00,
    0xff,0xff,
    0xdc,&usbGetHidDescriptor,

  #endif // _HID_


    //---- USB Standard Requests -----//
    // clear device feature
    USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE,
    USB_REQ_CLEAR_FEATURE,
    FEATURE_REMOTE_WAKEUP,0x00,         // feature selector
    0x00,0x00,
    0x00,0x00,
    0xff,&usbClearDeviceFeature,

    // clear endpoint feature
    USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_ENDPOINT,
    USB_REQ_CLEAR_FEATURE,
    FEATURE_ENDPOINT_STALL,0x00,
    0xff,0x00,
    0x00,0x00,
    0xf7,&usbClearEndpointFeature,

    // get configuration
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE,
    USB_REQ_GET_CONFIGURATION,
    0x00,0x00,
    0x00,0x00,
    0x01,0x00,
    0xff,&usbGetConfiguration,

    // get device descriptor
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE,
    USB_REQ_GET_DESCRIPTOR,
    0xff,DESC_TYPE_DEVICE,              // bValueL is index and bValueH is type
    0xff,0xff,
    0xff,0xff,
    0xd0,&usbGetDeviceDescriptor,

    // get configuration descriptor
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE,
    USB_REQ_GET_DESCRIPTOR,
    0xff,DESC_TYPE_CONFIG,              // bValueL is index and bValueH is type
    0xff,0xff,
    0xff,0xff,
    0xd0,&usbGetConfigurationDescriptor,

    // get string descriptor
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE,
    USB_REQ_GET_DESCRIPTOR,
    0xff,DESC_TYPE_STRING,              // bValueL is index and bValueH is type
    0xff,0xff,
    0xff,0xff,
    0xd0,&usbGetStringDescriptor,

    // get interface
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
    USB_REQ_GET_INTERFACE,
    0x00,0x00,
    0xff,0xff,
    0x01,0x00,
    0xf3,&usbGetInterface,

    // get device status
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE,
    USB_REQ_GET_STATUS,
    0x00,0x00,
    0x00,0x00,
    0x02,0x00,
    0xff,&usbGetDeviceStatus,

    // get interface status
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
    USB_REQ_GET_STATUS,
    0x00,0x00,
    0xff,0x00,
    0x02,0x00,
    0xf7,&usbGetInterfaceStatus,

    // get endpoint status
    USB_REQ_TYPE_INPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_ENDPOINT,
    USB_REQ_GET_STATUS,
    0x00,0x00,
    0xff,0x00,
    0x02,0x00,
    0xf7,&usbGetEndpointStatus,

    // set address
    USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE,
    USB_REQ_SET_ADDRESS,
    0xff,0x00,
    0x00,0x00,
    0x00,0x00,
    0xdf,&usbSetAddress,

    // set configuration
    USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE,
    USB_REQ_SET_CONFIGURATION,
    0xff,0x00,
    0x00,0x00,
    0x00,0x00,
    0xdf,&usbSetConfiguration,

    // set device feature
    USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_DEVICE,
    USB_REQ_SET_FEATURE,
    0xff,0x00,                      // feature selector
    0x00,0x00,
    0x00,0x00,
    0xdf,&usbSetDeviceFeature,

    // set endpoint feature
    USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_ENDPOINT,
    USB_REQ_SET_FEATURE,
    0xff,0x00,                      // feature selector
    0xff,0x00,                      // endpoint number <= 127
    0x00,0x00,
    0xd7,&usbSetEndpointFeature,

    // set interface
    USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
    USB_REQ_SET_INTERFACE,
    0xff,0x00,                      // feature selector
    0xff,0x00,                      // interface number
    0x00,0x00,
    0xd7,&usbSetInterface,

    // end of usb descriptor -- this one will be matched to any USB request
    //                          since bCompareMask is 0x00.
    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
    0x00,&usbInvalidRequest     // end of list
};

typedef VOID (*tpF)(VOID);

VOID usbDecodeAndProcessUsbRequest(VOID)
{
    BYTE  bMask,bResult,bTemp;
    BYTE* pbUsbRequestList;
    //BYTE  abSetupPacketBuffer[8];
    ptDEVICE_REQUEST ptSetupPacket = &tSetupPacket;
    BYTE  bRequestType,bRequest;
    DWORD lAddrOfFunction;

    // point to beginning of the matrix
    pbUsbRequestList = (PBYTE)&tUsbRequestList[0];

    while(1){

        bRequestType = *pbUsbRequestList++;
        bRequest     = *pbUsbRequestList++;

        if(((bRequestType == 0xff) && (bRequest == 0xff)) ||
            (tSetupPacket.bmRequestType/*abSetupPacketBuffer[0]*/ == (USB_REQ_TYPE_INPUT | USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_DEVICE)) ||
            (tSetupPacket.bmRequestType/*abSetupPacketBuffer[0]*/ == (USB_REQ_TYPE_OUTPUT | USB_REQ_TYPE_VENDOR | USB_REQ_TYPE_DEVICE)))
        {
            pbUsbRequestList -= 2;
            break;
        }

        if((bRequestType == tSetupPacket.bmRequestType/*abSetupPacketBuffer[0]*/) && (bRequest == tSetupPacket.bRequest/*abSetupPacketBuffer[1]*/)){

            // compare the first two
            bResult = 0xc0;
            bMask   = 0x20;
            // first two bytes matched, compare the rest
            for(bTemp = 2; bTemp < 8; bTemp++)
            {
                if (*((BYTE*)ptSetupPacket + bTemp) == *pbUsbRequestList)
               // if(pSetupPacket[bTemp]/*abSetupPacketBuffer[bTemp]*/ == *pbUsbRequestList++)
                {
                    bResult |= bMask;
                }
                pbUsbRequestList++;
                bMask = bMask >> 1;
            }
            // now we have the result
            if((*pbUsbRequestList & bResult) == *pbUsbRequestList)
            {
                pbUsbRequestList -= 8;
                break;
            }
            else
            {
                pbUsbRequestList += (sizeof(tDEVICE_REQUEST_COMPARE)-8);
            }
        }
        else
        {
            pbUsbRequestList += (sizeof(tDEVICE_REQUEST_COMPARE)-2);
        }
    }

    // if another setup packet comes before we have the chance to process current
    // setup request, we return here without processing the request
    // this check is not necessary but still kept here to reduce response(or simulation) time

    if((USBIFG & STPOWIFG) != 0x00)
    {
        return;
    }

    // now we found the match and jump to the function accordingly.
    lAddrOfFunction = *((PDWORD)((pbUsbRequestList) + 10));

    // call function
    ((tpF)lAddrOfFunction)();
}

/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
