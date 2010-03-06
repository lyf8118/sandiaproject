// (c)2009 by Texas Instruments Incorporated, All Rights Reserved.
/*----------------------------------------------------------------------------+
|                                                                             |
|                              Texas Instruments                              |
|                                                                             |
|                          MSP430 USB-Example (CDC Driver)                    |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: UsbCdc.c, v1.12 2009/05/26                                         |
|  Author: Rostyslav Stolyar                                                  |
|                                                                             |
|  WHO          WHEN         WHAT                                             |
|  ---          ----------   ------------------------------------------------ |
|  R.Stolyar    2008/09/03   born                                             |
|  R.Stolyar    2008/09/19   Changed USBCDC_sendData to send more then 64bytes|
|  R.Stolyar    2008/12/23   enhancements of CDC API                          |
|  R.Stolyar    2008/05/19   updated USBCDC_intfStatus()                      |
|  R.Stolyar    2009/05/26   added USBCDC_bytesInUSBBuffer()                  |
+----------------------------------------------------------------------------*/

#include "..\USB_Common\descriptors.h"

#ifdef _CDC_


#include "..\Common\device.h"
#include "..\Common\types.h"              // Basic Type declarations
#include "..\USB_Common\defMSP430USB.h"
#include "..\USB_Common\usb.h"            // USB-specific Data Structures
#include "..\USB_CDC_API\UsbCdc.h"
#include <string.h>


static ULONG lBaudrate = 0;
static BYTE bDataBits = 8;
static BYTE bStopBits = 0;
static BYTE bParity = 0;

static struct _CdcWrite
{
    WORD nCdcBytesToSend;        // holds counter of bytes to be sent
    WORD nCdcBytesToSendLeft;    // holds counter how many bytes is still to be sent
    const BYTE* pBufferToSend;   // holds the buffer with data to be sent
    BYTE bCurrentBufferXY;       // is 0 if current buffer to write data is X, or 1 if current buffer is Y
} CdcWriteCtrl;

static struct _CdcRead
{
    BYTE *pUserBuffer;     // holds the current position of user's receiving buffer. If NULL- no receiving operation started
    BYTE *pCurrentEpPos;   // current positon to read of received data from curent EP
    WORD nBytesToReceive;  // holds how many bytes was requested by receiveData() to receive
    WORD nBytesToReceiveLeft;        // holds how many bytes is still requested by receiveData() to receive
    BYTE * pCT1;           // holds current EPBCTxx register
    BYTE * pCT2;           // holds next EPBCTxx register
    BYTE * pEP2;           // holds addr of the next EP buffer
    BYTE nBytesInEp;       // how many received bytes still available in current EP
    BYTE bCurrentBufferXY; // indicates which buffer is used by host to transmit data via OUT endpoint3
} CdcReadCtrl;


extern WORD wUsbEventMask;

//function pointers
extern VOID *(*USB_TX_memcpy)(VOID * dest, const VOID * source, size_t count);
extern VOID *(*USB_RX_memcpy)(VOID * dest, const VOID * source, size_t count);


/*----------------------------------------------------------------------------+
| Global Variables                                                            |
+----------------------------------------------------------------------------*/

extern __no_init tEDB tInputEndPointDescriptorBlock[];
extern __no_init tEDB tOutputEndPointDescriptorBlock[];


VOID CdcResetData()
{
    // indicates which buffer is used by host to transmit data via OUT endpoint3 - X buffer is first
    //CdcReadCtrl.bCurrentBufferXY = X_BUFFER;

    memset(&CdcWriteCtrl, 0, sizeof(CdcWriteCtrl));
    memset(&CdcReadCtrl, 0, sizeof(CdcReadCtrl));
}

