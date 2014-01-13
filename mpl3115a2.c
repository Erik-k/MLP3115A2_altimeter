/***********************************************************************************************\
* Freescale MPL3115A2 Driver
*
* Filename: mma845x.c
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

/***********************************************************************************************\
* Private type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Private prototypes
\***********************************************************************************************/

/***********************************************************************************************\
* Private memory declarations
\***********************************************************************************************/

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE

extern byte SlaveAddressIIC;

#pragma DATA_SEG DEFAULT

/***********************************************************************************************\
* Public functions
\***********************************************************************************************/

/*********************************************************\
* Put MPL3115A2 into Active Mode
\*********************************************************/
void MPL3115A2_Active (void)
{
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, (IIC_RegRead(SlaveAddressIIC, CTRL_REG1) | ACTIVE_MASK));
}                 


/*********************************************************\
* Put MPL3115A2 into Standby Mode
\*********************************************************/
byte MPL3115A2_Standby (void)
{
  byte n;  
  /*
  **  Read current value of System Control 1 Register.
  **  Put sensor into Standby Mode.
  **  Return with previous value of System Control 1 Register.
  */
  n = IIC_RegRead(SlaveAddressIIC, CTRL_REG1);
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, n & STANDBY_SBYB_0);

  return(n);
}          


/*********************************************************\
* Initialize MPL3115A2
\*********************************************************/
void MPL3115A2_Init_Alt (void)
{
  
  /*
  **  Configure sensor for:
  **    - 128 Oversampling
  **    - Altitude Mode
  **    - Set Interrupts to Active Low/Open Drain
  **    - Set Data Event Flags for Pressure/Altitude and Temperature
  **    - Set Generate Data Event Flag
  */
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, (ALT_MASK | OS2_MASK | OS1_MASK | OS0_MASK));  
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, CLEAR_CTRLREG2);    
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG3, (PP_OD1_MASK | PP_OD2_MASK));  
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG4, INT_EN_CLEAR);
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG5, INT_CFG_CLEAR);    
  IIC_RegWrite(SlaveAddressIIC, PT_DATA_CFG_REG, (DREM_MASK | PDEFE_MASK | TDEFE_MASK));    
 
}

/*********************************************************\
* Initialize MPL3115A2
\*********************************************************/
void MPL3115A2_Init_Bar (void)
{
  
  /*
  **  Configure sensor for:
  **    - 128 Oversampling
  **    - Barometer Mode
  **    - Set Interrupts to Active Low/Open Drain
  **    - Set Data Event Flags for Pressure/Altitude and Temperature
  **    - Set Generate Data Event Flag
  */
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, (OS2_MASK | OS1_MASK | OS0_MASK));
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, CLEAR_CTRLREG2);    
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG3, (PP_OD1_MASK | PP_OD2_MASK));  
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG4, INT_EN_CLEAR);
  IIC_RegWrite(SlaveAddressIIC, CTRL_REG5, INT_CFG_CLEAR);    
  IIC_RegWrite(SlaveAddressIIC, PT_DATA_CFG_REG, (DREM_MASK | PDEFE_MASK | TDEFE_MASK));    
 
}

/***********************************************************************************************\
* Private functions
\***********************************************************************************************/
