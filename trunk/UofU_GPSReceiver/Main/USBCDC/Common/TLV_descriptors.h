/************************************************************
* TLV Descriptors for F5438
************************************************************/


// function declaration
extern void Get_TLV_info(unsigned char tag, unsigned char *length, unsigned int *data_address);


#ifndef __MSP430_HAS_TLV__              /* Definition to show that Module is available */
#define __MSP430_HAS_TLV__              /* Definition to show that Module is available */
#define TLV_START             (0x1A08)    /* Start Address of the TLV structure */
#define TLV_END               (0x1AFF)    /* End Address of the TLV structure */

#define TLV_LDTAG             (0x01)      //  Legacy descriptor (1xx, 2xx, 4xx families)
#define TLV_PDTAG             (0x02)      //  Peripheral discovery descriptor
#define TLV_Reserved1         (0x03)      //  Future usage
#define TLV_Reserved2         (0x04)      //  Future usage
#define TLV_BLANK             (0x05)      //  Blank descriptor
#define TLV_Reserved3         (0x06)      //  Future usage
#define TLV_SERNUM            (0x07)      //  Serial Number
#define TLV_DIERECORD         (0x08)      /*  Die Record  */
#define TLV_ADCCAL            (0x10)      //  ADC calibration
#define TLV_TAGEXT            (0xFE)      //  Tag extender
#define TLV_TAGEND            (0xFF)      //  Tag End of Table


#endif