/*
Sends data over interface intfNum, of size size and starting at address data.
Returns: kUSBCDC_sendStarted
         kUSBCDC_sendComplete
         kUSBCDC_intfBusyError
*/
BYTE USBCDC_sendData(const BYTE* data, WORD size, BYTE intfNum)
{
    if (size == 0)
    {
        return kUSBCDC_generalError;
    }
    if (CdcWriteCtrl.nCdcBytesToSendLeft != 0)
    {
        // the USB still sends previous data, we have to wait
        return kUSBCDC_intfBusyError;
    }

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        // data can not be read because of USB suspended
        return kUSBCDC_busNotAvailable;
    }

    //This function generate the USB interrupt. The data will be sent out from interrupt

    // atomic operation - desable interrupts
    __disable_interrupt();               // Disable global interrupts
    CdcWriteCtrl.nCdcBytesToSend = size;
    CdcWriteCtrl.nCdcBytesToSendLeft = size;
    CdcWriteCtrl.pBufferToSend = data;

    //trigger Endpoint Interrupt - to start send operation
    USBIEPIFG |= 1<<(EDB(CDC_INEP_ADDR)+1);  //IEPIFGx;

    __enable_interrupt();                // enable global interrupts

    return kUSBCDC_sendStarted;
}

//workaround for CDC windows driver: it doesn't give data to Application if was sent 64 byte
#define EP_MAX_PACKET_SIZE_CDC      0x3F

//this function is used only by USB interrupt
BOOL CdcToHostFromBuffer()
{
    BYTE byte_count, nTmp2;
    BYTE * pEP1;
    BYTE * pEP2;
    BYTE * pCT1;
    BYTE * pCT2;
    BYTE bWakeUp = FALSE; //TRUE for wake up after interrupt
    static BYTE bZeroPacketSent; // = FALSE;

    if (CdcWriteCtrl.nCdcBytesToSendLeft == 0)           // do we have somtething to send?
    {
        if (!bZeroPacketSent)               // zero packet was not yet sent
        {
            bZeroPacketSent = TRUE;

            CdcWriteCtrl.nCdcBytesToSend = 0;   // nothing to send

            //call event callback function
            if (wUsbEventMask & kUSB_sendCompletedEvent)
            {
                bWakeUp = USBCDC_handleSendCompleted(1);
            }

        } // if (!bSentZeroPacket)

        return bWakeUp;
    }

    bZeroPacketSent = FALSE;    // zero packet will be not sent: we have data

    if (CdcWriteCtrl.bCurrentBufferXY == X_BUFFER)
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)IEP3_X_BUFFER_ADDRESS;
        pCT1 = &tInputEndPointDescriptorBlock[EDB(CDC_INEP_ADDR)].bEPBCTX;

        //second EP buffer
        pEP2 = (BYTE*)IEP3_Y_BUFFER_ADDRESS;
        pCT2 = &tInputEndPointDescriptorBlock[EDB(CDC_INEP_ADDR)].bEPBCTY;
    }
    else
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)IEP3_Y_BUFFER_ADDRESS;
        pCT1 = &tInputEndPointDescriptorBlock[EDB(CDC_INEP_ADDR)].bEPBCTY;

        //second EP buffer
        pEP2 = (BYTE*)IEP3_X_BUFFER_ADDRESS;
        pCT2 = &tInputEndPointDescriptorBlock[EDB(CDC_INEP_ADDR)].bEPBCTX;
    }

    // how many byte we can send over one endpoint buffer
    byte_count = (CdcWriteCtrl.nCdcBytesToSendLeft > EP_MAX_PACKET_SIZE_CDC) ? EP_MAX_PACKET_SIZE_CDC : CdcWriteCtrl.nCdcBytesToSendLeft;
    nTmp2 = *pCT1;

    if(nTmp2 & EPBCNT_NAK)
    {
        USB_TX_memcpy(pEP1, CdcWriteCtrl.pBufferToSend, byte_count); // copy data into IEP3 X or Y buffer
        *pCT1 = byte_count;                      // Set counter for usb In-Transaction
        CdcWriteCtrl.bCurrentBufferXY = (CdcWriteCtrl.bCurrentBufferXY+1)&0x01; //switch buffer
        CdcWriteCtrl.nCdcBytesToSendLeft -= byte_count;
        CdcWriteCtrl.pBufferToSend += byte_count;             // move buffer pointer

        //try to send data over second buffer
        nTmp2 = *pCT2;
        if ((CdcWriteCtrl.nCdcBytesToSendLeft > 0) &&                                  // do we have more data to send?
            (nTmp2 & EPBCNT_NAK)) // if the second buffer is free?
        {
            // how many byte we can send over one endpoint buffer
            byte_count = (CdcWriteCtrl.nCdcBytesToSendLeft > EP_MAX_PACKET_SIZE_CDC) ? EP_MAX_PACKET_SIZE_CDC : CdcWriteCtrl.nCdcBytesToSendLeft;

            USB_TX_memcpy(pEP2, CdcWriteCtrl.pBufferToSend, byte_count); // copy data into IEP3 X or Y buffer
            *pCT2 = byte_count;                      // Set counter for usb In-Transaction
            CdcWriteCtrl.bCurrentBufferXY = (CdcWriteCtrl.bCurrentBufferXY+1)&0x01; //switch buffer
            CdcWriteCtrl.nCdcBytesToSendLeft -= byte_count;
            CdcWriteCtrl.pBufferToSend += byte_count;            //move buffer pointer
        }
    }
    return bWakeUp;
}

