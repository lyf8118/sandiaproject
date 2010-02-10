// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC/HID Driver)                |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbIsr.c, v1.12 2009/05/26                                         |
|  Author: Rostyslav Stolyar                                                  |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  R.Stolyar    2008/09/03   born                                             |
|  R.Stolyar    2008/12/23   enhancements of CDC API                          |
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| Include files                                                               |
+----------------------------------------------------------------------------*/

#include "..\Common\device.h"
#include "..\Common\types.h"              // Basic Type declarations
#include "..\USB_Common\defMSP430USB.h"
#include "..\USB_Common\descriptors.h"
#include "..\USB_Common\usb.h"            // USB-specific Data Structures
#ifdef _HID_
  #include "..\USB_HID_API\HidReportHandler.h"
  #include "..\USB_HID_API\UsbHid.h"
#else
# ifdef _CDC_
    #include "..\USB_CDC_API\UsbCdc.h"
    #include "..\USB_HID_API\UsbHid.h"
# endif
#endif
#include "UsbIsr.h"
#include <string.h>

/*----------------------------------------------------------------------------+
| External Variables                                                          |
+----------------------------------------------------------------------------*/
extern BYTE  bFunctionSuspended;

extern __no_init tEDB0 tEndPoint0DescriptorBlock;
extern __no_init tEDB tInputEndPointDescriptorBlock[];
extern __no_init tEDB tOutputEndPointDescriptorBlock[];

extern volatile BYTE bHostAsksUSBData;
extern volatile BYTE bTransferInProgress;
extern volatile BYTE bSecondUartTxDataCounter[];
extern volatile PBYTE pbSecondUartTxData;

extern BYTE bStatusAction;
extern WORD wUsbEventMask;

BOOL CdcToHostFromBuffer();
BOOL CdcToBufferFromHost();
BOOL IsReceiveInProgress();

BOOL HidToHostFromBuffer();
BOOL HidToBufferFromHost();


/*----------------------------------------------------------------------------+
| General Subroutines                                                         |
+----------------------------------------------------------------------------*/

#pragma vector=USB_UBM_VECTOR
__interrupt VOID iUsbInterruptHandler(VOID)
{
  BYTE bWakeUp = FALSE;
  switch (__even_in_range(USBVECINT & 0x3f, USBVECINT_OUTPUT_ENDPOINT7))
    {
    case USBVECINT_NONE:
      break;
    case USBVECINT_PWR_DROP:
      __no_operation();
      break;
    case USBVECINT_PLL_LOCK:
      break;
    case USBVECINT_PLL_SIGNAL:
      break;
    case USBVECINT_PLL_RANGE:
      if (wUsbEventMask & kUSB_clockFaultEvent)
      {
          bWakeUp = USB_handleClockEvent();
      }
      break;
    case USBVECINT_PWR_VBUSOn:
      PWRVBUSonHandler();
      if (wUsbEventMask & kUSB_VbusOnEvent)
      {
          bWakeUp = USB_handleVbusOnEvent();
      }
      break;
    case USBVECINT_PWR_VBUSOff:
      PWRVBUSoffHandler();
      if (wUsbEventMask & kUSB_VbusOffEvent)
      {
          bWakeUp = USB_handleVbusOffEvent();
      }
      break;
    case USBVECINT_USB_TIMESTAMP:
      break;
    case USBVECINT_INPUT_ENDPOINT0:
      IEP0InterruptHandler();
      break;
    case USBVECINT_OUTPUT_ENDPOINT0:
      OEP0InterruptHandler();
      break;
    case USBVECINT_RSTR:
      USB_reset();
      if (wUsbEventMask & kUSB_UsbResetEvent)
      {
          bWakeUp = USB_handleResetEvent();
      }
      break;
    case USBVECINT_SUSR:
      USB_suspend();
      if (wUsbEventMask & kUSB_UsbSuspendEvent)
      {
          bWakeUp = USB_handleSuspendEvent();
      }
      break;
    case USBVECINT_RESR:
      USB_resume();
      if (wUsbEventMask & kUSB_UsbResumeEvent)
      {
          bWakeUp = USB_handleResumeEvent();
      }
      //after resume we will wake up! Independ what event handler says.
      bWakeUp = TRUE;

      break;
    case USBVECINT_SETUP_PACKET_RECEIVED:

      // NAK both IEP and OEP enpoints
      tEndPoint0DescriptorBlock.bIEPBCNT = EPBCNT_NAK;
      tEndPoint0DescriptorBlock.bOEPBCNT = EPBCNT_NAK;
      SetupPacketInterruptHandler();
      break;

    case USBVECINT_STPOW_PACKET_RECEIVED:
      break;

    case USBVECINT_INPUT_ENDPOINT1:
        #ifdef _HID_
            //Transmit_HID_Report();
            bWakeUp = HidToHostFromBuffer();
        #endif  // _HID_
        break;

    case USBVECINT_INPUT_ENDPOINT2:
      break;

    case USBVECINT_INPUT_ENDPOINT3:
      #ifdef _CDC_
          //send saved bytes from buffer...
          bWakeUp = CdcToHostFromBuffer();
      #endif
      break;

    case USBVECINT_INPUT_ENDPOINT4:
      break;
    case USBVECINT_INPUT_ENDPOINT5:
      break;
    case USBVECINT_INPUT_ENDPOINT6:
      break;
    case USBVECINT_INPUT_ENDPOINT7:
      break;

    case USBVECINT_OUTPUT_ENDPOINT1:
      #ifdef _HID_

          //call callback function if no receive operation is underway
          if (!IsReceiveInProgress())
          {
              if (wUsbEventMask & kUSB_dataReceivedEvent)
              {
                  bWakeUp = USBHID_handleDataReceived(1);
              }
          }
          else
          {
              //complete receive opereation - copy data to user buffer
              bWakeUp = HidToBufferFromHost();
          }
      #endif  // _HID_
      break;

    case USBVECINT_OUTPUT_ENDPOINT2:
      break;

    case USBVECINT_OUTPUT_ENDPOINT3:
      #ifdef _CDC_
          //call callback function if no receive operation is underway
          if (!IsReceiveInProgress())
          {
              if (wUsbEventMask & kUSB_dataReceivedEvent)
              {
                  bWakeUp = USBCDC_handleDataReceived(1);
              }
          }
          else
          {
              //complete receive opereation - copy data to user buffer
              bWakeUp = CdcToBufferFromHost();
          }
      #endif
      break;

    case USBVECINT_OUTPUT_ENDPOINT4:
      break;
    case USBVECINT_OUTPUT_ENDPOINT5:
      break;
    case USBVECINT_OUTPUT_ENDPOINT6:
      break;
    case USBVECINT_OUTPUT_ENDPOINT7:
      break;
    default:
      break;
    }
    if (bWakeUp)
    {
        __bic_SR_register_on_exit(LPM3_bits); // Exit LPM3
        __no_operation();                     // Required for debugger
    }
}

