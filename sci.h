/***********************************************************************************************\
* Freescale MPL3115A2 Driver
*
* Filename: sci.h
*
*
* (c) Copyright 2009, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
\***********************************************************************************************/
#ifndef _SCI_H_
#define _SCI_H_

/***********************************************************************************************\
* Public macros
\***********************************************************************************************/

#define BUFFER_RX_SIZE        16
#define BUFFER_TX_SIZE        200

#ifdef _MC9S08QE64_H

/*** SCI1BD - SCI1 Baud Rate Register; 0x00000020 ***/
#define SCIBDH                          SCI1BDH
#define SCIBDL                          SCI1BDL
#define SCIBD                           SCI1BD

/*** SCI1C1 - SCI1 Control Register 1; 0x00000022 ***/
#define SCIC1                           SCI1C1
#define SCIC1_PT                        SCI1C1_PT
#define SCIC1_PE                        SCI1C1_PE
#define SCIC1_ILT                       SCI1C1_ILT
#define SCIC1_WAKE                      SCI1C1_WAKE
#define SCIC1_M                         SCI1C1_M
#define SCIC1_RSRC                      SCI1C1_RSRC
#define SCIC1_SCISWAI                   SCI1C1_SCISWAI
#define SCIC1_LOOPS                     SCI1C1_LOOPS

#define SCIC1_PT_MASK                   SCI1C1_PT_MASK
#define SCIC1_PE_MASK                   SCI1C1_PE_MASK
#define SCIC1_ILT_MASK                  SCI1C1_ILT_MASK
#define SCIC1_WAKE_MASK                 SCI1C1_WAKE_MASK
#define SCIC1_M_MASK                    SCI1C1_M_MASK
#define SCIC1_RSRC_MASK                 SCI1C1_RSRC_MASK
#define SCIC1_SCISWAI_MASK              SCI1C1_SCISWAI_MASK
#define SCIC1_LOOPS_MASK                SCI1C1_LOOPS_MASK

/*** SCI1C2 - SCI1 Control Register 2; 0x00000023 ***/
#define SCIC2                           SCI1C2
#define SCIC2_SBK                       SCI1C2_SBK
#define SCIC2_RWU                       SCI1C2_RWU
#define SCIC2_RE                        SCI1C2_RE
#define SCIC2_TE                        SCI1C2_TE
#define SCIC2_ILIE                      SCI1C2_ILIE
#define SCIC2_RIE                       SCI1C2_RIE
#define SCIC2_TCIE                      SCI1C2_TCIE
#define SCIC2_TIE                       SCI1C2_TIE

#define SCIC2_SBK_MASK                  SCI1C2_SBK_MASK
#define SCIC2_RWU_MASK                  SCI1C2_RWU_MASK
#define SCIC2_RE_MASK                   SCI1C2_RE_MASK
#define SCIC2_TE_MASK                   SCI1C2_TE_MASK
#define SCIC2_ILIE_MASK                 SCI1C2_ILIE_MASK
#define SCIC2_RIE_MASK                  SCI1C2_RIE_MASK
#define SCIC2_TCIE_MASK                 SCI1C2_TCIE_MASK
#define SCIC2_TIE_MASK                  SCI1C2_TIE_MASK

/*** SCI1S1 - SCI1 Status Register 1; 0x00000024 ***/
#define SCIS1                           SCI1S1
#define SCIS1_PF                        SCI1S1_PF
#define SCIS1_FE                        SCI1S1_FE
#define SCIS1_NF                        SCI1S1_NF
#define SCIS1_OR                        SCI1S1_OR
#define SCIS1_IDLE                      SCI1S1_IDLE
#define SCIS1_RDRF                      SCI1S1_RDRF
#define SCIS1_TC                        SCI1S1_TC
#define SCIS1_TDRE                      SCI1S1_TDRE

#define SCIS1_PF_MASK                   SCI1S1_PF_MASK
#define SCIS1_FE_MASK                   SCI1S1_FE_MASK
#define SCIS1_NF_MASK                   SCI1S1_NF_MASK
#define SCIS1_OR_MASK                   SCI1S1_OR_MASK
#define SCIS1_IDLE_MASK                 SCI1S1_IDLE_MASK
#define SCIS1_RDRF_MASK                 SCI1S1_RDRF_MASK
#define SCIS1_TC_MASK                   SCI1S1_TC_MASK
#define SCIS1_TDRE_MASK                 SCI1S1_TDRE_MASK

/*** SCI1S2 - SCI1 Status Register 2; 0x00000025 ***/
#define SCIS2                           SCI1S2
#define SCIS2_RAF                       SCI1S2_RAF
#define SCIS2_LBKDE                     SCI1S2_LBKDE
#define SCIS2_BRK13                     SCI1S2_BRK13
#define SCIS2_RWUID                     SCI1S2_RWUID
#define SCIS2_RXINV                     SCI1S2_RXINV
#define SCIS2_RXEDGIF                   SCI1S2_RXEDGIF
#define SCIS2_LBKDIF                    SCI1S2_LBKDIF

#define SCIS2_RAF_MASK                  SCI1S2_RAF_MASK
#define SCIS2_LBKDE_MASK                SCI1S2_LBKDE_MASK
#define SCIS2_BRK13_MASK                SCI1S2_BRK13_MASK
#define SCIS2_RWUID_MASK                SCI1S2_RWUID_MASK
#define SCIS2_RXINV_MASK                SCI1S2_RXINV_MASK
#define SCIS2_RXEDGIF_MASK              SCI1S2_RXEDGIF_MASK
#define SCIS2_LBKDIF_MASK               SCI1S2_LBKDIF_MASK