/*
Aborts an active send operation on interface intfNum.
Returns the number of bytes that were sent prior to the abort, in size.
*/
BYTE USBCDC_abortSend(WORD* size, BYTE intfNum)
{
    __disable_interrupt(); //disable interrupts - atomic operation

    *size = (CdcWriteCtrl.nCdcBytesToSend - CdcWriteCtrl.nCdcBytesToSendLeft);
    CdcWriteCtrl.nCdcBytesToSend = 0;
    CdcWriteCtrl.nCdcBytesToSendLeft = 0;

    __enable_interrupt();   //enable interrupts
    return kUSB_succeed;
}

// This function copies data from OUT endpoint into user's buffer
// Arguments:
//    pEP - pointer to EP to copy from
//    pCT - pointer to pCT control reg
//
VOID CopyUsbToBuff(BYTE* pEP, BYTE* pCT)
{
    BYTE nCount;

    // how many byte we can get from one endpoint buffer
    nCount = (CdcReadCtrl.nBytesToReceiveLeft > CdcReadCtrl.nBytesInEp) ? CdcReadCtrl.nBytesInEp : CdcReadCtrl.nBytesToReceiveLeft;

    USB_RX_memcpy(CdcReadCtrl.pUserBuffer, pEP, nCount); // copy data from OEP3 X or Y buffer
    CdcReadCtrl.nBytesToReceiveLeft -= nCount;
    CdcReadCtrl.pUserBuffer += nCount;          // move buffer pointer
                                                // to read rest of data next time from this place

    if (nCount == CdcReadCtrl.nBytesInEp)       // all bytes are copied from receive buffer?
    {
        //switch current buffer
        CdcReadCtrl.bCurrentBufferXY = (CdcReadCtrl.bCurrentBufferXY+1) &0x01;

        CdcReadCtrl.nBytesInEp = 0;

        //clear NAK, EP ready to receive data
        *pCT = 0x00;
    }
    else
    {
        // set correct rest_data available in EP buffer
        if (CdcReadCtrl.nBytesInEp > nCount)
        {
            CdcReadCtrl.nBytesInEp -= nCount;
            CdcReadCtrl.pCurrentEpPos = pEP + nCount;
        }
        else
        {
            CdcReadCtrl.nBytesInEp = 0;
        }
    }
}

