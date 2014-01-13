/***********************************************************************************************\
* Freescale MPL3115A2 Driver
*
* Filename: iic.c
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

#pragma MESSAGE DISABLE C4002 /* Warning C4002: Result not used */

/***********************************************************************************************\
* Private macros
\***********************************************************************************************/

/***********************************************************************************************\
* Private type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Private prototypes
\***********************************************************************************************/

void IIC_Start(void);
void IIC_Stop(void);
void IIC_RepeatStart(void);
void IIC_Delay(void);
void IIC_CycleWrite(byte bout);
byte IIC_CycleRead(byte ack);
void IIC_Bus_Reset(void);
byte IIC_StopRead(void);

/***********************************************************************************************\
* Private memory declarations
\***********************************************************************************************/

#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE

static byte error;
static word timeout;

#pragma DATA_SEG DEFAULT

#define BUFFER_OUT_SIZE       8

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

/***********************************************************************************************\
* Public functions
\***********************************************************************************************/

/*********************************************************\
* IIC Write Register
\*********************************************************/
void IIC_RegWrite(byte address, byte reg,byte val)
{
  IICC_TX = 1;                                  // Transmit Mode
  IIC_Start();                                  // Send Start
  IIC_CycleWrite(address);                      // Send IIC "Write" Address
  IIC_CycleWrite(reg);                          // Send Register
  IIC_CycleWrite(val);                          // Send Value
  IIC_Stop();                                   // Send Stop
}


/*********************************************************\
* IIC Read Register
\*********************************************************/
byte IIC_RegRead(byte address, byte reg)
{
  byte b;
  IICC_TX = 1;                                  // Transmit Mode
  IIC_Start();                                  // Send Start
  IIC_CycleWrite(address);                      // Send IIC "Write" Address
  IIC_CycleWrite(reg);                          // Send Register
  IIC_RepeatStart();                            // Send Repeat Start
  IIC_CycleWrite(address+1);                    // Send IIC "Read" Address
  b = IIC_CycleRead(1);                         // *** Dummy read: reads "IIC_ReadAddress" value ***
  b = IIC_StopRead();                           // Send Stop Read command
  return b;
}


/*********************************************************\
* IIC Write Multiple Registers
\*********************************************************/
void IIC_RegWriteN(byte address, byte reg1,byte N,byte *array)
{
  IICC_TX = 1;                                  // Transmit Mode
  IIC_Start();                                  // Send Start
  IIC_CycleWrite(address);                      // Send IIC "Write" Address
  IIC_CycleWrite(reg1);                         // Send Register
  while (N>0)                                   // Send N Values
  {
    IIC_CycleWrite(*array);
    array++;
    N--;
  }
  IIC_Stop();                                   // Send Stop
}


/*********************************************************\
* IIC Read Multiple Registers
\*********************************************************/
void IIC_RegReadN(byte address, byte reg1,byte N,byte *array)
{
  byte b;
  IICC_TX = 1;                                  // Transmit Mode
  IIC_Start();                                  // Send Start
  IIC_CycleWrite(address);                      // Send IIC "Write" Address
  IIC_CycleWrite(reg1);                         // Send Register
  IIC_RepeatStart();                            // Send Repeat Start
  IIC_CycleWrite(address+1);                    // Send IIC "Read" Address
  b = IIC_CycleRead(N);                         // *** Dummy read: reads "IIC_ReadAddress" value ***
  while (N>1)                                   // Read N-1 Register Values
  {
    N--;
    b = IIC_CycleRead(N);
    *array = b;
    array++;
    
  }
  b = IIC_StopRead();
  //b = IIC_CycleRead(1);
  *array = b;                                   // Read Last value
  //IIC_Stop();
}


/***********************************************************************************************\
* Private functions
\***********************************************************************************************/
/*********************************************************\
* Initiate IIC Bus Reset
\*********************************************************/

void IIC_Bus_Reset(void)
{
      int loop;
     
    
    // Disable the I2C block on the Host Controller 
    IICC1 &= ~(init_IICC1);            

    PAUSE;
    
    
      /* Create START condition (SDA goes low while SCL is high) */
      I2C_SDA_DD = 1; // SDA = 0
      PAUSE;
      I2C_SCL_DD = 0; // SCL = 1
      PAUSE;

      /* Release SDA back high */
      I2C_SDA_DD = 0; // SDA = 1
      PAUSE;
    
      /* Clock SCL for at least 9 clocks until SDA goes high */
      loop = 0;

      while (loop < 90)
      {
            loop++;
            /* Apply one SCL clock pulse */
            I2C_SCL_DD = 1; // SCL = 1
            PAUSE;
            I2C_SCL_DD = 0; // SCL = 0
            PAUSE;
            /* If SDA is high and a complete byte was sent then exit the loop */ 
            if ((I2C_SDA_PIN) && ((loop % 9) == 0))
                  break;
      }
    
      /* Create STOP condition (SDA goes high while SCL is high) */
      I2C_SDA_DD = 1; // SDA = 0
      PAUSE;
      I2C_SCL_DD = 0; // SCL = 1
      PAUSE;
      I2C_SDA_DD = 0; // SDA = 1
      PAUSE;

      //Set operation back to default for all pins on PTBDD and Enable I2C
      PTBDD = init_PTBDD;
      IICC1 = init_IICC1;   
}

/*********************************************************\
* Initiate IIC Start Condition
\*********************************************************/
void IIC_Start(void)
{
  IICC_MST = 1;
  timeout = 0;
  while ((!IICS_BUSY) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x01;
} //*** Wait until BUSY=1


/*********************************************************\
* Initiate IIC Stop Condition
\*********************************************************/
void IIC_Stop(void)
{
  IICC_MST = 0;
  timeout = 0;
  while ( (IICS_BUSY) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x02;
} //*** Wait until BUSY=0


/*********************************************************\
* Initiate IIC Repeat Start Condition
\*********************************************************/
void IIC_RepeatStart(void)
{
  IICC_RSTA = 1;
  timeout = 0;
  while ((!IICS_BUSY) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x04;
} //*** Wait until BUSY=1


/*********************************************************\
* IIC Delay
\*********************************************************/
void IIC_Delay(void)
{
  byte IICd;
  for (IICd=0; IICd<100; IICd++);
}


/*********************************************************\
* IIC Cycle Write
\*********************************************************/
void IIC_CycleWrite(byte bout)
{
  timeout = 0;
  while ((!IICS_TCF) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x08; 
  IICD = bout; 
  timeout = 0;
  while ((!IICS_IICIF) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x10;
  IICS_IICIF = 1; 
  if (IICS_RXAK)
    error |= 0x20;
}


/*********************************************************\
* IIC Cycle Read
\*********************************************************/
byte IIC_CycleRead(byte byteLeft)
{
  byte bread; 
  timeout = 0;
  while ((!IICS_TCF) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error|=0x08;
  IICC_TX = 0;
  IICC_TXAK = byteLeft <= 1 ? 1 : 0;  //Set NACK when reading the last byte
  bread = IICD; 
  timeout = 0;
  while ((!IICS_IICIF) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x10;
  IICS_IICIF=1;
  return bread;
}

/*********************************************************\
* Initiate IIC Stop Condition on Read
\*********************************************************/
byte IIC_StopRead(void)
{
  IICC_MST = 0;
  timeout = 0;
  while ( (IICS_BUSY) && (timeout<1000))
    timeout++;
  if (timeout >= 1000)
    error |= 0x02;
  
  return IICD;
} //*** Wait until BUSY=0
