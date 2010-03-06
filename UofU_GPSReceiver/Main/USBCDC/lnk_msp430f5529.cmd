/******************************************************************************/
/* lnk_msp430tc0701.cmd - LINKER COMMAND FILE FOR LINKING MSP430TC0701 PROGRAMS */
/*                                                                            */
/*  Ver | dd mmm yyyy | Who  | Description of changes                         */
/* =====|=============|======|=============================================   */
/*  0.01| 08 Mar 2004 | A.D. | First prototype                                */
/*  0.02| 26 Mai 2004 | A.D. | Leading symbol underscores removed,            */
/*      |             |      | Interrupt vector definition changed            */
/*  0.03| 22 Jun 2004 | A.D. | File reformatted                               */
/*                                                                            */
/*   Usage:  lnk430 <obj files...>    -o <out file> -m <map file> lnk.cmd     */
/*           cl430  <src files...> -z -o <out file> -m <map file> lnk.cmd     */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/* These linker options are for command line linking only.  For IDE linking,  */
/* you should set your linker options in Project Properties                   */
/* -c                                               LINK USING C CONVENTIONS  */
/* -stack  0x0100                                   SOFTWARE STACK SIZE       */
/* -heap   0x0100                                   HEAP AREA SIZE            */

/*----------------------------------------------------------------------------*/
/* 'Allocate' peripheral registers at given addresses                         */
/*----------------------------------------------------------------------------*/

