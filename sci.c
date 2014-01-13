/***********************************************************************************************\
* Freescale MPL3115A2 Driver
*
* Filename: sci.c
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

/*
**  The following macros are constants used in the fractional decimal conversion routine.
**
**    FRAC_2d1  =  2^-1  =  0.5
**    FRAC_2d2  =  2^-2  =  0.25    etc...
*/
#define FRAC_2d1              5000
#define FRAC_2d2              2500
#define FRAC_2d3              1250
#define FRAC_2d4               625
#define FRAC_2d5               313
#define FRAC_2d6               156
#define FRAC_2d7                78
#define FRAC_2d8                39
#define FRAC_2d9                20
#define FRAC_2d10               10

/***********************************************************************************************\
* Private type definitions
\***********************************************************************************************/

/***********************************************************************************************\
* Private prototypes
\***********************************************************************************************/

void SCISendChar (char c);
void ManageInputPointers (void);

/***********************************************************************************************\
* Private memory declarations
\***********************************************************************************************/

#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE
static byte *ptrStringIn;
static byte *ptrStringTail;
static byte *ptrStringOut;

#pragma DATA_SEG DEFAULT

static byte BufferTx[BUFFER_TX_SIZE];

/***********************************************************************************************\
* Public memory declarations
\***********************************************************************************************/

#pragma DATA_SEG __SHORT_SEG _DATA_ZEROPAGE

byte InputStringIndex;
byte BufferRx[BUFFER_RX_SIZE];

#pragma DATA_SEG DEFAULT

/***********************************************************************************************\
* Public functions
\***********************************************************************************************/

/*********************************************************\
* SCI Control Initialization
\*********************************************************/
void SCIControlInit (void)
{
  ptrStringIn = &BufferTx[1];
  ptrStringOut = &BufferTx[0];
  ptrStringTail = &BufferTx[0];
  InputStringIndex = 0;
}


/*********************************************************\
* Buffered and Interrupt driven SCI Character String Send
\*********************************************************/
void SCISendString (byte *pStr)
{
  /*
  **  Copy string into transmit circular buffer
  */
  while (*pStr != 0)
  {
    /*
    **  Wait until there's room in the buffer.
    **  Then copy each byte into the buffer.
    */
    if (ptrStringOut != ptrStringIn)
    {
      *ptrStringIn++ = *pStr++;
      ManageInputPointers();
    }
  }
  /*
  **  Enable SCI transmit interrupts
  */
  //if (SCIC2_TIE == 0)
  //{
    SCIC2_TIE = 1;
  //}
}


/*********************************************************\
* SCI single character output
\*********************************************************/
void SCI_CharOut(byte data)
{
  /*
  **  Wait until there's room in the buffer.
  **  Then copy the byte into the buffer.
  */
  while (ptrStringOut == ptrStringIn);
  *ptrStringIn++ = data;
  ManageInputPointers();
  /*
  **  Enable SCI transmit interrupts
  */
  if (SCIC2_TIE == 0)
  {
    SCIC2_TIE = 1;
  }
}


/*********************************************************\
** SCI Interrupt Service Routine to Send a Character String.
\*********************************************************/
interrupt void isr_SCI_TX (void)
{
  /*
  **  Perform dummy read to clear interrupt request.
  */
  SCIS1;
  /*
  **  Output the next byte in the buffer.
  */
  if (ptrStringOut != ptrStringTail)
  {
    SCID = *ptrStringOut++;
    if (ptrStringOut == &BufferTx[BUFFER_TX_SIZE])
    {
      ptrStringOut = &BufferTx[0];
    }
  }
  else
  {
    SCIC2_TIE = 0;
  }
}


/*********************************************************\
* SCI single hex nibble character output
\*********************************************************/
void SCI_NibbOut(byte data)
{
  byte c;
  c = data + 0x30;
  if (c > 0x39)
    c += 0x07;
  SCI_CharOut(c);
}


