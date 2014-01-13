/***********************************************************************************************\
* Freescale MPL3115A2 Driver
*
* Filename: data_flash.h
*
*
* (c) Copyright 2009, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
\***********************************************************************************************/
#ifndef _DATA_FLASH_H_
#define _DATA_FLASH_H_

/***********************************************************************************************\
* Public macros
\***********************************************************************************************/

/***********************************************************************************************\
* Public type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

/***********************************************************************************************\
* Public prototypes
\***********************************************************************************************/

void DATAFLASH_Unprotect(void);
void DATAFLASH_Erase4k(byte memp, byte memh, byte meml);
void DATAFLASH_Erase(void);
byte DATAFLASH_CheckErased(void);
void DATAFLASH_Write(byte memp, byte memh, byte meml, byte data);
void DATAFLASH_WriteNext(byte data);
byte DATAFLASH_Read(byte memp, byte memh, byte meml);
void DATAFLASH_WriteSequential(byte memp, byte memh, byte meml, byte data);
void DATAFLASH_WriteSequentialNext(byte data);
void DATAFLASH_WriteSequentialDone(void);

void DATAFLASH_WriteEnableLatch(void);
byte DATAFLASH_Status(void);
byte DATAFLASH_Busy(void);

#endif  /* _DATA_FLASH_H_ */