/*
Receives data over interface intfNum, of size size, into memory starting at address data.
Returns:
    kUSBCDC_receiveStarted  if the receiving process started.
    kUSBCDC_receiveCompleted  all requested date are received.
    kUSBCDC_receiveInProgress  previous receive opereation is in progress. The requested receive operation can be not started.
    kUSBCDC_generalError  error occurred.
*/
BYTE USBCDC_receiveData(BYTE* data, WORD size, BYTE intfNum)
{
    BYTE nTmp1;

    if (CdcReadCtrl.pUserBuffer != NULL)        // receive process already started
    {
        return kUSBCDC_intfBusyError;
    }

    if ((size == 0) ||                          // read size is 0
        (data == NULL))
    {
        return kUSBCDC_generalError;
    }

    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        // data can not be read because of USB suspended
        return kUSBCDC_busNotAvailable;
    }

    __disable_interrupt();                      //interrupts disable

    CdcReadCtrl.nBytesToReceive = size;         // bytes to receive
    CdcReadCtrl.nBytesToReceiveLeft = size;     // left bytes to receive
    CdcReadCtrl.pUserBuffer = data;             // set user receive buffer

    //read rest of data from buffer, if any
    if (CdcReadCtrl.nBytesInEp > 0)
    {
        // copy data from pEP-endpoint into User's buffer
        CopyUsbToBuff(CdcReadCtrl.pCurrentEpPos, CdcReadCtrl.pCT1);

        if (CdcReadCtrl.nBytesToReceiveLeft == 0)     // the Receive opereation is completed
        {
            CdcReadCtrl.pUserBuffer = NULL;     // no more receiving pending
            USBCDC_handleReceiveCompleted(1);      // call event handler in interrupt context
            __enable_interrupt();               // interrupts enable
            return kUSBCDC_receiveCompleted;    // receive completed
        }

        // check other EP buffer for data - exchange pCT1 with pCT2
        if (CdcReadCtrl.pCT1 == &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX)
        {
            CdcReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY;
            CdcReadCtrl.pCurrentEpPos = (BYTE*)OEP3_Y_BUFFER_ADDRESS;
        }
        else
        {
            CdcReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX;
            CdcReadCtrl.pCurrentEpPos = (BYTE*)OEP3_X_BUFFER_ADDRESS;
        }

        nTmp1 = *CdcReadCtrl.pCT1;
        //try read data from second buffer
        if (nTmp1 & EPBCNT_NAK)                 // if the second buffer has received data?
        {
            nTmp1 = nTmp1 &0x7f;                // clear NAK bit
            CdcReadCtrl.nBytesInEp = nTmp1;     // holds how many valids bytes in the EP buffer
            CopyUsbToBuff(CdcReadCtrl.pCurrentEpPos, CdcReadCtrl.pCT1);
        }

        if (CdcReadCtrl.nBytesToReceiveLeft == 0)     // the Receive opereation is completed
        {
            CdcReadCtrl.pUserBuffer = NULL;     // no more receiving pending
            __enable_interrupt();               // interrupts enable
            return kUSBCDC_receiveCompleted;    // receive completed
        }
    } //read rest of data from buffer, if any

    //read 'fresh' data, if available
    nTmp1 = 0;
    if (CdcReadCtrl.bCurrentBufferXY == X_BUFFER)  //this is current buffer
    {
        if (tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX & EPBCNT_NAK) //this buffer has a valid data packet
        {
            //this is the active EP buffer
            //pEP1
            CdcReadCtrl.pCurrentEpPos = (BYTE*)OEP3_X_BUFFER_ADDRESS;
            CdcReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX;

            //second EP buffer
            CdcReadCtrl.pEP2 = (BYTE*)OEP3_Y_BUFFER_ADDRESS;
            CdcReadCtrl.pCT2 = &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY;
            nTmp1 = 1;    //indicate that data is available
        }
    }
    else // Y_BUFFER
    if (tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY & EPBCNT_NAK)
    {
        //this is the active EP buffer
        CdcReadCtrl.pCurrentEpPos = (BYTE*)OEP3_Y_BUFFER_ADDRESS;
        CdcReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY;

        //second EP buffer
        CdcReadCtrl.pEP2 = (BYTE*)OEP3_X_BUFFER_ADDRESS;
        CdcReadCtrl.pCT2 = &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX;
        nTmp1 = 1;    //indicate that data is available
    }

    if (nTmp1)
    {
        // how many byte we can get from one endpoint buffer
        nTmp1 = *CdcReadCtrl.pCT1;
        while(nTmp1 == 0)
        {
            nTmp1 = *CdcReadCtrl.pCT1;
        }

        if(nTmp1 & EPBCNT_NAK)
        {
            nTmp1 = nTmp1 &0x7f;            // clear NAK bit
            CdcReadCtrl.nBytesInEp = nTmp1; // holds how many valids bytes in the EP buffer

            CopyUsbToBuff(CdcReadCtrl.pCurrentEpPos, CdcReadCtrl.pCT1);

            nTmp1 = *CdcReadCtrl.pCT2;
            //try read data from second buffer
            if ((CdcReadCtrl.nBytesToReceiveLeft > 0) &&       // do we have more data to send?
                (nTmp1 & EPBCNT_NAK))                 // if the second buffer has received data?
            {
                nTmp1 = nTmp1 &0x7f;                  // clear NAK bit
                CdcReadCtrl.nBytesInEp = nTmp1;       // holds how many valids bytes in the EP buffer
                CopyUsbToBuff(CdcReadCtrl.pEP2, CdcReadCtrl.pCT2);
                CdcReadCtrl.pCT1 = CdcReadCtrl.pCT2;
            }
        }
    }

    if (CdcReadCtrl.nBytesToReceiveLeft == 0)     // the Receive opereation is completed
    {
        CdcReadCtrl.pUserBuffer = NULL;           // no more receiving pending
        USBCDC_handleReceiveCompleted(1);                // call event handler in interrupt context
        __enable_interrupt();                     // interrupts enable
        return kUSBCDC_receiveCompleted;
    }

    //interrupts enable
    __enable_interrupt();
    return kUSBCDC_receiveStarted;
}


