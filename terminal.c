/***********************************************************************************************\
* Freescale MPL3115A2 Driver
*
* Filename: terminal.c
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

byte ProcessHexInput (byte *in, byte *out);
void CopyBT (byte *ptr);
void PrintALT (void);
void PrintBAR (void);
void PrintTEMP (void);
void PrintStatus (void);
void PrintALT_FIFO (void);
void PrintBAR_FIFO (void);
void PrintTEMP_FIFO (void);
void PrintFIFO (void);
void ClearInterrupts_MPL3115A2 (void);
void ClearIntControlRegs_MPL3115A2 (void);

/***********************************************************************************************\
* Private memory declarations
\***********************************************************************************************/

#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE


BIT_FIELD StreamMode;                       // stream mode control flags

extern BIT_FIELD RegisterFlag;
extern byte value[];                        // working value result scratchpad

static byte temp;                           // temporary byte variable
static byte index;                          // input character string index

extern byte SlaveAddressIIC;                // slave I2C address

extern byte functional_block;               // sensor function
                                        
static tword a_mcsb_value;                  // 16-bit value

static tword a_dec_value;                   // 16-bit value
static tword t_msb_value;                   // 8-bit Temperature MSB
static tword t_lsb_value;                   // 8-bit Temperature LSB

static byte actv;                           // temporary location to store active state

#pragma DATA_SEG DEFAULT
static tbar_18 bar_value_raw;               // 18-bit Barometric value
static tbar_18 bar_value_sh;                // 18-bit right shifted barometric value

extern tfifo_sample fifo_data[FIFO_BUFFER_SIZE];   // FIFO sample buffer


/***********************************************************************************************\
* Public functions
\***********************************************************************************************/

/*********************************************************\
**  Terminal Strings
\*********************************************************/

const byte string_Prompt []       = {"\r\nMPL3115A2> "};
const byte string_What []         = {" <-- what?"};
const byte string_FS []           = {"\r\n\n**  Freescale Semiconductor  **"};
const byte string_part []         = {"\r\n**  MPL3315A2 Demo           **\r\n"};
//const byte string_S08 []          = {"\r\n**      using the MC9S08QE8  **"};
//const byte string_emptyline []    = {"\r\n**                           **"};
//const byte string_DT []           = {"\r\n**  "__DATE__"    "__TIME__"  **\r\n\n"};
const byte string_ALT []          = {" Alt. "};
const byte string_BAR []          = {" Bar. "}; 
const byte string_ACT []          = {"Active mode\r\n"}; 


/*********************************************************\
**  Initialize Terminal Interface
\*********************************************************/
void TerminalInit (void)
{
  SCISendString((byte*)string_FS);
  SCISendString((byte*)string_part);
//  SCISendString((byte*)string_S08);
//  SCISendString((byte*)string_emptyline);
//  SCISendString((byte*)string_DT);

  SPI_SS_SELECT;
  SPI_ChrShift(0x01);
  SPI_ChrShift(0x00);
  SPI_SS_DESELECT;

  StreamMode.Byte = 0;
}
/*********************************************************\
**  Process Terminal Interface
\*********************************************************/