/*********************************************************\
* SCI hex byte character output
\*********************************************************/
void SCI_ByteOut(byte data)
{
  byte c;
  c = data;
  c >>= 4;
  SCI_NibbOut(c);
  c = data & 0x0F;
  SCI_NibbOut(c);
}


/*********************************************************\
* SCI output CR LF
\*********************************************************/
void SCI_putCRLF (void)
{
  SCI_CharOut(ASCII_CR);
  SCI_CharOut(ASCII_LF);
}


/*********************************************************\
**  SCI Interrupt Service Routine to Receive a Character String.
\*********************************************************/
interrupt void isr_SCI_RX (void)
{	
	byte data;
	/*
	**  Ensure Rx data buffer is full
	*/
	do
	{
		data = SCIS1;
	}
	while ((data & 0x20) == 0);
	/*
	**  Check for Errors (Framing, Noise, Parity)
	*/
	if ((data & 0x07) != 0)
	{
		SCID;
		return;
	}
  /*
  **  Process input character
  */
  data = SCID;
  switch (data)
  {
    case ASCII_BS:
    case ASCII_DEL:
      /*
      **  Backspace and Delete will remove the most recent entry
      */
      if (InputStringIndex > 0)
      {
        InputStringIndex -= 1;
          SCISendChar(ASCII_BS);
          SCISendChar(' ');
          SCISendChar(ASCII_BS);
      }
      break;

    case ASCII_CR:
      /*
      **  Carriage Return null terminates the string and flags it for processing
      */
      BufferRx[InputStringIndex] = 0;
      SCI_INPUT_READY = 1;
      InputStringIndex = 0;
      break;

    default:
      /*
      **  All other entries are considered to be valid data, however only printable ASCII
      **  characters will be saved in the buffer and echoed back
      */
      if ((InputStringIndex + 1) < BUFFER_RX_SIZE)
      {
        if ((data > 0x1F) && (data < 0x80))
        {
          SCISendChar(data);
          /*
          **  Convert to upper case in buffer
          */
          if ((data > 0x60) && (data <= 0x7B))
          {
            data -= 0x20;
          }
          BufferRx[InputStringIndex++] = data;
        }
      }
      break;
  }
}


/*********************************************************\
* SCI single character input
\*********************************************************/
byte SCI_CharIn(void)
{
  while (!SCIS1_RDRF);
  return SCID;
}


/*********************************************************\
* SCI hex byte character input
\*********************************************************/
byte SCI_ByteIn(void)
{
  byte h,l;
  h = SCI_CharIn();
  h -= 0x30;
  if (h > 0x09)
    h -= 0x07;
  l = SCI_CharIn();
  l -= 0x30;
  if (l > 0x09)
    l-=0x07;
  return h*16+l;
}


/*********************************************************\
* SCI output signed 12-bit left-justified integer as decimal
*
* Example:  0x23D0  =  "+ 573"
*           0xDC30  =  "- 573"
*
\*********************************************************/
void SCI_s12dec_Out (tword data)
{
  byte a, b, c, d;
  word r;
  /*
  **  Determine sign and output
  */
  if (data.Byte.hi > 0x7F)
  {
    SCI_CharOut ('-');
    data.Word = ~data.Word + 1;
  }
  else
  {
    SCI_CharOut ('+');
  }
  /*
  **  Calculate
  */
  a = (byte)((data.Word >>4) / 1000);
  r = (data.Word >>4) % 1000;
  b = (byte)(r / 100);
  r %= 100;
  c = (byte)(r / 10);
  d = (byte)(r % 10);
  /*
  **  Format
  */
  if (a == 0)
  {
    a = 0xF0;
    if (b == 0)
    {
      b = 0xF0;
      if (c == 0)
      {
        c = 0xF0;
      }
    }
  }
  /*
  **  Output result
  */
  SCI_NibbOut (a);
  SCI_NibbOut (b);
  SCI_NibbOut (c);
  SCI_NibbOut (d);
}

