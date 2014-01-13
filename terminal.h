/***********************************************************************************************\
* Freescale MPL3115A2 Driver
*
* Filename: terminal.h
*
*
* (c) Copyright 2009, Freescale, Inc.  All rights reserved.
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale Semiconductor.
*
\***********************************************************************************************/
#ifndef _TERMINAL_H_
#define _TERMINAL_H_

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

void TerminalInit (void);
void ProcessTerminal (void);
void OutputStreamTerminal (void);
void OutputTerminal (byte BlockID);
void Print_OSR_ST (void);
void OutputStream (void);

#endif  /* _TERMINAL_H_ */