/*----------------------------------------------------------------------------+
| Interrupt Sub-routines                                                      |
+----------------------------------------------------------------------------*/

VOID SetupPacketInterruptHandler(VOID)
{
    BYTE bTemp;

    //USBCTL |= FRSTE;      // Function Reset Connection Enable - execute after first setup packet was received

    usbProcessNewSetupPacket:

    // copy the MSB of bmRequestType to DIR bit of USBCTL
    if((tSetupPacket.bmRequestType & USB_REQ_TYPE_INPUT) == USB_REQ_TYPE_INPUT)
    {
        USBCTL |= DIR;
    }
    else
    {
        USBCTL &= ~DIR;
    }

    bStatusAction = STATUS_ACTION_NOTHING;

    // clear out return data buffer
    for(bTemp=0; bTemp<USB_RETURN_DATA_LENGTH; bTemp++)
    {
        abUsbRequestReturnData[bTemp] = 0x00;
    }

    // decode and process the request
    usbDecodeAndProcessUsbRequest();

    // check if there is another setup packet pending
    // if it is, abandon current one by NAKing both data endpoint 0
    if((USBIFG & STPOWIFG) != 0x00)
    {
        USBIFG &= ~(STPOWIFG | SETUPIFG);
        goto usbProcessNewSetupPacket;
    }
}

//----------------------------------------------------------------------------
VOID PWRVBUSoffHandler(VOID)
{
    volatile unsigned int i;
    for (i =0; i < 1000; i++);
    if (!(USBPWRCTL & USBBGVBV))
    {
        USBKEYPID   =    0x9628;        // set KEY and PID to 0x9628 -> access to configuration registers enabled
        bEnumerationStatus = 0x00;      // device is not enumerated
        bFunctionSuspended = FALSE;     // device is not suspended
        USBCNF     =    0;              // disable USB module
        USBPLLCTL   =    0;             // disable PLL
        USBPWRCTL &= ~(VBOFFIE + VBOFFIFG);          // disable interrupt VBUSoff
        USBKEYPID   =    0x9600;        // access to configuration registers disabled
    }
}

//----------------------------------------------------------------------------

VOID PWRVBUSonHandler(VOID)
{
    volatile unsigned int i;
    for (i =0; i < 1000; i++);          // waiting till voltage will be stable

    USBKEYPID =  0x9628;                // set KEY and PID to 0x9628 -> access to configuration registers enabled
    USBPWRCTL |= VBOFFIE;               // enable interrupt VBUSoff
    USBPWRCTL &= ~ (VBONIFG + VBOFFIFG);             // clean int flag (bouncing)
    USBKEYPID =  0x9600;                // access to configuration registers disabled
}

//----------------------------------------------------------------------------
VOID IEP0InterruptHandler(VOID)
{
    tEndPoint0DescriptorBlock.bOEPBCNT = 0x00;

    if(bStatusAction == STATUS_ACTION_DATA_IN)
    {
      usbSendNextPacketOnIEP0();
    }
    else
    {
      tEndPoint0DescriptorBlock.bIEPCNFG |= EPCNF_STALL; // no more data
    }
}

//----------------------------------------------------------------------------
VOID OEP0InterruptHandler(VOID)
{
    tEndPoint0DescriptorBlock.bIEPBCNT = 0x00;     // will be set by the hardware

    if(bStatusAction == STATUS_ACTION_DATA_OUT)
    {
        usbReceiveNextPacketOnOEP0();
        if(bStatusAction == STATUS_ACTION_NOTHING)
        {
            switch(tSetupPacket.bRequest)
            {
            #ifdef _CDC_
                case USB_CDC_SET_LINE_CODING:
                    Handler_SetLineCoding();
                    break;
            #endif // _CDC_
            default:;
            }
        }
    }
    else
    {
        tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL; // no more data
    }
}

/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
