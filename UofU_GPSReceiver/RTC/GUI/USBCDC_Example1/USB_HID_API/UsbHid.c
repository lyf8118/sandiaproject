// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (HID Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbHid.c, v1.12 2009/05/26                                         |
|  Author: Rostyslav Stolyar                                                  |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  R.Stolyar    2009/02/20   ported from CdcHid                               |
|  R.Stolyar    2009/05/19   updated USBHID_intfStatus()                      |
|  R.Stolyar    2009/05/26   added USBHID_bytesInUSBBuffer()                  |
+----------------------------------------------------------------------------*/

#include "..\Common\device.h"
#include "..\Common\types.h"              // Basic Type declarations
#include "..\USB_Common\defMSP430USB.h"
#include "..\USB_Common\usb.h"                // USB-specific Data Structures
#include "UsbHid.h"
#include "..\USB_Common\descriptors.h"
#include <string.h>

#ifdef _HID_

//function pointers
extern VOID *(*USB_TX_memcpy)(VOID * dest, const VOID * source, size_t count);
extern VOID *(*USB_RX_memcpy)(VOID * dest, const VOID * source, size_t count);



static struct _HidWrite
{
    WORD nHidBytesToSend;            // holds counter of bytes to be sent
    WORD nHidBytesToSendLeft;        // holds counter how many bytes is still to be sent
    const BYTE* pHidBufferToSend;    // holds the buffer with data to be sent
    BYTE bCurrentBufferXY;           // indicates which buffer is to use next for for write into IN OUT endpoint
} HidWriteCtrl;

static struct _HidRead
{
    BYTE *pUserBuffer;     // holds the current position of user's receiving buffer. If NULL- no receiving operation started
    BYTE *pCurrentEpPos;   // current positon to read of received data from curent EP
    WORD nBytesToReceive;  // holds how many bytes was requested by receiveData() to receive
    WORD nBytesToReceiveLeft;        // holds how many bytes is still requested by receiveData() to receive
    BYTE * pCT1;           // holds current EPBCTxx register
    BYTE * pCT2;           // holds next EPBCTxx register
    BYTE * pEP2;           // holds addr of the next EP buffer
    BYTE nBytesInEp;       // how many received bytes still available in current EP
    BYTE bCurrentBufferXY; // indicates which buffer is used by host to transmit data via OUT endpoint
} HidReadCtrl;

extern WORD wUsbEventMask;

/*----------------------------------------------------------------------------+
| Global Variables                                                            |
+----------------------------------------------------------------------------*/

extern __no_init tEDB tInputEndPointDescriptorBlock[];
extern __no_init tEDB tOutputEndPointDescriptorBlock[];


VOID HidCopyUsbToBuff(BYTE* pEP, BYTE* pCT);

/*----------------------------------------------------------------------------+
| Functions' implementatin                                                    |
+----------------------------------------------------------------------------*/

//resets internal HID data structure
VOID HidResetData()
{
    // indicates which buffer is used by host to transmit data via OUT endpoint3 - X buffer is first
    //HidReadCtrl.bCurrentBufferXY = X_BUFFER;

    memset(&HidReadCtrl, 0, sizeof(HidReadCtrl));
    memset(&HidWriteCtrl, 0, sizeof(HidWriteCtrl));
}


/*
Sends a pre-built report reportData to the host.
  Returns:  kUSBHID_sendComplete
            kUSBHID_intfBusyError
            kUSBCDC_busNotAvailable
*/
BYTE USBHID_sendReport(const BYTE * reportData, BYTE intfNum)
{
    BYTE byte_count;
    BYTE * pEP1;
    BYTE * pCT1;

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        return kUSBHID_busNotAvailable;
    }

    if (HidWriteCtrl.bCurrentBufferXY == X_BUFFER)
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)IEP1_X_BUFFER_ADDRESS;
        pCT1 = &tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPBCTX;
    }
    else
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)IEP1_Y_BUFFER_ADDRESS;
        pCT1 = &tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPBCTY;
    }

    byte_count = USBHID_REPORT_LENGTH;  // we support only one length of report

    if(*pCT1 & EPBCNT_NAK)              // if this EP is empty
    {
        USB_TX_memcpy(pEP1, reportData, byte_count);  // copy data into IEP3 X or Y buffer
        *pCT1 = byte_count;                           // Set counter for usb In-Transaction
        HidWriteCtrl.bCurrentBufferXY = (HidWriteCtrl.bCurrentBufferXY+1)&0x01; //switch buffer
        return kUSBHID_sendComplete;
    }
    return kUSBHID_intfBusyError;
}