/*********************************************************\
* SCI output unsigned 18-bit left justified integer as decimal
*
* Example:  
*           
*
\*********************************************************/
void SCI_s18decun_Out (tbar_18 data)
{
  byte a, b, c, d, e, f, g, h;
  dword r;
  /*
  **  Calculate
  */

  a = (byte)((data.LWord>>6) / 10000000);     //shift by 6 

  r = (data.LWord>>6) % 10000000;
  
  b = (byte)(r / 1000000);

  r %= 1000000;
              
  c = (byte)(r / 100000);

  r %= 100000;

  d = (byte)(r / 10000);
  
  r %= 10000;  
  
  e = (byte)(r / 1000);
  
  r %= 1000;
  
  f = (byte)(r / 100);
  
  r %= 100;
  
  g = (byte)(r / 10);
  
  h = (byte)(r % 10);
  
  
  /*
  **  Format
  */
  if (a == 0)
  {
    a = 0xF0;
    if (b == 0)
    {
      b = 0xF0;
      if (c == 0)
      {
        c = 0xF0;
        if (d == 0) {
          d = 0xF0;
          if (e==0){
            e = 0xF0;
            if (f == 0){
              f = 0xF0;
              if ( g == 0 ) {
                g = 0xF0;  
              }
            }        
          }
        }
      }
    }
  }
  /*
  **  Output result
  */
//  SCI_NibbOut (a);
//  SCI_NibbOut (b);
//  SCI_NibbOut (c);
  SCI_NibbOut (d);
  SCI_NibbOut (e);
  SCI_NibbOut (f);
  SCI_NibbOut (g);
  SCI_NibbOut (h);
}




/*********************************************************\
* SCI output signed 16-bit integer as decimal
*
* Example:  0x23D0  =  "+ 573"
*           0xDC30  =  "- 573"
*
\*********************************************************/
void SCI_s16dec_Out (tword data)
{
  byte a, b, c, d;
  word r;
  /*
  **  Determine sign and output
  */
  if (data.Byte.hi > 0x7F)
  {
    SCI_CharOut ('-');
    data.Word = ~data.Word + 1;
  }
  else
  {
    SCI_CharOut ('+');
  }
  /*
  **  Calculate
  */
  a = (byte)((data.Word) / 1000);
  r = (data.Word) % 1000;
  b = (byte)(r / 100);
  r %= 100;
  c = (byte)(r / 10);
  d = (byte)(r % 10);
  /*
  **  Format
  */
  if (a == 0)
  {
    a = 0xF0;
    if (b == 0)
    {
      b = 0xF0;
      if (c == 0)
      {
        c = 0xF0;
      }
    }
  }
  /*
  **  Output result
  */
  SCI_NibbOut (a);
  SCI_NibbOut (b);
  SCI_NibbOut (c);
  SCI_NibbOut (d);
}

/*********************************************************\
* SCI output unsigned 8-bit left-justified integer as decimal
*
* Example:  0x5400  =  "+ 84"
*           0xAB00  =  "- 84"
*
\*********************************************************/
void SCI_s8decun_Out (tword data)
{
  byte a, b, c;
  word r;
  /*
  **  Calculate
  */
  a = (byte)((data.Word >>8) / 100); //Shift the data over since it is MSB only
  r = (data.Word >>8) % 100;
  b = (byte)(r / 10);
  c = (byte)(r % 10);
  /*
  **  Format
  */
  if (a == 0)
  {
    a = 0x00;
    if (b == 0)
    {
      b = 0x00;
    }
  }
  /*
  **  Output result
  */
  SCI_NibbOut (a);
  SCI_NibbOut (b);
  SCI_NibbOut (c);
}