/************************************************************
* STANDARD BITS
************************************************************/
/************************************************************
* STATUS REGISTER BITS
************************************************************/
/************************************************************
* CPU
************************************************************/
/************************************************************
* PERIPHERAL FILE MAP
************************************************************/
/************************************************************
* ADC12 PLUS
************************************************************/
ADC12CTL0          = 0x0700;
ADC12CTL0_L        = 0x0700;
ADC12CTL0_H        = 0x0701;
ADC12CTL1          = 0x0702;
ADC12CTL1_L        = 0x0702;
ADC12CTL1_H        = 0x0703;
ADC12CTL2          = 0x0704;
ADC12CTL2_L        = 0x0704;
ADC12CTL2_H        = 0x0705;
ADC12IFG           = 0x070A;
ADC12IFG_L         = 0x070A;
ADC12IFG_H         = 0x070B;
ADC12IE            = 0x070C;
ADC12IE_L          = 0x070C;
ADC12IE_H          = 0x070D;
ADC12IV            = 0x070E;
ADC12IV_L          = 0x070E;
ADC12IV_H          = 0x070F;
ADC12MEM0          = 0x0720;
ADC12MEM0_L        = 0x0720;
ADC12MEM0_H        = 0x0721;
ADC12MEM1          = 0x0722;
ADC12MEM1_L        = 0x0722;
ADC12MEM1_H        = 0x0723;
ADC12MEM2          = 0x0724;
ADC12MEM2_L        = 0x0724;
ADC12MEM2_H        = 0x0725;
ADC12MEM3          = 0x0726;
ADC12MEM3_L        = 0x0726;
ADC12MEM3_H        = 0x0727;
ADC12MEM4          = 0x0728;
ADC12MEM4_L        = 0x0728;
ADC12MEM4_H        = 0x0729;
ADC12MEM5          = 0x072A;
ADC12MEM5_L        = 0x072A;
ADC12MEM5_H        = 0x072B;
ADC12MEM6          = 0x072C;
ADC12MEM6_L        = 0x072C;
ADC12MEM6_H        = 0x072D;
ADC12MEM7          = 0x072E;
ADC12MEM7_L        = 0x072E;
ADC12MEM7_H        = 0x072F;
ADC12MEM8          = 0x0730;
ADC12MEM8_L        = 0x0730;
ADC12MEM8_H        = 0x0731;
ADC12MEM9          = 0x0732;
ADC12MEM9_L        = 0x0732;
ADC12MEM9_H        = 0x0733;
ADC12MEM10         = 0x0734;
ADC12MEM10_L       = 0x0734;
ADC12MEM10_H       = 0x0735;
ADC12MEM11         = 0x0736;
ADC12MEM11_L       = 0x0736;
ADC12MEM11_H       = 0x0737;
ADC12MEM12         = 0x0738;
ADC12MEM12_L       = 0x0738;
ADC12MEM12_H       = 0x0739;
ADC12MEM13         = 0x073A;
ADC12MEM13_L       = 0x073A;
ADC12MEM13_H       = 0x073B;
ADC12MEM14         = 0x073C;
ADC12MEM14_L       = 0x073C;
ADC12MEM14_H       = 0x073D;
ADC12MEM15         = 0x073E;
ADC12MEM15_L       = 0x073E;
ADC12MEM15_H       = 0x073F;
ADC12MCTL0         = 0x0710;
ADC12MCTL0_L       = 0x0710;
ADC12MCTL0_H       = 0x0711;
ADC12MCTL1         = 0x0711;
ADC12MCTL1_L       = 0x0711;
ADC12MCTL1_H       = 0x0712;
ADC12MCTL2         = 0x0712;
ADC12MCTL2_L       = 0x0712;
ADC12MCTL2_H       = 0x0713;
ADC12MCTL3         = 0x0713;
ADC12MCTL3_L       = 0x0713;
ADC12MCTL3_H       = 0x0714;
ADC12MCTL4         = 0x0714;
ADC12MCTL4_L       = 0x0714;
ADC12MCTL4_H       = 0x0715;
ADC12MCTL5         = 0x0715;
ADC12MCTL5_L       = 0x0715;
ADC12MCTL5_H       = 0x0716;
ADC12MCTL6         = 0x0716;
ADC12MCTL6_L       = 0x0716;
ADC12MCTL6_H       = 0x0717;
ADC12MCTL7         = 0x0717;
ADC12MCTL7_L       = 0x0717;
ADC12MCTL7_H       = 0x0718;
ADC12MCTL8         = 0x0718;
ADC12MCTL8_L       = 0x0718;
ADC12MCTL8_H       = 0x0719;
ADC12MCTL9         = 0x0719;
ADC12MCTL9_L       = 0x0719;
ADC12MCTL9_H       = 0x071A;
ADC12MCTL10        = 0x071A;
ADC12MCTL10_L      = 0x071A;
ADC12MCTL10_H      = 0x071B;
ADC12MCTL11        = 0x071B;
ADC12MCTL11_L      = 0x071B;
ADC12MCTL11_H      = 0x071C;
ADC12MCTL12        = 0x071C;
ADC12MCTL12_L      = 0x071C;
ADC12MCTL12_H      = 0x071D;
ADC12MCTL13        = 0x071D;
ADC12MCTL13_L      = 0x071D;
ADC12MCTL13_H      = 0x071E;
ADC12MCTL14        = 0x071E;
ADC12MCTL14_L      = 0x071E;
ADC12MCTL14_H      = 0x071F;
ADC12MCTL15        = 0x071F;
ADC12MCTL15_L      = 0x071F;
ADC12MCTL15_H      = 0x0720;
/*************************************************************
* CRC Module
*************************************************************/
CRCDI              = 0x0150;
CRCDI_L            = 0x0150;
CRCDI_H            = 0x0151;
CRCINIRES          = 0x0154;
CRCINIRES_L        = 0x0154;
CRCINIRES_H        = 0x0155;
/************************************************************
* DAC12
************************************************************/
DAC12_0CTL0        = 0x0780;
DAC12_0CTL0_L      = 0x0780;
DAC12_0CTL0_H      = 0x0781;
DAC12_0CTL1        = 0x0782;
DAC12_0CTL1_L      = 0x0782;
DAC12_0CTL1_H      = 0x0783;
DAC12_0DAT         = 0x0784;
DAC12_0DAT_L       = 0x0784;
DAC12_0DAT_H       = 0x0785;
DAC12_0CALCTL      = 0x0786;
DAC12_0CALCTL_L    = 0x0786;
DAC12_0CALCTL_H    = 0x0787;
DAC12_0CALDAT      = 0x0788;
DAC12_0CALDAT_L    = 0x0788;
DAC12_0CALDAT_H    = 0x0789;
DAC12_1CTL0        = 0x0790;
DAC12_1CTL0_L      = 0x0790;
DAC12_1CTL0_H      = 0x0791;
DAC12_1CTL1        = 0x0792;
DAC12_1CTL1_L      = 0x0792;
DAC12_1CTL1_H      = 0x0793;
DAC12_1DAT         = 0x0794;
DAC12_1DAT_L       = 0x0794;
DAC12_1DAT_H       = 0x0795;
DAC12_1CALCTL      = 0x0796;
DAC12_1CALCTL_L    = 0x0796;
DAC12_1CALCTL_H    = 0x0797;
DAC12_1CALDAT      = 0x0798;
DAC12_1CALDAT_L    = 0x0798;
DAC12_1CALDAT_H    = 0x0799;
DAC12_IV           = 0x079E;
DAC12_IV_L         = 0x079E;
DAC12_IV_H         = 0x079F;
/************************************************************
* DMA_X
************************************************************/
DMACTL0            = 0x0500;
DMACTL0_L          = 0x0500;
DMACTL0_H          = 0x0501;
DMACTL1            = 0x0502;
DMACTL1_L          = 0x0502;
DMACTL1_H          = 0x0503;
DMACTL2            = 0x0504;
DMACTL2_L          = 0x0504;
DMACTL2_H          = 0x0505;
DMACTL3            = 0x0506;
DMACTL3_L          = 0x0506;
DMACTL3_H          = 0x0507;
DMACTL4            = 0x0508;
DMACTL4_L          = 0x0508;
DMACTL4_H          = 0x0509;
DMAIV              = 0x050E;
DMAIV_L            = 0x050E;
DMAIV_H            = 0x050F;
DMA0CTL            = 0x0510;
DMA0CTL_L          = 0x0510;
DMA0CTL_H          = 0x0511;
DMA0SA             = 0x0512;
DMA0SA_L           = 0x0512;
DMA0SA_H           = 0x0513;
DMA0DA             = 0x0516;
DMA0DA_L           = 0x0516;
DMA0DA_H           = 0x0517;
DMA0SZ             = 0x051A;
DMA0SZ_L           = 0x051A;
DMA0SZ_H           = 0x051B;
DMA1CTL            = 0x0520;
DMA1CTL_L          = 0x0520;
DMA1CTL_H          = 0x0521;
DMA1SA             = 0x0522;
DMA1SA_L           = 0x0522;
DMA1SA_H           = 0x0523;
DMA1DA             = 0x0526;
DMA1DA_L           = 0x0526;
DMA1DA_H           = 0x0527;
DMA1SZ             = 0x052A;
DMA1SZ_L           = 0x052A;
DMA1SZ_H           = 0x052B;
DMA2CTL            = 0x0530;
DMA2CTL_L          = 0x0530;
DMA2CTL_H          = 0x0531;
DMA2SA             = 0x0532;
DMA2SA_L           = 0x0532;
DMA2SA_H           = 0x0533;
DMA2DA             = 0x0536;
DMA2DA_L           = 0x0536;
DMA2DA_H           = 0x0537;
DMA2SZ             = 0x053A;
DMA2SZ_L           = 0x053A;
DMA2SZ_H           = 0x053B;
/*************************************************************
* Flash Memory
*************************************************************/
FCTL1              = 0x0140;
FCTL1_L            = 0x0140;
FCTL1_H            = 0x0141;
FCTL3              = 0x0144;
FCTL3_L            = 0x0144;
FCTL3_H            = 0x0145;
FCTL4              = 0x0146;
FCTL4_L            = 0x0146;
FCTL4_H            = 0x0147;
/************************************************************
* HARDWARE MULTIPLIER 32Bit
************************************************************/
MPY                = 0x04C0;
MPY_L              = 0x04C0;
MPY_H              = 0x04C1;
MPYS               = 0x04C2;
MPYS_L             = 0x04C2;
MPYS_H             = 0x04C3;
MAC                = 0x04C4;
MAC_L              = 0x04C4;
MAC_H              = 0x04C5;
MACS               = 0x04C6;
MACS_L             = 0x04C6;
MACS_H             = 0x04C7;
OP2                = 0x04C8;
OP2_L              = 0x04C8;
OP2_H              = 0x04C9;
RESLO              = 0x04CA;
RESLO_L            = 0x04CA;
RESLO_H            = 0x04CB;
RESHI              = 0x04CC;
RESHI_L            = 0x04CC;
RESHI_H            = 0x04CD;
SUMEXT             = 0x04CE;
SUMEXT_L           = 0x04CE;
SUMEXT_H           = 0x04CF;
MPY32L             = 0x04D0;
MPY32L_L           = 0x04D0;
MPY32L_H           = 0x04D1;
MPY32H             = 0x04D2;
MPY32H_L           = 0x04D2;
MPY32H_H           = 0x04D3;
MPYS32L            = 0x04D4;
MPYS32L_L          = 0x04D4;
MPYS32L_H          = 0x04D5;
MPYS32H            = 0x04D6;
MPYS32H_L          = 0x04D6;
MPYS32H_H          = 0x04D7;
MAC32L             = 0x04D8;
MAC32L_L           = 0x04D8;
MAC32L_H           = 0x04D9;
MAC32H             = 0x04DA;
MAC32H_L           = 0x04DA;
MAC32H_H           = 0x04DB;
MACS32L            = 0x04DC;
MACS32L_L          = 0x04DC;
MACS32L_H          = 0x04DD;
MACS32H            = 0x04DE;
MACS32H_L          = 0x04DE;
MACS32H_H          = 0x04DF;
OP2L               = 0x04E0;
OP2L_L             = 0x04E0;
OP2L_H             = 0x04E1;
OP2H               = 0x04E2;
OP2H_L             = 0x04E2;
OP2H_H             = 0x04E3;
RES0               = 0x04E4;
RES0_L             = 0x04E4;
RES0_H             = 0x04E5;
RES1               = 0x04E6;
RES1_L             = 0x04E6;
RES1_H             = 0x04E7;
RES2               = 0x04E8;
RES2_L             = 0x04E8;
RES2_H             = 0x04E9;
RES3               = 0x04EA;
RES3_L             = 0x04EA;
RES3_H             = 0x04EB;
MPY32CTL0          = 0x04EC;
MPY32CTL0_L        = 0x04EC;
MPY32CTL0_H        = 0x04ED;
/************************************************************
* DIGITAL I/O Port1/2 Pull up / Pull down Resistors
************************************************************/
PAIN               = 0x0200;
PAIN_L             = 0x0200;
PAIN_H             = 0x0201;
PAOUT              = 0x0202;
PAOUT_L            = 0x0202;
PAOUT_H            = 0x0203;
PADIR              = 0x0204;
PADIR_L            = 0x0204;
PADIR_H            = 0x0205;
PAREN              = 0x0206;
PAREN_L            = 0x0206;
PAREN_H            = 0x0207;
PADS               = 0x0208;
PADS_L             = 0x0208;
PADS_H             = 0x0209;
PASEL              = 0x020A;
PASEL_L            = 0x020A;
PASEL_H            = 0x020B;
PAIES              = 0x0218;
PAIES_L            = 0x0218;
PAIES_H            = 0x0219;
PAIE               = 0x021A;
PAIE_L             = 0x021A;
PAIE_H             = 0x021B;
PAIFG              = 0x021C;
PAIFG_L            = 0x021C;
PAIFG_H            = 0x021D;
P1IV               = 0x020E;
P1IV_L             = 0x020E;
P1IV_H             = 0x020F;
P2IV               = 0x021E;
P2IV_L             = 0x021E;
P2IV_H             = 0x021F;
                         
                     
/************************************************************
* DIGITAL I/O Port3/4 Pull up / Pull down Resistors
************************************************************/
PBIN               = 0x0220;
PBIN_L             = 0x0220;
PBIN_H             = 0x0221;
PBOUT              = 0x0222;
PBOUT_L            = 0x0222;
PBOUT_H            = 0x0223;
PBDIR              = 0x0224;
PBDIR_L            = 0x0224;
PBDIR_H            = 0x0225;
PBREN              = 0x0226;
PBREN_L            = 0x0226;
PBREN_H            = 0x0227;
PBDS               = 0x0228;
PBDS_L             = 0x0228;
PBDS_H             = 0x0229;
PBSEL              = 0x022A;
PBSEL_L            = 0x022A;
PBSEL_H            = 0x022B;
                         
                         
/************************************************************
* DIGITAL I/O Port5/6 Pull up / Pull down Resistors
************************************************************/
PCIN               = 0x0240;
PCIN_L             = 0x0240;
PCIN_H             = 0x0241;
PCOUT              = 0x0242;
PCOUT_L            = 0x0242;
PCOUT_H            = 0x0243;
PCDIR              = 0x0244;
PCDIR_L            = 0x0244;
PCDIR_H            = 0x0245;
PCREN              = 0x0246;
PCREN_L            = 0x0246;
PCREN_H            = 0x0247;
PCDS               = 0x0248;
PCDS_L             = 0x0248;
PCDS_H             = 0x0249;
PCSEL              = 0x024A;
PCSEL_L            = 0x024A;
PCSEL_H            = 0x024B;
                            
                         
/************************************************************
* DIGITAL I/O Port7/8 Pull up / Pull down Resistors
************************************************************/
PDIN               = 0x0260;
PDIN_L             = 0x0260;
PDIN_H             = 0x0261;
PDOUT              = 0x0262;
PDOUT_L            = 0x0262;
PDOUT_H            = 0x0263;
PDDIR              = 0x0264;
PDDIR_L            = 0x0264;
PDDIR_H            = 0x0265;
PDREN              = 0x0266;
PDREN_L            = 0x0266;
PDREN_H            = 0x0267;
PDDS               = 0x0268;
PDDS_L             = 0x0268;
PDDS_H             = 0x0269;
PDSEL              = 0x026A;
PDSEL_L            = 0x026A;
PDSEL_H            = 0x026B;
                            
                         
/************************************************************
* DIGITAL I/O Port9 Pull up / Pull down Resistors
************************************************************/
PEIN               = 0x0280;
PEIN_L             = 0x0280;
PEIN_H             = 0x0281;
PEOUT              = 0x0282;
PEOUT_L            = 0x0282;
PEOUT_H            = 0x0283;
PEDIR              = 0x0284;
PEDIR_L            = 0x0284;
PEDIR_H            = 0x0285;
PEREN              = 0x0286;
PEREN_L            = 0x0286;
PEREN_H            = 0x0287;
PEDS               = 0x0288;
PEDS_L             = 0x0288;
PEDS_H             = 0x0289;
PESEL              = 0x028A;
PESEL_L            = 0x028A;
PESEL_H            = 0x028B;
                            