/*
Receives report reportData from the host.
Return:     kUSBHID_receiveCompleted
            kUSBHID_receiveError
            kUSBCDC_busNotAvailable
*/
BYTE USBHID_receiveReport(BYTE * reportData, BYTE intfNum)
{
    BYTE ret = kUSBHID_receiveError;
    BYTE nTmp1 = 0;

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        return kUSBHID_busNotAvailable;
    }

    if (HidReadCtrl.bCurrentBufferXY == X_BUFFER)  //this is current buffer
    {
        if (tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX & EPBCNT_NAK) //this buffer has a valid data packet
        {
            //this is the active EP buffer
            //pEP1
            HidReadCtrl.pCurrentEpPos = (BYTE*)OEP1_X_BUFFER_ADDRESS;
            HidReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX;

            //second EP buffer
            HidReadCtrl.pEP2 = (BYTE*)OEP1_Y_BUFFER_ADDRESS;
            HidReadCtrl.pCT2 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY;
            nTmp1 = 1;    //indicate that data is available
        }
    }
    else // Y_BUFFER
    if (tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY & EPBCNT_NAK)
    {
        //this is the active EP buffer
        HidReadCtrl.pCurrentEpPos = (BYTE*)OEP1_Y_BUFFER_ADDRESS;
        HidReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY;

        //second EP buffer
        HidReadCtrl.pEP2 = (BYTE*)OEP1_X_BUFFER_ADDRESS;
        HidReadCtrl.pCT2 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX;
        nTmp1 = 1;    //indicate that data is available
    }

    if (nTmp1)
    {
        // how many byte we can get from one endpoint buffer
        nTmp1 = *HidReadCtrl.pCT1;

        if(nTmp1 & EPBCNT_NAK)
        {
            nTmp1 = nTmp1 &0x7f;            // clear NAK bit
            HidReadCtrl.nBytesInEp = nTmp1; // holds how many valids bytes in the EP buffer

            USB_RX_memcpy(reportData, HidReadCtrl.pCurrentEpPos, nTmp1);
            //memcpy(reportData, HidReadCtrl.pEP1, nTmp1);
            HidReadCtrl.bCurrentBufferXY = (HidReadCtrl.bCurrentBufferXY+1) &0x01;
            HidReadCtrl.nBytesInEp = 0;
            *HidReadCtrl.pCT1 = 0;          // clear NAK, EP ready to receive data

            ret = kUSBHID_receiveCompleted;
        }
    }
    return ret;
}


/*
Sends data over interface intfNum, of size size and starting at address data.
Returns: kUSBHID_sendStarted
         kUSBHID_sendComplete
         kUSBHID_intBusyError
*/
BYTE USBHID_sendData(const BYTE* data, WORD size, BYTE intfNum)
{
    if (size == 0)
    {
        return kUSBHID_sendComplete;
    }
    if (HidWriteCtrl.nHidBytesToSendLeft != 0)
    {
        // the USB still sends previous data, we have to wait
        return kUSBHID_intfBusyError;
    }

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        // data can not be read because of USB suspended
        return kUSBHID_busNotAvailable;
    }

    //This function generate the USB interrupt. The data will be sent out from interrupt

    // atomic operation - desable interrupts
    __disable_interrupt();               // Disable global interrupts
    HidWriteCtrl.nHidBytesToSend = size;
    HidWriteCtrl.nHidBytesToSendLeft = size;
    HidWriteCtrl.pHidBufferToSend = data;

    //trigger Endpoint Interrupt - to start send operation
    USBIEPIFG |= 1<<(EDB(HID_INEP_ADDR)+1);   //IEPIFGx;

    __enable_interrupt();                // enable global interrupts

    return kUSBHID_sendStarted;
}

