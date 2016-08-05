/**************************************************************************//**
 * @file bootloader.c
 * @brief EFM32 Bootloader. Preinstalled on all new EFM32 devices
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

#include <stdbool.h>
#include "em_device.h"
#include "usart.h"
#include "xmodem.h"
#include "boot.h"
#include "crc.h"
#include "config.h"
#include "flash.h"
#include "variant.h"

#define BOOTLOADER_VERSION_STRING "V0." __BUILD_NUMBER

/**************************************************************************//**
 * Strings.
 *****************************************************************************/
uint8_t crcString[]     = "\r\nCRC: ";
uint8_t newLineString[] = "\r\n";
uint8_t readyString[]   = "\r\nReady\r\n";
uint8_t okString[]      = "\r\nOK\r\n";
uint8_t failString[]    = "\r\nFail\r\n";
uint8_t unknownString[] = "\r\n?\r\n";


/* Config pins */
void GPIO_pinMode(uint32_t port, uint32_t pin, uint32_t mode)
{
  uint32_t mask, val;

  if(pin < 8) {
    mask = GPIO_MODE_MASK << (pin << 2);
    val = GPIO->P[port].MODEL & ~mask;
    GPIO->P[port].MODEL = val | mode << (pin << 2);
  } else {
    mask = GPIO_MODE_MASK << ((pin & 0x7) << 2);
    val = GPIO->P[port].MODEH & ~mask;
    GPIO->P[port].MODEH = val | mode << ((pin & 0x7) << 2);
  }
}

/* LED functions */
void ledOn(void)  {GPIO->P[LED_PORT].DOUT = ~(1 << LED_PIN);}
void ledOff(void) {GPIO->P[LED_PORT].DOUT = 1 << LED_PIN;}

void led_cycle(uint32_t on_cnt, uint32_t off_cnt)
{
  static uint8_t on = 0;
  static uint32_t ledcnt = 0;

  if((on == 0) && (ledcnt++ < off_cnt)) {
    return;
  } else if((on == 0) && (ledcnt++ >= off_cnt)) {
    ledOn();
    on = 1;
    ledcnt = 0;
  } else if((on == 1) && (ledcnt++ < on_cnt)) {
    return;
  } else if((on == 1) && (ledcnt++ >= on_cnt)) {
    ledOff();
    on = 0;
    ledcnt = 0;
  }
}



/**************************************************************************//**
 * @brief
 *   Helper function to print flash write verification using CRC
 * @param start
 *   The start of the block to calculate CRC of.
 * @param end
 *   The end of the block. This byte is not included in the checksum.
 *****************************************************************************/
void verify(uint32_t start, uint32_t end)
{
  USART_printString(crcString);
  USART_printHex(CRC_calc((void *) start, (void *) end));
  USART_printString(newLineString);
}

/**************************************************************************//**
 * @brief
 *   The main command line loop. Placed in Ram so that it can still run after
 *   a destructive write operation.
 *****************************************************************************/
void commandlineLoop(void)
{
  uint32_t flashSize;
  uint8_t  c;
  uint32_t shouldBoot = 1;
  /* Find the size of the flash. DEVINFO->MSIZE is the
   * size in KB so left shift by 10. */
  flashSize = ((DEVINFO->MSIZE & _DEVINFO_MSIZE_FLASH_MASK) >> _DEVINFO_MSIZE_FLASH_SHIFT) << 10;

  /* The main command loop */
  while (1) {
    /* Retrieve new character */
	if(USART_rxReady())
	{
		c = USART_rxByte();
		shouldBoot = 0;
	}else
	{
		c = 0;
	}
    
    /* Echo */
    if (c != 0) {
      USART_txByte(c);
    }
    switch (c) {
      /* Upload command */
    case 'u':
      USART_printString(readyString);
      XMODEM_download(BOOTLOADER_SIZE, flashSize);
      break;
      /* Boot into new program */
    case 'b':
      BOOT_boot();
      break;
      /* Verify content by calculating CRC of entire flash */
    case 'v':
      verify(0, flashSize);
      break;
      /* Verify content by calculating CRC of application area */
    case 'c':
      verify(BOOTLOADER_SIZE, flashSize);
      break;
      /* Reset command */
    case 'r':
      /* Write to the Application Interrupt/Reset Command Register to reset
       * the EFM32. See section 9.3.7 in the reference manual. */
      SCB->AIRCR = 0x05FA0004;
      break;
    default:
      break;
    }
    if(c != 0) {
      /* Unknown command */
      /* Timeout waiting for RX - avoid printing the unknown string. */
      USART_printString(unknownString);
    }
	
	if(shouldBoot != 0)
	{
		shouldBoot++;
	}
	
	//wait for keypress for about 5 seconds
	if(shouldBoot == 1000000)
	{
		BOOT_boot();
	}
	
	//turn the led on for about .5 second then off for about .5 seconds
	led_cycle(100000, 100000);
  }
  
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  uint32_t tuning;

  //added a slight wait here so the programmer has time to start programming after a reset
  for(volatile int i = 0; i < 500000; i++);
   
  /* Enable clocks for peripherals. */
  CMU->HFPERCLKDIV = CMU_HFPERCLKDIV_HFPERCLKEN;
  CMU->HFPERCLKEN0 = CMU_HFPERCLKEN0_GPIO;

  /* Enable LE and DMA interface */
  CMU->HFCORECLKEN0 = CMU_HFCORECLKEN0_LE | CMU_HFCORECLKEN0_DMA;

  /* Enable LFRCO for RTC */
  CMU->OSCENCMD = CMU_OSCENCMD_LFRCOEN;
  /* Setup LFA to use LFRCRO */
  CMU->LFCLKSEL = CMU_LFCLKSEL_LFA_LFRCO | CMU_LFCLKSEL_LFB_HFCORECLKLEDIV2;

  /* Change to 21MHz internal osciallator to increase speed of
   * bootloader */
  tuning = ((DEVINFO->HFRCOCAL1 & _DEVINFO_HFRCOCAL1_BAND21_MASK)
           >> _DEVINFO_HFRCOCAL1_BAND21_SHIFT);

  CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_21MHZ | tuning;

  /* Setup LED pin */
  GPIO_pinMode(LED_PORT, LED_PIN, GPIO_MODE_PUSHPULL); // set up led pin

  /* Setup pins for USART */
  CONFIG_UsartSetup();
  
  /* Print a message to show that we are in bootloader mode */
  USART_printString(newLineString);
  USART_printString((uint8_t*)BOOTLOADER_VERSION_STRING);
  USART_printString((uint8_t*)" ChipID: ");
  /* Print the chip ID. This is useful for production tracking */
  USART_printHex(DEVINFO->UNIQUEH);
  USART_printHex(DEVINFO->UNIQUEL);

  /* Initialize flash for writing */
  FLASH_init();

  /* Start executing command line */
  commandlineLoop();
}