void ProcessTerminal (void)
{
  /*
  **  Output command prompt if required.
  */
  if (PROMPT_WAIT == FALSE)
  {
    SCISendString((byte*)string_Prompt);
    PROMPT_WAIT = TRUE;
  }
  /*
  **  Get input from terminal.
  */
  if (SCI_INPUT_READY == TRUE)
  {
    INPUT_ERROR = FALSE;
    /*
    **  Use command line input if not streaming data
    */
    if ((PT_STREAM == FALSE) && (INT_STREAM == FALSE))
    {
      PROMPT_WAIT = FALSE;
      /*
      **  Get first input character - only uppercase
      */
      switch (BufferRx[0])
      {
        /***************************************************************************************/
        case '?':
          /*
          **  Help : list valid commands
          */
          SCISendString("List of MPL3115A2 commands:\r\n");
          SCISendString("Mn         : Mode 0=Standby; 1=Active;2=Alt;3=Bar\r\n");
          SCISendString("CC         : Check Status/Control Registers\r\n");
          SCISendString("CW         : Check Alarm Registers\r\n");          
          SCISendString("RR xx      : Register xx Read\r\n");
          SCISendString("RW xx = nn : Register xx Write value nn\r\n");
          SCISendString("RO n       : OSR Ratio  0=1;1=2;2=4;3=8;4=16;5=32;6=64;7=128\r\n");
          SCISendString("RT n       : ST Time Step  0=2^0;1=2^1;...;F=2^F\r\n");          
          SCISendString("RF         : Report OSR, Time Step, Mode\r\n");
          SCISendString("Sx         : Stream Polling\r\n");          
          SCISendString("I n        : Stream via INTn\r\n");
          
          SCISendString("AAn xx yy wm wl : Altitude Alarm\r\n");
          SCISendString("APn xx yy wm wl : Pressure Alarm\r\n");  
          SCISendString("ATn xx wm       : Temp and Window alarm\r\n");  
          SCISendString("                :  n 1= INT1; 2= INT2\r\n");
          SCISendString("                :  xx: Target MSB; yy: Target LSB\r\n");
          SCISendString("                :  wm: Window MSB; wl: Window LSB\r\n");

          SCISendString("FO         : Stream via FIFO Overflow mode\r\n");
          SCISendString("FW ww      : Watermark Mode\r\n");
          SCISendString("           :  ww: Watermark= 1 to 32\r\n");
          break;

        /***************************************************************************************/
        case 'C':
          /*
          **  Check Register Values
          */
  
                      

           switch (BufferRx[1])
          {
            /////////////////////////////////////////////////////////////////////////////////////
            case 'C':
              /*
              **  C  : Check Status Register
              */
              SCISendString("\r\n");
              //SCISendString("Check\r\n");
              SCISendString("Status     ");
              temp = IIC_RegRead(SlaveAddressIIC, DR_STATUS_00_REG);
              SCI_ByteOut(temp);
              SCISendString("\r\n");
              
              SCISendString("CTRL_REG1  ");
              temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG1);
              SCI_ByteOut(temp); 
              SCISendString("\r\n");
              
              
              SCISendString("CTRL_REG2  ");
              temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG2);
              SCI_ByteOut(temp); 
              SCISendString("\r\n");
              
                                                     
              SCISendString("CTRL_REG3  ");
              temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG3);
              SCI_ByteOut(temp);                            
              SCISendString("\r\n");
              

              SCISendString("CTRL_REG4  ");
              temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG4);
              SCI_ByteOut(temp);               
              SCISendString("\r\n");
              
              
              SCISendString("CTRL_REG5  ");
              temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG5);
              SCI_ByteOut(temp);               
              SCISendString("\r\n");
              
              SCISendString("SYSMOD_REG ");
              temp = IIC_RegRead(SlaveAddressIIC, SYSMOD_REG);
              SCI_ByteOut(temp);               
              SCISendString("\r\n");

              SCISendString("INT SOURCE ");
              temp = IIC_RegRead(SlaveAddressIIC, INT_SOURCE_REG);
              SCI_ByteOut(temp);               
              SCISendString("\r\n");
              
              break;
              
            /////////////////////////////////////////////////////////////////////////////////////
            case 'W':              
            
              SCISendString("\r\n");
              //SCISendString(" - Check\r\n");
              SCISendString("Alt/Bar Target MSB  ");
              temp = IIC_RegRead(SlaveAddressIIC, P_TGT_MSB);
              SCI_ByteOut(temp);
              SCISendString("\r\n");

              
              SCISendString("Alt/Bar Target LSB  ");
              temp = IIC_RegRead(SlaveAddressIIC, P_TGT_LSB);
              SCI_ByteOut(temp);
              SCISendString("\r\n");
              
              SCISendString("\r\n");
              SCISendString("Alt/Bar Wind MSB    ");
              temp = IIC_RegRead(SlaveAddressIIC, P_TGT_WND_MSB);
              SCI_ByteOut(temp);
              SCISendString("\r\n");

              
              SCISendString("Alt/Bar Wind LSB    ");
              temp = IIC_RegRead(SlaveAddressIIC, P_TGT_WND_LSB);
              SCI_ByteOut(temp);
              SCISendString("\r\n");              
              
              SCISendString("\r\n");
              SCISendString("Temp Target         ");
              temp = IIC_RegRead(SlaveAddressIIC, T_TGT);
              SCI_ByteOut(temp);
              SCISendString("\r\n");
              
              SCISendString("Temp Window         ");
              temp = IIC_RegRead(SlaveAddressIIC, T_TGT_WND);
              SCI_ByteOut(temp); 
              SCISendString("\r\n");
                                      
            break;
            
            default:
              INPUT_ERROR = TRUE;
              break;            

          }
          break;
          
         /***************************************************************************************/
       
        case 'M':
          /*
          **  Control sensor mode of operation
          */
          switch (BufferRx[1])
          {
            /////////////////////////////////////////////////////////////////////////////////////
            case '0':
              /*
              **  M0  : Enter Standby
              */
//              SCISendString("Putting sensor in standy mode \r\n");
              temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG1);
              temp &= (STANDBY_SBYB_0);
              IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, temp);
              break;

             /////////////////////////////////////////////////////////////////////////////////////
            case '1':
              /*
              **  M1  : Enter Active
              */
             SCISendString((byte*)string_ACT);
             Print_OSR_ST();             
             MPL3115A2_Active();       
             break;
             
            /////////////////////////////////////////////////////////////////////////////////////
            case '2':
              /*
              **  M1  : Enter Altimeter Mode
              */
             SCISendString((byte*)string_ACT);
             SCISendString("Altimeter OSR-128 ST-2^0 \r\n");
             MPL3115A2_Init_Alt();
             MPL3115A2_Active();       
             break;

            /////////////////////////////////////////////////////////////////////////////////////
            case '3':
              /*
              **  M3  : Enter Barometer Mode
              */
             SCISendString((byte*)string_ACT);
             SCISendString("Barometer OSR-128 ST-2^0 \r\n");
             MPL3115A2_Init_Bar();
             MPL3115A2_Active();       
             break;
          }
          break;

        /***************************************************************************************/
        case 'R':
          /*
          **  Sensor register access
          */
          switch (BufferRx[1])
          {
            /////////////////////////////////////////////////////////////////////////////////////
            case 'R':
              /*
              **  RR xx  : Register Read
              */
              index = 2;
              
              /*
              **  Skip past space characters
              */
              while (BufferRx[index] == ' ')
              {
                index++;
              }
              /*
              **  Process hexadecimal register address input
              */
              temp = ProcessHexInput (&BufferRx[index], &value[0]);
              if (temp != 0)
              {
                index += temp;
                /*
                **  Check for end of string null
                */
                if (BufferRx[index++] == 0)
                {
                  SCISendString("= ");
                  /*
                  **  Go read register and report result
                  */
                  temp = IIC_RegRead(SlaveAddressIIC, value[0]);
                  hex2ASCII(temp,&value[0]);
                  value[2] = 0;                  
                  SCISendString(&value[0]);
                }
                else
                {
                  INPUT_ERROR = TRUE;
                }
              }
              else
              {
                SCI_putCRLF();
                /*
                **  If no register identified, then read all registers
                */
//                SCISendString("Read All Registers\r\n");
                for (value[5]=0; value[5]<0x2D;)
                {
                  hex2ASCII(value[5],&value[0]);
                  value[2] = ' ';
                  value[3] = '=';
                  value[4] = 0;
                  SCISendString(&value[0]);

                  for (value[4]=4; value[4]!=0; value[4]--)
                  {
                    value[0] = ' ';
                    temp = IIC_RegRead(SlaveAddressIIC, value[5]++);
                    hex2ASCII(temp,&value[1]);
                    value[3] = 0;                    
                    SCISendString(&value[0]);
                  }
                  SCI_putCRLF();
                }
              }
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            case 'W':
              /*
              **  RW xx = nn  : Register Write
              */
             index = 2;
              /*
              **  Skip past space characters
              */
              while (BufferRx[index] == ' ')
              {
                index++;
              }
              /*
              **  Process hexadecimal register address input
              */
              temp = ProcessHexInput (&BufferRx[index], &value[0]);
              if (temp != 0)
              {
                index += temp;
                /*
                **  Check for "="
                */
                while (BufferRx[index] == ' ')
                {
                  index++;
                }
                if (BufferRx[index++] == '=')
                {
                  while (BufferRx[index] == ' ')
                  {
                    index++;
                  }
                  /*
                  **  Process hexadecimal register data input
                  */                                                                      
                  temp = ProcessHexInput (&BufferRx[index], &value[1]);
                  if (temp != 0)
                  {
                    index += temp;
                    /*
                    **  Check for end of string null
                    */
                    if (BufferRx[index++] == 0)
                    {
                      /*
                      **  Go write register
                      */
                      temp = MPL3115A2_Standby();
                      IIC_RegWrite(SlaveAddressIIC, value[0], value[1]);

                      /*
                      **  Go read register and verify
                      */
                      temp = IIC_RegRead(SlaveAddressIIC, value[0]);
                      if (temp == value[1])
                      {
                        SCISendString(" Success");

                      }
                      else
                      {
                        SCISendString(" Fail");
                      }
                    }
                    else
                    {
                      INPUT_ERROR = TRUE;                   
                    }
                  }
                  else
                  {
                    INPUT_ERROR = TRUE;
                  }
                }
                else
                {
                  INPUT_ERROR = TRUE;
                }
              }
              else
              {
                INPUT_ERROR = TRUE;
              }
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            case 'T':
              /*
              **  RT n  : Change ST Time Step
              */
              index = 2;
              /*
              **  Skip past space characters
              */
              while (BufferRx[index] == ' ')
              {
                index++;
              }             
              /*
              ** Check if Part is in Active mode
              ** if active put into Standby
              */
              
              actv = IIC_RegRead(SlaveAddressIIC, CTRL_REG1) & ACTIVE_MASK;
              if (actv) {            
                temp = MPL3115A2_Standby();            
              }        
                             
              /*
              **  Accept numerical entries from '1' to 'F'
              */          
              temp = ProcessHexInput (&BufferRx[index], &value[0]);
              
              if (value[0] <= 0xF) {         
                temp = IIC_RegRead(SlaveAddressIIC, (CTRL_REG2 & CLEAR_ST_MASK));                
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG2, (temp|value[0]));
                Print_OSR_ST();                 
              } else {
                  INPUT_ERROR = TRUE;
              }  
              break;
            /////////////////////////////////////////////////////////////////////////////////////
            case 'O':
              /*
              **  RO n  : Change OSR
              */
              index = 2;
              /*
              **  Skip past space characters
              */
              while (BufferRx[index] == ' ')
              {
                index++;
              }             
              /*
              **  Accept numerical entries from '1' to '7'
              */
              value[0] = BufferRx[index] - '0';
              if (value[0] < 8) {
                value[0] <<= 3;                       
                /**
                ** Put part into Standby and write OSR, put part back into active mode
                */                           
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, (MPL3115A2_Standby()&CLEAR_OSR));
                temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG1);
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, temp | value[0]);
                Print_OSR_ST();                 
              } else {
                  INPUT_ERROR = TRUE;
              }
              break;
   

            case 'F':
              /*
              **  RF  : Report OSR , Time Step, and Mode
              */
              SCISendString("\r\n");
              Print_OSR_ST();
              break;

            /////////////////////////////////////////////////////////////////////////////////////
            default:            
              INPUT_ERROR = TRUE;
              break;
          }
          break;

        /***************************************************************************************/
        case 'S':
          /*
          ** Check to see if device is active
          ** if not active activate device     
          */
          actv = IIC_RegRead(SlaveAddressIIC, CTRL_REG1) & ACTIVE_MASK;
          if (!actv) {            
              MPL3115A2_Active();            
          }         
          
          POLL_ACTIVE = TRUE;
          PT_STREAM  = TRUE;
          PROMPT_WAIT = TRUE;
          
          /*
          ** Determine what mode the part is in 
          ** and set functional_block
          */
          temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG1) & ALT_MASK;
          
          if (temp) {
            /*
            ** Part is in Altimeter Mode
            */  
            functional_block = FBID_ALT_POLL;
            
          } else {
            /*
            ** Part is in Barometer Mode
            */  
            functional_block = FBID_BAR_POLL;            
            
          }                        
        break;  

        /***************************************************************************************/
        case 'I':
          /*
          **  Stream data via interrupts
          */
          index = 2;
          StreamMode.Byte = 0;
          /*
          **  Skip past space characters
          */
          while (BufferRx[index] == ' ')
            index++;
          
          /*
          **  Enter Standby mode
          */
          temp = MPL3115A2_Standby();      
          
          ClearInterrupts_MPL3115A2();          
          ClearIntControlRegs_MPL3115A2();
          /*
          **  Select Interrupt number for Interrupt based streaming
          */
          if (!INPUT_ERROR)
          {
            /*
            **  Set INT number
            */
            switch (BufferRx[index]) {
              case '1' : value[1] = 0x80;  // Direct interrupts to INT1
                         break;
              case '2' : value[1] = 0x00;  // Direct interrupts to INT2
                         break;              
              default  : INPUT_ERROR = TRUE; break; // Invalid INT number
            }                              
          if (!INPUT_ERROR)
          {
            PROMPT_WAIT = TRUE;

            temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG1) & ALT_MASK;
            
            if (temp) {
              /*
              ** Part is in Altimeter Mode
              */  
              functional_block = FBID_ALT_INT;            
            } else {
              /*
              ** Part is in Barometer Mode
              */  
              functional_block = FBID_BAR_INT;                 
            }
             
              /*
              **  Activate sensor interrupts
              **  - enable Data Ready interrupt
              **  - route Data Ready to selected INT
              **  - interrupts are falling edge open drain output
              */
              IIC_RegWrite(SlaveAddressIIC, CTRL_REG4, INT_EN_DRDY_MASK);
              IIC_RegWrite(SlaveAddressIIC, CTRL_REG5, value[1]);            
                
              INT_STREAM = TRUE;            
              INT_BOTH_FALLING;              
             // Clear and Enable MC9S08 Interrupts
             InterruptsActive();          

             MPL3115A2_Active();                      
            }
          }
          break;
            
         /***************************************************************************************/
        case 'A':
          /*
          **  Sensor alarm access
          */
          switch (BufferRx[1])
          {
            /////////////////////////////////////////////////////////////////////////////////////
            case 'A':
            case 'P':
              /*
              **  AAn xx yy wm wl: Altitude Alarm
              **  APn xx yy wm wl: Pressure Alarm
              */
              index = 2;

              /*
              **  Enter Standby mode and
              **  put part in proper mode
              */
              temp = MPL3115A2_Standby();   
              temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG1);                 
 
              switch (BufferRx[1]) {
                case 'A': IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, (temp | ALT_MASK)); break;        //Set to Altimeter mode
                case 'P': IIC_RegWrite(SlaveAddressIIC, CTRL_REG1, (temp & CLEAR_MODE_MASK)); break; //Set to Barometer mode
                default : INPUT_ERROR = TRUE; break; // Invalid                 
              }
                           
              SCISendString("\r\nWaiting for Interrupt ");
              Print_OSR_ST();
              
              ClearInterrupts_MPL3115A2();          
              ClearIntControlRegs_MPL3115A2();
              WINDOW_ACT = FALSE;
                
              /*
              **  Select Interrupt number for Interrupt based streaming
              */
              switch (BufferRx[index]) {
                case '1' : value[0] = (INT_EN_PW_MASK | INT_EN_PTH_MASK);  // Direct interrupts to INT1
                           break;
                case '2' : value[0] = INT_CFG_CLEAR;  // Direct interrupts to INT2
                           break;              
                default  : INPUT_ERROR = TRUE; break; // Invalid INT number
              }
              index++;
              /*
              **  Skip past space characters
              */
              while (BufferRx[index] == ' ')
              {
                index++;
              }
              
              temp = ProcessHexInput (&BufferRx[index], &value[1]);
              if (temp != 0)
              {
                index += temp;
                /*
                **  Check for " "
                */
                while (BufferRx[index] == ' ')
                {
                  index++;
                }
                
                  /*
                  **  Process hexadecimal register data input
                  */                                                                      
                  temp = ProcessHexInput (&BufferRx[index], &value[2]);
                  if (temp != 0)
                  {
                    index += temp;
                    /*
                    **  Check for end of string null
                    */
                    if (BufferRx[index++] == 0)
                    {
                      /*
                      **  Go write registers
                      */
                      IIC_RegWrite(SlaveAddressIIC, P_TGT_MSB, value[1]);
                      IIC_RegWrite(SlaveAddressIIC, P_TGT_LSB, value[2]);                 
                    }
                    else {                                           
                       /*
                       **  Check for " "
                       */
                       while (BufferRx[index] == ' ')
                       {
                          index++;
                       }

                      temp = ProcessHexInput (&BufferRx[index], &value[3]);
                      if (temp != 0)
                      {
                        index += temp;
                        /*
                        **  Check for " "
                        */
                        while (BufferRx[index] == ' ')
                        {
                          index++;
                        }
                        
                          /*
                          **  Process hexadecimal register data input
                          */                                                                      
                          temp = ProcessHexInput (&BufferRx[index], &value[4]);
                          if (temp != 0)
                          {
                            index += temp;
                            /*
                            **  Check for end of string null
                            */
                            if (BufferRx[index++] == 0)
                            {
                              /*
                              **  Go write registers
                              */
                              IIC_RegWrite(SlaveAddressIIC, P_TGT_MSB, value[1]);
                              IIC_RegWrite(SlaveAddressIIC, P_TGT_LSB, value[2]);                 
                              IIC_RegWrite(SlaveAddressIIC, P_TGT_WND_MSB, value[3]);                 
                              IIC_RegWrite(SlaveAddressIIC, P_TGT_WND_LSB, value[4]);                 
                              WINDOW_ACT = TRUE;
                            }
                            else {                                           
                              INPUT_ERROR = TRUE; 
                            }
                          }
                          else
                          {
                            INPUT_ERROR = TRUE;
                          }
                      }
                      else
                      {
                        INPUT_ERROR = TRUE;
                      }
                  }
                  }
                  else
                  {
                    INPUT_ERROR = TRUE;
                  }
              }
              else
              {
                INPUT_ERROR = TRUE;
              }


              /*
              **  Activate sensor interrupts
              **  - enable Data Ready interrupt
              **  - route Data Ready to selected INT
              **  - interrupts are falling edge open drain output
              */
              if (WINDOW_ACT) {                
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG4, (INT_EN_PW_MASK | INT_EN_PTH_MASK));
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG5, value[0]);  
              } else {
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG4, INT_EN_PTH_MASK);
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG5, value[0]);                  
              }
              
              functional_block = FBID_ARM_PA;                         
                
              INT_STREAM = TRUE;            
              INT_BOTH_FALLING;              
              // Clear and Enable MC9S08 Interrupts
             InterruptsActive();          

             MPL3115A2_Active();                      

              
             break;
            
             
            /////////////////////////////////////////////////////////////////////////////////////
            case 'T':            
              /*
              **  ATn xx wmsb: Temperature Alarm
              */
              index = 2;

              /*
              **  Enter Standby mode
              */
              temp = MPL3115A2_Standby();      
              
              ClearInterrupts_MPL3115A2();          
              ClearIntControlRegs_MPL3115A2();
                
              /*
              **  Select Interrupt number for Interrupt based streaming
              */
              switch (BufferRx[index]) {
                case '1' : value[0] = (INT_CFG_TW_MASK | INT_CFG_TTH_MASK);  // Direct interrupts to INT1
                           break;              
                case '2' : value[0] = INT_CFG_CLEAR;  // Direct interrupts to INT2
                           break;              
                default  : INPUT_ERROR = TRUE; break; // Invalid INT number
              }
              index++;
              /*
              **  Skip past space characters
              */
              while (BufferRx[index] == ' ')
              {
                index++;
              }            
              
              /*
              ** Read Hex Values to write to alarm register MSB, LSB
              */
              temp = ProcessHexInput (&BufferRx[index], &value[1]);
              if (temp != 0)
              {
                index += temp;
                /*
                **  Check for " "
                */
                while (BufferRx[index] == ' ')
                {
                  index++;
                }
                                               
                 if (BufferRx[index++] != 0) {
                  WINDOW_ACT = TRUE;
                  
                  /*
                  **  Process hexadecimal register data input
                  */                                                                      
                  temp = ProcessHexInput (&BufferRx[index], &value[2]);
                  if (temp != 0)
                  {
                    index += temp;
                    /*
                    **  Check for end of string null
                    */
                    if (BufferRx[index++] == 0)
                    {
                      /*
                      **  Go write registers
                      */
                      IIC_RegWrite(SlaveAddressIIC, T_TGT, value[1]);
                      IIC_RegWrite(SlaveAddressIIC, T_TGT_WND, value[2]);            

                    }
                    else      //if (BufferRx[index++] == 0)
                    {
                      INPUT_ERROR = TRUE;                   
                    }
                  
                  }//                      if (BufferRx[index++] != 0)
                
                  else                       //if (temp != 0)
                  {
                    INPUT_ERROR = TRUE;
                  }
                  
                 } else {   //   if (BufferRx[index++] != 0)
                  
                  IIC_RegWrite(SlaveAddressIIC, T_TGT, value[1]);
                  WINDOW_ACT= FALSE;
                 }  
              }
              else                                  //if (temp != 0)
              {
                INPUT_ERROR = TRUE;
              }


              /*
              **  Activate sensor interrupts
              **  - enable Temp Alarm interrupt
              **  - route Data Ready to selected INT
              **  - interrupts are falling edge open drain output              
              */
              if (WINDOW_ACT) {
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG4, (INT_EN_TW_MASK | INT_EN_TTH_MASK) );
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG5, value[0]);                                                          
              } else {                
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG4, INT_EN_TTH_MASK );
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG5, value[0]);                                                          
              }
              
              INT_STREAM = TRUE;            
              INT_BOTH_FALLING;     
              functional_block = FBID_ARM_T;          

              // Clear and Enable MC9S08 Interrupts
              InterruptsActive();          
     
              MPL3115A2_Active();                      
                            
              break;


            /////////////////////////////////////////////////////////////////////////////////////
            default:            
              INPUT_ERROR = TRUE;
              break;
          }
          