//this function is used only by USB interrupt
BOOL HidToHostFromBuffer()
{
    BYTE byte_count, nTmp2;
    BYTE * pEP1;
    BYTE * pEP2;
    BYTE * pCT1;
    BYTE * pCT2;
    BYTE bWakeUp = FALSE; // per default we do not wake up after interrupt

    if (HidWriteCtrl.nHidBytesToSendLeft == 0)           // do we have somtething to send?
    {

        HidWriteCtrl.nHidBytesToSend = 0;

        //call event callback function
        if (wUsbEventMask & kUSB_sendCompletedEvent)
        {
            bWakeUp = USBHID_handleSendCompleted(1);
        }
        return bWakeUp;
    }

    if(!(tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPCNF & EPCNF_TOGGLE))
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)IEP1_X_BUFFER_ADDRESS;
        pCT1 = &tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPBCTX;

        //second EP buffer
        pEP2 = (BYTE*)IEP1_Y_BUFFER_ADDRESS;
        pCT2 = &tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPBCTY;
    }
    else
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)IEP1_Y_BUFFER_ADDRESS;
        pCT1 = &tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPBCTY;

        //second EP buffer
        pEP2 = (BYTE*)IEP1_X_BUFFER_ADDRESS;
        pCT2 = &tInputEndPointDescriptorBlock[EDB(HID_INEP_ADDR)].bEPBCTX;
    }

    // how many byte we can send over one endpoint buffer
    //  2 bytes a reserved: [0] - HID Report Descriptor, [1] - count of valid bytes
    byte_count = (HidWriteCtrl.nHidBytesToSendLeft > EP_MAX_PACKET_SIZE-2) ? EP_MAX_PACKET_SIZE-2 : HidWriteCtrl.nHidBytesToSendLeft;
    nTmp2 = *pCT1;

    if(nTmp2 & EPBCNT_NAK)
    {
        USB_TX_memcpy(pEP1+2, HidWriteCtrl.pHidBufferToSend, byte_count);  // copy data into IEP3 X or Y buffer
        pEP1[0] = 0x3F;                                     // set HID report descriptor: 0x3F
        pEP1[1] = byte_count;                               // set HID report descriptor

        // 64 bytes will be send: we use only one HID report descriptor
        *pCT1 = 0x40;                                       // Set counter for usb In-Transaction

        HidWriteCtrl.nHidBytesToSendLeft -= byte_count;
        HidWriteCtrl.pHidBufferToSend += byte_count;        // move buffer pointer

        //try to send data over second buffer
        nTmp2 = *pCT2;
        if ((HidWriteCtrl.nHidBytesToSendLeft > 0) &&       // do we have more data to send?
            (nTmp2 & EPBCNT_NAK)) // if the second buffer is free?
        {
            // how many byte we can send over one endpoint buffer
            byte_count = (HidWriteCtrl.nHidBytesToSendLeft > EP_MAX_PACKET_SIZE-2) ? EP_MAX_PACKET_SIZE-2 : HidWriteCtrl.nHidBytesToSendLeft;

            USB_TX_memcpy(pEP2+2, HidWriteCtrl.pHidBufferToSend, byte_count); // copy data into IEP3 X or Y buffer
            pEP2[0] = 0x3F;                                 // set HID report descriptor: 0x3F
            pEP2[1] = byte_count;                           // set HID report descriptor

            // 64 bytes will be send: we use only one HID report descriptor
            *pCT2 = 0x40;                                   // Set counter for usb In-Transaction

            HidWriteCtrl.nHidBytesToSendLeft -= byte_count;
            HidWriteCtrl.pHidBufferToSend += byte_count;    // move buffer pointer
        }
    }
    return bWakeUp;
}

/*
Aborts an active send operation on interface intfNum.
Returns the number of bytes that were sent prior to the abort, in size.
*/
BYTE USBHID_abortSend(WORD* size, BYTE intfNum)
{
    __disable_interrupt(); //disable interrupts - atomic operation

    *size = (HidWriteCtrl.nHidBytesToSend - HidWriteCtrl.nHidBytesToSendLeft);
    HidWriteCtrl.nHidBytesToSend = 0;
    HidWriteCtrl.nHidBytesToSendLeft = 0;

    __enable_interrupt();   //enable interrupts
    return kUSB_succeed;
}

