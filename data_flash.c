/***********************************************************************************************\
* Freescale MPL3115A2 Driver
*
* Filename: data_flash.c
*
*
* (c) Copyright 2009, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
\***********************************************************************************************/

#include "system.h"

/***********************************************************************************************\
* Private macros
\***********************************************************************************************/

#define ReadArray_c               0x0B
#define ReadArrayLow_c            0x03
#define BlockErase4k_c            0x20
#define BlockErase32k_c           0x52
#define BlockErase64k_c           0xD8
#define Program_c                 0x02
#define WriteEnable_c             0x06
#define WriteDisable_c            0x04
#define ProtectSector_c           0x36
#define UnprotectSector_c         0x39
#define ReadStatus_c              0x05
#define WriteStatus_c             0x01

/***********************************************************************************************\
* Private type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Private prototypes
\***********************************************************************************************/

void DATAFLASH_WriteEnableLatch(void);
byte DATAFLASH_Status(void);
byte DATAFLASH_Busy(void);

/***********************************************************************************************\
* Private memory declarations
\***********************************************************************************************/

#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE

byte s_memp;
byte s_memh;
byte s_meml;

#pragma DATA_SEG DEFAULT

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

/***********************************************************************************************\
* Public functions
\***********************************************************************************************/

/*********************************************************\
* Unprotect the Data Flash
\*********************************************************/
void DATAFLASH_Unprotect(void)
{
  byte sector;
  for (sector=0;sector<8;sector++)
  {
    DATAFLASH_WriteEnableLatch();
    SPI_FullShift4(UnprotectSector_c, sector, 0x00, 0x00);
  }
//  DATAFLASH_WriteEnableLatch();
//  SPI_FullShift4(UnprotectSector_c, 0x07, 0x80, 0x00);  
//  DATAFLASH_WriteEnableLatch();
//  SPI_FullShift4(UnprotectSector_c, 0x07, 0xA0, 0x00);  
//  DATAFLASH_WriteEnableLatch();
//  SPI_FullShift4(UnprotectSector_c, 0x07, 0xC0, 0x00);  
}


/*********************************************************\
* Erase a 4k Data Flash Block
\*********************************************************/
void DATAFLASH_Erase4k(byte memp, byte memh, byte meml)
{
  DATAFLASH_WriteEnableLatch();
  SPI_FullShift4(BlockErase4k_c, memp, memh, meml);
  while (DATAFLASH_Status());
}


/*********************************************************\
* Erase the Data Flash from 0x000000 to 0x077FFF
*   - leaves last 32k untouched
\*********************************************************/
void DATAFLASH_Erase(void)
{
  byte sector;
  for (sector=0; sector<7; sector++)
  {
    DATAFLASH_WriteEnableLatch();
    SPI_FullShift4(BlockErase64k_c, sector, 0x00, 0x00);
    SCI_CharOut('.');
    while (DATAFLASH_Status());
  }
  DATAFLASH_WriteEnableLatch();
  SPI_FullShift4(BlockErase32k_c, 0x07, 0x00, 0x00);
  SCI_CharOut('.');
  while (DATAFLASH_Status());
}


/*********************************************************\
* Check if the Data Flash has been erased
\*********************************************************/
byte DATAFLASH_CheckErased(void)
{
  byte erased256;
  dword count;
  erased256 = count = 0;
  SPI_SS_SELECT;
  SPI_ChrShift5(ReadArray_c, 0x00, 0x00, 0x00, 0x00);
  while ((SPI_ChrShiftR(0x00) == 0xFF) && (count < 0x00100))
    count++;
  SPI_SS_DESELECT;
  if (count > 0x000FF)
    erased256 = 1;
  return erased256;
}


/*********************************************************\
* Write a Byte to Data Flash
\*********************************************************/
void DATAFLASH_Write(byte memp, byte memh, byte meml, byte data)
{
  DATAFLASH_WriteEnableLatch();
  s_memp = memp;
  s_memh = memh;
  s_meml = meml;
  SPI_SS_SELECT;
  SPI_ChrShift5(Program_c, memp, memh, meml, data);
  SPI_SS_DESELECT;
}


/*********************************************************\
* Write the Next Byte to Data Flash              ???
\*********************************************************/
void DATAFLASH_WriteNext(byte data)
{
  DATAFLASH_Write(s_memp,s_memh,s_meml,data);
  s_meml++;
  if (!s_meml)
  {
    s_memh++;
    if (!s_memh)
      s_memp++;
  }
}


/*********************************************************\
* Read a Byte from Data Flash
\*********************************************************/
byte DATAFLASH_Read(byte memp, byte memh, byte meml)
{
  byte a;
  DATAFLASH_WriteEnableLatch();
  SPI_SS_SELECT;
  SPI_ChrShift5(ReadArray_c, memp, memh, meml, 0x00);
  a = SPI_ChrShiftR(0x00);
  SPI_SS_DESELECT;
  return a;
}


/*********************************************************\
* Write Sequential data to Data Flash
\*********************************************************/
void DATAFLASH_WriteSequential(byte memp, byte memh, byte meml, byte data)
{
  DATAFLASH_WriteEnableLatch();
  SPI_SS_SELECT;
  SPI_ChrShift5(0xAF,memp,memh,meml,data);
  SPI_SS_DESELECT;
}


/*********************************************************\
* Write Next Sequential data to Data Flash         ???
\*********************************************************/
void DATAFLASH_WriteSequentialNext(byte data)
{
  while (DATAFLASH_Busy());
  SPI_SS_SELECT;
  SPI_ChrShift(0xAF);
  SPI_ChrShift(data);
  SPI_SS_DESELECT;
  s_meml++;
  if (!s_meml)
  {
    s_memh++;
    if (!s_memh)
      s_memp++;
  } 
}


/*********************************************************\
* Finish Write Sequential data to Data Flash
\*********************************************************/
void DATAFLASH_WriteSequentialDone(void)
{
  SPI_SS_SELECT;
  SPI_ChrShift(WriteDisable_c);
  SPI_SS_DESELECT;
}


/***********************************************************************************************\
* Private functions
\***********************************************************************************************/

/*********************************************************\
* Write Data Flash Enable Latch
\*********************************************************/
void DATAFLASH_WriteEnableLatch(void)
{
  SPI_SS_SELECT;
  SPI_ChrShift(WriteEnable_c);
  SPI_SS_DESELECT;
}


/*********************************************************\
* Get Data Flash Status
\*********************************************************/
byte DATAFLASH_Status(void)
{
  byte cstat;
  SPI_SS_SELECT;
  SPI_ChrShift(ReadStatus_c);
  cstat = SPI_ChrShiftR(0x00) & 0x01;
  SPI_SS_DESELECT;
  return cstat;
}


/*********************************************************\
* Check if Data Flash is Busy
\*********************************************************/
byte DATAFLASH_Busy(void)
{
  byte a;
  SPI_SS_SELECT;
  SPI_ChrShift(ReadStatus_c);
  a = SPI_ChrShiftR(0x00);
  SPI_SS_DESELECT;
  return (a & 0x01);
}