//          break;
          
          break;
                
        /***************************************************************************************/
        case 'F':
          /*
          **  Stream data via FIFO
          **  O - Overflow mode part will read 32 samples
          **  W - Watermark mode part will read to watermark number
          */                   
          index = 2;
          
          /*
          **  Enter Standby mode
          */
          temp = MPL3115A2_Standby();      
          
          ClearInterrupts_MPL3115A2();          
          ClearIntControlRegs_MPL3115A2();
          
          /*
          **  Clear FIFO MODE
          */
          IIC_RegWrite(SlaveAddressIIC, F_SETUP_REG, F_CLEAR_MASK);          

          switch (BufferRx[1])
          {
          
            /////////////////////////////////////////////////////////////////////////////////////
            case 'O':
                /*
                ** Set FIFO to full stop mode 
                **  Activate sensor interrupts
                **  - enable FIFO interrupt
                **  - route to INT 2
                **  - interrupts are falling edge open drain output
                */
                
                IIC_RegWrite(SlaveAddressIIC, F_SETUP_REG, F_MODE10_MASK);
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG4, INT_EN_FIFO_MASK);
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG5, INT_CFG_CLEAR);           
                functional_block = FBID_FIFO;                                              

            break;  
            /////////////////////////////////////////////////////////////////////////////////////
            case 'W':

                /*
                **  Skip past space characters
                */
                while (BufferRx[index] == ' ') 
                {                  
                  index++;
                }
                
                IIC_RegWrite(SlaveAddressIIC, F_SETUP_REG, F_CLEAR_MASK); 
                
                value[1] = 0;
                
                if (isnum (BufferRx[index]) == TRUE)
                {
                  value[1] = (BufferRx[index++] - '0');   
                  if (isnum (BufferRx[index]) == TRUE) 
                  {
                    value[1] *= 10;
                    value[1] += (BufferRx[index++] - '0');   
                  }
                  
                }
                
                if (value[1] > 31) 
                {
                  INPUT_ERROR = TRUE;  
                }                        
                
                /*
                ** Set FIFO to circular mode 
                ** Set Watermark
                **  Activate sensor interrupts
                **  - enable FIFO interrupt
                **  - route Data Ready to INT 2
                **  - interrupts are falling edge open drain output
                */
                
                IIC_RegWrite(SlaveAddressIIC, F_SETUP_REG, (F_MODE01_MASK | value[1]));
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG4, INT_EN_FIFO_MASK);
                IIC_RegWrite(SlaveAddressIIC, CTRL_REG5, INT_CFG_CLEAR);           
                functional_block = FBID_FIFO;                           
                   

            break;          
                    
            /////////////////////////////////////////////////////////////////////////////////////
            default:            
              INPUT_ERROR = TRUE;
              break;
          }    //end switch (BufferRx[1])
           
            
          INT_STREAM = TRUE;            
          INT_BOTH_FALLING;              
         // Clear and Enable MC9S08 Interrupts
         InterruptsActive();          

         MPL3115A2_Active();                      
          
        break;

          


        /***************************************************************************************/
        default:
          /*
          **  Undefined inputs are ignored.
          */
          INPUT_ERROR = TRUE;
          break;
      }  // end switch(BufferRx[0])
      if (INPUT_ERROR == TRUE)
      {
        SCISendString((byte*)string_What);
      }
      SCI_INPUT_READY = FALSE;
      temp = 0;
    }
    /*
    **  Data streaming is stopped by a CR.
    */
    else
    {
      /*
      **  Turn off data streaming
      */
      INT_PINS_DISABLED;
      POLL_ACTIVE = FALSE;
      PT_STREAM = FALSE;
      INT_STREAM = FALSE;
      SCI_INPUT_READY = FALSE;
      PROMPT_WAIT = FALSE;


    }
  }
}