// This function copies data from OUT endpoint into user's buffer
// Arguments:
//    pEP - pointer to EP to copy from
//    pCT - pointer to pCT control reg
//
VOID HidCopyUsbToBuff(BYTE* pEP, BYTE* pCT)
{
    BYTE nCount;

    // how many byte we can get from one endpoint buffer
    nCount = (HidReadCtrl.nBytesToReceiveLeft > HidReadCtrl.nBytesInEp) ? HidReadCtrl.nBytesInEp : HidReadCtrl.nBytesToReceiveLeft;

    USB_RX_memcpy(HidReadCtrl.pUserBuffer, pEP, nCount); // copy data from OEPx X or Y buffer
    HidReadCtrl.nBytesToReceiveLeft -= nCount;
    HidReadCtrl.pUserBuffer += nCount;          // move buffer pointer
                                                // to read rest of data next time from this place

    if (nCount == HidReadCtrl.nBytesInEp)       // all bytes are copied from receive buffer?
    {
        //switch current buffer
        HidReadCtrl.bCurrentBufferXY = (HidReadCtrl.bCurrentBufferXY+1) &0x01;

        HidReadCtrl.nBytesInEp = 0;

        //clear NAK, EP ready to receive data
        *pCT = 0;
    }
    else
    {
        // set correct rest_data available in EP buffer
        if (HidReadCtrl.nBytesInEp > nCount)
        {
            HidReadCtrl.nBytesInEp -= nCount;
            HidReadCtrl.pCurrentEpPos = pEP + nCount;
        }
        else
        {
            HidReadCtrl.nBytesInEp = 0;
        }
    }
}