//this function is used only by USB interrupt.
//It fills user receiving buffer with received data
BOOL CdcToBufferFromHost()
{
    BYTE * pEP1;
    BYTE nTmp1;
    BYTE bWakeUp = FALSE; // per default we do not wake up after interrupt

    if (CdcReadCtrl.nBytesToReceiveLeft == 0)       // do we have somtething to receive?
    {
        CdcReadCtrl.pUserBuffer = NULL;             // no more receiving pending
        return bWakeUp;
    }

    // No data to receive...
    if (!((tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX |
           tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY)
           & 0x80))
    {
        return bWakeUp;
    }

    if (CdcReadCtrl.bCurrentBufferXY == X_BUFFER)   //X is current buffer
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)OEP3_X_BUFFER_ADDRESS;
        CdcReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX;

        //second EP buffer
        CdcReadCtrl.pEP2 = (BYTE*)OEP3_Y_BUFFER_ADDRESS;
        CdcReadCtrl.pCT2 = &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY;
    }
    else
    {
        //this is the active EP buffer
        pEP1 = (BYTE*)OEP3_Y_BUFFER_ADDRESS;
        CdcReadCtrl.pCT1 = &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY;

        //second EP buffer
        CdcReadCtrl.pEP2 = (BYTE*)OEP3_X_BUFFER_ADDRESS;
        CdcReadCtrl.pCT2 = &tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX;
    }

    // how many byte we can get from one endpoint buffer
    nTmp1 = *CdcReadCtrl.pCT1;
    while(nTmp1 == 0)
    {
        nTmp1 = *CdcReadCtrl.pCT1;
    }

    if(nTmp1 & EPBCNT_NAK)
    {
        nTmp1 = nTmp1 &0x7f;            // clear NAK bit
        CdcReadCtrl.nBytesInEp = nTmp1; // holds how many valids bytes in the EP buffer

        CopyUsbToBuff(pEP1, CdcReadCtrl.pCT1);

        nTmp1 = *CdcReadCtrl.pCT2;
        //try read data from second buffer
        if ((CdcReadCtrl.nBytesToReceiveLeft > 0) &&       // do we have more data to send?
            (nTmp1 & EPBCNT_NAK))                 // if the second buffer has received data?
        {
            nTmp1 = nTmp1 &0x7f;                  // clear NAK bit
            CdcReadCtrl.nBytesInEp = nTmp1;       // holds how many valids bytes in the EP buffer
            CopyUsbToBuff(CdcReadCtrl.pEP2, CdcReadCtrl.pCT2);
            CdcReadCtrl.pCT1 = CdcReadCtrl.pCT2;
        }
    }

    if (CdcReadCtrl.nBytesToReceiveLeft == 0)     // the Receive opereation is completed
    {
        CdcReadCtrl.pUserBuffer = NULL;   // no more receiving pending
        if (wUsbEventMask & kUSB_receiveCompletedEvent)
        {
            bWakeUp = USBCDC_handleReceiveCompleted(1);
        }

        if (CdcReadCtrl.nBytesInEp)       // Is not read data still available in the EP?
        {
            if (wUsbEventMask & kUSB_dataReceivedEvent)
            {
                bWakeUp = USBCDC_handleDataReceived(1);
            }
        }
    }
    return bWakeUp;
}