/*********************************************************\
**  Terminal Output
\*********************************************************/

void OutputTerminal (byte BlockID)
{
  switch (BlockID)
  {
    /////////////////////////////////////////////////////////////////////////////////////////////////
    case FBID_ALT_POLL:
      /*
      **  Stream Altimeter Data 
      */
       RegisterFlag.Byte = IIC_RegRead(SlaveAddressIIC, DR_STATUS_00_REG);
            
       if (RegisterFlag.PDR_BIT == 1)               
           if (RegisterFlag.TDR_BIT == 1) {       
              SCISendString("\r\n");
              PrintStatus (); 
              SCISendString((byte*)string_ALT);     
              PrintALT ();
              SCISendString(" Temp");
              PrintTEMP ();                             
              SCI_putCRLF();
       }      
      break;
      
    /////////////////////////////////////////////////////////////////////////////////////////////////
    case FBID_BAR_POLL:
      /*
      **  Stream Barometer Data 
      */
       RegisterFlag.Byte = IIC_RegRead(SlaveAddressIIC, DR_STATUS_00_REG);
      
       bar_value_raw.LWord = 0; 
       bar_value_sh.LWord = 0;
       temp = IIC_RegRead(SlaveAddressIIC, DR_STATUS_00_REG);      
       if (RegisterFlag.PDR_BIT == 1)
           if (RegisterFlag.TDR_BIT == 1) {
              SCISendString("\r\n");
              PrintStatus ();      
              SCISendString((byte*)string_BAR);
              PrintBAR ();
              SCISendString(" Temp");
              PrintTEMP ();                             
              SCI_putCRLF();                                        
       }      
      break;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    case FBID_ALT_INT:
      /*
      **  Stream Altimeter Data 
      */        
              SCISendString("\r\n");
              PrintStatus ();  
              SCISendString((byte*)string_ALT); 
              PrintALT ();
              SCISendString(" Temp");
              PrintTEMP ();                             
              SCI_putCRLF();
      break;
      
    /////////////////////////////////////////////////////////////////////////////////////////////////
    case FBID_BAR_INT:
      /*
      **  Stream Barometer Data 
      */
              SCISendString("\r\n");
              PrintStatus ();        
              SCISendString((byte*)string_BAR);
              PrintBAR ();
              SCISendString(" Temp");
              PrintTEMP ();                             
              SCI_putCRLF();                                        
      break;
      
    /////////////////////////////////////////////////////////////////////////////////////////////////
    case FBID_ARM_T:
      /*
      **  Stream Alarm Data 
      */
      SCISendString("\r\n");
      SCISendString("INT Source ");
      RegisterFlag.Byte = IIC_RegRead(SlaveAddressIIC, INT_SOURCE_REG);
      SCI_ByteOut(RegisterFlag.Byte);                            
      
      if (RegisterFlag.SRC_TTH == 1) {        
        SCISendString(" Temp. Alarm");
        PrintTEMP ();
        SCI_putCRLF();                                        
      } else if (RegisterFlag.SRC_TW == 1) {
        SCISendString(" Temp. Window Alarm");
        PrintTEMP ();
        SCI_putCRLF();                                                        
      }
      break;
    /////////////////////////////////////////////////////////////////////////////////////////////////
    case FBID_ARM_PA:
      /*
      **  Stream Altimeter Data 
      */        
      SCISendString("\r\n");
      SCISendString("INT Source ");
      RegisterFlag.Byte = IIC_RegRead(SlaveAddressIIC, INT_SOURCE_REG);
      SCI_ByteOut(RegisterFlag.Byte); 
      
      if (RegisterFlag.SRC_PTH == 1) {        
        value[0] = IIC_RegRead(SlaveAddressIIC, CTRL_REG1);
        value[0] &= MODE_MASK;
        if (value[0] == 0x80){
          SCISendString(" Alt Alarm");
          PrintALT();
        } else {
          SCISendString(" Bar Alarm");
          PrintBAR();         
        }                                            
        SCI_putCRLF();    
                 
      } else if (RegisterFlag.SRC_PW == 1) {        
        value[0] = IIC_RegRead(SlaveAddressIIC, CTRL_REG1);
        value[0] &= MODE_MASK;
        if (value[0] == 0x80){
          SCISendString(" Alt Window Alarm");
          PrintALT();          
        } else {
          SCISendString(" Bar Window Alarm");
          PrintBAR ();         
        }                                                                              
        SCI_putCRLF();                     
      }                   
      break;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    case FBID_FIFO:
      /*
      **  Stream FIFO
      */        

      PrintFIFO ();        
          
      break;
            
    default:
      /*
      **  Undefined inputs are ignored.
      */
      INPUT_ERROR = TRUE;
    break;
            
  } //End switch (BlockID)
  
} //End OutputTerminal function