/*
Receives data over interface intfNum, of size size, into memory starting at address data.
Returns:
    kUSBHID_receiveStarted  if the receiving process started.
    kUSBHID_receiveCompleted  all requested date are received.
    kUSBHID_receiveInProgress  previous receive opereation is in progress. The requested receive operation can be not started.
    kUSBHID_receiveError  error occurred.
*/
BYTE USBHID_receiveData(BYTE* data, WORD size, BYTE intfNum)
{
    BYTE nTmp1;

    if (HidReadCtrl.pUserBuffer != NULL)        // receive process already started
    {
        return kUSBHID_receiveInProgress;
    }

    if ((size == 0) ||                          // read size is 0
        (data == NULL))
    {
        return kUSBHID_receiveError;
    }

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        return kUSBHID_busNotAvailable;
    }

    __disable_interrupt();                      //interrupts disable

    HidReadCtrl.nBytesToReceive = size;         // bytes to receive
    HidReadCtrl.nBytesToReceiveLeft = size;     // left bytes to receive
    HidReadCtrl.pUserBuffer = data;             // set user receive buffer

    //read rest of data from buffer, if any
    if (HidReadCtrl.nBytesInEp > 0)
    {
        // copy data from pEP-endpoint into User's buffer
        HidCopyUsbToBuff(HidReadCtrl.pCurrentEpPos, HidReadCtrl.pCT1);

        if (HidReadCtrl.nBytesToReceiveLeft == 0)     // the Receive opereation is completed
        {
            HidReadCtrl.pUserBuffer = NULL;     // no more receiving pending
            USBHID_handleReceiveCompleted(1);      // call event handler in interrupt context
            __enable_interrupt();               // interrupts enable
            return kUSBHID_receiveCompleted;    // receive completed
        }

        // check other EP buffer for data - exchange pCT1 with pCT2
        if (HidReadCtrl.pCT1 == &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX)
        {
            HidReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY;
            HidReadCtrl.pCurrentEpPos = (BYTE*)OEP1_Y_BUFFER_ADDRESS;
        }
        else
        {
            HidReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX;
            HidReadCtrl.pCurrentEpPos = (BYTE*)OEP1_X_BUFFER_ADDRESS;
        }
        nTmp1 = *HidReadCtrl.pCT1;
        //try read data from second buffer
        if (nTmp1 & EPBCNT_NAK)                 // if the second buffer has received data?
        {
            nTmp1 = nTmp1 &0x7f;                // clear NAK bit
            HidReadCtrl.nBytesInEp = nTmp1;     // holds how many valids bytes in the EP buffer
            HidCopyUsbToBuff(HidReadCtrl.pCurrentEpPos, HidReadCtrl.pCT1);
        }

        if (HidReadCtrl.nBytesToReceiveLeft == 0)     // the Receive opereation is completed
        {
            HidReadCtrl.pUserBuffer = NULL;     // no more receiving pending
            USBHID_handleReceiveCompleted(1);      // call event handler in interrupt context
            __enable_interrupt();               // interrupts enable
            return kUSBHID_receiveCompleted;    // receive completed
        }
    } //read rest of data from buffer, if any

    //read 'fresh' data, if available
    nTmp1 = 0;
    if (HidReadCtrl.bCurrentBufferXY == X_BUFFER)  //this is current buffer
    {
        if (tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX & EPBCNT_NAK) //this buffer has a valid data packet
        {
            //this is the active EP buffer
            //pEP1
            HidReadCtrl.pCurrentEpPos = (BYTE*)OEP1_X_BUFFER_ADDRESS;
            HidReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX;

            //second EP buffer
            HidReadCtrl.pEP2 = (BYTE*)OEP1_Y_BUFFER_ADDRESS;
            HidReadCtrl.pCT2 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY;
            nTmp1 = 1;    //indicate that data is available
        }
    }
    else // Y_BUFFER
    if (tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY & EPBCNT_NAK)
    {
        //this is the active EP buffer
        HidReadCtrl.pCurrentEpPos = (BYTE*)OEP1_Y_BUFFER_ADDRESS;
        HidReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY;

        //second EP buffer
        HidReadCtrl.pEP2 = (BYTE*)OEP1_X_BUFFER_ADDRESS;
        HidReadCtrl.pCT2 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX;
        nTmp1 = 1;    //indicate that data is available
    }

    if (nTmp1)
    {
        // how many byte we can get from one endpoint buffer
        nTmp1 = *HidReadCtrl.pCT1;

        if(nTmp1 & EPBCNT_NAK)
        {
            nTmp1 = nTmp1 &0x7f;                      // clear NAK bit
            HidReadCtrl.nBytesInEp = *(HidReadCtrl.pCurrentEpPos+1); // holds how many valids bytes in the EP buffer
            if (HidReadCtrl.nBytesInEp > nTmp1-2)
            {
                HidReadCtrl.nBytesInEp = nTmp1-2;
            }
            HidReadCtrl.pCurrentEpPos += 2;           // here starts user data
            HidCopyUsbToBuff(HidReadCtrl.pCurrentEpPos, HidReadCtrl.pCT1);

            nTmp1 = *HidReadCtrl.pCT2;
            //try read data from second buffer
            if ((HidReadCtrl.nBytesToReceiveLeft > 0) &&       // do we have more data to receive?
                (nTmp1 & EPBCNT_NAK))                 // if the second buffer has received data?
            {
                nTmp1 = nTmp1 &0x7f;                  // clear NAK bit
                HidReadCtrl.nBytesInEp = *(HidReadCtrl.pCurrentEpPos+1); // holds how many valids bytes in the EP buffer
                if (HidReadCtrl.nBytesInEp > nTmp1-2)
                {
                    HidReadCtrl.nBytesInEp = nTmp1-2;
                }
                HidReadCtrl.pCurrentEpPos += 2;             // here starts user data
                HidCopyUsbToBuff(HidReadCtrl.pEP2, HidReadCtrl.pCT2);
                HidReadCtrl.pCT1 = HidReadCtrl.pCT2;
            }
        }
    }

    if (HidReadCtrl.nBytesToReceiveLeft == 0)     // the Receive opereation is completed
    {
        HidReadCtrl.pUserBuffer = NULL;           // no more receiving pending
        USBHID_handleReceiveCompleted(1);            // call event handler in interrupt context
        __enable_interrupt();                     // interrupts enable
        return kUSBHID_receiveCompleted;
    }

    //interrupts enable
    __enable_interrupt();
    return kUSBHID_receiveStarted;
}

