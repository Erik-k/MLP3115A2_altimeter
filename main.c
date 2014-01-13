/***********************************************************************************************\
* Freescale MPL3115A2 Driver
*
* Filename: main.c
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

extern void _Startup(void);             // External startup function declared in file Start08.c


/***********************************************************************************************\
* Private memory declarations
\***********************************************************************************************/

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE

BIT_FIELD SystemFlag;                       // system control flags

extern BIT_FIELD StreamMode;                // stream mode control flags

byte SlaveAddressIIC;                       // slave I2C address

byte functional_block;                      // function

byte value[6];                              // working value result scratchpad

BIT_FIELD RegisterFlag;                     // temporary register variable

byte full_scale;                            // current full scale setting

#pragma DATA_SEG DEFAULT

tfifo_sample fifo_data[FIFO_BUFFER_SIZE];   // FIFO sample buffer


/***********************************************************************************************\
* Public functions
\***********************************************************************************************/

#pragma MESSAGE DISABLE C1420 /* Warning C1420: Result of function-call is ignored */

/*********************************************************\
* Main Control Loop
\*********************************************************/
void main(void)
{
  /*
  **  Initialize system variables.
  */
  SystemFlag.Byte = 0;
  SCIControlInit();
  /*
  **  Wait for user input before proceeding.
  */
  SCI_CharIn();   
  SCI_INPUT_READY = 0;
  EnableInterrupts;    
  /*
  **  Initiate terminal interface.
  */
  IIC_Bus_Reset();
  TerminalInit();
  /*
  **  Verify IIC communications with the pressure sensor
  */
  SlaveAddressIIC = MPL3115A2_IIC_ADDRESS;
  if (SA0_PIN == 1)
  {
    SlaveAddressIIC += 2;
  }
  /*
  **  Brute force delay for about 5ms
  */
  for (value[0]=0x10; value[0]!=0; value[0]--)
  {
    for (value[1]=0xFF; value[1]!=0; value[1]--) {}
  }

  /*Initialize MPL3115A2 and setup as Altimeter with 
    128 oversampling and data read every second*/

  MPL3115A2_Init_Alt();
  SCISendString("Initializing to:\r\n");
  MPL3115A2_Active();

  /*
  **  Output current status of the sensor
  */
  Print_OSR_ST();
  SCI_putCRLF();
  
                
  /**********************************************************************************************
  **  Enter the main control loop.
  */
  for(;;)
  {  
    /*
    **  Go process terminal input
    */
    ProcessTerminal();
   /*
    **  Check interrupt source
    */
    if (CHECK_INT == TRUE)
    {
      /*
      **  Clear the Interrupt Check flag
      */
      CHECK_INT = FALSE;
      //SCISendString("In CHECK_INT IF \r\n");    
      /*
      **  Read the Interrupt Source Register
      */
      RegisterFlag.Byte = IIC_RegRead(SlaveAddressIIC, INT_SOURCE_REG);
      if ((RegisterFlag.SRC_DRDY_BIT == 1) || (RegisterFlag.SRC_FIFO_BIT == 1) 
           || (RegisterFlag.SRC_TTH == 1) || (RegisterFlag.SRC_TW == 1)
           || (RegisterFlag.SRC_PTH == 1) || (RegisterFlag.SRC_PW == 1))
      {
        /*
        **  Enable polling of the data - once.
        */
        POLL_ACTIVE = TRUE;
        
      }
    }   
     
    
    /*
    **  Determine if any sensor registers need to be polled
    */
   if (POLL_ACTIVE == TRUE) {
      switch (functional_block)
      {
        /////////////////////////////////////////////////////////////////////////////////////////
        case FBID_ALT_POLL:
          OutputTerminal (FBID_ALT_POLL);
          break;

        /////////////////////////////////////////////////////////////////////////////////////////          
        case FBID_BAR_POLL:   
          OutputTerminal (FBID_BAR_POLL);          
          break;

        /////////////////////////////////////////////////////////////////////////////////////////
        case FBID_ALT_INT:
          OutputTerminal (FBID_ALT_INT);
          break;

        /////////////////////////////////////////////////////////////////////////////////////////          
        case FBID_BAR_INT:   
          OutputTerminal (FBID_BAR_INT);          
          break;
        /////////////////////////////////////////////////////////////////////////////////////////          
        case FBID_ARM_T:   
          OutputTerminal (FBID_ARM_T);          
          break;
        /////////////////////////////////////////////////////////////////////////////////////////      
        case FBID_ARM_PA:   
          OutputTerminal (FBID_ARM_PA);          
          break;
        /////////////////////////////////////////////////////////////////////////////////////////      
        case FBID_FIFO:   
          /*
          **  FIFO
          **
          **  Read the FIFO Status Register (0x10) and then read the FIFO Data (0x12)
          */
          RegisterFlag.Byte = IIC_RegRead(SlaveAddressIIC, F_STATUS_REG);
          /*
          **  Go read FIFO with a single multi-byte IIC access
          */
          value[4] = (RegisterFlag.Byte & F_CNT_MASK) * 5;
          IIC_RegReadN(SlaveAddressIIC, OUT_P_MSB_REG, value[4], &fifo_data[0].Sample.BT.b_msb);
          OutputTerminal (FBID_FIFO);
          break;
                                      
        /////////////////////////////////////////////////////////////////////////////////////////                  
        default:
          INPUT_ERROR = TRUE;
          break;
         
      } //End switch (functional_block) 
      
     if (INT_STREAM == TRUE)
     {
       POLL_ACTIVE = FALSE;
     }    
    
   }
    
  }
}


