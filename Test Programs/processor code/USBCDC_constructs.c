#include "Common\device.h"
#include "Common\types.h"          // Basic Type declarations

#include "USB_Common\descriptors.h"
#include "USB_Common\usb.h"        // USB-specific functions

#include "main.h"
#ifdef _CDC_
    #include "USB_CDC_API\UsbCdc.h"
#endif
#ifdef _HID_
    #include "USB_HID_API\UsbHid.h"
#endif

#include <intrinsics.h>
#include "USBCDC_constructs.h"




//*********************************************************************************************
// Please see the MSP430 USB CDC API Programmer's Guide Sec. 9 for a full description of these 
// functions, how they work, and how to use them.  
//*********************************************************************************************




// This call assumes no previous send operation is underway; also assumes size is non-zero.  
// Returns zero if send completed; non-zero if it failed, with 1 = timeout and 2 = bus is gone.  
BYTE sendData_waitTilDone(BYTE* dataBuf, WORD size, BYTE intfNum, ULONG ulTimeout)
{
  ULONG sendCounter = 0;
  WORD bytesSent, bytesReceived;
  BYTE ret;

  ret = USBCDC_sendData(dataBuf,size,intfNum);  // ret is either sendStarted or busNotAvailable
  if(ret == kUSBCDC_busNotAvailable)
    return 2;
  
  while(1)                                      // ret was sendStarted
  {
    ret = USBCDC_intfStatus(intfNum,&bytesSent,&bytesReceived);
    if(ret & kUSBCDC_busNotAvailable)           // This may happen at any time
      return 2;
    else if(ret & kUSBCDC_waitingForSend)
    {
      if(ulTimeout && (sendCounter++ >= ulTimeout))  // Incr counter & try again
        return(1);                                   // Timed out
    }
    else return 0;                              // If neither busNotAvailable nor waitingForSend, it succeeded
  }
}



// This call assumes a previous send operation might be underway; also assumes size is non-zero.  
// Returns zero if send completed; non-zero if it failed, with 1 = timeout and 2 = bus is gone.  
BYTE sendData_inBackground(BYTE* dataBuf, WORD size, BYTE intfNum, ULONG ulTimeout)
{
  ULONG sendCounter = 0; 
  WORD bytesSent, bytesReceived;
  
  while(USBCDC_intfStatus(intfNum,&bytesSent,&bytesReceived) & kUSBCDC_waitingForSend)
  {
    if(ulTimeout && ((sendCounter++)>ulTimeout))  // A send operation is underway; incr counter & try again
      return 1;                                   // Timed out               
  }
  
  // At this point, a return from sendData() has to be either busNotAvailable or sendStarted
  if(USBCDC_sendData(dataBuf,size,intfNum) == kUSBCDC_busNotAvailable)    // This may happen at any time
    return 2;
  else return 0;                                  // Indicate success            
}                                  



// This call assumes a previous receive operation is NOT underway; also assumes size is non-zero.  
// Returns zero if receive completed; non-zero if it failed, with 1 = timeout and 2 = bus is gone
BYTE receiveData_waitTilDone(BYTE* dataBuf, WORD size, BYTE intfNum, ULONG ulTimeout)
{
  ULONG rcvCounter = 0;
  BYTE ret;
  WORD bytesSent, bytesReceived;
  
  ret = USBCDC_receiveData(dataBuf,size,intfNum); 
  if(ret == kUSBCDC_busNotAvailable)
    return 2;       // Indicate bus is gone
  if(ret == kUSBCDC_receiveCompleted)
    return 0;       // Indicate success
  
  while(1)
  {
    ret = USBCDC_intfStatus(intfNum,&bytesSent,&bytesReceived);
    if(ret & kUSBCDC_busNotAvailable)
      return 2;
    else if(ret & kUSBCDC_waitingForReceive)
    {
      if(ulTimeout && (rcvCounter++ >= ulTimeout))
         return 1;   // Indicate timed out 
    }
    else return 0;   // Indicate success
  }
}
                         
                         
// This call assumes a prevoius receive operation is NOT underway.  It only retrieves what data is waiting in the buffer
// It doesn't check for kUSBCDC_busNotAvailable, b/c it doesn't matter if it's not.  size is the maximum that
// is allowed to be received before exiting; i.e., it is the size allotted to dataBuf.  
// Returns the number of bytes received.  
WORD receiveDataInBuffer(BYTE* dataBuf, WORD size, BYTE intfNum)
{
  WORD bytesInBuf;
  BYTE* currentPos=dataBuf;
 
  while(bytesInBuf = USBCDC_bytesInUSBBuffer(intfNum))
  {
    if((WORD)(currentPos-dataBuf+bytesInBuf) > size) 
      break;
 
    USBCDC_receiveData(currentPos,bytesInBuf,intfNum); 
    currentPos += bytesInBuf;
  } 
  return (currentPos-dataBuf);
}