/************************************************************
* DIGITAL I/O PortJ Pull up / Pull down Resistors
************************************************************/
PJIN               = 0x0320;
PJIN_L             = 0x0320;
PJIN_H             = 0x0321;
PJOUT              = 0x0322;
PJOUT_L            = 0x0322;
PJOUT_H            = 0x0323;
PJDIR              = 0x0324;
PJDIR_L            = 0x0324;
PJDIR_H            = 0x0325;
PJREN              = 0x0326;
PJREN_L            = 0x0326;
PJREN_H            = 0x0327;
PJDS               = 0x0328;
PJDS_L             = 0x0328;
PJDS_H             = 0x0329;
/************************************************************
* PMM - Power Management System
************************************************************/
PMMCTL0            = 0x0120;
PMMCTL0_L          = 0x0120;
PMMCTL0_H          = 0x0121;
PMMCTL1            = 0x0122;
PMMCTL1_L          = 0x0122;
PMMCTL1_H          = 0x0123;
SVSMHCTL           = 0x0124;
SVSMHCTL_L         = 0x0124;
SVSMHCTL_H         = 0x0125;
SVSMLCTL           = 0x0126;
SVSMLCTL_L         = 0x0126;
SVSMLCTL_H         = 0x0127;
SVSMIO             = 0x0128;
SVSMIO_L           = 0x0128;
SVSMIO_H           = 0x0129;
PMMIFG             = 0x012C;
PMMIFG_L           = 0x012C;
PMMIFG_H           = 0x012D;
PMMRIE             = 0x012E;
PMMRIE_L           = 0x012E;
PMMRIE_H           = 0x012F;
/*************************************************************
* RAM Control Module
*************************************************************/
RCCTL0             = 0x0158;
RCCTL0_L           = 0x0158;
RCCTL0_H           = 0x0159;
/************************************************************
* Real Time Clock
************************************************************/
RTCCTL01           = 0x04A0;
RTCCTL01_L         = 0x04A0;
RTCCTL01_H         = 0x04A1;
RTCCTL23           = 0x04A2;
RTCCTL23_L         = 0x04A2;
RTCCTL23_H         = 0x04A3;
RTCPS0CTL          = 0x04A8;
RTCPS0CTL_L        = 0x04A8;
RTCPS0CTL_H        = 0x04A9;
RTCPS1CTL          = 0x04AA;
RTCPS1CTL_L        = 0x04AA;
RTCPS1CTL_H        = 0x04AB;
RTCPS              = 0x04AC;
RTCPS_L            = 0x04AC;
RTCPS_H            = 0x04AD;
RTCIV              = 0x04AE;
RTCIV_L            = 0x04AE;
RTCIV_H            = 0x04AF;
RTCTIM0            = 0x04B0;
RTCTIM0_L          = 0x04B0;
RTCTIM0_H          = 0x04B1;
RTCTIM1            = 0x04B2;
RTCTIM1_L          = 0x04B2;
RTCTIM1_H          = 0x04B3;
RTCDATE            = 0x04B4;
RTCDATE_L          = 0x04B4;
RTCDATE_H          = 0x04B5;
RTCYEAR            = 0x04B6;
RTCYEAR_L          = 0x04B6;
RTCYEAR_H          = 0x04B7;
RTCAMINHR          = 0x04B8;
RTCAMINHR_L        = 0x04B8;
RTCAMINHR_H        = 0x04B9;
RTCADOWDAY         = 0x04BA;
RTCADOWDAY_L       = 0x04BA;
RTCADOWDAY_H       = 0x04BB;
/************************************************************
* SFR - Special Function Register Module
************************************************************/
SFRIE1             = 0x0100;
SFRIE1_L           = 0x0100;
SFRIE1_H           = 0x0101;
SFRIFG1            = 0x0102;
SFRIFG1_L          = 0x0102;
SFRIFG1_H          = 0x0103;
SFRRPCR            = 0x0104;
SFRRPCR_L          = 0x0104;
SFRRPCR_H          = 0x0105;
/************************************************************
* SYS - System Module
************************************************************/
SYSCTL             = 0x0180;
SYSCTL_L           = 0x0180;
SYSCTL_H           = 0x0181;
SYSBSLC            = 0x0182;
SYSBSLC_L          = 0x0182;
SYSBSLC_H          = 0x0183;
SYSARB             = 0x0184;
SYSARB_L           = 0x0184;
SYSARB_H           = 0x0185;
SYSJMBC            = 0x0186;
SYSJMBC_L          = 0x0186;
SYSJMBC_H          = 0x0187;
SYSJMBI0           = 0x0188;
SYSJMBI0_L         = 0x0188;
SYSJMBI0_H         = 0x0189;
SYSJMBI1           = 0x018A;
SYSJMBI1_L         = 0x018A;
SYSJMBI1_H         = 0x018B;
SYSJMBO0           = 0x018C;
SYSJMBO0_L         = 0x018C;
SYSJMBO0_H         = 0x018D;
SYSJMBO1           = 0x018E;
SYSJMBO1_L         = 0x018E;
SYSJMBO1_H         = 0x018F;
SYSBERRIV          = 0x0198;
SYSBERRIV_L        = 0x0198;
SYSBERRIV_H        = 0x0199;
SYSUNIV            = 0x019A;
SYSUNIV_L          = 0x019A;
SYSUNIV_H          = 0x019B;
SYSSNIV            = 0x019C;
SYSSNIV_L          = 0x019C;
SYSSNIV_H          = 0x019D;
SYSRSTIV           = 0x019E;
SYSRSTIV_L         = 0x019E;
SYSRSTIV_H         = 0x019F;
/************************************************************
* Timer0_A5
************************************************************/
TA0CTL             = 0x0340;
TA0CTL_L           = 0x0340;
TA0CTL_H           = 0x0341;
TA0CCTL0           = 0x0342;
TA0CCTL0_L         = 0x0342;
TA0CCTL0_H         = 0x0343;
TA0CCTL1           = 0x0344;
TA0CCTL1_L         = 0x0344;
TA0CCTL1_H         = 0x0345;
TA0CCTL2           = 0x0346;
TA0CCTL2_L         = 0x0346;
TA0CCTL2_H         = 0x0347;
TA0CCTL3           = 0x0348;
TA0CCTL3_L         = 0x0348;
TA0CCTL3_H         = 0x0349;
TA0CCTL4           = 0x034A;
TA0CCTL4_L         = 0x034A;
TA0CCTL4_H         = 0x034B;
TA0R               = 0x0350;
TA0R_L             = 0x0350;
TA0R_H             = 0x0351;
TA0CCR0            = 0x0352;
TA0CCR0_L          = 0x0352;
TA0CCR0_H          = 0x0353;
TA0CCR1            = 0x0354;
TA0CCR1_L          = 0x0354;
TA0CCR1_H          = 0x0355;
TA0CCR2            = 0x0356;
TA0CCR2_L          = 0x0356;
TA0CCR2_H          = 0x0357;
TA0CCR3            = 0x0358;
TA0CCR3_L          = 0x0358;
TA0CCR3_H          = 0x0359;
TA0CCR4            = 0x035A;
TA0CCR4_L          = 0x035A;
TA0CCR4_H          = 0x035B;
TA0IV              = 0x036E;
TA0IV_L            = 0x036E;
TA0IV_H            = 0x036F;
TA0EX0             = 0x0360;
TA0EX0_L           = 0x0360;
TA0EX0_H           = 0x0361;
/************************************************************
* Timer1_A3
************************************************************/
TA1CTL             = 0x0380;
TA1CTL_L           = 0x0380;
TA1CTL_H           = 0x0381;
TA1CCTL0           = 0x0382;
TA1CCTL0_L         = 0x0382;
TA1CCTL0_H         = 0x0383;
TA1CCTL1           = 0x0384;
TA1CCTL1_L         = 0x0384;
TA1CCTL1_H         = 0x0385;
TA1CCTL2           = 0x0386;
TA1CCTL2_L         = 0x0386;
TA1CCTL2_H         = 0x0387;
TA1R               = 0x0390;
TA1R_L             = 0x0390;
TA1R_H             = 0x0391;
TA1CCR0            = 0x0392;
TA1CCR0_L          = 0x0392;
TA1CCR0_H          = 0x0393;
TA1CCR1            = 0x0394;
TA1CCR1_L          = 0x0394;
TA1CCR1_H          = 0x0395;
TA1CCR2            = 0x0396;
TA1CCR2_L          = 0x0396;
TA1CCR2_H          = 0x0397;
TA1IV              = 0x03AE;
TA1IV_L            = 0x03AE;
TA1IV_H            = 0x03AF;
TA1EX0             = 0x03A0;
TA1EX0_L           = 0x03A0;
TA1EX0_H           = 0x03A1;
/************************************************************
* Timer B7
************************************************************/
TBCTL              = 0x03C0;
TBCTL_L            = 0x03C0;
TBCTL_H            = 0x03C1;
TBCCTL0            = 0x03C2;
TBCCTL0_L          = 0x03C2;
TBCCTL0_H          = 0x03C3;
TBCCTL1            = 0x03C4;
TBCCTL1_L          = 0x03C4;
TBCCTL1_H          = 0x03C5;
TBCCTL2            = 0x03C6;
TBCCTL2_L          = 0x03C6;
TBCCTL2_H          = 0x03C7;
TBCCTL3            = 0x03C8;
TBCCTL3_L          = 0x03C8;
TBCCTL3_H          = 0x03C9;
TBCCTL4            = 0x03CA;
TBCCTL4_L          = 0x03CA;
TBCCTL4_H          = 0x03CB;
TBCCTL5            = 0x03CC;
TBCCTL5_L          = 0x03CC;
TBCCTL5_H          = 0x03CD;
TBCCTL6            = 0x03CE;
TBCCTL6_L          = 0x03CE;
TBCCTL6_H          = 0x03CF;
TBR                = 0x03D0;
TBR_L              = 0x03D0;
TBR_H              = 0x03D1;
TBCCR0             = 0x03D2;
TBCCR0_L           = 0x03D2;
TBCCR0_H           = 0x03D3;
TBCCR1             = 0x03D4;
TBCCR1_L           = 0x03D4;
TBCCR1_H           = 0x03D5;
TBCCR2             = 0x03D6;
TBCCR2_L           = 0x03D6;
TBCCR2_H           = 0x03D7;
TBCCR3             = 0x03D8;
TBCCR3_L           = 0x03D8;
TBCCR3_H           = 0x03D9;
TBCCR4             = 0x03DA;
TBCCR4_L           = 0x03DA;
TBCCR4_H           = 0x03DB;
TBCCR5             = 0x03DC;
TBCCR5_L           = 0x03DC;
TBCCR5_H           = 0x03DD;
TBCCR6             = 0x03DE;
TBCCR6_L           = 0x03DE;
TBCCR6_H           = 0x03DF;
TBEX0              = 0x03E0;
TBEX0_L            = 0x03E0;
TBEX0_H            = 0x03E1;
TBIV               = 0x03EE;
TBIV_L             = 0x03EE;
TBIV_H             = 0x03EF;
/************************************************************
* UNIFIED CLOCK SYSTEM
************************************************************/
UCSCTL0            = 0x0160;
UCSCTL0_L          = 0x0160;
UCSCTL0_H          = 0x0161;
UCSCTL1            = 0x0162;
UCSCTL1_L          = 0x0162;
UCSCTL1_H          = 0x0163;
UCSCTL2            = 0x0164;
UCSCTL2_L          = 0x0164;
UCSCTL2_H          = 0x0165;
UCSCTL3            = 0x0166;
UCSCTL3_L          = 0x0166;
UCSCTL3_H          = 0x0167;
UCSCTL4            = 0x0168;
UCSCTL4_L          = 0x0168;
UCSCTL4_H          = 0x0169;
UCSCTL5            = 0x016A;
UCSCTL5_L          = 0x016A;
UCSCTL5_H          = 0x016B;
UCSCTL6            = 0x016C;
UCSCTL6_L          = 0x016C;
UCSCTL6_H          = 0x016D;
UCSCTL7            = 0x016E;
UCSCTL7_L          = 0x016E;
UCSCTL7_H          = 0x016F;
UCSCTL8            = 0x0170;
UCSCTL8_L          = 0x0170;
UCSCTL8_H          = 0x0171;
/************************************************************
* USB
************************************************************/
USBKEYPID          = 0x0900;
USBKEYPID_L        = 0x0900;
USBKEYPID_H        = 0x0901;
USBMCNF            = 0x0902;
USBMCNF_L          = 0x0902;
USBMCNF_H          = 0x0903;
USBPHYCTL          = 0x0904;
USBPHYCTL_L        = 0x0904;
USBPHYCTL_H        = 0x0905;
USBPWRCTL          = 0x0908;
USBPWRCTL_L        = 0x0908;
USBPWRCTL_H        = 0x0909;
USBPLLCTL          = 0x0910;
USBPLLCTL_L        = 0x0910;
USBPLLCTL_H        = 0x0911;
USBPLLDIVB         = 0x0912;
USBPLLDIVB_L       = 0x0912;
USBPLLDIVB_H       = 0x0913;
USBPLLIR           = 0x0914;
USBPLLIR_L         = 0x0914;
USBPLLIR_H         = 0x0915;
USBIEPCNF_0        = 0x0920;
USBIEPCNF_0_L      = 0x0920;
USBIEPCNF_0_H      = 0x0921;
USBIEPCNT_0        = 0x0921;
USBIEPCNT_0_L      = 0x0921;
USBIEPCNT_0_H      = 0x0922;
USBOEPCNF_0        = 0x0922;
USBOEPCNF_0_L      = 0x0922;
USBOEPCNF_0_H      = 0x0923;
USBOEPCNT_0        = 0x0923;
USBOEPCNT_0_L      = 0x0923;
USBOEPCNT_0_H      = 0x0924;
USBIEPIE           = 0x092E;
USBIEPIE_L         = 0x092E;
USBIEPIE_H         = 0x092F;
USBOEPIE           = 0x092F;
USBOEPIE_L         = 0x092F;
USBOEPIE_H         = 0x0930;
USBIEPIFG          = 0x0930;
USBIEPIFG_L        = 0x0930;
USBIEPIFG_H        = 0x0931;
USBOEPIFG          = 0x0931;
USBOEPIFG_L        = 0x0931;
USBOEPIFG_H        = 0x0932;
USBVECINT          = 0x0932;
USBVECINT_L        = 0x0932;
USBVECINT_H        = 0x0933;
USBMAINT           = 0x0936;
USBMAINT_L         = 0x0936;
USBMAINT_H         = 0x0937;
USBTSREG           = 0x0938;
USBTSREG_L         = 0x0938;
USBTSREG_H         = 0x0939;
USBFN              = 0x093A;
USBFN_L            = 0x093A;
USBFN_H            = 0x093B;
USBCTL             = 0x093C;
USBCTL_L           = 0x093C;
USBCTL_H           = 0x093D;
USBIE              = 0x093D;
USBIE_L            = 0x093D;
USBIE_H            = 0x093E;
USBIFG             = 0x093E;
USBIFG_L           = 0x093E;
USBIFG_H           = 0x093F;
USBFUNADR          = 0x093F;
USBFUNADR_L        = 0x093F;
USBFUNADR_H        = 0x0940;
USBIEPSIZXY_7      = 0x23FF;
USBIEPSIZXY_7_L    = 0x23FF;
USBIEPSIZXY_7_H    = 0x2400;
USBIEPBCTY_7       = 0x23FE;
USBIEPBCTY_7_L     = 0x23FE;
USBIEPBCTY_7_H     = 0x23FF;
USBIEPBBAY_7       = 0x23FD;
USBIEPBBAY_7_L     = 0x23FD;
USBIEPBBAY_7_H     = 0x23FE;
USBIEPBCTX_7       = 0x23FA;
USBIEPBCTX_7_L     = 0x23FA;
USBIEPBCTX_7_H     = 0x23FB;
USBIEPBBAX_7       = 0x23F9;
USBIEPBBAX_7_L     = 0x23F9;
USBIEPBBAX_7_H     = 0x23FA;
USBIEPCNF_7        = 0x23F8;
USBIEPCNF_7_L      = 0x23F8;
USBIEPCNF_7_H      = 0x23F9;
USBIEPSIZXY_6      = 0x23F7;
USBIEPSIZXY_6_L    = 0x23F7;
USBIEPSIZXY_6_H    = 0x23F8;
USBIEPBCTY_6       = 0x23F6;
USBIEPBCTY_6_L     = 0x23F6;
USBIEPBCTY_6_H     = 0x23F7;
USBIEPBBAY_6       = 0x23F5;
USBIEPBBAY_6_L     = 0x23F5;
USBIEPBBAY_6_H     = 0x23F6;
USBIEPBCTX_6       = 0x23F2;
USBIEPBCTX_6_L     = 0x23F2;
USBIEPBCTX_6_H     = 0x23F3;
USBIEPBBAX_6       = 0x23F1;
USBIEPBBAX_6_L     = 0x23F1;
USBIEPBBAX_6_H     = 0x23F2;
USBIEPCNF_6        = 0x23F0;
USBIEPCNF_6_L      = 0x23F0;
USBIEPCNF_6_H      = 0x23F1;
USBIEPSIZXY_5      = 0x23EF;
USBIEPSIZXY_5_L    = 0x23EF;
USBIEPSIZXY_5_H    = 0x23F0;
USBIEPBCTY_5       = 0x23EE;
USBIEPBCTY_5_L     = 0x23EE;
USBIEPBCTY_5_H     = 0x23EF;
USBIEPBBAY_5       = 0x23ED;
USBIEPBBAY_5_L     = 0x23ED;
USBIEPBBAY_5_H     = 0x23EE;
USBIEPBCTX_5       = 0x23EA;
USBIEPBCTX_5_L     = 0x23EA;
USBIEPBCTX_5_H     = 0x23EB;
USBIEPBBAX_5       = 0x23E9;
USBIEPBBAX_5_L     = 0x23E9;
USBIEPBBAX_5_H     = 0x23EA;
USBIEPCNF_5        = 0x23E8;
USBIEPCNF_5_L      = 0x23E8;
USBIEPCNF_5_H      = 0x23E9;
USBIEPSIZXY_4      = 0x23E7;
USBIEPSIZXY_4_L    = 0x23E7;
USBIEPSIZXY_4_H    = 0x23E8;
USBIEPBCTY_4       = 0x23E6;
USBIEPBCTY_4_L     = 0x23E6;
USBIEPBCTY_4_H     = 0x23E7;
USBIEPBBAY_4       = 0x23E5;
USBIEPBBAY_4_L     = 0x23E5;
USBIEPBBAY_4_H     = 0x23E6;
USBIEPBCTX_4       = 0x23E2;
USBIEPBCTX_4_L     = 0x23E2;
USBIEPBCTX_4_H     = 0x23E3;
USBIEPBBAX_4       = 0x23E1;
USBIEPBBAX_4_L     = 0x23E1;
USBIEPBBAX_4_H     = 0x23E2;
USBIEPCNF_4        = 0x23E0;
USBIEPCNF_4_L      = 0x23E0;
USBIEPCNF_4_H      = 0x23E1;
USBIEPSIZXY_3      = 0x23DF;
USBIEPSIZXY_3_L    = 0x23DF;
USBIEPSIZXY_3_H    = 0x23E0;
USBIEPBCTY_3       = 0x23DE;
USBIEPBCTY_3_L     = 0x23DE;
USBIEPBCTY_3_H     = 0x23DF;
USBIEPBBAY_3       = 0x23DD;
USBIEPBBAY_3_L     = 0x23DD;
USBIEPBBAY_3_H     = 0x23DE;
USBIEPBCTX_3       = 0x23DA;
USBIEPBCTX_3_L     = 0x23DA;
USBIEPBCTX_3_H     = 0x23DB;
USBIEPBBAX_3       = 0x23D9;
USBIEPBBAX_3_L     = 0x23D9;
USBIEPBBAX_3_H     = 0x23DA;
USBIEPCNF_3        = 0x23D8;
USBIEPCNF_3_L      = 0x23D8;
USBIEPCNF_3_H      = 0x23D9;
USBIEPSIZXY_2      = 0x23D7;
USBIEPSIZXY_2_L    = 0x23D7;
USBIEPSIZXY_2_H    = 0x23D8;
USBIEPBCTY_2       = 0x23D6;
USBIEPBCTY_2_L     = 0x23D6;
USBIEPBCTY_2_H     = 0x23D7;
USBIEPBBAY_2       = 0x23D5;
USBIEPBBAY_2_L     = 0x23D5;
USBIEPBBAY_2_H     = 0x23D6;
USBIEPBCTX_2       = 0x23D2;
USBIEPBCTX_2_L     = 0x23D2;
USBIEPBCTX_2_H     = 0x23D3;
USBIEPBBAX_2       = 0x23D1;
USBIEPBBAX_2_L     = 0x23D1;
USBIEPBBAX_2_H     = 0x23D2;
USBIEPCNF_2        = 0x23D0;
USBIEPCNF_2_L      = 0x23D0;
USBIEPCNF_2_H      = 0x23D1;
USBIEPSIZXY_1      = 0x23CF;
USBIEPSIZXY_1_L    = 0x23CF;
USBIEPSIZXY_1_H    = 0x23D0;
USBIEPBCTY_1       = 0x23CE;
USBIEPBCTY_1_L     = 0x23CE;
USBIEPBCTY_1_H     = 0x23CF;
USBIEPBBAY_1       = 0x23CD;
USBIEPBBAY_1_L     = 0x23CD;
USBIEPBBAY_1_H     = 0x23CE;
USBIEPBCTX_1       = 0x23CA;
USBIEPBCTX_1_L     = 0x23CA;
USBIEPBCTX_1_H     = 0x23CB;
USBIEPBBAX_1       = 0x23C9;
USBIEPBBAX_1_L     = 0x23C9;
USBIEPBBAX_1_H     = 0x23CA;
USBIEPCNF_1        = 0x23C8;
USBIEPCNF_1_L      = 0x23C8;
USBIEPCNF_1_H      = 0x23C9;
USBOEPSIZXY_7      = 0x23BF;
USBOEPSIZXY_7_L    = 0x23BF;
USBOEPSIZXY_7_H    = 0x23C0;
USBOEPBCTY_7       = 0x23BE;
USBOEPBCTY_7_L     = 0x23BE;
USBOEPBCTY_7_H     = 0x23BF;
USBOEPBBAY_7       = 0x23BD;
USBOEPBBAY_7_L     = 0x23BD;
USBOEPBBAY_7_H     = 0x23BE;
USBOEPBCTX_7       = 0x23BA;
USBOEPBCTX_7_L     = 0x23BA;
USBOEPBCTX_7_H     = 0x23BB;
USBOEPBBAX_7       = 0x23B9;
USBOEPBBAX_7_L     = 0x23B9;
USBOEPBBAX_7_H     = 0x23BA;
USBOEPCNF_7        = 0x23B8;
USBOEPCNF_7_L      = 0x23B8;
USBOEPCNF_7_H      = 0x23B9;
USBOEPSIZXY_6      = 0x23B7;
USBOEPSIZXY_6_L    = 0x23B7;
USBOEPSIZXY_6_H    = 0x23B8;
USBOEPBCTY_6       = 0x23B6;
USBOEPBCTY_6_L     = 0x23B6;
USBOEPBCTY_6_H     = 0x23B7;
USBOEPBBAY_6       = 0x23B5;
USBOEPBBAY_6_L     = 0x23B5;
USBOEPBBAY_6_H     = 0x23B6;
USBOEPBCTX_6       = 0x23B2;
USBOEPBCTX_6_L     = 0x23B2;
USBOEPBCTX_6_H     = 0x23B3;
USBOEPBBAX_6       = 0x23B1;
USBOEPBBAX_6_L     = 0x23B1;
USBOEPBBAX_6_H     = 0x23B2;
USBOEPCNF_6        = 0x23B0;
USBOEPCNF_6_L      = 0x23B0;
USBOEPCNF_6_H      = 0x23B1;
USBOEPSIZXY_5      = 0x23AF;
USBOEPSIZXY_5_L    = 0x23AF;
USBOEPSIZXY_5_H    = 0x23B0;
USBOEPBCTY_5       = 0x23AE;
USBOEPBCTY_5_L     = 0x23AE;
USBOEPBCTY_5_H     = 0x23AF;
USBOEPBBAY_5       = 0x23AD;
USBOEPBBAY_5_L     = 0x23AD;
USBOEPBBAY_5_H     = 0x23AE;
USBOEPBCTX_5       = 0x23AA;
USBOEPBCTX_5_L     = 0x23AA;
USBOEPBCTX_5_H     = 0x23AB;
USBOEPBBAX_5       = 0x23A9;
USBOEPBBAX_5_L     = 0x23A9;
USBOEPBBAX_5_H     = 0x23AA;
USBOEPCNF_5        = 0x23A8;
USBOEPCNF_5_L      = 0x23A8;
USBOEPCNF_5_H      = 0x23A9;
USBOEPSIZXY_4      = 0x23A7;
USBOEPSIZXY_4_L    = 0x23A7;
USBOEPSIZXY_4_H    = 0x23A8;
USBOEPBCTY_4       = 0x23A6;
USBOEPBCTY_4_L     = 0x23A6;
USBOEPBCTY_4_H     = 0x23A7;
USBOEPBBAY_4       = 0x23A5;
USBOEPBBAY_4_L     = 0x23A5;
USBOEPBBAY_4_H     = 0x23A6;
USBOEPBCTX_4       = 0x23A2;
USBOEPBCTX_4_L     = 0x23A2;
USBOEPBCTX_4_H     = 0x23A3;
USBOEPBBAX_4       = 0x23A1;
USBOEPBBAX_4_L     = 0x23A1;
USBOEPBBAX_4_H     = 0x23A2;
USBOEPCNF_4        = 0x23A0;
USBOEPCNF_4_L      = 0x23A0;
USBOEPCNF_4_H      = 0x23A1;
USBOEPSIZXY_3      = 0x239F;
USBOEPSIZXY_3_L    = 0x239F;
USBOEPSIZXY_3_H    = 0x23A0;
USBOEPBCTY_3       = 0x239E;
USBOEPBCTY_3_L     = 0x239E;
USBOEPBCTY_3_H     = 0x239F;
USBOEPBBAY_3       = 0x239D;
USBOEPBBAY_3_L     = 0x239D;
USBOEPBBAY_3_H     = 0x239E;
USBOEPBCTX_3       = 0x239A;
USBOEPBCTX_3_L     = 0x239A;
USBOEPBCTX_3_H     = 0x239B;
USBOEPBBAX_3       = 0x2399;
USBOEPBBAX_3_L     = 0x2399;
USBOEPBBAX_3_H     = 0x239A;
USBOEPCNF_3        = 0x2398;
USBOEPCNF_3_L      = 0x2398;
USBOEPCNF_3_H      = 0x2399;
USBOEPSIZXY_2      = 0x2397;
USBOEPSIZXY_2_L    = 0x2397;
USBOEPSIZXY_2_H    = 0x2398;
USBOEPBCTY_2       = 0x2396;
USBOEPBCTY_2_L     = 0x2396;
USBOEPBCTY_2_H     = 0x2397;
USBOEPBBAY_2       = 0x2395;
USBOEPBBAY_2_L     = 0x2395;
USBOEPBBAY_2_H     = 0x2396;
USBOEPBCTX_2       = 0x2392;
USBOEPBCTX_2_L     = 0x2392;
USBOEPBCTX_2_H     = 0x2393;
USBOEPBBAX_2       = 0x2391;
USBOEPBBAX_2_L     = 0x2391;
USBOEPBBAX_2_H     = 0x2392;
USBOEPCNF_2        = 0x2390;
USBOEPCNF_2_L      = 0x2390;
USBOEPCNF_2_H      = 0x2391;
USBOEPSIZXY_1      = 0x238F;
USBOEPSIZXY_1_L    = 0x238F;
USBOEPSIZXY_1_H    = 0x2390;
USBOEPBCTY_1       = 0x238E;
USBOEPBCTY_1_L     = 0x238E;
USBOEPBCTY_1_H     = 0x238F;
USBOEPBBAY_1       = 0x238D;
USBOEPBBAY_1_L     = 0x238D;
USBOEPBBAY_1_H     = 0x238E;
USBOEPBCTX_1       = 0x238A;
USBOEPBCTX_1_L     = 0x238A;
USBOEPBCTX_1_H     = 0x238B;
USBOEPBBAX_1       = 0x2389;
USBOEPBBAX_1_L     = 0x2389;
USBOEPBBAX_1_H     = 0x238A;
USBOEPCNF_1        = 0x2388;
USBOEPCNF_1_L      = 0x2388;
USBOEPCNF_1_H      = 0x2389;
USBSTABUFF         = 0x1C00;
USBSTABUFF_L       = 0x1C00;
USBSTABUFF_H       = 0x1C01;
/************************************************************
* USCI
************************************************************/
UCA0CTL0           = 0x05C1;
UCA0CTL0_L         = 0x05C1;
UCA0CTL0_H         = 0x05C2;
UCA0CTL1           = 0x05C0;
UCA0CTL1_L         = 0x05C0;
UCA0CTL1_H         = 0x05C1;
UCA0BR0            = 0x05C6;
UCA0BR0_L          = 0x05C6;
UCA0BR0_H          = 0x05C7;
UCA0BR1            = 0x05C7;
UCA0BR1_L          = 0x05C7;
UCA0BR1_H          = 0x05C8;
UCA0MCTL           = 0x05C8;
UCA0MCTL_L         = 0x05C8;
UCA0MCTL_H         = 0x05C9;
UCA0STAT           = 0x05CA;
UCA0STAT_L         = 0x05CA;
UCA0STAT_H         = 0x05CB;
UCA0RXBUF          = 0x05CC;
UCA0RXBUF_L        = 0x05CC;
UCA0RXBUF_H        = 0x05CD;
UCA0TXBUF          = 0x05CE;
UCA0TXBUF_L        = 0x05CE;
UCA0TXBUF_H        = 0x05CF;
UCA0ABCTL          = 0x05D0;
UCA0ABCTL_L        = 0x05D0;
UCA0ABCTL_H        = 0x05D1;
UCA0IRTCTL         = 0x05D2;
UCA0IRTCTL_L       = 0x05D2;
UCA0IRTCTL_H       = 0x05D3;
UCA0IRRCTL         = 0x05D3;
UCA0IRRCTL_L       = 0x05D3;
UCA0IRRCTL_H       = 0x05D4;
UCA0IE             = 0x05DC;
UCA0IE_L           = 0x05DC;
UCA0IE_H           = 0x05DD;
UCA0IFG            = 0x05DD;
UCA0IFG_L          = 0x05DD;
UCA0IFG_H          = 0x05DE;
UCA0IV             = 0x05DE;
UCA0IV_L           = 0x05DE;
UCA0IV_H           = 0x05DF;
UCB0CTL0           = 0x05E1;
UCB0CTL0_L         = 0x05E1;
UCB0CTL0_H         = 0x05E2;
UCB0CTL1           = 0x05E0;
UCB0CTL1_L         = 0x05E0;
UCB0CTL1_H         = 0x05E1;
UCB0BR0            = 0x05E6;
UCB0BR0_L          = 0x05E6;
UCB0BR0_H          = 0x05E7;
UCB0BR1            = 0x05E7;
UCB0BR1_L          = 0x05E7;
UCB0BR1_H          = 0x05E8;
UCB0STAT           = 0x05EA;
UCB0STAT_L         = 0x05EA;
UCB0STAT_H         = 0x05EB;
UCB0RXBUF          = 0x05EC;
UCB0RXBUF_L        = 0x05EC;
UCB0RXBUF_H        = 0x05ED;
UCB0TXBUF          = 0x05EE;
UCB0TXBUF_L        = 0x05EE;
UCB0TXBUF_H        = 0x05EF;
UCB0I2COA          = 0x05F0;
UCB0I2COA_L        = 0x05F0;
UCB0I2COA_H        = 0x05F1;
UCB0I2CSA          = 0x05F2;
UCB0I2CSA_L        = 0x05F2;
UCB0I2CSA_H        = 0x05F3;
UCB0IE             = 0x05FC;
UCB0IE_L           = 0x05FC;
UCB0IE_H           = 0x05FD;
UCB0IFG            = 0x05FD;
UCB0IFG_L          = 0x05FD;
UCB0IFG_H          = 0x05FE;
UCB0IV             = 0x05FE;
UCB0IV_L           = 0x05FE;
UCB0IV_H           = 0x05FF;
UCA1CTL0           = 0x0601;
UCA1CTL0_L         = 0x0601;
UCA1CTL0_H         = 0x0602;
UCA1CTL1           = 0x0600;
UCA1CTL1_L         = 0x0600;
UCA1CTL1_H         = 0x0601;
UCA1BR0            = 0x0606;
UCA1BR0_L          = 0x0606;
UCA1BR0_H          = 0x0607;
UCA1BR1            = 0x0607;
UCA1BR1_L          = 0x0607;
UCA1BR1_H          = 0x0608;
UCA1MCTL           = 0x0608;
UCA1MCTL_L         = 0x0608;
UCA1MCTL_H         = 0x0609;
UCA1STAT           = 0x060A;
UCA1STAT_L         = 0x060A;
UCA1STAT_H         = 0x060B;
UCA1RXBUF          = 0x060C;
UCA1RXBUF_L        = 0x060C;
UCA1RXBUF_H        = 0x060D;
UCA1TXBUF          = 0x060E;
UCA1TXBUF_L        = 0x060E;
UCA1TXBUF_H        = 0x060F;
UCA1ABCTL          = 0x0610;
UCA1ABCTL_L        = 0x0610;
UCA1ABCTL_H        = 0x0611;
UCA1IRTCTL         = 0x0612;
UCA1IRTCTL_L       = 0x0612;
UCA1IRTCTL_H       = 0x0613;
UCA1IRRCTL         = 0x0613;
UCA1IRRCTL_L       = 0x0613;
UCA1IRRCTL_H       = 0x0614;
UCA1IE             = 0x061C;
UCA1IE_L           = 0x061C;
UCA1IE_H           = 0x061D;
UCA1IFG            = 0x061D;
UCA1IFG_L          = 0x061D;
UCA1IFG_H          = 0x061E;
UCA1IV             = 0x061E;
UCA1IV_L           = 0x061E;
UCA1IV_H           = 0x061F;
UCB1CTL0           = 0x0621;
UCB1CTL0_L         = 0x0621;
UCB1CTL0_H         = 0x0622;
UCB1CTL1           = 0x0620;
UCB1CTL1_L         = 0x0620;
UCB1CTL1_H         = 0x0621;
UCB1BR0            = 0x0626;
UCB1BR0_L          = 0x0626;
UCB1BR0_H          = 0x0627;
UCB1BR1            = 0x0627;
UCB1BR1_L          = 0x0627;
UCB1BR1_H          = 0x0628;
UCB1STAT           = 0x062A;
UCB1STAT_L         = 0x062A;
UCB1STAT_H         = 0x062B;
UCB1RXBUF          = 0x062C;
UCB1RXBUF_L        = 0x062C;
UCB1RXBUF_H        = 0x062D;
UCB1TXBUF          = 0x062E;
UCB1TXBUF_L        = 0x062E;
UCB1TXBUF_H        = 0x062F;
UCB1I2COA          = 0x0630;
UCB1I2COA_L        = 0x0630;
UCB1I2COA_H        = 0x0631;
UCB1I2CSA          = 0x0632;
UCB1I2CSA_L        = 0x0632;
UCB1I2CSA_H        = 0x0633;
UCB1IE             = 0x063C;
UCB1IE_L           = 0x063C;
UCB1IE_H           = 0x063D;
UCB1IFG            = 0x063D;
UCB1IFG_L          = 0x063D;
UCB1IFG_H          = 0x063E;
UCB1IV             = 0x063E;
UCB1IV_L           = 0x063E;
UCB1IV_H           = 0x063F;
/************************************************************
* WATCHDOG TIMER A
************************************************************/
WDTCTL             = 0x015C;
WDTCTL_L           = 0x015C;
WDTCTL_H           = 0x015D;
 
