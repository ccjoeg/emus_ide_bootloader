/**************************************************************************//**
 * @file leuart.c
 * @brief LEUART code for the EFM32 bootloader
 * @author Silicon Labs
 * @version x.xx
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include "em_device.h"
#include "usart.h"
#include "variant.h"

/***************************************************************************//**
 * @brief
 *   Prints an int in hex.
 *
 * @param integer
 *   The integer to be printed.
 ******************************************************************************/
 RAMFUNC void USART_printHex(uint32_t integer)
{
  uint8_t c;
  int i, digit;

  for (i = 0; i <= 7; i++)
  {
    digit = integer >> 28;
    c = digit + 0x30;
    if (digit >= 10)
    {
      c += 7;
    }
    USART_txByte(c);
    integer <<= 4;
  }
}

/**************************************************************************//**
 * @brief Transmit null-terminated string to BOOTLOADER_USART
 *****************************************************************************/
RAMFUNC void USART_printString(uint8_t *string)
{
  while (*string != 0)
  {
    USART_txByte(*string++);
  }
}


RAMFUNC uint8_t USART_rxReady(void)
{
	if(TTY1 != 0)
	  {
		  if(TTY1[TTY1_STATUS_REG] & TTY1_STATUS_RXDATAV)
		  {
			  TTY0 = 0;
			  return 1;
		  }
	  }
	  if(TTY0 != 0)
	  {
		  if(TTY0[TTY0_STATUS_REG] & TTY0_STATUS_RXDATAV)
		  {
			  TTY1 = 0;
			  return 1;
		  }
	  }	    
	  return 0;
}

/**************************************************************************//**
 * @brief Transmit single byte to BOOTLOADER_USART
 *****************************************************************************/
RAMFUNC uint8_t USART_rxByte(void)
{
  uint32_t timer = 1000000;
  while(--timer ) 
  {
	 if(USART_rxReady())
	 {
		 break;
	 }
  }  
  
  if (timer > 0)
  {
	 if(TTY0 != 0)
	 {
		return((uint8_t)(TTY0[RXDATA_REG] & 0xFF));
	 }
	 else
	 {
		 return((uint8_t)(TTY1[RXDATA_REG] & 0xFF));
	 }
  }
  else
  {
    return 0;
  }
}


/**************************************************************************//**
 * @brief Transmit single byte to BOOTLOADER_USART
 *****************************************************************************/
RAMFUNC void USART_txByte(uint8_t data)
{
  /* Check that transmit buffer is empty */

  //check if TTY0 is valid and send to it if it is
  if(TTY0 != 0)
  {
	  while (!(TTY0[TTY0_STATUS_REG] & TTY0_STATUS_TXBL)) ;
	  TTY0[TTY0_TXDATA_REG] = (uint32_t) data;
  }
  
  //check if TTY1 is valid and send to it if it is
  if(TTY1 != 0)
  {
	  while (!(TTY1[TTY1_STATUS_REG] & TTY1_STATUS_TXBL)) ;
	  TTY1[TTY1_TXDATA_REG] = (uint32_t) data;
  }
  
}

