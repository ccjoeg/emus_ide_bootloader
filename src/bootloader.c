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
#include "em_gpio.h"
#include "em_cmu.h"
#include "usart.h"
#include "xmodem.h"
#include "boot.h"
#include "crc.h"
#include "config.h"
#include "flash.h"
#include "variant.h"

#define BOOTLOADER_VERSION_STRING __BUILD_NUMBER

/**************************************************************************//**
 * Strings.
 *****************************************************************************/
uint8_t crcString[]     = "\r\nCRC: ";
uint8_t newLineString[] = "\r\n";
uint8_t readyString[]   = "\r\nReady\r\n";
uint8_t okString[]      = "\r\nOK\r\n";
uint8_t failString[]    = "\r\nFail\r\n";
uint8_t unknownString[] = "\r\n?\r\n";

uint8_t unlockString[] = "EngimusingUnlock";

volatile unsigned long *ORIG_TTY0;
volatile unsigned long *ORIG_TTY1;
volatile unsigned long *ORIG_TTY2;
volatile unsigned long *ORIG_TTY3;
volatile unsigned long *ORIG_TTY4;
uint32_t currentHandshakeTty;
bool bootloaderUnlocked = false;

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
void ledOn(void)  {GPIO->P[LED_PORT].DOUT &= ~(1 << LED_PIN);}
void ledOff(void) {GPIO->P[LED_PORT].DOUT |= 1 << LED_PIN;}

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

int32_t check_for_break()
{
  if(ORIG_TTY0[TTY0_RXDATAXP_REG] & RXDATAXP_FERRP) 
  {
    TTY0 = ORIG_TTY0;
	TTY1 = 0;
	TTY2 = 0;
    TTY3 = 0;
    TTY4 = 0;
    return 0;
  }
  if(ORIG_TTY1) {
    if(ORIG_TTY1[TTY1_RXDATAXP_REG] & RXDATAXP_FERRP) {
	  TTY1 = ORIG_TTY1;
	  TTY0 = 0;
	  TTY2 = 0;
      TTY3 = 0;
      TTY4 = 0;
      return 1;
    }
  }
  
#ifdef TTY2_RXDATAXP_REG
  if(ORIG_TTY2) {
    if(ORIG_TTY2[TTY2_RXDATAXP_REG] & RXDATAXP_FERRP) {
      TTY2 = ORIG_TTY2;
	  TTY0 = 0;
	  TTY1 = 0;
      TTY3 = 0;
      TTY4 = 0;
      return 2;
    }
  }
#endif

#ifdef TTY3_RXDATAXP_REG
  if(ORIG_TTY3) {
    if(ORIG_TTY3[TTY3_RXDATAXP_REG] & RXDATAXP_FERRP) {
      TTY3 = ORIG_TTY3;
	  TTY0 = 0;
	  TTY1 = 0;
      TTY2 = 0;
      TTY4 = 0;
      return 3;
    }
  }
#endif

#ifdef TTY4_RXDATAXP_REG
  if(ORIG_TTY4) {
    if(ORIG_TTY4[TTY4_RXDATAXP_REG] & RXDATAXP_FERRP) {
      TTY4 = ORIG_TTY4;
	  TTY0 = 0;
	  TTY1 = 0;
      TTY2 = 0;
      TTY3 = 0;
      return 4;
    }
  }
#endif
   return -1;
}

//states of the handshake process
#define NOUART 0
#define FIRSTBREAK 1
#define FOUNDSPACE 2
#define UARTRDY 3