/***********************************************************************************************\
* Private functions
\***********************************************************************************************/

/*********************************************************\
* Print OSR and Mode
\*********************************************************/
void Print_OSR_ST (void)
{
  SCISendString("OSR = ");

  value[2] = IIC_RegRead(SlaveAddressIIC, CTRL_REG1);
  value[0] = value[2] & DR_MASK;
  value[0] >>= 3;

  if (value[0] == 0) {
    SCISendString(" 1 ");                                        
  } else if (value[0] == 1) {
    SCISendString(" 2 ");                                          
  } else if (value[0] == 2) {
    SCISendString(" 4 ");                                          
  } else if (value[0] == 3) {
    SCISendString(" 8 ");                                          
  } else if (value[0] == 4) {
    SCISendString(" 16 ");                                          
  } else if (value[0] == 5) {
    SCISendString(" 32 ");                                          
  } else if (value[0] == 6) {
    SCISendString(" 64 ");                                          
  } else if (value[0] == 7) {
    SCISendString(" 128 ");                                          
  } 
  
  value[3] = IIC_RegRead(SlaveAddressIIC, CTRL_REG2);
  value[4] = value[3] & ST_MASK;
  
  SCISendString(" Time Step = ");  
  SCISendString(" 2^");
  SCI_ByteOut(value[4]);
  SCISendString(" ");  
  
  value[2] &= MODE_MASK;
  if (value[2] == 0x80){
    SCISendString(" Mode = Altimeter");                                        
  } else {
    SCISendString(" Mode = Barometer");                                      
  }
  
}


 /*********************************************************\
* Clear Interrupts
\*********************************************************/
void ClearInterrupts_MPL3115A2 (void)
{
    /*
    **  Clear any existing interrupts by reading
    **  the pressure and temperature data registers
    */
    temp = IIC_RegRead(SlaveAddressIIC, OUT_P_MSB_REG);
    temp = IIC_RegRead(SlaveAddressIIC, OUT_P_CSB_REG);
    temp = IIC_RegRead(SlaveAddressIIC, OUT_P_LSB_REG);                             
    temp = IIC_RegRead(SlaveAddressIIC, OUT_T_MSB_REG);              
    temp = IIC_RegRead(SlaveAddressIIC, OUT_T_LSB_REG);                     

}

 /*********************************************************\
* Clear Interrupts
\*********************************************************/
void ClearIntControlRegs_MPL3115A2 (void)
{
    /*
    **  Clear MPL3115A2 Interrupt Control Registers
    */          
    IIC_RegWrite(SlaveAddressIIC, CTRL_REG4,INT_EN_CLEAR);
    IIC_RegWrite(SlaveAddressIIC, CTRL_REG5,INT_CFG_CLEAR);                    
}

