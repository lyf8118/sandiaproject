//====================================================================
//    File: hal_UCS.h
//
//    Texas Instruments
//
//    Provides Functions to Initialize the UCS/FLL and clock sources
//    04/17/08
//
//====================================================================


#ifndef __hal_UCS
#define __hal_UCS

#ifndef XT1LFOFFG               // Defines if not available in header file
#define XT1LFOFFG 0
#endif
#ifndef XT1HFOFFG               // Defines if not available in header file
#define XT1HFOFFG 0
#endif
#ifndef XT2OFFG                 // Defines if not available in header file
#define XT2OFFG 0
#endif

#define XT1_TO_MCLK   UCSCTL4 = (UCSCTL4 & ~(SELM_7)) | (SELM__XT1CLK)  /*Select XT1 for MCLK */
#define XT2_TO_MCLK   UCSCTL4 = (UCSCTL4 & ~(SELM_7)) | (SELM__XT2CLK)  /* Select XT2 for MCLK */
#define XT1_TO_SMCLK  UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT1CLK)  /* Select XT1 for SMCLK */
#define XT2_TO_SMCLK  UCSCTL4 = (UCSCTL4 & ~(SELS_7)) | (SELS__XT2CLK)  /* Select XT2 for SMCLK */

//#define MCLK_DIV_1    UCSCTL5 = (UCSCTL5 & ~(DIVM_7)) | (DIVM_0)        /* set MCLK/1 */
//#define MCLK_DIV_2    UCSCTL5 = (UCSCTL5 & ~(DIVM_7)) | (DIVM_1)        /* set MCLK/2 */
//#define SMCLK_DIV_1   UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS_0)        /* set SMCLK/1 */
//#define SMCLK_DIV_2   UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS_1)        /* set SMCLK/2 */

#define MCLK_DIV(x)   UCSCTL5 = (UCSCTL5 & ~(DIVM_7)) | (DIVM__##x)     /* set MCLK/x */
#define SMCLK_DIV(x)  UCSCTL5 = (UCSCTL5 & ~(DIVS_7)) | (DIVS__##x)     /* set SMCLK/x */


//====================================================================
/**
 * Startup routine for 32kHz Cristal on LFXT1
 *
*/
void LFXT_Start(void);

//====================================================================
/**
 * Startup routine for XT1
 *
*/
void XT1_Start(void);

//====================================================================
/**
 * Use XT1 in Bypasss mode
 *
*/
void XT1Bypass(void);

//====================================================================
/**
 * Startup routine for XT2
 *
*/
void XT2_Start(void);

//====================================================================
/**
 * Use XT2 in Bypasss mode for MCLK
 *
*/
void XT2Bypass(void);


//====================================================================
/**
  * Initializes FLL of the UCS
  *
  * \param fsystem  required system frequency (MCLK) in kHz
  * \param ratio       ratio between fsystem and FLLREFCLK
  */
void Init_FLL(const unsigned int fsystem, const unsigned int ratio);


#endif /* __hal_UCS */