/************************************************************
* Interrupt Vectors (offset from 0xFFC0)
************************************************************/
/************************************************************
* End of Modules
************************************************************/

/****************************************************************************/
/* SPECIFY THE SYSTEM MEMORY MAP                                            */
/****************************************************************************/

MEMORY
{
    SFR                     : origin = 0x0000, length = 0x0010
    PERIPHERALS_8BIT        : origin = 0x0010, length = 0x00F0
    PERIPHERALS_16BIT       : origin = 0x0100, length = 0x0100
    RAM                     : origin = 0x2400, length = 0x4000
    INFOA                   : origin = 0x1980, length = 0x0080
    INFOB                   : origin = 0x1900, length = 0x0080
    INFOC                   : origin = 0x1880, length = 0x0080
    INFOD                   : origin = 0x1800, length = 0x0080
    FLASH                   : origin = 0x6400, length = 0x9B80
    FLASH2                  : origin = 0x10000,length = 0x36400
    INT00                   : origin = 0xFF80, length = 0x0002
    INT01                   : origin = 0xFF82, length = 0x0002
    INT02                   : origin = 0xFF84, length = 0x0002
    INT03                   : origin = 0xFF86, length = 0x0002
    INT04                   : origin = 0xFF88, length = 0x0002
    INT05                   : origin = 0xFF8A, length = 0x0002
    INT06                   : origin = 0xFF8C, length = 0x0002
    INT07                   : origin = 0xFF8E, length = 0x0002
    INT08                   : origin = 0xFF90, length = 0x0002
    INT09                   : origin = 0xFF92, length = 0x0002
    INT10                   : origin = 0xFF94, length = 0x0002
    INT11                   : origin = 0xFF96, length = 0x0002
    INT12                   : origin = 0xFF98, length = 0x0002
    INT13                   : origin = 0xFF9A, length = 0x0002
    INT14                   : origin = 0xFF9C, length = 0x0002
    INT15                   : origin = 0xFF9E, length = 0x0002
    INT16                   : origin = 0xFFA0, length = 0x0002
    INT17                   : origin = 0xFFA2, length = 0x0002
    INT18                   : origin = 0xFFA4, length = 0x0002
    INT19                   : origin = 0xFFA6, length = 0x0002
    INT20                   : origin = 0xFFA8, length = 0x0002
    INT21                   : origin = 0xFFAA, length = 0x0002
    INT22                   : origin = 0xFFAC, length = 0x0002
    INT23                   : origin = 0xFFAE, length = 0x0002
    INT24                   : origin = 0xFFB0, length = 0x0002
    INT25                   : origin = 0xFFB2, length = 0x0002
    INT26                   : origin = 0xFFB4, length = 0x0002
    INT27                   : origin = 0xFFB6, length = 0x0002
    INT28                   : origin = 0xFFB8, length = 0x0002
    INT29                   : origin = 0xFFBA, length = 0x0002
    INT30                   : origin = 0xFFBC, length = 0x0002
    INT31                   : origin = 0xFFBE, length = 0x0002
    INT32                   : origin = 0xFFC0, length = 0x0002
    INT33                   : origin = 0xFFC2, length = 0x0002
    INT34                   : origin = 0xFFC4, length = 0x0002
    INT35                   : origin = 0xFFC6, length = 0x0002
    INT36                   : origin = 0xFFC8, length = 0x0002
    INT37                   : origin = 0xFFCA, length = 0x0002
    INT38                   : origin = 0xFFCC, length = 0x0002
    INT39                   : origin = 0xFFCE, length = 0x0002
    INT40                   : origin = 0xFFD0, length = 0x0002
    INT41                   : origin = 0xFFD2, length = 0x0002
    INT42                   : origin = 0xFFD4, length = 0x0002
    INT43                   : origin = 0xFFD6, length = 0x0002
    INT44                   : origin = 0xFFD8, length = 0x0002
    INT45                   : origin = 0xFFDA, length = 0x0002
    INT46                   : origin = 0xFFDC, length = 0x0002
    INT47                   : origin = 0xFFDE, length = 0x0002
    INT48                   : origin = 0xFFE0, length = 0x0002
    INT49                   : origin = 0xFFE2, length = 0x0002
    INT50                   : origin = 0xFFE4, length = 0x0002
    INT51                   : origin = 0xFFE6, length = 0x0002
    INT52                   : origin = 0xFFE8, length = 0x0002
    INT53                   : origin = 0xFFEA, length = 0x0002
    INT54                   : origin = 0xFFEC, length = 0x0002
    INT55                   : origin = 0xFFEE, length = 0x0002
    INT56                   : origin = 0xFFF0, length = 0x0002
    INT57                   : origin = 0xFFF2, length = 0x0002
    INT58                   : origin = 0xFFF4, length = 0x0002
    INT59                   : origin = 0xFFF6, length = 0x0002
    INT60                   : origin = 0xFFF8, length = 0x0002
    INT61                   : origin = 0xFFFA, length = 0x0002
    INT62                   : origin = 0xFFFC, length = 0x0002
    RESET                   : origin = 0xFFFE, length = 0x0002
}