/*********************************************************\
*
\*********************************************************/
void PrintFIFO (void)
{
  /*
  **  Identify if this is either a FIFO Overflow or Watermark event
  */
  if (RegisterFlag.F_OVF_BIT == 1)
  {
    SCISendString  ("FIFO Overflow\r\n");
  }
  if (RegisterFlag.F_WMRK_FLAG_BIT == 1)
  {
    SCISendString  ("FIFO Watermark\r\n");
  }
  /*
  **  Display FIFO count
  */
  value[0] = RegisterFlag.Byte & F_CNT_MASK;
  /*
  **  Output results
  */
  
  for (value[1]=0; value[1]!=value[0]; value[1]++)
  {
    
    CopyBT(&fifo_data[value[1]].Sample.BT.b_msb);
    /*
    **  Output formats are:
    **    - Altimeter
    **    - Barometer
    */
    temp = IIC_RegRead(SlaveAddressIIC, CTRL_REG1) & ALT_MASK;
    
    if (temp) {
      /*
      ** Part is in Altimeter Mode
      */      
      SCISendString((byte*)string_ALT);
      PrintALT_FIFO();
      SCISendString(" Temp. ");
      PrintTEMP_FIFO();
    } else 
      /*
      ** Part is in Barometer Mode
      */
    {
      SCISendString("Bar. ");
      PrintBAR_FIFO();
      SCISendString(" Temp. ");
      PrintTEMP_FIFO();            
    }
    SCI_putCRLF();
  }
}


