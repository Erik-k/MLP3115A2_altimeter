/***********************************************************************************************\
* Freescale MPL3115A2 Driver
*
* Filename: spi.h
*
*
* (c) Copyright 2009, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
\***********************************************************************************************/
#ifndef _SPI_H_
#define _SPI_H_

/***********************************************************************************************\
* Public macros
\***********************************************************************************************/

#ifdef _MC9S08QE64_H

/*** SPI1C1 - SPI Control Register 1; 0x00000028 ***/
#define SPIC1                           SPI1C1
#define SPIC1_LSBFE                     SPI1C1_LSBFE
#define SPIC1_SSOE                      SPI1C1_SSOE
#define SPIC1_CPHA                      SPI1C1_CPHA
#define SPIC1_CPOL                      SPI1C1_CPOL
#define SPIC1_MSTR                      SPI1C1_MSTR
#define SPIC1_SPTIE                     SPI1C1_SPTIE
#define SPIC1_SPE                       SPI1C1_SPE
#define SPIC1_SPIE                      SPI1C1_SPIE

#define SPIC1_LSBFE_MASK                SPI1C1_LSBFE_MASK
#define SPIC1_SSOE_MASK                 SPI1C1_SSOE_MASK
#define SPIC1_CPHA_MASK                 SPI1C1_CPHA_MASK
#define SPIC1_CPOL_MASK                 SPI1C1_CPOL_MASK
#define SPIC1_MSTR_MASK                 SPI1C1_MSTR_MASK
#define SPIC1_SPTIE_MASK                SPI1C1_SPTIE_MASK
#define SPIC1_SPE_MASK                  SPI1C1_SPE_MASK
#define SPIC1_SPIE_MASK                 SPI1C1_SPIE_MASK

/*** SPI1C2 - SPI Control Register 2; 0x00000029 ***/
#define SPIC2                           SPI1C2
#define SPIC2_SPC0                      SPI1C2_SPC0
#define SPIC2_SPISWAI                   SPI1C2_SPISWAI
#define SPIC2_BIDIROE                   SPI1C2_BIDIROE
#define SPIC2_MODFEN                    SPI1C2_MODFEN

#define SPIC2_SPC0_MASK                 SPI1C2_SPC0_MASK
#define SPIC2_SPISWAI_MASK              SPI1C2_SPISWAI_MASK
#define SPIC2_BIDIROE_MASK              SPI1C2_BIDIROE_MASK
#define SPIC2_MODFEN_MASK               SPI1C2_MODFEN_MASK

/*** SPI1BR - SPI Baud Rate Register; 0x0000002A ***/
#define SPIBR                           SPI1BR
#define SPIBR_SPR0                      SPI1BR_SPR0
#define SPIBR_SPR1                      SPI1BR_SPR1
#define SPIBR_SPR2                      SPI1BR_SPR2
#define SPIBR_SPPR0                     SPI1BR_SPPR0
#define SPIBR_SPPR1                     SPI1BR_SPPR1
#define SPIBR_SPPR2                     SPI1BR_SPPR2
#define SPIBR_SPR                       SPI1BR_SPR
#define SPIBR_SPPR                      SPI1BR_SPPR

#define SPIBR_SPR0_MASK                 SPI1BR_SPR0_MASK
#define SPIBR_SPR1_MASK                 SPI1BR_SPR1_MASK
#define SPIBR_SPR2_MASK                 SPI1BR_SPR2_MASK
#define SPIBR_SPPR0_MASK                SPI1BR_SPPR0_MASK
#define SPIBR_SPPR1_MASK                SPI1BR_SPPR1_MASK
#define SPIBR_SPPR2_MASK                SPI1BR_SPPR2_MASK
#define SPIBR_SPR_MASK                  SPI1BR_SPR_MASK
#define SPIBR_SPR_BITNUM                SPI1BR_SPR_BITNUM
#define SPIBR_SPPR_MASK                 SPI1BR_SPPR_MASK
#define SPIBR_SPPR_BITNUM               SPI1BR_SPPR_BITNUM

/*** SPIS - SPI Status Register; 0x0000002B ***/
#define SPIS                            SPI1S
#define SPIS_MODF                       SPI1S_MODF
#define SPIS_SPTEF                      SPI1S_SPTEF
#define SPIS_SPRF                       SPI1S_SPRF

#define SPIS_MODF_MASK                  SPI1S_MODF_MASK
#define SPIS_SPTEF_MASK                 SPI1S_SPTEF_MASK
#define SPIS_SPRF_MASK                  SPI1S_SPRF_MASK

/*** SPI1D - SPI Data Register; 0x0000002D ***/
#define SPID                            SPI1D

#endif    /* _MC9S08QE64_H */


/***********************************************************************************************
**
**  Serial Peripheral Interface (SPI)
**
**  0x0028  SPIC1     SPI Control Register 1
**  0x0029  SPIC2     SPI Control Register 2
**  0x002A  SPIBR     SPI Baud Rate Register
**  0x002B  SPIS      SPI Status Register
**  0x002D  SPID      SPI Data Register
**
**  SPI target bit rate = 5MHz
**  MCU bus frequency = 9.216MHz
**
**  SPI SPPR = 0 ;  hence prescale = 1
**  SPI SPR  = 0 ;  hence rate     = 2
**
**  SPI bit rare = bus / prescale / rate
**               = 9.216MHz / 1 / 2
**               = 4.608MHz
*/

#define init_SPIC1    0b01011100
/*                      00000100 = reset
**                      ||||||||
**                      |||||||+-- LSBFE    =0  : MSB first
**                      ||||||+--- SSOE     =0  : SS controlled via GPIO
**                      |||||+---- CPHA     =0  : First edge occurs in the middle
**                      ||||+----- CPOL     =0  : Active high clock - idles low
**                      |||+------ MSTR     =1  : SPI configured as master
**                      ||+------- SPTIE    =0  : SPI transmit interrupts disabled
**                      |+-------- SPE      =1  : SPI module enabled
**                      +--------- SPIE     =0  : SPI receive interrupts disabled
*/

#define init_SPIC2    0b00000000
/*                      00000000 = reset
**                      xxx||x||
**                         || |+-- SPC0     =0  : SPI uses separate MOSI/MISO
**                         || +--- SPISWAI  =0  : SPI operates in wait mode
**                         |+----- BIDIROE  =0  :
**                         +------ MODFEN   =0  : SS controlled via GPIO
*/

#define init_SPIBR    0b00000000
/*                      00000000 = reset
**                      x|||||||
**                       |||++++-- SPR      =0  : SPI clock divider set for divide-by-2
**                       +++------ SPPR     =0  : SPI clock prescaler set for divide-by-1
*/


/***********************************************************************************************\
* Public type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

/***********************************************************************************************\
* Public prototypes
\***********************************************************************************************/

void SPI_ChrShift(byte data);
byte SPI_ChrShiftR(byte data);
void SPI_ChrShift5(byte data1, byte data2, byte data3, byte data4, byte data5);
void SPI_FullShift4(byte data1, byte data2, byte data3, byte data4);


#endif  /* _SPI_H_ */