/*********************************************************\
* Power-on Reset Entry Point
\*********************************************************/
#pragma NO_FRAME
#pragma NO_EXIT
void _EntryPoint(void)
{
  /*
  **  Initialize General System Control
  */
  SOPT1 = init_SOPT1;                   // System Options Register 1
  SOPT2 = init_SOPT2;                   // System Options Register 2
  SPMSC1 = init_SPMSC1;                 // System Power Management Status and Control 1 Register
  SPMSC2 = init_SPMSC2;                 // System Power Management Status and Control 2 Register
  SPMSC3 = init_SPMSC3;                 // System Power Management Status and Control 3 Register
  SCGC1 = init_SCGC1;                   // System Clock Gating Control 1 Register
  SCGC2 = init_SCGC2;                   // System Clock Gating Control 2 Register

  /*
  **  Initialize Internal Clock Source
  */
  ICSTRM = NVICSTRM;                    // ICS Trim Register
  ICSSC = NVICSTRM;                     // ICS Fine Trim
  ICSC1 = init_ICSC1;                   // ICS Control Register 1
  ICSC2 = init_ICSC2;                   // ICS Control Register 2

  while(!ICSSC_IREFST) {}               // Wait until source of reference clock is internal clock
  ICSSC = init_ICSSC;                   // ICS Status and Control
  while((ICSSC & 0xC0) != 0x00) {}      // Wait until the FLL switches to Low range DCO mode

  /*
  **  Initialize Port I/O
  */
  PTAD = init_PTAD;                     // Port A Data Register
  PTAPE = init_PTAPE;                   // Port A Pull Enable Register
  PTASE = init_PTASE;                   // Port A Slew Rate Enable Register
  PTADS = init_PTADS;                   // Port A Drive Strength Selection Register
  PTADD = init_PTADD;                   // Port A Data Direction Register
  PTBD = init_PTBD;                     // Port B Data Register
  PTBPE = init_PTBPE;                   // Port B Pull Enable Register
  PTBSE = init_PTBSE;                   // Port B Slew Rate Enable Register
  PTBDS = init_PTBDS;                   // Port B Drive Strength Selection Register
  PTBDD = init_PTBDD;                   // Port B Data Direction Register
  PTCD = init_PTCD;                     // Port C Data Register
  PTCPE = init_PTCPE;                   // Port C Pull Enable Register
  PTCSE = init_PTCSE;                   // Port C Slew Rate Enable Register
  PTCDS = init_PTCDS;                   // Port C Drive Strength Selection Register
  PTCDD = init_PTCDD;                   // Port C Data Direction Register
  PTDD = init_PTDD;                     // Port D Data Register
  PTDPE = init_PTDPE;                   // Port D Pull Enable Register
  PTDSE = init_PTDSE;                   // Port D Slew Rate Enable Register
  PTDDS = init_PTDDS;                   // Port D Drive Strength Selection Register
  PTDDD = init_PTDDD;                   // Port D Data Direction Register

  /*
  **  Initialize Interrupt Pins (IRQ and KBI)
  */
  IRQSC = init_IRQSC;                   // Interrupt Pin Request Status and Control Register
  KBISC = init_KBISC;                   // KBI Interrupt Status and Control Register
  KBIPE = init_KBIPE;                   // KBI Interrupt Pin Select Register
  KBIES = init_KBIES;                   // KBI Interrupt Edge Select Register

  /*
  **  Initialize Inter-Integrated Circuit (IIC)
  */
  IICF = init_IICF;                     // IIC Frequency Divider Register
  IICC1 = init_IICC1;                   // IIC Control Register 1

  /*
  **  Initialize Serial Communications Interface (SCI)
  */
  SCIBDH = init_SCIBDH;                 // SCI Baud Rate Register High
  SCIBDL = init_SCIBDL;                 // SCI Baud Rate Register Low
  SCIC1 = init_SCIC1;                   // SCI Control Register 1
  SCIC2 = init_SCIC2;                   // SCI Control Register 2
  SCIC3 = init_SCIC3;                   // SCI Control Register 3

  /*
  **  Initialize Serial Peripheral Interface (SPI)
  */
  SPIBR = init_SPIBR;                   // SPI Baud Rate Register
  SPIC2 = init_SPIC2;                   // SPI Control Register 2
  SPIC1 = init_SPIC1;                   // SPI Control Register 1

  /*
  **  Perform ANSI startup and jump into main control
  */
  __asm   jmp _Startup;                 // Jump to C startup code
}


/*********************************************************\
**  Activate sensor interrupts
\*********************************************************/
void InterruptsActive (void)
{
  /*
  **  Clear and Enable keyboard input interrupts
  */
  CLEAR_KBI_INTERRUPT;
  INT_PINS_ENABLED;
}

/*********************************************************\
* Keyboard Interrupt Service Routine
\*********************************************************/
interrupt void isr_KBI (void)
{
  /*
  **  Clear the interrupt flag
  */
  CLEAR_KBI_INTERRUPT;
  CHECK_INT = TRUE;
  //SCISendString("Interrupt Asserted \r\n");
}


/*********************************************************\
* Dummy Interrupt Service Routine
\*********************************************************/
interrupt void DummyIRQ (void)
{
}


/***********************************************************************************************\
* Private functions
\***********************************************************************************************/

#pragma CODE_SEG REVISION

const byte Vendor[] @0xFFC0 = "Freescale";