// helper for USB interrupt handler
BOOL IsReceiveInProgress()
{
    return (CdcReadCtrl.pUserBuffer != NULL);
}


/*
Aborts an active receive operation on interface intfNum.
  Returns the number of bytes that were received and transferred
  to the data location established for this receive operation.
*/
BYTE USBCDC_abortReceive(WORD* size, BYTE intfNum)
{
    //interrupts disable
    __disable_interrupt();

    *size = 0; //set received bytes count to 0

    //is receive operation underway?
    if (CdcReadCtrl.pUserBuffer)
    {
        //how many bytes are already received?
        *size = CdcReadCtrl.nBytesToReceive - CdcReadCtrl.nBytesToReceiveLeft;

        CdcReadCtrl.nBytesInEp = 0;
        CdcReadCtrl.pUserBuffer = NULL;
        CdcReadCtrl.nBytesToReceiveLeft = 0;
    }

     //interrupts enable
    __enable_interrupt();
    return kUSB_succeed;
}

/*
This function rejects payload data that has been received from the host.
*/
BYTE USBCDC_rejectData(BYTE intfNum)
{
    // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
    if (bFunctionSuspended)
    {
        return kUSBCDC_busNotAvailable;
    }

    //interrupts disable
    __disable_interrupt();

    //Is receive operation underway?
    // - do not flush buffers if any operation still active.
    if (!CdcReadCtrl.pUserBuffer)
    {
        BYTE tmp1 = tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX & EPBCNT_NAK;
        BYTE tmp2 = tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY & EPBCNT_NAK;

        if (tmp1 ^ tmp2) // switch current buffer if any and only ONE of buffers is full
        {
            //switch current buffer
            CdcReadCtrl.bCurrentBufferXY = (CdcReadCtrl.bCurrentBufferXY+1) &0x01;
        }

        tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX = 0;  //flush buffer X
        tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY = 0;  //flush buffer Y
        CdcReadCtrl.nBytesInEp = 0;                     // indicates that no more data available in the EP
    }

    __enable_interrupt();       //interrupts enable
    return kUSB_succeed;
}

/*
This function indicates the status of the itnerface intfNum.
  If a send operation is active for this interface,
  the function also returns the number of bytes that have been transmitted to the host.
  If a receiver operation is active for this interface, the function also returns
  the number of bytes that have been received from the host and are waiting at the assigned address.

returns kUSBCDC_waitingForSend (indicates that a call to USBCDC_SendData()
  has been made, for which data transfer has not been completed)

returns kUSBCDC_waitingForReceive (indicates that a receive operation
  has been initiated, but not all data has yet been received)

returns kUSBCDC_dataWaiting (indicates that data has been received
  from the host, waiting in the USB receive buffers)
*/
BYTE USBCDC_intfStatus(BYTE intfNum, WORD* bytesSent, WORD* bytesReceived)
{
    BYTE ret = 0;
    *bytesSent = 0;
    *bytesReceived = 0;

    //interrupts disable
    __disable_interrupt();

    // Is send operation underway?
    if (CdcWriteCtrl.nCdcBytesToSendLeft != 0)
    {
        ret |= kUSBCDC_waitingForSend;
        *bytesSent = CdcWriteCtrl.nCdcBytesToSend - CdcWriteCtrl.nCdcBytesToSendLeft;
    }

    //Is receive operation underway?
    if (CdcReadCtrl.pUserBuffer != NULL)
    {
        ret |= kUSBCDC_waitingForReceive;
        *bytesReceived = CdcReadCtrl.nBytesToReceive - CdcReadCtrl.nBytesToReceiveLeft;
    }
    else // receive operation not started
    {
        // do not access USB memory if suspended (PLL off). It may produce BUS_ERROR
        if (!bFunctionSuspended)
        {
            if((tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX & EPBCNT_NAK)  | //any of buffers has a valid data packet
               (tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY & EPBCNT_NAK))
            {
                ret |= kUSBCDC_dataWaiting;
            }
        }
    }

    if ((bFunctionSuspended) ||
        (bEnumerationStatus != ENUMERATION_COMPLETE))
    {
        // if suspended or not enumerated - report no other tasks pending
        ret = kUSBCDC_busNotAvailable;
    }

    //interrupts enable
    __enable_interrupt();

    __no_operation();
    return ret;
}