/*********************************************************\
*
\*********************************************************/
byte ProcessHexInput (byte *in, byte *out)
{
  byte data;

  data = *in++;
  if (ishex(data) == TRUE)
  {
    data = tohex(data);
  }
  else
  {
    return (0);
  }
  if (ishex(*in) == TRUE)
  {
    data <<= 4;
    data += tohex(*in);
    *out = data;
    return (2);
  }
  else if ((*in == ' ') || (*in == 0))
  {
    *out = data;
    return (1);
  }
  return (0);
}


/*********************************************************\
*
\*********************************************************/
void CopyBT (byte *ptr)
{
  bar_value_raw.Byte.tbar_msb = *ptr++;
  bar_value_raw.Byte.tbar_csb = *ptr++;
  bar_value_raw.Byte.tbar_lsb = *ptr++;
  t_msb_value.Byte.hi = *ptr++; 
  t_lsb_value.Byte.hi = *ptr++; 
}


/*********************************************************\
*
\*********************************************************/
void PrintALT_FIFO (void)
{
  SCISendString(" Hex ");
  temp = bar_value_raw.Byte.tbar_msb;
  a_mcsb_value.Byte.hi = bar_value_raw.Byte.tbar_msb;
  SCI_ByteOut(temp);   
  temp = bar_value_raw.Byte.tbar_csb;
  a_mcsb_value.Byte.lo = temp;
  SCI_ByteOut(temp);
  temp = bar_value_raw.Byte.tbar_lsb;
  a_dec_value.Byte.lo = 0x0;
  a_dec_value.Byte.hi = temp;
  SCI_ByteOut(temp);
  SCISendString(" Dec ");
  SCI_s16dec_Out(a_mcsb_value);
  SCI_s4fracun_Out(a_dec_value);
}