/*********************************************************\
* SCI output signed 8-bit left-justified integer as decimal
*
* Example:  0x5400  =  "+ 84"
*           0xAB00  =  "- 84"
*
\*********************************************************/
void SCI_s8dec_Out (tword data)
{
  byte a, b, c;
  word r;
  /*
  **  Determine sign and output
  */
  if (data.Byte.hi > 0x7F)
  {
    SCI_CharOut ('-');
    data.Word = ~data.Word + 1;
  }
  else
  {
    SCI_CharOut ('+');
  }
  /*
  **  Calculate
  */
  a = (byte)((data.Word >>8) / 100); //Shift the data over since it is MSB only
  r = (data.Word >>8) % 100;
  b = (byte)(r / 10);
  c = (byte)(r % 10);
  /*
  **  Format
  */
  if (a == 0)
  {
    a = 0xF0;
    if (b == 0)
    {
      b = 0xF0;
    }
  }
  /*
  **  Output result
  */
  SCI_NibbOut (a);
  SCI_NibbOut (b);
  SCI_NibbOut (c);
}

/*********************************************************\
* SCI output signed 2-bit left-justified integer as fraction (4 decimal places)
*
* Example:  0x2000  =  "0.50"
*           
*
\*********************************************************/
void SCI_s2fracun_Out (tword data)
{
  BIT_FIELD value;
  word result;
  byte a, b, c, d;
  word r;
  
  SCI_CharOut ('.');
  /*
  **  Determine mantissa value
  */
  result = 0;
  value.Byte = data.Byte.hi;
  if (value.Bit._5 == 1)
    result += FRAC_2d1;
  if (value.Bit._4 == 1)
    result += FRAC_2d2;
  
  /*
  **  Convert mantissa value to 4 decimal places
  */
  r = result % 1000;
  a = (byte)(result / 1000);
  b = (byte)(r / 100);
  r %= 100;
  c = (byte)(r / 10);
  d = (byte)(r % 10);
  /*
  **  Output mantissa
  */
  SCI_NibbOut (a);
  SCI_NibbOut (b);
//  SCI_NibbOut (c);
//  SCI_NibbOut (d);
}


/*********************************************************\
* SCI output signed 4-bit left-justified integer as fraction (4 decimal places)
*
* Example:  0xc000  =  "0.7500"
*           
*
\*********************************************************/
void SCI_s4fracun_Out (tword data)
{
  BIT_FIELD value;
  word result;
  byte a, b, c, d;
  word r;
  
  SCI_CharOut ('.');
  /*
  **  Determine mantissa value
  */
  result = 0;
  value.Byte = data.Byte.hi;
  if (value.Bit._7 == 1)
    result += FRAC_2d1;
  if (value.Bit._6 == 1)
    result += FRAC_2d2;
  if (value.Bit._5 == 1)
    result += FRAC_2d3;
  if (value.Bit._4 == 1)
    result += FRAC_2d4;
  //  
  
  /*
  **  Convert mantissa value to 4 decimal places
  */
  r = result % 1000;
  a = (byte)(result / 1000);
  b = (byte)(r / 100);
  r %= 100;
  c = (byte)(r / 10);
  d = (byte)(r % 10);
  /*
  **  Output mantissa
  */
  SCI_NibbOut (a);
  SCI_NibbOut (b);
  SCI_NibbOut (c);
  SCI_NibbOut (d);
}