/*
Returns how many bytes are in the buffer are received and ready to be read.
*/
BYTE USBCDC_bytesInUSBBuffer(BYTE intfNum)
{
    BYTE bTmp1 = 0;

    //interrupts disable
    __disable_interrupt();

    if (CdcReadCtrl.nBytesInEp > 0)         // If a RX operation is underway, part of data may was read of the OEP buffer
    {
        bTmp1 = CdcReadCtrl.nBytesInEp;
        if (*CdcReadCtrl.pCT2 & EPBCNT_NAK) // the next buffer has a valid data packet
        {
            bTmp1 += *CdcReadCtrl.pCT2 & 0x7F;
        }
    }
    else
    {
        if (tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX & EPBCNT_NAK) //this buffer has a valid data packet
        {
            bTmp1 = tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTX & 0x7F;
        }
        if (tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY & EPBCNT_NAK) //this buffer has a valid data packet
        {
            bTmp1 += tOutputEndPointDescriptorBlock[EDB(CDC_OUTEP_ADDR)].bEPBCTY & 0x7F;
        }
    }

    //interrupts enable
    __enable_interrupt();
    return bTmp1;
}


//----------------------------------------------------------------------------
//  Line Coding Structure
//  dwDTERate     | 4 | Data terminal rate, in bits per second
//  bCharFormat   | 1 | Stop bits, 0 = 1 Stop bit, 1 = 1,5 Stop bits, 2 = 2 Stop bits
//  bParityType   | 1 | Parity, 0 = None, 1 = Odd, 2 = Even, 3= Mark, 4 = Space
//  bDataBits     | 1 | Data bits (5,6,7,8,16)
//----------------------------------------------------------------------------
VOID usbGetLineCoding(VOID)
{
    abUsbRequestReturnData[6] = bDataBits;               // Data bits = 8
    abUsbRequestReturnData[5] = bParity;                 // No Parity
    abUsbRequestReturnData[4] = bStopBits;               // Stop bits = 1

    abUsbRequestReturnData[3] = lBaudrate >> 24;
    abUsbRequestReturnData[2] = lBaudrate >> 16;
    abUsbRequestReturnData[1] = lBaudrate >> 8;
    abUsbRequestReturnData[0] = lBaudrate;

    wBytesRemainingOnIEP0 = 0x07;                   // amount of data to be send over EP0 to host
    usbSendDataPacketOnEP0((PBYTE)&abUsbRequestReturnData[0]);  // send data to host
}

//----------------------------------------------------------------------------

VOID usbSetLineCoding(VOID)
{
    usbReceiveDataPacketOnEP0((PBYTE) &abUsbRequestIncomingData);     // receive data over EP0 from Host
}

//----------------------------------------------------------------------------

VOID usbSetControlLineState(VOID)
{
    usbSendZeroLengthPacketOnIEP0();    // Send ZLP for status stage
}

//----------------------------------------------------------------------------

VOID Handler_SetLineCoding(VOID)
{
    // Baudrate Settings

    lBaudrate = (ULONG)abUsbRequestIncomingData[3] << 24 | (ULONG)abUsbRequestIncomingData[2]<<16 |
      (ULONG)abUsbRequestIncomingData[1]<<8 | abUsbRequestIncomingData[0];
}

#endif //ifdef _CDC_

/*----------------------------------------------------------------------------+
| End of source file                                                          |
+----------------------------------------------------------------------------*/
/*------------------------ Nothing Below This Line --------------------------*/