/****************************************************************************/
/* SPECIFY THE SECTIONS ALLOCATION INTO MEMORY                              */
/****************************************************************************/

SECTIONS
{
    .bss       : {} > RAM                /* GLOBAL & STATIC VARS              */
    .sysmem    : {} > RAM                /* DYNAMIC MEMORY ALLOCATION AREA    */
    .stack     : {} > RAM (HIGH)         /* SOFTWARE SYSTEM STACK             */

    .text      : {}>> FLASH | FLASH2     /* CODE                              */
    .text:_isr : {} > FLASH              /* ISR CODE SPACE                    */
    .cinit     : {} > FLASH              /* INITIALIZATION TABLES             */
    .const     : {} > FLASH | FLASH2     /* CONSTANT DATA                     */
    .cio       : {} > RAM                /* C I/O BUFFER                      */

    .pinit     : {} > FLASH              /* C++ CONSTRUCTOR TABLES            */

    .infoA     : {} > INFOA              /* MSP430 INFO FLASH MEMORY SEGMENTS */
    .infoB     : {} > INFOB
    .infoC     : {} > INFOC
    .infoD     : {} > INFOD

    .int00   : {} > INT00                /* MSP430 INTERRUPT VECTORS          */
    .int01   : {} > INT01
    .int02   : {} > INT02
    .int03   : {} > INT03
    .int04   : {} > INT04
    .int05   : {} > INT05
    .int06   : {} > INT06
    .int07   : {} > INT07
    .int08   : {} > INT08
    .int09   : {} > INT09
    .int10   : {} > INT10
    .int11   : {} > INT11
    .int12   : {} > INT12
    .int13   : {} > INT13
    .int14   : {} > INT14
    .int15   : {} > INT15
    .int16   : {} > INT16
    .int17   : {} > INT17
    .int18   : {} > INT18
    .int19   : {} > INT19
    .int20   : {} > INT20
    .int21   : {} > INT21
    .int22   : {} > INT22
    .int23   : {} > INT23
    .int24   : {} > INT24
    .int25   : {} > INT25
    .int26   : {} > INT26
    .int27   : {} > INT27
    .int28   : {} > INT28
    .int29   : {} > INT29
    .int30   : {} > INT30
    .int31   : {} > INT31
    .int32   : {} > INT32
    .int33   : {} > INT33
    .int34   : {} > INT34
    .int35   : {} > INT35
    .int36   : {} > INT36
    .int37   : {} > INT37
    .int38   : {} > INT38
    .int39   : {} > INT39
    .int40   : {} > INT40
    .int41   : {} > INT41
    .int42   : {} > INT42
    .int43   : {} > INT43
    .int44   : {} > INT44
    .int45   : {} > INT45
    .int46   : {} > INT46
    .int47   : {} > INT47
    .int48   : {} > INT48
    .int49   : {} > INT49
    .int50   : {} > INT50
    .int51   : {} > INT51
    .int52   : {} > INT52
    .int53   : {} > INT53
    .int54   : {} > INT54
    .int55   : {} > INT55
    .int56   : {} > INT56
    .int57   : {} > INT57
    .int58   : {} > INT58
    .int59   : {} > INT59
    .int60   : {} > INT60
    .int61   : {} > INT61
    .int62   : {} > INT62
    .reset   : {} > RESET              /* MSP430 RESET VECTOR               */ 
}