uint32_t processSerialHandshake(uint32_t state)
{
    int32_t breakedTty = check_for_break();
    
    //found a different break state so lets restart the handshake.
    if(breakedTty >= 0 && breakedTty != currentHandshakeTty)
    {
        currentHandshakeTty = breakedTty;
        state = NOUART;
    }
    
    if(state == NOUART)
    {
        if(breakedTty >= 0)
        {
          state = FIRSTBREAK;  
          currentHandshakeTty = breakedTty;
        }
    }
    else if(state == FIRSTBREAK)
    {
         if(USART_rxReady()) 
         {
             uint8_t c = USART_rxByte();
            if(c == ' ')
            {
                state = FOUNDSPACE;
            }
            else if(c == 'r')
            {
                /* Write to the Application Interrupt/Reset Command Register to reset
                * the EFM32. See section 9.3.7 in the reference manual. */
                SCB->AIRCR = 0x05FA0004;
            }

        } 
    }
    else if(state == FOUNDSPACE)
    {
        if(breakedTty >= 0)
        {
            state = UARTRDY;
        }  
    }
        
    return state;
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

void checkUnlockCode()
{
    uint8_t curChar = 0;
    USART_printString((uint8_t*)"\r\nUnlock Code:");
    uint8_t c = USART_rxByte();
    USART_txByte(c);
    
    while( curChar < sizeof(unlockString) - 1 && c == unlockString[curChar])
    {
        curChar++;
        if(curChar < sizeof(unlockString) - 1)
        { 
            c = USART_rxByte();
            USART_txByte(c);
        }
    }
    
    if(curChar == sizeof(unlockString) - 1)
    {
        bootloaderUnlocked = true;
        USART_printString((uint8_t*)"\r\nBootloader Unlocked!\r\n");
    }else
    {
        USART_printString((uint8_t*)"\r\nIncorrect Bootloader Unlock Code!\r\n");
    }
}


/**************************************************************************//**
 * @brief
 *   The main command line loop. Placed in Ram so that it can still run after
 *   a destructive write operation.
 *****************************************************************************/
RAMFUNC void commandlineLoop(void)
{
  uint32_t flashSize;
  uint8_t  c;
  uint32_t shouldBoot = 1;
  /* Find the size of the flash. DEVINFO->MSIZE is the
   * size in KB so left shift by 10. */
  flashSize = ((DEVINFO->MSIZE & _DEVINFO_MSIZE_FLASH_MASK) >> _DEVINFO_MSIZE_FLASH_SHIFT) << 10;

  uint32_t bootloaderState = NOUART;
  
  /* The main command loop */
  while (1) {
    /* Retrieve new character */
    if(bootloaderState == UARTRDY)
    {
        if(USART_rxReady()) {
          c = USART_rxByte();
          shouldBoot = 0;
        } else {
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
        case 'd':
          if(bootloaderUnlocked)
          {
            USART_printString( readyString );
            XMODEM_download( 0, flashSize);
            USART_printString((uint8_t*)"\r\nBootloader Upload Complete!\r\nResetting Board\r\n");
            
            SCB->AIRCR = 0x05FA0004;
          }
          else
          {
              USART_printString((uint8_t*)"\r\nBootloader Locked! Use 'l' command to unlock.\r\n");
          }
          break;
        case 'l':
          checkUnlockCode();
          break;
//Debugging code for printing out contents of flash
/*
        case '4':

          for(int i = 0; i < 1024; i++)
          {
              USART_printHex((uint32_t)*((uint32_t*)(0x3000) + i));
          }
          break;
        case '3':

          for(int i = 0; i < 1024; i++)
          {
              USART_printHex((uint32_t)*((uint32_t*)(0x2000) + i));
          }
          break;
        case '2':

          for(int i = 0; i < 1024; i++)
          {
              USART_printHex((uint32_t)*((uint32_t*)(0x1000) + i));
          }
          break;
        case '1':

          for(int i = 0; i < 1024; i++)
          {
              USART_printHex((uint32_t)*((uint32_t*)(0x0000) + i));
          }
          break;
*/
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
    }
    if(shouldBoot != 0) {
      shouldBoot++;
    }
    //wait for keypress for about 5 seconds
    if((shouldBoot == 1000000) && (GPIO_PinInGet(PORTF,2) == 0)) {
      BOOT_boot();
    }
   
    //turn the led on for about .5 second then off for about .5 seconds
    led_cycle(100000, 100000);
    
    // check for break,space,break handshake condition which turns on a TTY or switches to a different one.
    bootloaderState = processSerialHandshake(bootloaderState);
    
  }
}


void CMU_ClockEnable(CMU_Clock_TypeDef clock, bool enable)
{
  volatile uint32_t *reg;
  uint32_t          bit;
  uint32_t          sync = 0;

  /* Identify enable register */
  switch (((unsigned)clock >> CMU_EN_REG_POS) & CMU_EN_REG_MASK) {
#if defined(_CMU_CTRL_HFPERCLKEN_MASK)
    case CMU_CTRL_EN_REG:
      reg = &CMU->CTRL;
      break;
#endif

#if defined(_CMU_HFCORECLKEN0_MASK)
    case CMU_HFCORECLKEN0_EN_REG:
      reg = &CMU->HFCORECLKEN0;
#if defined(CMU_MAX_FREQ_HFLE)
      setHfLeConfig(SystemCoreClockGet());
#endif
      break;
#endif

#if defined(_CMU_HFBUSCLKEN0_MASK)
    case CMU_HFBUSCLKEN0_EN_REG:
      reg = &CMU->HFBUSCLKEN0;
      break;
#endif

#if defined(_CMU_HFPERCLKDIV_MASK)
    case CMU_HFPERCLKDIV_EN_REG:
      reg = &CMU->HFPERCLKDIV;
      break;
#endif

    case CMU_HFPERCLKEN0_EN_REG:
      reg = &CMU->HFPERCLKEN0;
      break;

#if defined(_CMU_HFPERCLKEN1_MASK)
    case CMU_HFPERCLKEN1_EN_REG:
      reg = &CMU->HFPERCLKEN1;
      break;
#endif

    case CMU_LFACLKEN0_EN_REG:
      reg  = &CMU->LFACLKEN0;
      sync = CMU_SYNCBUSY_LFACLKEN0;
      break;

    case CMU_LFBCLKEN0_EN_REG:
      reg  = &CMU->LFBCLKEN0;
      sync = CMU_SYNCBUSY_LFBCLKEN0;
      break;

#if defined(_CMU_LFCCLKEN0_MASK)
    case CMU_LFCCLKEN0_EN_REG:
      reg = &CMU->LFCCLKEN0;
      sync = CMU_SYNCBUSY_LFCCLKEN0;
      break;
#endif

#if defined(_CMU_LFECLKEN0_MASK)
    case CMU_LFECLKEN0_EN_REG:
      reg  = &CMU->LFECLKEN0;
      sync = CMU_SYNCBUSY_LFECLKEN0;
      break;
#endif

#if defined(_CMU_SDIOCTRL_MASK)
    case CMU_SDIOREF_EN_REG:
      reg = &CMU->SDIOCTRL;
      enable = !enable;
      break;
#endif

#if defined(_CMU_QSPICTRL_MASK)
    case CMU_QSPI0REF_EN_REG:
      reg = &CMU->QSPICTRL;
      enable = !enable;
      break;
#endif
#if defined(_CMU_USBCTRL_MASK)
    case CMU_USBRCLK_EN_REG:
      reg = &CMU->USBCTRL;
      break;
#endif
#if defined(_CMU_PDMCTRL_MASK)
    case CMU_PDMREF_EN_REG:
      reg = &CMU->PDMCTRL;
      break;
#endif

    case CMU_PCNT_EN_REG:
      reg = &CMU->PCNTCTRL;
      break;

    default: /* Cannot enable/disable a clock point. */
      EFM_ASSERT(false);
      return;
  }

  /* Get the bit position used to enable/disable. */
  bit = ((unsigned)clock >> CMU_EN_BIT_POS) & CMU_EN_BIT_MASK;

  /* LF synchronization required. */
  /*if (sync > 0UL) {
    syncReg(sync);
  }*/

  /* Set/clear bit as requested. */
  BUS_RegBitWrite(reg, bit, (uint32_t)enable);
}


/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  uint32_t tuning;

  //added a slight wait here so the programmer has time to start programming after a reset
  for(volatile int i = 0; i < 500000; i++);

// Enable clocks for peripherals.
#if defined(_SILICON_LABS_32B_SERIES_1)
  CMU->CTRL        = CMU_CTRL_HFPERCLKEN;
  CMU->HFBUSCLKEN0 = CMU_HFBUSCLKEN0_GPIO | CMU_HFBUSCLKEN0_LE
                     | CMU_HFBUSCLKEN0_LDMA;

  // Enable LFRCO for RTC.
  CMU->LFECLKSEL = CMU_LFECLKSEL_LFE_LFRCO;
  CMU->LFECLKEN0 = CMU_LFECLKEN0_RTCC;
  CMU->OSCENCMD  = CMU_OSCENCMD_LFRCOEN;
  
  //  CMU->LFACLKSEL = CMU_LFACLKSEL_LFA_LFRCO;
  CMU->LFBCLKSEL = CMU_LFBCLKSEL_LFB_HFCLKLE;
#else
  /* Enable clocks for peripherals. */
  CMU->HFPERCLKDIV = CMU_HFPERCLKDIV_HFPERCLKEN;
  CMU->HFPERCLKEN0 = CMU_HFPERCLKEN0_GPIO;

  /* Enable LE and DMA interface */
  CMU->HFCORECLKEN0 = CMU_HFCORECLKEN0_LE | CMU_HFCORECLKEN0_DMA;

  /* Enable LFRCO for RTC */
  CMU->OSCENCMD = CMU_OSCENCMD_LFRCOEN;
  /* Setup LFA to use LFRCRO */
  CMU->LFCLKSEL = CMU_LFCLKSEL_LFA_LFRCO | CMU_LFCLKSEL_LFB_HFCORECLKLEDIV2;
#endif


#if defined(_SILICON_LABS_32B_SERIES_0)
#if defined (_DEVINFO_HFRCOCAL1_BAND28_MASK)
  // Change to 28MHz internal oscillator to increase speed of
  // bootloader.
  tuning = (DEVINFO->HFRCOCAL1 & _DEVINFO_HFRCOCAL1_BAND28_MASK)
           >> _DEVINFO_HFRCOCAL1_BAND28_SHIFT;

  CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_28MHZ | tuning;
#ifndef NDEBUG
  // Set new clock division based on the 28Mhz clock.
  DEBUG_USART->CLKDIV = 3634;
#endif

#elif defined(_DEVINFO_HFRCOCAL1_BAND21_MASK)
  // Change to 21MHz internal oscillator to increase speed of
  // bootloader.
  tuning = ((DEVINFO->HFRCOCAL1 & _DEVINFO_HFRCOCAL1_BAND21_MASK)
           >> _DEVINFO_HFRCOCAL1_BAND21_SHIFT);

  CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_21MHZ | tuning;
#ifndef NDEBUG
  // Set new clock division based on the 21Mhz clock.
  DEBUG_USART->CLKDIV = 2661;
#endif

#else
#error "Can not make correct clock selection."
#endif
#endif

 // CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

  CMU_ClockEnable(cmuClock_HFLE, true);
  //CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
 // CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
  
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Setup LED pin */
  GPIO_pinMode(LED_PORT, LED_PIN, GPIO_MODE_PUSHPULL); // set up led pin


  
  /* set up pin for bootloader recovery */
  GPIO_pinMode(PORTF,  2, GPIO_MODE_INPUTPULL);
  GPIO_PinOutClear(PORTF,2);
  
  /* Setup pins for USART */
  CONFIG_UsartSetup();

  
  
  //keep around the original pointers so they can all receive break commands.
  ORIG_TTY0 = TTY0;
  ORIG_TTY1 = TTY1;
  ORIG_TTY2 = TTY2;
  ORIG_TTY3 = TTY3;
  ORIG_TTY4 = TTY4;
   
  /* Print a message to show that we are in bootloader mode */
  USART_printString(newLineString);
  
  USART_printString((uint8_t*)BOOTLOADER_VERSION_STRING);
  USART_printString((uint8_t*)" ChipID: ");
  /* Print the chip ID. This is useful for production tracking */
  USART_printHex(DEVINFO->UNIQUEH);
  USART_printHex(DEVINFO->UNIQUEL);
  
  USART_printString((uint8_t*)"\r\nType:");
  USART_printString((uint8_t*)BOARD_TYPE);
  USART_printString(newLineString);
  
  /* Initialize flash for writing */
  FLASH_init();

  //clear the pointers until we recieve a break command on one of them
  TTY0 = 0;
  TTY1 = 0;
  TTY2 = 0;
  TTY3 = 0;
  TTY4 = 0;
  
  /* Start executing command line */
  commandlineLoop();
}