/*** SCI1C3 - SCI1 Control Register 3; 0x00000026 ***/
#define SCIC3                           SCI1C3
#define SCIC3_PEIE                      SCI1C3_PEIE
#define SCIC3_FEIE                      SCI1C3_FEIE
#define SCIC3_NEIE                      SCI1C3_NEIE
#define SCIC3_ORIE                      SCI1C3_ORIE
#define SCIC3_TXINV                     SCI1C3_TXINV
#define SCIC3_TXDIR                     SCI1C3_TXDIR
#define SCIC3_T8                        SCI1C3_T8
#define SCIC3_R8                        SCI1C3_R8

#define SCIC3_PEIE_MASK                 SCI1C3_PEIE_MASK
#define SCIC3_FEIE_MASK                 SCI1C3_FEIE_MASK
#define SCIC3_NEIE_MASK                 SCI1C3_NEIE_MASK
#define SCIC3_ORIE_MASK                 SCI1C3_ORIE_MASK
#define SCIC3_TXINV_MASK                SCI1C3_TXINV_MASK
#define SCIC3_TXDIR_MASK                SCI1C3_TXDIR_MASK
#define SCIC3_T8_MASK                   SCI1C3_T8_MASK
#define SCIC3_R8_MASK                   SCI1C3_R8_MASK

/*** SCI1D - SCI1 Data Register; 0x00000027 ***/
#define SCID                            SCI1D

#endif    /* _MC9S08QE64_H */


/***********************************************************************************************
**
**  Serial Communications Interface (SCI)
**
**  0x0020  SCIBDH    SCI Baud Rate Register High
**  0x0021  SCIBDL    SCI Baud Rate Register Low
**  0x0022  SCIC1     SCI Control Register 1
**  0x0023  SCIC2     SCI Control Register 2
**  0x0024  SCIS1     SCI Status Register 1
**  0x0025  SCIS2     SCI Status Register 2
**  0x0026  SCIC3     SCI Control Register 3
**  0x0027  SCID      SCI Data Register
**
**  SCI target baudrate = 115.2k
**  MCU bus frequency = 9.216MHz
**
**  SCI Baud Rate Register = 5
**
**  SCI baudrate = bus / (16 * BR)
**               = 9.216MHz / (16 * 5)
**               = 115.2k
*/

#define init_SCIBDH   0x00
#define init_SCIBDL   0x05

#define init_SCIC1    0b00000000
/*                      00000000 = reset
**                      ||||||||
**                      |||||||+-- PT       =0  : Even parity, if PE =1
**                      ||||||+--- PE       =0  : No parity
**                      |||||+---- ILT      =0  : Idle character bit count starts after start bit
**                      ||||+----- WAKE     =0  : Idle-line wakeup
**                      |||+------ M        =0  : 8-bit mode
**                      ||+------- RSRC     =0  : no meaning when LOOPS =0
**                      |+-------- SCISWAI  =0  : SCI runs in Wait mode
**                      +--------- LOOPS    =0  : Normal operation
*/

#define init_SCIC2    0b00101100
/*                      00000000 = reset
**                      ||||||||
**                      |||||||+-- SBK      =0  : Normal transmitter operation
**                      ||||||+--- RWU      =0  : Normal SCI operation
**                      |||||+---- RE       =1  : Receiver enabled
**                      ||||+----- TE       =1  : Transmitter enabled
**                      |||+------ ILIE     =0  : Idle line interrupt disabled
**                      ||+------- RIE      =1  : Receive interrupt enabled
**                      |+-------- TCIE     =0  : Transmit complete interrupt disabled
**                      +--------- TIE      =0  : Transmit interrupt disabled
*/
#define SCItxActive                 (SCIC2_TIE == 1)

#define init_SCIC3    0b00000000
/*                      00000000 = reset
**                      ||||||||
**                      |||||||+-- PEIE     =0  : Parity error interrupt disabled
**                      ||||||+--- FEIE     =0  : Framing error interrupt disabled
**                      |||||+---- NEIE     =0  : Noise error interrupt disabled
**                      ||||+----- ORIE     =0  : Overrun interrupt disabled
**                      |||+------ TXINV    =0  : Transmit data not inverted
**                      ||+------- TXDIR    =0  : Single wire TxD pin is an input
**                      |+-------- T8       =0  : Ninth Transmit bit
**                      +--------- R8
*/

#define ASCII_BS      0x08
#define ASCII_LF      0x0A
#define ASCII_CR      0x0D
#define ASCII_DEL     0x7F

/***********************************************************************************************\
* Public type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE

extern byte BufferRx[BUFFER_RX_SIZE];

#pragma DATA_SEG DEFAULT

/***********************************************************************************************\
* Public prototypes
\***********************************************************************************************/

void SCIControlInit (void);
void SCISendString (byte *pStr);
void SCI_CharOut(byte data);
void SCI_NibbOut(byte data);
void SCI_ByteOut(byte data);
void SCI_putCRLF (void);
byte SCI_CharIn(void);
byte SCI_ByteIn(void);
void SCI_s12dec_Out (tword data);
void SCI_s16dec_Out (tword data);
void SCI_s18decun_Out (tbar_18 data);
void SCI_s8dec_Out(tword data);
void SCI_s8decun_Out(tword data);
void SCI_s12frac_Out (tword data);
void SCI_s4fracun_Out (tword data);
void SCI_s2fracun_Out (tword data);


byte isnum (byte data);
byte ishex (byte data);
byte tohex (byte data);
void hex2ASCII (byte data, byte* ptr);


#endif  /* _SCI_H_ */