//this function is used only by USB interrupt.
//It fills user receiving buffer with received data
BOOL HidToBufferFromHost()
{
    BYTE * pEP1;
    BYTE nTmp1;
    BYTE bWakeUp = FALSE; // per default we do not wake up after interrupt

    if (HidReadCtrl.nBytesToReceiveLeft == 0)       // do we have somtething to receive?
    {
        HidReadCtrl.pUserBuffer = NULL;             // no more receiving pending
        return bWakeUp;
    }

    // No data to receive...
    if (!((tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX |
           tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY)
           & 0x80))
    {
        return bWakeUp;
    }

    if (HidReadCtrl.bCurrentBufferXY == X_BUFFER)   //X is current buffer
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)OEP1_X_BUFFER_ADDRESS;
        HidReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX;

        //second EP buffer
        HidReadCtrl.pEP2 = (BYTE*)OEP1_Y_BUFFER_ADDRESS;
        HidReadCtrl.pCT2 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY;
    }
    else
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)OEP1_Y_BUFFER_ADDRESS;
        HidReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY;

        //second EP buffer
        HidReadCtrl.pEP2 = (BYTE*)OEP1_X_BUFFER_ADDRESS;
        HidReadCtrl.pCT2 = &tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX;
    }

    // how many byte we can get from one endpoint buffer
    nTmp1 = *HidReadCtrl.pCT1;

    if(nTmp1 & EPBCNT_NAK)
    {
        nTmp1 = nTmp1 &0x7f;                // clear NAK bit
        HidReadCtrl.nBytesInEp = *(pEP1+1); // holds how many valids bytes in the EP buffer
        if (HidReadCtrl.nBytesInEp > nTmp1-2)
        {
            HidReadCtrl.nBytesInEp = nTmp1-2;
        }
        pEP1 += 2;                      // here starts user data
        HidCopyUsbToBuff(pEP1, HidReadCtrl.pCT1);

        nTmp1 = *HidReadCtrl.pCT2;
        //try read data from second buffer
        if ((HidReadCtrl.nBytesToReceiveLeft > 0) &&       // do we have more data to send?
            (nTmp1 & EPBCNT_NAK))                   // if the second buffer has received data?
        {
            nTmp1 = nTmp1 &0x7f;                    // clear NAK bit
            HidReadCtrl.nBytesInEp = *(pEP1+1);     // holds how many valids bytes in the EP buffer
            if (HidReadCtrl.nBytesInEp > nTmp1-2)
            {
                HidReadCtrl.nBytesInEp = nTmp1-2;
            }
            HidReadCtrl.pEP2 += 2;                              // here starts user data
            HidCopyUsbToBuff(HidReadCtrl.pEP2, HidReadCtrl.pCT2);
            HidReadCtrl.pCT1 = HidReadCtrl.pCT2;
        }
    }

    if (HidReadCtrl.nBytesToReceiveLeft == 0)       // the Receive opereation is completed
    {
        HidReadCtrl.pUserBuffer = NULL;             // no more receiving pending
        if (wUsbEventMask & kUSB_receiveCompletedEvent)
        {
            bWakeUp = USBHID_handleReceiveCompleted(1);
        }

        if (HidReadCtrl.nBytesInEp)                 // Is not read data still available in the EP?
        {
            if (wUsbEventMask & kUSB_dataReceivedEvent)
            {
                bWakeUp = USBHID_handleDataReceived(1);
            }
        }
    }
    return bWakeUp;
}

// helper for USB interrupt handler
BOOL IsReceiveInProgress()
{
    return (HidReadCtrl.pUserBuffer != NULL);
}


/*
Aborts an active receive operation on interface intfNum.
  Returns the number of bytes that were received and transferred
  to the data location established for this receive operation.
*/
BYTE USBHID_abortReceive(WORD* size, BYTE intfNum)
{
    //interrupts disable
    __disable_interrupt();

    *size = 0;    //set received bytes count to 0

    //is receive operation underway?
    if (HidReadCtrl.pUserBuffer)
    {
        //how many bytes are already received?
        *size = HidReadCtrl.nBytesToReceive - HidReadCtrl.nBytesToReceiveLeft;

        HidReadCtrl.nBytesInEp = 0;
        HidReadCtrl.pUserBuffer = NULL;
        HidReadCtrl.nBytesToReceiveLeft = 0;
    }

     //interrupts enable
    __enable_interrupt();
    return kUSB_succeed;
}