/*********************************************************\
* SCI output signed 12-bit left-justified integer as fraction (4 decimal places)
*
* Example:  0x23D0  =  "+0.5596g"
*           0xDC30  =  "-0.5596g"
*
\*********************************************************/
void SCI_s12frac_Out (tword data)
{
  BIT_FIELD value;
  word result;
  byte a, b, c, d;
  word r;
  /*
  **  Determine sign and output
  */
  if (data.Byte.hi > 0x7F)
  {
    SCI_CharOut ('-');
   
    data.Word &= 0xFFF0;
    data.Word = ~data.Word + 1;
  }
  else
  {
    SCI_CharOut ('+');
  }
  /*
  **  Determine integer value and output
  */
//  if (full_scale == FULL_SCALE_2G)
//  {
//    SCI_NibbOut((data.Byte.hi & 0x40) >>6);
//    data.Word = data.Word <<2;
//  }
//  else if (full_scale == FULL_SCALE_4G)
//  {
//    SCI_NibbOut((data.Byte.hi & 0x60) >>5);
//    data.Word = data.Word <<3;
//  }
//  else
//  {
    SCI_NibbOut((data.Byte.hi & 0x70) >>4);
    data.Word = data.Word <<4;
//  }
  SCI_CharOut ('.');
  /*
  **  Determine mantissa value
  */
  result = 0;
  value.Byte = data.Byte.hi;
  if (value.Bit._7 == 1)
    result += FRAC_2d1;
  if (value.Bit._6 == 1)
    result += FRAC_2d2;
  if (value.Bit._5 == 1)
    result += FRAC_2d3;
  if (value.Bit._4 == 1)
    result += FRAC_2d4;
  //  
  data.Word = data.Word <<4;
  value.Byte = data.Byte.hi;
  //  
  if (value.Bit._7 == 1)
    result += FRAC_2d5;
  if (value.Bit._6 == 1)
    result += FRAC_2d6;
  if (value.Bit._5 == 1)
    result += FRAC_2d7;
  if (value.Bit._4 == 1)
    result += FRAC_2d8;
  //
//  if (full_scale != FULL_SCALE_8G)
//  {
//    if (value.Bit._3 == 1)
//      result += FRAC_2d9;
//    if (full_scale == FULL_SCALE_2G)
//      if (value.Bit._2 == 1)
//        result += FRAC_2d10;
//  }
  /*
  **  Convert mantissa value to 4 decimal places
  */
  r = result % 1000;
  a = (byte)(result / 1000);
  b = (byte)(r / 100);
  r %= 100;
  c = (byte)(r / 10);
  d = (byte)(r % 10);
  /*
  **  Output mantissa
  */
  SCI_NibbOut (a);
  SCI_NibbOut (b);
  SCI_NibbOut (c);
  SCI_NibbOut (d);
  SCI_CharOut ('g');
}


/*********************************************************\
**  ASCII character is numeric check
\*********************************************************/
byte isnum (byte data)
{
  if ((data >= '0') && (data <= '9'))
    return (TRUE);
  return (FALSE);
}


/*********************************************************\
**  ASCII character is hexadecimal check
\*********************************************************/
byte ishex (byte data)
{
  if (isnum(data) == TRUE)
    return (TRUE);
  if ((data >= 'A') && (data <= 'F'))
    return (TRUE);
  return (FALSE);
}


/*********************************************************\
**  ASCII character to hexadecimal conversion
\*********************************************************/
byte tohex (byte data)
{
  data -= '0';
  if (data > 9)
    data -= 7;
  return (data);
}


/*********************************************************\
**  Convert hexadecimal to ASCII.
\*********************************************************/
void hex2ASCII (byte data, byte* ptr)
{
  byte temp;

  temp = data >> 4;
  if (temp > 9)
    temp += ('A' - '9' - 1);
  *ptr++ = temp + '0';

  temp = data & 0x0F;
  if (temp > 9)
    temp += ('A' - '9' - 1);
  *ptr = temp + '0';
}


/***********************************************************************************************\
* Private functions
\***********************************************************************************************/

/*********************************************************\
**  Simple SCI Character Send
\*********************************************************/
void SCISendChar (char c)
{
  while (!SCIS1_TDRE);
  SCID = c;
}


/*********************************************************\
**  Manage Input Pointers in Circular Buffer
\*********************************************************/
void ManageInputPointers (void)
{
  if (ptrStringIn == &BufferTx[BUFFER_TX_SIZE])
  {
    ptrStringIn = &BufferTx[0];
  }
  if (++ptrStringTail == &BufferTx[BUFFER_TX_SIZE])
  {
    ptrStringTail = &BufferTx[0];
  }
}