void PrintBAR_FIFO (void) 
{
  SCISendString(" Hex ");
  temp = bar_value_raw.Byte.tbar_lsb;
  a_dec_value.Byte.lo = 0x0;
  a_dec_value.Byte.hi = temp;              
                
  SCI_ByteOut(bar_value_raw.Byte.tbar_msb);
  SCI_ByteOut(bar_value_raw.Byte.tbar_csb);              
  SCI_ByteOut(bar_value_raw.Byte.tbar_lsb);

  SCISendString(" Dec ");
  temp = bar_value_raw.Byte.tbar_lsb;
  a_dec_value.Byte.lo = 0x0;
  a_dec_value.Byte.hi = temp;              
  
  SCI_s18decun_Out(bar_value_raw);
  SCI_s2fracun_Out(a_dec_value);   
}

void PrintTEMP_FIFO (void) 
{
  SCISendString(" Hex ");
  temp =   t_msb_value.Byte.hi;
  SCI_ByteOut(temp);   
  t_msb_value.Byte.lo = 0x0;
  temp = t_lsb_value.Byte.hi;  
  t_lsb_value.Byte.lo = 0x0;              
  SCI_ByteOut(temp);
  SCISendString(" Dec ");
  SCI_s8dec_Out(t_msb_value);
  SCI_s4fracun_Out(t_lsb_value);    
}

  

void PrintALT (void)
{
  SCISendString(" Hex ");
  temp =   IIC_RegRead(SlaveAddressIIC, OUT_P_MSB_REG);
  a_mcsb_value.Byte.hi = temp;
  SCI_ByteOut(temp);   
  temp = IIC_RegRead(SlaveAddressIIC, OUT_P_CSB_REG);
  a_mcsb_value.Byte.lo = temp;
  SCI_ByteOut(temp);
  temp = IIC_RegRead(SlaveAddressIIC, OUT_P_LSB_REG);
  a_dec_value.Byte.lo = 0x0;
  a_dec_value.Byte.hi = temp;
  SCI_ByteOut(temp);
  SCISendString(" Dec ");
  SCI_s16dec_Out(a_mcsb_value);
  SCI_s4fracun_Out(a_dec_value);
}

void PrintBAR (void) 
{
  SCISendString(" Hex ");
  temp =   IIC_RegRead(SlaveAddressIIC, OUT_P_MSB_REG);
  bar_value_raw.Byte.tbar_msb = temp;

  temp = IIC_RegRead(SlaveAddressIIC, OUT_P_CSB_REG);
  bar_value_raw.Byte.tbar_csb = temp;

  temp = IIC_RegRead(SlaveAddressIIC, OUT_P_LSB_REG);
  bar_value_raw.Byte.tbar_lsb = temp;
  a_dec_value.Byte.lo = 0x0;
  a_dec_value.Byte.hi = temp;              
                
  SCI_ByteOut(bar_value_raw.Byte.tbar_msb);
  SCI_ByteOut(bar_value_raw.Byte.tbar_csb);              
  SCI_ByteOut(bar_value_raw.Byte.tbar_lsb);

  SCISendString(" Dec ");
  
  SCI_s18decun_Out(bar_value_raw);
  SCI_s2fracun_Out(a_dec_value);   
}

void PrintTEMP (void) 
{
  SCISendString(" Hex ");
  temp =   IIC_RegRead(SlaveAddressIIC, OUT_T_MSB_REG);
  SCI_ByteOut(temp);   
  t_msb_value.Byte.hi = temp;
  t_msb_value.Byte.lo = 0x0;
  temp = IIC_RegRead(SlaveAddressIIC, OUT_T_LSB_REG);       
  t_lsb_value.Byte.hi = temp;
  t_lsb_value.Byte.lo = 0x0;              
  SCI_ByteOut(temp);
  SCISendString(" Dec ");
  SCI_s8dec_Out(t_msb_value);
  SCI_s4fracun_Out(t_lsb_value);    
}

void PrintStatus (void) 
{ 
    SCISendString("Status ");
    temp = IIC_RegRead(SlaveAddressIIC, DR_STATUS_00_REG);
    SCI_ByteOut(temp);                            
}