/*
This function rejects payload data that has been received from the host.
*/
BYTE USBHID_rejectData(BYTE intfNum)
{
    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if (bFunctionSuspended)
    {
        return kUSBHID_busNotAvailable;
    }

    //interrupts disable
    __disable_interrupt();

    //Is receive operation underway?
    // - do not flush buffers if any operation still active.
    if (!HidReadCtrl.pUserBuffer)
    {
        BYTE tmp1 = tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX & EPBCNT_NAK;
        BYTE tmp2 = tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY & EPBCNT_NAK;

        if (tmp1 ^ tmp2) // switch current buffer if any and only ONE of the buffers is full
        {
            //switch current buffer
            HidReadCtrl.bCurrentBufferXY = (HidReadCtrl.bCurrentBufferXY+1) &0x01;
        }

        tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX = 0;  //flush buffer X
        tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY = 0;  //flush buffer Y
        HidReadCtrl.nBytesInEp = 0;                     // indicates that no more data available in the EP
    }

    //interrupts enable
    __enable_interrupt();
    return kUSB_succeed;
}

/*
This function indicates the status of the interface intfNum.
  If a send operation is active for this interface,
  the function also returns the number of bytes that have been transmitted to the host.
  If a receiver operation is active for this interface, the function also returns
  the number of bytes that have been received from the host and are waiting at the assigned address.

returns kUSBHID_waitingForSend (indicates that a call to USBHID_SendData()
  has been made, for which data transfer has not been completed)

returns kUSBHID_waitingForReceive (indicates that a receive operation
  has been initiated, but not all data has yet been received)

returns kUSBHID_dataWaiting (indicates that data has been received
  from the host, waiting in the USB receive buffers)
*/
BYTE USBHID_intfStatus(BYTE intfNum, WORD* bytesSent, WORD* bytesReceived)
{
    BYTE ret = 0;
    *bytesSent = 0;
    *bytesReceived = 0;

    //interrupts disable
    __disable_interrupt();

    // Is send operation underway?
    if (HidWriteCtrl.nHidBytesToSendLeft != 0)
    {
        ret |= kUSBHID_waitingForSend;
        *bytesSent = HidWriteCtrl.nHidBytesToSend - HidWriteCtrl.nHidBytesToSendLeft;
    }

    //Is receive operation underway?
    if (HidReadCtrl.pUserBuffer != NULL)
    {
        ret |= kUSBHID_waitingForReceive;
        *bytesReceived = HidReadCtrl.nBytesToReceive - HidReadCtrl.nBytesToReceiveLeft;
    }
    else // not receive operation started
    {
        // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
        if (!bFunctionSuspended)
        {
            if((tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX & EPBCNT_NAK)  | //any of buffers has a valid data packet
               (tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY & EPBCNT_NAK))
            {
                ret |= kUSBHID_dataWaiting;
            }
        }
    }

    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        // if suspended or not enumerated  - report no other tasks pending
        ret = kUSBHID_busNotAvailable;
    }

    //interrupts enable
    __enable_interrupt();

    return ret;
}

/*
Returns how many bytes are in the buffer are received and ready to be read.
*/
BYTE USBHID_bytesInUSBBuffer(BYTE intfNum)
{
    BYTE bTmp1 = 0;

    //interrupts disable
    __disable_interrupt();

    if (HidReadCtrl.nBytesInEp > 0)         // If a RX operation is underway, part of data may was read of the OEP buffer
    {
        bTmp1 = HidReadCtrl.nBytesInEp;
        if (*HidReadCtrl.pCT2 & EPBCNT_NAK) // the next buffer has a valid data packet
        {
            bTmp1 += *HidReadCtrl.pCT2 & 0x7F;
        }
    }
    else
    {
        if (tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX & EPBCNT_NAK) //this buffer has a valid data packet
        {
            bTmp1 = tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTX & 0x7F;
        }
        if (tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY & EPBCNT_NAK) //this buffer has a valid data packet
        {
            bTmp1 += tOutputEndPointDescriptorBlock[EDB(HID_OUTEP_ADDR)].bEPBCTY & 0x7F;
        }
    }

    //interrupts enable
    __enable_interrupt();
    return bTmp1;
}

#endif //ifdef _HID_

/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
